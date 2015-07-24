#include <cgimage.h>

#include <cgimage/error.h>
#include <cgimage/endian.h>

#include <zlib.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#define PNG_MAGIC "\211PNG\r\n\032\n"

typedef struct {
    uint32 size;
    uint32 type;
    uint32 crc;
} PNGChunkHeader;

typedef struct {
    uint32 width;
    uint32 height;
    uint8  bit_depth;
    uint8  color_type;
    uint8  compression;
    uint8  filter;
    uint8  interlace;
} PNGImageHeader;

typedef struct {
    unsigned char* buffer_in;
    unsigned char* buffer_out;
    unsigned char* buffer_backup;

    z_stream zlib;
    
    unsigned int sample_size;
    unsigned int pixel_size;
    unsigned int scanline_size;
    unsigned int buffer_size;

    unsigned int scanline;    
    
    unsigned int plane_count;
    unsigned int sample_mask;
} PNGDecoderState;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ERROR(msg) \
    CGError_reportFormat( \
        __FILE__, "CGImage_loadPNG", __LINE__, \
        "%s: %s", filename, (msg) \
    )
#define FERROR(msg) ERROR(ferror(stream) ? strerror(errno) : (msg))

CGImage* CGImage_loadPNG(const char* filename, FILE* stream) {
    CGImage* image = NULL;
    
    PNGChunkHeader chunk;
    PNGImageHeader header;

    PNGDecoderState decoder;
    
    char magic[8];
    
    int chunk_number = 0;
    int done = 0;

    memset(&decoder, 0, sizeof(PNGDecoderState));
    
    /* read png file magic */
    if (fread(magic, 1, 8, stream) != 8) {
        FERROR("Premature end of file");
        goto loadPNG_error;
    }

    /* check file magic */
    if (strncmp(magic, PNG_MAGIC, 8)) {
        ERROR("Invalid file format");
        goto loadPNG_error;
    }
    
    /* read png chunks */
    while (!done && !feof(stream)) {
        /* read chunk header */
        if (CGEndian_readf(stream, "2D", &chunk.size) != 2) {
            FERROR("Premature end of file");
            goto loadPNG_error;
        }

        /* first chunk must be IHDR */
        if ((chunk_number == 0) && (chunk.type != 0x49484452)) {
            ERROR("First chunk must be IHDR");
            goto loadPNG_error;
        }

        /* IHDR */
        if (chunk.type == 0x49484452) {
            if (chunk_number != 0) {
                ERROR("Repeated IHDR chunk found");
                goto loadPNG_error;
            }

            if (chunk.size != 13) {
                ERROR("Invalid IHDR chunk size");
                goto loadPNG_error;
            }
            
            if (CGEndian_readf(
                    stream, "2D 5B",
                    &header.width,
                    &header.bit_depth
                ) != 7)
            {
                FERROR("Premature end of file");
                goto loadPNG_error;
            }
            
            /* check header fields */
            if (header.compression != 0) {
                ERROR("Compression method not supported");
                goto loadPNG_error;
            }
            
            if (header.filter != 0) {
                ERROR("Filter method not supported");
                goto loadPNG_error;
            }
            
            if (header.interlace != 0) {
                ERROR("Interlace method not supported");
                goto loadPNG_error;
            }
            
            if (header.color_type & ~0x07) {
                ERROR("Sample type not supported");
                goto loadPNG_error;
            }
            
            if (header.color_type & 0x01) {
                ERROR("Palette images not supported");
                goto loadPNG_error;
            }
            
            /* initialize decoder */
            decoder.plane_count =
                (header.color_type & 0x02 ? 3 : 1) +
                (header.color_type & 0x04 ? 1 : 0);
            decoder.sample_mask =
                ~(~0 << header.bit_depth);
            
            decoder.sample_size =
                (header.bit_depth + 7) / 8;
            decoder.pixel_size =
                (header.bit_depth * decoder.plane_count + 7) / 8;
            decoder.scanline_size =
                (header.bit_depth * decoder.plane_count * header.width + 7) / 8;
            
            /* create image buffer */
            image = CGImage_create(
                header.width, header.height, decoder.plane_count
            );
            if (!image || !image->data) {
                ERROR(strerror(ENOMEM));
                goto loadPNG_error;
            }
            
            /* initialize the zlib inflate stream */
            decoder.buffer_size    = decoder.scanline_size + 1;
            decoder.buffer_in      = malloc(decoder.buffer_size);
            decoder.buffer_out     = malloc(decoder.buffer_size);
            decoder.buffer_backup  = malloc(decoder.buffer_size);
            
            decoder.zlib.zalloc    = (alloc_func)Z_NULL;
            decoder.zlib.zfree     = (free_func)Z_NULL;
            decoder.zlib.opaque    = (voidpf)Z_NULL;

            decoder.zlib.next_in   = decoder.buffer_in;
            decoder.zlib.avail_in  = 0;
            decoder.zlib.next_out  = decoder.buffer_out;
            decoder.zlib.avail_out = decoder.buffer_size;

            if (inflateInit(&decoder.zlib) != Z_OK) {
                ERROR(decoder.zlib.msg);
                goto loadPNG_error;
            }

        /* IEND */
        } else if (chunk.type == 0x49454e44) {
            done = 1;
        
        /* IDAT */
        } else if (chunk.type == 0x49444154) {
            unsigned int data_left = chunk.size;

            while ((data_left > 0) || (decoder.zlib.avail_in)) {
                unsigned int read_count = MIN(
                    decoder.buffer_size - decoder.zlib.avail_in,
                    data_left
                );
                
                /* read compressed data */
                if (read_count > 0) {
                    if (fread(decoder.buffer_in + decoder.zlib.avail_in,
                              1, read_count, stream) != read_count
                    ) {
                        FERROR("Premature end of file");
                        goto loadPNG_error;
                    }

                    data_left             -= read_count;
                    decoder.zlib.avail_in += read_count;
                }
                
                /* inflate data */
                if ((decoder.zlib.avail_in  > 0) &&
                    (decoder.zlib.avail_out > 0)
                ) {
                    if (inflate(&decoder.zlib, Z_NO_FLUSH) < 0) {
                        ERROR(decoder.zlib.msg);
                        goto loadPNG_error;
                    }
                
                    memmove(
                        decoder.buffer_in,
                        decoder.zlib.next_in,
                        decoder.zlib.avail_in
                    );
                    decoder.zlib.next_in = decoder.buffer_in;
                }
                
                /* process inflated data */
                if (decoder.zlib.avail_out == 0) {
                    unsigned char* ptr = decoder.buffer_out;
                    unsigned char filter = *ptr++;
                    unsigned int pixel, byte;
                    int bit_shift;
                    
                    if (decoder.scanline >= image->height) {
                        ERROR("Too many pixels");
                        goto loadPNG_error;
                    }
                
                    if (filter > 4) {
                        ERROR("Invalid scanline filter");
                        goto loadPNG_error;
                    }
                    
                    /* filter scanline */
                    if ((filter == 1) ||
                        ((filter == 3) && (decoder.scanline == 0))
                    ) {
                        for (byte = decoder.pixel_size + 1; byte < decoder.buffer_size; ++byte)
                            decoder.buffer_out[byte] +=
                                decoder.buffer_out[byte - decoder.pixel_size];
                        
                    } else if ((filter == 2) && (decoder.scanline != 0)) {
                        for (byte = 1; byte < decoder.buffer_size; ++byte)
                            decoder.buffer_out[byte] += decoder.buffer_backup[byte];
                        
                    } else if ((filter == 3) && (decoder.scanline != 0)) {
                        for (byte = 1; byte < 1 + decoder.pixel_size; ++byte)
                            decoder.buffer_out[byte] +=
                                decoder.buffer_backup[byte] / 2;
                        for (        ; byte < decoder.buffer_size; ++byte)
                            decoder.buffer_out[byte] +=
                                ((int)(decoder.buffer_out[byte - decoder.pixel_size]) +
                                 (int)(decoder.buffer_backup[byte])) >> 1;
                        
                    } else if (filter == 4) {
                        unsigned char a, b, c, out;
                        int p, pa, pb, pc;
                        for (byte = 1; byte < decoder.buffer_size; ++byte) {
                            a = (byte > decoder.pixel_size)
                                ? decoder.buffer_out[byte - decoder.pixel_size]
                                : 0;
                            b = (decoder.scanline > 0)
                                ? decoder.buffer_backup[byte]
                                : 0;
                            c = (byte > decoder.pixel_size) && (decoder.scanline > 0)
                                ? decoder.buffer_backup[byte - decoder.pixel_size]
                                : 0;
                            
                            p  = a + b - c;
                            pa = abs(p - a);
                            pb = abs(p - b);
                            pc = abs(p - c);
                            
                            if ((pa <= pb) && (pa <= pc))
                                out = a;
                            else if (pb <= pc)
                                out = b;
                            else
                                out = c;
                            
                            decoder.buffer_out[byte] += out;
                        }
                            
                    }
                    
                    /* decode scanline */
                    bit_shift = 8 * decoder.sample_size - header.bit_depth;
                    for (pixel = 0; pixel < header.width; ++pixel) {
                        unsigned int sample, plane;
                    
                        for (plane = 0; plane < decoder.plane_count; ++plane) {
                            unsigned int i;

                            /* read all bytes that contain sample data */
                            for (sample = 0, i = 0; i < decoder.sample_size; ++i)
                                sample = (sample << 8) | ptr[i];
                            
                            /* extract sample data */
                            sample = (sample >> bit_shift) & decoder.sample_mask;
                            
                            /* scale sample to 8 bit color bit_depth */
                            sample = (sample * 255 + 1) / decoder.sample_mask;
                                
                            image->data[(decoder.scanline * image->width + pixel) * image->bpp + plane] = sample;

                            bit_shift -= header.bit_depth;
                            if (bit_shift < 0) {
                                ptr       +=     decoder.sample_size;
                                bit_shift += 8 * decoder.sample_size;
                            }
                        }
                    }
                    
                    /* exchange buffers */
                    ptr                   = decoder.buffer_backup;
                    decoder.buffer_backup = decoder.buffer_out;
                    decoder.buffer_out    = ptr;
                    
                    decoder.zlib.next_out  = decoder.buffer_out;
                    decoder.zlib.avail_out = decoder.buffer_size;
                    
                    ++decoder.scanline;
                }
            }
            
        /* unknown chunks */
        } else {
            if (((chunk.type >> 24) & 0x20) == 0) {
                ERROR("Unknown critical chunk found");
                goto loadPNG_error;
            }
            
            if (((chunk.type >>  8) & 0x20) != 0) {
                ERROR("Invalid chunk found (reserved bit set)");
                goto loadPNG_error;
            }

            /* skip chunk */
            fseek(stream, chunk.size, SEEK_CUR);
        }
        
        /* skip chunk crc */
        if (CGEndian_readf(stream, "D", &chunk.crc) != 1) {
            FERROR("Premature end of file");
            goto loadPNG_error;
        }
        
        ++chunk_number;
    }

    /* no IEND chunk found? */
    if (!done) {
        ERROR("File truncated");
        goto loadPNG_error;
    }
    
    if (decoder.scanline < image->height) {
        ERROR("Too few pixels");
        goto loadPNG_error;
    }

    goto loadPNG_end;

    loadPNG_error:
        CGImage_free(image);
        image = NULL;

    loadPNG_end:
        inflateEnd(&decoder.zlib);
        free(decoder.buffer_in);
        free(decoder.buffer_out);
        free(decoder.buffer_backup);
        return image;
}

#undef FERROR
#undef ERROR
