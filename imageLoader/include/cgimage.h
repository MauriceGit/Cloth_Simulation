/*********************************************************************
* lescegra - image loader                                            *
*                                                                    *
* http://geeky.kicks-ass.org/projects/lescegra.html                  *
*                                                                    *
* Copyright 2003-2005 by Enno Cramer <uebergeek@web.de>              *
*                                                                    *
* This library is free software; you can redistribute it and/or      *
* modify it under the terms of the GNU Library General Public        *
* License as published by the Free Software Foundation; either       *
* version 2 of the License, or (at your option) any later version.   *
*                                                                    *
* This library is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
* Library General Public License for more details.                   *
*                                                                    *
* You should have received a copy of the GNU Library General Public  *
* License along with this library; if not, write to the Free         *
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. *
*********************************************************************/

#ifndef CG_IMAGE_H
#define CG_IMAGE_H 1

#include <stdio.h>

/**
 * Class: CGImage
 * A bitmap with a variable number of color channels.
 * Every channel has a resolution of 8 bit per pixel.
 */

/**
 * Constants: Image Types
 *   CG_IMAGE_GRAYSCALE       - grayscale bitmap
 *   CG_IMAGE_LUMINANCE       - grayscale bitmap
 *   CG_IMAGE_LUMINANCE_ALPHA - grayscale bitmap with alpha channel
 *   CG_IMAGE_RGB             - color bitmap
 *   CG_IMAGE_COLOR           - color bitmap
 *   CG_IMAGE_RGBA            - color bitmap with alpha channel
 *   CG_IMAGE_COLOR_ALPHA     - color bitmap with alpha channel
 */
#define CG_IMAGE_GRAYSCALE       1
#define CG_IMAGE_RGB             3
#define CG_IMAGE_RGBA            4

#define CG_IMAGE_LUMINANCE       1
#define CG_IMAGE_LUMINANCE_ALPHA 2
#define CG_IMAGE_COLOR           3
#define CG_IMAGE_COLOR_ALPHA     4

typedef struct CGImage {

    /**
     * Field: data
     * Pixel data, tightly packed in row major layout.
     */
    unsigned char* data;

    /**
     * Field: width
     * Bitmap width in pixel.
     */
    unsigned int width;

    /**
     * Field: height
     * Bitmap height in pixel.
     */
    unsigned int height;

    /**
     * Field: bpp
     * Number of color channels.
     */
    unsigned int bpp;
} CGImage;

/**
 * Constructor: CGImage_create
 * Allocate and initialize an image with the given resolution and
 * number of color channels.
 *
 * Parameters:
 *   width  - width of the bitmap
 *   height - height of the bitmap
 *   bpp    - number of channels
 */
CGImage* CGImage_create(
    unsigned int width,
    unsigned int height,
    unsigned int bpp
);

/**
 * Constructor: CGImage_init
 * Constructor for CGImage. Allocate a pixel buffer of the given
 * dimension and resolution.
 *
 * Parameters:
 *   width  - image width in pixel
 *   height - image height in pixel
 *   bpp    - image resolution in byte per pixel (number of color channels)
 */
void CGImage_init(
    CGImage* self,
    unsigned int width,
    unsigned int height,
    unsigned int bpp
);

/**
 * Function: CGImage_mirrorX
 * Mirror the image horizontally.
 */
void CGImage_mirrorX(CGImage* self);

/**
 * Function: CGImage_mirrorY
 * Mirror the image vertically.
 */
void CGImage_mirrorY(CGImage* self);

/**
 * Function: CGImage_copy
 * Copy part of the image.
 *
 * Parameters:
 *   x     - left border of the image region to copy
 *   y     - upper border of the image region to cope
 *   w     - width of the image region to copy
 *   h     - height of the image region to copy
 *   dst   - target image
 *   dst_x - left border of the target image region
 *   dst_y - upper border of the target image region
 */
void CGImage_copy(
    CGImage* self,
    int x, int y,
    unsigned int w, unsigned int h,
    CGImage* dst,
    int dst_x, int dst_y
);

/**
 * Function: CGImage_load
 * Load an image stored in any of the supported image formats
 * (currently PCX and TGA). The apropriate loader is determined from
 * the file contents.
 *
 * Parameters:
 *   filename - name of the image file to load
 *
 * Returns:
 *   bitmap stored in the file referenced by filename
 */
CGImage* CGImage_load(const char* filename);

/**
 * Function: CGImage_loadStream
 * Load an image stored in any of the supported image formats
 * (currently PCX and TGA). The apropriate loader is determined from
 * the file contents.
 *
 * Parameters:
 *   filename - name of the image file to load (only for error reporting)
 *   stream   - bitmap data stream
 *
 * Returns:
 *   bitmap stored in data stream
 */
CGImage* CGImage_loadStream(const char* filename, FILE* stream);

/**
 * Destructor: CGImage_free
 * Free all allocated resources.
 */
void CGImage_free(CGImage* self);

#endif
