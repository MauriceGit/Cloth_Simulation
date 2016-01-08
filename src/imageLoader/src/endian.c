#include <cgimage/endian.h>

#include <cgimage/error.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <config.h>

#ifdef WORDS_BIGENDIAN
# define HOST_ENDIANESS CG_ENDIAN_BIG
#else
# define HOST_ENDIANESS CG_ENDIAN_LITTLE
#endif

CGEndian_Endianess CGEndian_endianess(void) {
    static union {
        int  as_int;
        char as_char;
    } one = { 1 };
    
    if (one.as_char == 1) {
        return CG_ENDIAN_LITTLE;
    } else {
        return CG_ENDIAN_BIG;
    }
}

void CGEndian_swap(void* buffer, int size) {
    char tmp;
    char* src;
    char* dst;
    
    src = (char*)buffer;
    dst = (char*)buffer + size - 1;
    while (src < dst) {
        tmp = *src;
        *src = *dst;
        *dst = tmp;
        ++src; --dst;
    }
}

void CGEndian_swapArray(void* buffer, int size, int count) {
    int c;
    
    for (c = 0; c < count; ++c) {
        CGEndian_swap((char*)buffer + c * size, size);
    }
}

void CGEndian_swapShort(short int* buffer) {
    CGEndian_swap(buffer, sizeof(short int));
}

void CGEndian_swapInt(int* buffer) {
    CGEndian_swap(buffer, sizeof(int));
}

void CGEndian_swapLong(long int* buffer) {
    CGEndian_swap(buffer, sizeof(long int));
}

int CGEndian_readf(FILE* stream, const char* format, ...)
{
    va_list args;
    const char* c = format;
    int cont = 1, items = 0;

    va_start(args, format);

    while (cont && *c) {
        void* buffer;
        int size, order, count;
        int read;

        while (isspace((unsigned char)*c))
            ++c;

        /* extract optional repetition count */
        if (isdigit((unsigned char)*c)) {
            count = 0;

            while (isdigit((unsigned char)*c)) {
                count *= 10;
                count += *c - '0';
                ++c;
            }

            while (isspace((unsigned char)*c))
                ++c;

        } else {
            count = 1;
        }

        cg_assert(*c != 0);

        /* extract field size and byte order */
        switch (*c++) {
            case 'b':
                size  = 1;
                order = CG_ENDIAN_LITTLE;
                break;

            case 'B':
                size  = 1;
                order = CG_ENDIAN_BIG;
                break;

            case 'w':
                size  = 2;
                order = CG_ENDIAN_LITTLE;
                break;
                
            case 'W':
                size  = 2;
                order = CG_ENDIAN_BIG;
                break;

            case 'd':
                size  = 4;
                order = CG_ENDIAN_LITTLE;
                break;

            case 'D':
                size  = 4;
                order = CG_ENDIAN_BIG;
                break;

            case 'q':
                size  = 8;
                order = CG_ENDIAN_LITTLE;
                break;

            case 'Q':
                size  = 8;
                order = CG_ENDIAN_BIG;
                break;

            default:
                CGError_abortFormat(
                    __FILE__, "CGEndian_readf", __LINE__,
                    "invalid format string: %s", format
                );
                return -1;
        }
        
        buffer = va_arg(args, void*);

        if (buffer) {
            items += read = fread(buffer, size, count, stream);

            if ((size > 1) && (order != HOST_ENDIANESS))
                CGEndian_swapArray(buffer, size, read);

            cont = read == count;

        } else {
            items += count;

            cont = fseek(stream, count * size, SEEK_CUR) != -1;
        }
    }

    va_end(args);

    return items;
}
