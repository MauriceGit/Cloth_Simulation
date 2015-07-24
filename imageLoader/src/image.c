#include <cgimage.h>

#include <cgimage/error.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>

CGImage* CGImage_create(
    unsigned int width,
    unsigned int height,
    unsigned int bpp
) {
    CGImage* self = malloc(sizeof(CGImage));

    if (self)
        CGImage_init(self, width, height, bpp);

    return self;
}

void CGImage_init(
    CGImage* self,
    unsigned int width,
    unsigned int height,
    unsigned int bpp
) {
    self->width = width;
    self->height = height;
    self->bpp = bpp;

    self->data = calloc(width * height, bpp);
    cg_assert(self->data != NULL);
}

void CGImage_copy(
    CGImage* self,
    int x, int y,
    unsigned int w, unsigned int h,
    CGImage* dst,
    int dst_x, int dst_y
) {
    unsigned int r, c, b, bpp;

    if (
        (w == 0) || (h == 0)
         || ((x >= 0) && ((unsigned int)x >= self->width))
         || ((x < 0) && ((unsigned int)-x >= w))
         || ((y >= 0) && ((unsigned int)y >= self->height))
         || ((y < 0) && ((unsigned int)-y >= h))
         || ((dst_x >= 0) && ((unsigned int)dst_x >= dst->width))
         || ((dst_x < 0) && ((unsigned int)-dst_x >= w))
         || ((dst_y >= 0) && ((unsigned int)dst_y >= dst->height))
         || ((dst_y < 0) && ((unsigned int)-dst_y >= h))
    )
        return;

    /* clip on source image */
    if (x < 0) {
        w     += x;
        dst_x -= x;
        x      = 0;
    }

    if (y < 0) {
        h     += y;
        dst_y -= y;
        y      = 0;
    }

    if ((x + w) > self->width)
        w = self->width - x;

    if ((y + h) > self->height)
        h = self->height - y;

    /* clip on destination image */
    if (dst_x < 0) {
        x     -= dst_x;
        w     += dst_x;
        dst_x  = 0;
    }

    if (dst_y < 0) {
        y     -= dst_y;
        w     += dst_y;
        dst_y  = 0;
    }

    if ((dst_x + w) >= dst->width)
        w = dst->width - dst_x;

    if ((dst_y + h) >= dst->height)
        h = dst->height - dst_y;

    /* copy that data */
    bpp = self->bpp < dst->bpp ? self->bpp : dst->bpp;

    for (r = 0; r < h; ++r) {
        for (c = 0; c < w; ++c) {
            for (b = 0; b < bpp; ++b) {
                dst->data[((dst_y + r) * dst->width + (dst_x + c)) * dst->bpp + b] =
                        self->data[((y + r) * self->width + (x + c)) * self->bpp + b];
            }
        }
    }
}

void CGImage_mirrorX(CGImage* self) {
    unsigned char* tmp;
    unsigned int x, y;

    tmp = malloc(self->bpp);

    for (y = 0; y < self->height; ++y) {
        for (x = 0; x < self->width / 2; ++x) {
            memcpy(tmp, self->data + (y * self->width + x) * self->bpp, self->bpp);
            memcpy(self->data + (y * self->width + x) * self->bpp, self->data + (y * self->width + (self->width - x - 1)) * self->bpp, self->bpp);
            memcpy(self->data + (y * self->width + (self->width - x - 1)) * self->bpp, tmp, self->bpp);
        }
    }
}

void CGImage_mirrorY(CGImage* self) {
    unsigned char* tmp;
    unsigned int tmp_size, y;

    tmp_size = self->width * self->bpp;
    tmp = malloc(tmp_size);

    for (y = 0; y < self->height / 2; ++y) {
        memcpy(tmp, self->data + y * tmp_size, tmp_size);
        memcpy(self->data + y * tmp_size, self->data + (self->height - y - 1) * tmp_size, tmp_size);
        memcpy(self->data + (self->height - y - 1) * tmp_size, tmp, tmp_size);
    }

    free(tmp);
}

void CGImage_free(CGImage* self) {
    if (self)
        free(self->data);

    free(self);
}

CGImage* CGImage_load(const char* filename) {
    CGImage* image = NULL;
    FILE* stream    = NULL;

    /* open file */
    if (!(stream = fopen(filename, "rb"))) {
        CGError_reportFormat(
            __FILE__, "CGImage_load", __LINE__,
            "%s: %s", filename, strerror(errno)
        );

    } else {
        image = CGImage_loadStream(filename, stream);
        fclose(stream);
    }

    return image;
}

CGImage* CGImage_loadBMP(const char* filename, FILE* stream);
CGImage* CGImage_loadPCX(const char* filename, FILE* stream);
CGImage* CGImage_loadPNG(const char* filename, FILE* stream);
CGImage* CGImage_loadPPM(const char* filename, FILE* stream);
CGImage* CGImage_loadTGA(const char* filename, FILE* stream);

typedef enum {
    FORMAT_UNKNOWN,
    FORMAT_BMP,
    FORMAT_PCX,
    FORMAT_PNG,
    FORMAT_PPM,
    FORMAT_TGA
} ImageFormat;

static ImageFormat CGImage_identify(
    const char* filename,
    const unsigned char* buffer,
    unsigned int cnt
) {
    unsigned int slen = strlen(filename);
    const char* ext = filename + slen - (slen >= 4 ? 4 : 0);

    /* try to detect image format by content */
    if ((cnt >= 2) && !memcmp(buffer, "BM", 2))
        return FORMAT_BMP;

    if ((cnt >= 1) && (buffer[0] == 10)) /* is this wise? */
        return FORMAT_PCX;

    if ((cnt >= 8) && !memcmp(buffer, "\211PNG\r\n\032\n", 8))
        return FORMAT_PNG;

    if ((cnt >= 2) && !memcmp(buffer, "P6", 2))
        return FORMAT_PPM;
        
    /* fallback to extension */
    if (!strcmp(ext, ".bmp") || !strcmp(ext, ".BMP"))
        return FORMAT_BMP;

    if (!strcmp(ext, ".pcx") || !strcmp(ext, ".PCX"))
        return FORMAT_PCX;

    if (!strcmp(ext, ".png") || !strcmp(ext, ".PNG"))
        return FORMAT_PNG;

    if (!strcmp(ext, ".ppm") || !strcmp(ext, ".PPM"))
        return FORMAT_PPM;

    if (!strcmp(ext, ".tga") || !strcmp(ext, ".TGA"))
        return FORMAT_TGA;

    return FORMAT_UNKNOWN;
}

CGImage* CGImage_loadStream(const char* filename, FILE* stream) {
    unsigned char magic[32];
    long int read_count;

    ImageFormat format;

    read_count = fread(magic, 1, sizeof(magic), stream);

    if (fseek(stream, -read_count, SEEK_CUR) != 0) {
        CGError_reportFormat(
            __FILE__, "CGImage_loadStream", __LINE__,
            "%s: %s", filename, strerror(errno)
        );
        return NULL;
    }

    format = CGImage_identify(filename, magic, read_count);

    switch (format) {
        case FORMAT_BMP:
            return CGImage_loadBMP(filename, stream);

        case FORMAT_PCX:
            return CGImage_loadPCX(filename, stream);

#ifdef HAVE_LIBZ
        case FORMAT_PNG:
            return CGImage_loadPNG(filename, stream);
#endif

        case FORMAT_PPM:
            return CGImage_loadPPM(filename, stream);

        case FORMAT_TGA:
            return CGImage_loadTGA(filename, stream);

        default:
            CGError_reportFormat(
                __FILE__, "CGImage_loadStream", __LINE__,
                "%s: %s", filename, "unknown image format"
            );
            return NULL;
    }
}
