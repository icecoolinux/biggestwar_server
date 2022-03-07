#include "compress_image.h"

// structure to store PNG image bytes
struct mem_encode
{
	char *buffer;
	size_t size;
};

void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	// with libpng15 next line causes pointer deference error; use libpng12 
	struct mem_encode* p=(struct mem_encode*)png_get_io_ptr(png_ptr); // was png_ptr->io_ptr 
	size_t nsize = p->size + length;

	// allocate or grow buffer 
	if(p->buffer)
		p->buffer = (char*)realloc(p->buffer, nsize);
	else
		p->buffer = (char*)malloc(nsize);

	if(!p->buffer)
		png_error(png_ptr, "Write Error");

	// copy new bytes to end of buffer 
	memcpy(p->buffer + p->size, data, length);
	p->size += length;
}

// This is optional but included to show how png_set_write_fn() is called 
void my_png_flush(png_structp png_ptr)
{
}

int get_png(int width, int height, unsigned char* row_pointers, char* &png)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_uint_32 bytes_per_row;
	
	
	
    /* Initialize the write struct. */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        return -1;

    /* Initialize the info struct. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        return -1;
    }

    /* Set up error handling. */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }

    /* Set image attributes. */
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

	
	

    
    // static 
	struct mem_encode state;

	// initialise - put this before png_write_png() call 
	state.buffer = NULL;
	state.size = 0;

	// if my_png_flush() is not needed, change the arg to NULL 
	png_set_write_fn(png_ptr, &state, my_png_write_data, my_png_flush);
    

    /* Actually write the image data. */
	//png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_info(png_ptr, info_ptr);
	for(int i=0; i<height; i++)
		png_write_row(png_ptr, &(row_pointers[i*width*3]) );

    //png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	png_write_end(png_ptr, info_ptr);

	

    /* Finish writing. */
    png_destroy_write_struct(&png_ptr, &info_ptr);
	
	png = state.buffer;
    return state.size;
}












/******************** JPEG COMPRESSION SAMPLE INTERFACE *******************/

/* This half of the example shows how to feed data into the JPEG compressor.
 * We present a minimal version that does not worry about refinements such
 * as error recovery (the JPEG code will just exit() if it gets an error).
 */


/*
 * IMAGE DATA FORMATS:
 *
 * The standard input image format is a rectangular array of pixels, with
 * each pixel having the same number of "component" values (color channels).
 * Each pixel row is an array of JSAMPLEs (which typically are unsigned chars).
 * If you are working with color data, then the color values for each pixel
 * must be adjacent in the row; for example, R,G,B,R,G,B,R,G,B,... for 24-bit
 * RGB color.
 *
 * For this example, we'll assume that this data structure matches the way
 * our application has stored the image in memory, so we can just pass a
 * pointer to our image buffer.  In particular, let's say that the image is
 * RGB color and is described by:
 */

// extern JSAMPLE *image_buffer;   /* Points to large array of R,G,B-order data */
// extern int image_height;        /* Number of rows in image */
// extern int image_width;         /* Number of columns in image */


/*
 * Sample routine for JPEG compression.  We assume that the target file name
 * and a compression quality factor are passed in.
 */

GLOBAL(void)
get_jpeg(int width, int height, unsigned char* input, int quality, unsigned char* &output, long unsigned &output_size)
{
	/* This struct contains the JPEG compression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	* It is possible to have several such structures, representing multiple
	* compression/decompression processes, in existence at once.  We refer
	* to any one struct (and its associated working data) as a "JPEG object".
	*/
	struct jpeg_compress_struct cinfo;
	/* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct jpeg_error_mgr jerr;
	/* More stuff */
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
	int row_stride;               /* physical row width in image buffer */

	/* Step 1: allocate and initialize JPEG compression object */

	/* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	jpeg_mem_dest(&cinfo, &output, &output_size);

	/* Step 3: set parameters for compression */

	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = width;      /* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 3;           /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	/* Step 4: Start compressor */

	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
	row_stride = width * 3; /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could pass
		* more than one scanline at a time if that's more convenient.
		*/
		row_pointer[0] = &input[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);

	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);

	/* And we're done! */
}





int compress_image(bool usePNG, int width, int height, unsigned char* row_pointers, char* &output)
{
	if(usePNG)
		return get_png(width, height, row_pointers, output);
	else
	{
		long unsigned size = 0;
		unsigned char* output_uc = NULL;
		get_jpeg(width, height, row_pointers, JPEG_QUALITY, output_uc, size);
		output = (char*)output_uc;
		return size;
	}
}




