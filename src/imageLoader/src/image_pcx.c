#include <cgimage.h>

#include <cgimage/error.h>
#include <cgimage/endian.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#define PCX_MAGIC 10
#define PCX_ENCODING_RUNLENGTH 1

typedef struct {
    uint8  manufacturer; /* magic: 10 */
    uint8  version;
    uint8  encoding;     /* const 1, pcx run length encoding */
    uint8  bpp;
    uint16 dimension[4];
    uint16 dpi[2];
    uint8  colormap[48];
    uint8  reserved;     /* should be 0 */
    uint8  planes;       /* number of color planes */
    uint16 bpl;          /* buffersize needed to store one scanline colorplane */
    uint16 palette;
    uint16 screen_size[2];
    uint8  filler[54];
} PCXHeader;

#define ERROR(msg) \
    CGError_reportFormat( \
        __FILE__, "CGImage_loadPCX", __LINE__, \
        "%s: %s", filename, (msg) \
    )
#define FERROR(msg) ERROR(ferror(stream) ? strerror(errno) : (msg))

CGImage* CGImage_loadPCX(const char* filename, FILE* stream) {
    unsigned char* palette = NULL;
    CGImage* image = NULL;
    
    PCXHeader header;
    unsigned int count, w, h, i;
    int buffer;
    
    /* read header */
    if (CGEndian_readf(
            stream, "4b 6w 50b 4w 54b",
            &header.manufacturer,
            &header.dimension,
            &header.colormap,
            &header.bpl,
            &header.filler
        ) != 118)
    {
        FERROR("Premature end of file");
        goto loadPCX_error;
    }
    
    /* check magic & version */
    if ((header.manufacturer != PCX_MAGIC) || (header.version != 5) || (header.bpp != 8)) {
        ERROR("Invalid/Unsupported file format");
        goto loadPCX_error;
    }
    
    /* read color palette */
    if (header.planes == 1) {
        fseek(stream, -769, SEEK_END);
        if (fgetc(stream) == 12) {
            if (!(palette = malloc(256 * 3))) {
                ERROR(strerror(ENOMEM));
                goto loadPCX_error;
            }
            if (fread(palette, 1, 256 * 3, stream) != 256 * 3) {
                FERROR("Premature end of file");
                goto loadPCX_error;
            }
        }
    }
    
    w = header.dimension[2] - header.dimension[0] + 1;
    h = header.dimension[3] - header.dimension[1] + 1;
    
    image = CGImage_create(w, h, palette ? 3 : header.planes);
    if (!image || !image->data) {
        ERROR(strerror(ENOMEM));
        goto loadPCX_error;
    }
        
    /* read and convert image data */
    fseek(stream, sizeof(PCXHeader), SEEK_SET);
    
    /* 256 color palette image */
    if ((header.planes == 1) && palette) {
        i = 0;
        while(i < w * h * 3) {
            count = 1;
            if ((buffer = fgetc(stream)) == EOF) {
                FERROR("Premature end of file");
                goto loadPCX_error;
            }
            
            if ((buffer & 192) == 192) {
                count = buffer & ~192;
                buffer = fgetc(stream);
            }
            while (count-- > 0) {
                image->data[i++] = palette[buffer * 3 + 0];
                image->data[i++] = palette[buffer * 3 + 1];
                image->data[i++] = palette[buffer * 3 + 2];
            }
        }
        
    /* 8bit color planes */
    } else {
        for (h = 0; h < image->height; ++h) {       /* lines */
            for (i = 0; i < header.planes; ++i) {   /* planes */
                w = 0;
                while (w < image->width) {          /* pixel color plane */
                    count = 1;
                    if ((buffer = fgetc(stream)) == EOF) {
                        FERROR("Premature end of file");
                        goto loadPCX_error;
                    }

                    if ((buffer & 192) == 192) {
                        count = buffer & ~192;
                        buffer = fgetc(stream);
                    }
                    while (count-- > 0) {
                        image->data[(h * image->width + w++) * header.planes + i] = buffer;
                    }
                }
            }
        }
    }
    
    goto loadPCX_end;
    
    loadPCX_error:
        CGImage_free(image);
        image = NULL;
        
    loadPCX_end:
        free(palette);
        return image;
}

#undef FERROR
#undef ERROR
