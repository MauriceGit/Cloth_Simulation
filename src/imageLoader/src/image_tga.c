#include <cgimage.h>

#include <cgimage/error.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

typedef struct {
    uint8 id_length;
    uint8 palette_type;
    uint8 image_type;
    uint8 palette_spec[5];
    uint8 image_spec[10];
} TGAHeader;

static void putpixel(unsigned char* pixel, unsigned char* buffer, int buffersize) {
    if (buffersize == 4) {
        pixel[0] = buffer[2];
        pixel[1] = buffer[1];
        pixel[2] = buffer[0];
        pixel[3] = buffer[3];
    } else if (buffersize == 3) {
        pixel[0] = buffer[2];
        pixel[1] = buffer[1];
        pixel[2] = buffer[0];
    } else {
        pixel[0] = (buffer[1] & 0x7C) >> 2;
        pixel[1] = (buffer[1] & 0x03) | (buffer[0] & 0xE0) >> 5;
        pixel[2] = buffer[0] & 0x1F;
    }
}

#define ERROR(msg) \
    CGError_reportFormat( \
        __FILE__, "CGImage_loadTGA", __LINE__, \
        "%s: %s", filename, (msg) \
    )
#define FERROR(msg) ERROR(ferror(stream) ? strerror(errno) : (msg))

CGImage* CGImage_loadTGA(const char* filename, FILE* stream) {
    unsigned char* palette = NULL;
    CGImage* image = NULL;

    TGAHeader header;
    unsigned char buffer[4];
    unsigned int cmap_len, pixel_size;
    unsigned int width, height, bpp;
    unsigned int pixel;

    /* read header */
    if (fread(&header, 18, 1, stream) != 1) {
        FERROR("Premature end of file");
        goto loadTGA_error;
    }

    if ((header.image_type & ~0xB) || ((header.image_type && 0x3) == 0x3)) {
        ERROR("Usupported image format");
        goto loadTGA_error;
    }

    if ((header.image_type & 0x1) && (!header.palette_type)) {
        ERROR("Color-mapped image without color map");
        goto loadTGA_error;
    }

    bpp = pixel_size = (header.image_spec[8] + 7) / 8;

    /* skip image id */
    fseek(stream, header.id_length, SEEK_CUR);

    /* read color palette data */
    if (header.palette_type) {
        unsigned int cmap_size, cmap_entry;
        
        cmap_entry = header.palette_spec[4] / 8;
        cmap_len = (header.palette_spec[3] << 8) + header.palette_spec[2];
        cmap_size = cmap_entry == 4 ? 4 : 3;

        if ((header.image_type & 0x3) == 0x1) {
            if (!(palette = malloc(cmap_len * cmap_size))) {
                ERROR(strerror(ENOMEM));
                goto loadTGA_error;
            }

            for (pixel = 0; pixel < cmap_len; ++pixel) {
                if (fread(buffer, 1, cmap_entry, stream) != cmap_entry) {
                    FERROR("Premature end of file");
                    goto loadTGA_error;
                }

                putpixel(palette + pixel * cmap_size, buffer, cmap_entry);
            }
            
            bpp = cmap_size;
        } else {
            fseek(stream, cmap_len * cmap_entry, SEEK_CUR);
        }
    }
    
    width = (header.image_spec[5] << 8) + header.image_spec[4];
    height = (header.image_spec[7] << 8) + header.image_spec[6];
    if (!(image = CGImage_create(width, height, bpp)) || !image->data) {
        ERROR(strerror(ENOMEM));
        goto loadTGA_error;
    }

    pixel = 0;
    /* load pixel data */
    /* RLE encoded */
    if (header.image_type & 0x8) {
        while (pixel < width * height) {
            unsigned int count;
        
            if (fread(buffer, 1, 1, stream) != 1) {
                FERROR("Premature end of file");
                goto loadTGA_error;
            }
            
            count = (buffer[0] & 0x7F) + 1;

            /* RLE packet */
            if (buffer[0] & 0x80) {
                unsigned char color[4];
                if (fread(buffer, 1, pixel_size, stream) != pixel_size) {
                    FERROR("Premature end of file");
                    goto loadTGA_error;
                }

                if (palette) {
                    unsigned int idx, i;

                    for (i = 0, idx = 0; i < pixel_size; ++i) {
                        idx += buffer[i] << (8 * i);
                    }

                    memcpy(color, palette + idx * bpp, bpp);

                } else {
                    putpixel(color, buffer, pixel_size);
                }
                
                while (count--) {
                    memcpy(image->data + pixel * bpp, color, bpp);
                    ++pixel;
                }
                
            /* RAW packet */
            } else {
                while (count--) {
                    if (fread(buffer, 1, pixel_size, stream) != pixel_size) {
                        FERROR("Premature end of file");
                        goto loadTGA_error;
                    }

                    if (palette) {
                        unsigned int idx, i;

                        for (i = 0, idx = 0; i < pixel_size; ++i) {
                            idx += buffer[i] << (8 * i);
                        }

                        memcpy(
                            image->data + pixel * bpp,
                            palette + idx * bpp,
                            bpp
                        );

                    } else {
                        putpixel(
                            image->data + pixel * bpp,
                            buffer,
                            pixel_size
                        );
                    }
                    ++pixel;
                }
            }
        }
        
    /* uncompressed */
    } else {
        while (pixel < width * height) {
            if (fread(buffer, 1, pixel_size, stream) != pixel_size) {
                FERROR("Premature end of file");
                goto loadTGA_error;
            }
            
            if (palette) {
                unsigned int idx, i;
                
                for (i = 0, idx = 0; i < pixel_size; ++i) {
                    idx += buffer[i] << (8 * i);
                }
                
                memcpy(image->data + pixel++ * bpp, palette + idx * bpp, bpp);
            } else {
                putpixel(image->data + pixel++ * bpp, buffer, pixel_size);
            }
        }
    }
    
    if (!(header.image_spec[9] & 0x20)) CGImage_mirrorY(image);

    goto loadTGA_end;
    
    loadTGA_error:
        CGImage_free(image);
        image = NULL;
        
    loadTGA_end:
        free(palette);
        return image;
}

#undef FERROR
#undef ERROR
