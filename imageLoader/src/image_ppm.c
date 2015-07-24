#include <cgimage.h>

#include <cgimage/error.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#define ERROR(msg) \
    CGError_reportFormat( \
        __FILE__, "CGImage_loadPPM", __LINE__, \
        "%s: %s", filename, (msg) \
    )
#define FERROR(msg) ERROR(ferror(stream) ? strerror(errno) : (msg))

CGImage* CGImage_loadPPM(const char* filename, FILE* stream) {
    CGImage* image = NULL;

    char magic[2];
    unsigned int width, height, max;

    /* read header */
    if (fscanf(
            stream, "%2c%*[\n]%d%*[ ]%d%*[\n]%d%*[\n]",
            magic, &width, &height, &max
        ) != 4
    ) {
        FERROR("Premature end of file");
        goto loadPPM_error;
    }

    if (magic[0] != 'P' || magic[1] != '6') {
        ERROR("Usupported image format");
        goto loadPPM_error;
    }

    /* allocate memory */
    if (
        !(image = CGImage_create(width, height, 3))
        || !image->data
    ) {
        ERROR(strerror(ENOMEM));
        goto loadPPM_error;
    }

    /* read bitmap data */
    if (
        fread(
            image->data, 1, width * height * 3, stream
        ) != width * height * 3
    ) {
        FERROR("Premature end of file");
        goto loadPPM_error;
    }

    /* rescale intensities */
    if (max != 255) {
        unsigned int i;

        for (i = 0; i < width * height * 3; ++i)
            image->data[i] = (image->data[i] * 255) / max;
    }

    goto loadPPM_end;
    
    loadPPM_error:
        CGImage_free(image);
        image = NULL;
        
    loadPPM_end:
        return image;
}

#undef FERROR
#undef ERROR
