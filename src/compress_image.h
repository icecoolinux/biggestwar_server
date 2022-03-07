#ifndef _png_h_
#define _png_h_

#include <png.h>
#include <jpeglib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define JPEG_QUALITY 75

// Use png or jpeg.
// PNG is cpu heavy but reduce space and sending costs.
// JPEG is fast but is a bit bigger in space.
int compress_image(bool usePNG, int width, int height, unsigned char* row_pointers, char* &output);


#endif
