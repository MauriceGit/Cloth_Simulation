#include <cgimage.h>

#include <cgimage/error.h>
#include <cgimage/endian.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

typedef struct {
    uint16 type;
    uint32 size;
    uint16 reserved[2];
    uint32 offset;
} BMPFileHeader;

typedef struct {
    uint32 size;
    uint32 width;
    uint32 height;
    uint16 planes;
    uint16 bitcount;
    uint32 compression;
    uint32 sizeimage;
    uint32 xpelspermeter;
    uint32 ypelspermeter;
    uint32 clrused;
    uint32 clrimportant;
} BMPInfoHeader;

#define ERROR(msg) \
    CGError_reportFormat( \
        __FILE__, "CGImage_loadBMP", __LINE__, \
        "%s: %s", filename, (msg) \
    )
#define FERROR(msg) ERROR(ferror(stream) ? strerror(errno) : (msg))

CGImage* CGImage_loadBMP(const char* filename, FILE* stream) {
    CGImage* image = NULL;
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    
    unsigned int i;
    
    /* read header */
    if (
        CGEndian_readf(
            stream,
            "w d 2w d",
            &file_header.type, &file_header.size,
            &file_header.reserved, &file_header.offset
        ) != 5
    ) {
        FERROR("Premature end of file");
        goto loadBMP_error;   
    }
    
    /* check file type */
    if (file_header.type != 0x4d42) {
        ERROR("Invalid file format");
        goto loadBMP_error;
    }

    if (
        CGEndian_readf(
            stream, "3d 2w 6d",
            &info_header.size, &info_header.planes, &info_header.compression
        ) != 11
    ) {
        FERROR("Premature end of file");
        goto loadBMP_error;   
    }

    /* only uncompressed true color bitmaps for now */
    if (
        info_header.planes != 1
        || info_header.bitcount != 24
        || info_header.compression != 0
    ) {
        ERROR("Unsupported image format");
        goto loadBMP_error;
    }

    image = CGImage_create(info_header.width, info_header.height, 3);

    for (i = 0; i < info_header.width * info_header.height; ++i) {
        unsigned char buffer[3];

        if (fread(buffer, sizeof(buffer), 1, stream) != 1) {
            FERROR("Premature end of file");
            goto loadBMP_error;
        }

        image->data[i * 3 + 0] = buffer[2];
        image->data[i * 3 + 1] = buffer[1];
        image->data[i * 3 + 2] = buffer[0];
    }

    CGImage_mirrorY(image);
    
    goto loadBMP_end;
    
    loadBMP_error:
        CGImage_free(image);
        image = NULL;
    
    loadBMP_end:
        return image;
}

#undef FERROR
#undef ERROR
