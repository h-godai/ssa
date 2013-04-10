#include "stdafx.h"
#include <boost/shared_ptr.hpp>


#ifdef WIN32
#pragma warning (disable : 4996)
#endif

#include "JpegImage.hpp"
#include "../lib/jpeg6bx102/src/jpeglib.h"
#include "Util/Logger.h"



#include "../lib/jpeg6bx102/src/cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "../lib/jpeg6bx102/src/jversion.h"		/* for version message */

#include <ctype.h>		/* to declare isprint() */

namespace ts {

  using boost::shared_ptr;


/*
 * For 12-bit JPEG data, we either downscale the values to 8 bits
 * (to write standard byte-per-sample PPM/PGM files), or output
 * nonstandard word-per-sample PPM/PGM files.  Downscaling is done
 * if PPM_NORAWWORD is defined (this can be done in the Makefile
 * or in jconfig.h).
 * (When the core library supports data precision reduction, a cleaner
 * implementation will be to ask for that instead.)
 */

// #if BITS_IN_JSAMPLE == 8
//#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) (v)


/*
 * When JSAMPLE is the same size as char, we can just fwrite() the
 * decompressed data to the PPM or PGM file.  On PCs, in order to make this
 * work the output buffer must be allocated in near data space, because we are
 * assuming small-data memory model wherein fwrite() can't reach far memory.
 * If you need to process very wide images on a PC, you might have to compile
 * in large-memory model, or else replace fwrite() with a putc() loop ---
 * which will be much slower.
 */


/* Private version of data destination object */

struct JpegOutput {
  jpeg_decompress_struct& cinfo;
  ImageBuffer& output;
  JSAMPROW* buffer;
  int buffer_height;
  //uint8_t* iobuffer;
  //JSAMPROW pixrow;		/* decompressor output buffer */
  size_t buffer_width;		/* width of I/O buffer */
  JDIMENSION samples_per_row;	/* JSAMPLEs per output row */
  JpegOutput(jpeg_decompress_struct& cinfo_, ImageBuffer& output_)
    : cinfo(cinfo_), output(output_) {
  }
  virtual ~JpegOutput() {}


  virtual void put_pixel_rows(JDIMENSION) = 0;
  virtual void start_output(j_decompress_ptr) = 0;

  static shared_ptr<JpegOutput> createJpegOutput(jpeg_decompress_struct& cinfo, ImageBuffer& output);
private:
  void operator = (const JpegOutput&) {} // warning回避のため
};


/*
 * Write some pixel data.
 * In this module rows_supplied will always be 1.
 *
 * put_pixel_rows handles the "normal" 8-bit case where the decompressor
 * output buffer is physically the same as the fwrite buffer.
 */

struct JpegOutputPixelRows : public JpegOutput {
  JpegOutputPixelRows(jpeg_decompress_struct& cinfo_, ImageBuffer& output_)
    : JpegOutput(cinfo_, output_) {
  }
  virtual ~JpegOutputPixelRows() {}
  virtual void put_pixel_rows(JDIMENSION) {
    JSAMPROW ptr;
    JDIMENSION col;

    ptr = this->buffer[0];
    for (col = 0; col < cinfo.output_width; ++col) {
      uint8_t b = static_cast<uint8_t>(GETJSAMPLE(*ptr++));
      uint8_t g = static_cast<uint8_t>(GETJSAMPLE(*ptr++));
      uint8_t r = static_cast<uint8_t>(GETJSAMPLE(*ptr++));
      output.setPixelF(col, cinfo.output_scanline-1, 0xff000000 + (r<<16) + (g<<8) + b);
    }
  }
  virtual void start_output(j_decompress_ptr cinfo) {
  switch (cinfo->out_color_space) {
  case JCS_GRAYSCALE:
    /* emit header for raw PGM format */
//    fprintf(dest->pub.output_file, "P5\n%ld %ld\n%d\n",
//	    (long) cinfo->output_width, (long) cinfo->output_height,
//	    255);
    break;
  case JCS_RGB:
    /* emit header for raw PPM format */
//    fprintf(dest->pub.output_file, "P6\n%ld %ld\n%d\n",
//	    (long) cinfo->output_width, (long) cinfo->output_height,
//	    255);
    break;
  default:
 //   ERREXIT(cinfo, JERR_PPM_COLORSPACE);
    break
      ;
  }
  }
};



/*
 * Write some pixel data when color quantization is in effect.
 * We have to demap the color index values to straight data.
 */
struct JpegOutputDemappedRgb : public JpegOutput {
  JpegOutputDemappedRgb(jpeg_decompress_struct& cinfo_, ImageBuffer& output_)
    : JpegOutput(cinfo_, output_) {
  }
  virtual ~JpegOutputDemappedRgb() {}
  virtual void put_pixel_rows(JDIMENSION) {
    int pixval;
    JSAMPROW ptr;
    JSAMPROW color_map0 = cinfo.colormap[0];
    JSAMPROW color_map1 = cinfo.colormap[1];
    JSAMPROW color_map2 = cinfo.colormap[2];
    JDIMENSION col;

    ptr = this->buffer[0];
    for (col = 0; col < cinfo.output_width; ++col){
      pixval = GETJSAMPLE(*ptr++);
      output.setPixelF(col, cinfo.output_scanline-1,
                      0xff000000 + 
                      (color_map0[pixval] << 16) + 
                      (color_map1[pixval] << 8) +
                      (color_map2[pixval]));
    }
  }
  virtual void start_output(j_decompress_ptr) {
  }
};

struct JpegOutputDemappedGray : public JpegOutput {
  JpegOutputDemappedGray(jpeg_decompress_struct& cinfo_, ImageBuffer& output_)
    : JpegOutput(cinfo_, output_) {
  }
  virtual ~JpegOutputDemappedGray() {}
  virtual void put_pixel_rows(JDIMENSION) {
    int pixval;
    JSAMPROW ptr;
    JSAMPROW color_map = cinfo.colormap[0];
    JDIMENSION col;

    ptr = this->buffer[0];
    for (col = 0; col < cinfo.output_width; ++col) {
      pixval = GETJSAMPLE(*ptr++);
      output.setPixelF(col, cinfo.output_scanline-1,
                      0xff000000 + 
                      (color_map[pixval] << 16) + 
                      (color_map[pixval] << 8) +
                      color_map[pixval]);
    }
  }
  virtual void start_output(j_decompress_ptr) {
  }
};

shared_ptr<JpegOutput> JpegOutput::createJpegOutput(jpeg_decompress_struct& cinfo, ImageBuffer& output) {

  shared_ptr<JpegOutput> dest;

  /* Calculate output image dimensions so we can allocate space */
  jpeg_calc_output_dimensions(&cinfo);


  if (cinfo.quantize_colors) {
    /* When quantizing, we need an output buffer for colormap indexes
     * that's separate from the physical I/O buffer.  We also need a
     * separate buffer if pixel format translation must take place.
     */
    if (cinfo.out_color_space == JCS_GRAYSCALE) {
      dest = shared_ptr<JpegOutput>(new JpegOutputDemappedGray(cinfo, output));
    }
    else {
      dest = shared_ptr<JpegOutput>(new JpegOutputDemappedRgb(cinfo, output));
    }
    dest->samples_per_row = cinfo.output_width * cinfo.out_color_components;
    dest->buffer_width = dest->samples_per_row * sizeof(char);
    dest->buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr) &cinfo, JPOOL_IMAGE,
       cinfo.output_width * cinfo.output_components, (JDIMENSION) 1);
    dest->buffer_height = 1;
  } else {
    /* We will fwrite() directly from decompressor output buffer. */
    /* Synthesize a JSAMPARRAY pointer structure */
    /* Cast here implies near->far pointer conversion on PCs */
    dest = shared_ptr<JpegOutput>(new JpegOutputPixelRows(cinfo, output));
    dest->samples_per_row = cinfo.output_width * cinfo.out_color_components;
    dest->buffer_width = dest->samples_per_row * sizeof(char);
    dest->buffer = (*cinfo.mem->alloc_sarray)
                    ((j_common_ptr) &cinfo, JPOOL_IMAGE,
                     (JDIMENSION)dest->buffer_width, (JDIMENSION) 1);
    dest->buffer_height = 1;
  }
  if (!output.createBuffer(cinfo.output_width, cinfo.output_height)) {
    // バッファ作成失敗
    return shared_ptr<JpegOutput>();
  }

  return dest;
}





/*
 * Case-insensitive matching of possibly-abbreviated keyword switches.
 * keyword is the constant keyword (must be lower case already),
 * minchars is length of minimum legal abbreviation.
 */

GLOBAL(boolean)
keymatch (const char * arg, const char * keyword, int minchars)
{
  register int ca, ck;
  register int nmatched = 0;

  while ((ca = *arg++) != '\0') {
    if ((ck = *keyword++) == '\0')
      return FALSE;		/* arg longer than keyword, no good */
    if (isupper(ca))		/* force arg to lcase (assume ck is already) */
      ca = tolower(ca);
    if (ca != ck)
      return FALSE;		/* no good */
    nmatched++;			/* count matched characters */
  }
  /* reached end of argument; fail if it's too short for unique abbrev */
  if (nmatched < minchars)
    return FALSE;
  return TRUE;			/* A-OK */
}


/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include "../lib/jpeg6bx102/src/cderror.h"
  NULL
};



#if 0
LOCAL(int)
parse_switches (j_decompress_ptr cinfo, int argc, const char **argv,
		int last_file_arg_seen, boolean for_real)

{
  cinfo->err->trace_level = 0;
    } else if (keymatch(arg, "nosimd" , 4)) {
      jpeg_simd_mask((j_common_ptr) cinfo, JSIMD_NONE, JSIMD_ALL);
    } else if (keymatch(arg, "nommx"  , 3)) {
      jpeg_simd_mask((j_common_ptr) cinfo, JSIMD_NONE, JSIMD_MMX);
    } else if (keymatch(arg, "no3dnow", 3)) {
      jpeg_simd_mask((j_common_ptr) cinfo, JSIMD_NONE, JSIMD_3DNOW);
    } else if (keymatch(arg, "nosse"  , 4)) {
      jpeg_simd_mask((j_common_ptr) cinfo, JSIMD_NONE, JSIMD_SSE);
    } else if (keymatch(arg, "nosse2" , 6)) {
      jpeg_simd_mask((j_common_ptr) cinfo, JSIMD_NONE, JSIMD_SSE2);

	cinfo->dct_method = JDCT_ISLOW;
	cinfo->dct_method = JDCT_IFAST;
	cinfo->dct_method = JDCT_FLOAT;
	cinfo->dither_mode = JDITHER_FS;
	cinfo->dither_mode = JDITHER_NONE;
	cinfo->dither_mode = JDITHER_ORDERED;

    // fast
    /* Select recommended processing options for quick-and-dirty output. */
    cinfo->two_pass_quantize = FALSE;
    cinfo->dither_mode = JDITHER_ORDERED;
    if (! cinfo->quantize_colors) /* don't override an earlier -colors */
    cinfo->desired_number_of_colors = 216;
    cinfo->dct_method = JDCT_FASTEST;
    cinfo->do_fancy_upsampling = FALSE;

    /* Force monochrome output. */
    cinfo->out_color_space = JCS_GRAYSCALE;

    /* Suppress fancy upsampling */
    cinfo->do_fancy_upsampling = FALSE;
    /* Use fast one-pass quantization. */
    cinfo->two_pass_quantize = FALSE;
  return argn;			/* return index of next arg (file name) */
}
#endif


/*
 * Marker processor for COM and interesting APPn markers.
 * This replaces the library's built-in processor, which just skips the marker.
 * We want to print out the marker as text, to the extent possible.
 * Note this code relies on a non-suspending data source.
 */

LOCAL(unsigned int)
jpeg_getc (j_decompress_ptr cinfo)
/* Read next byte */
{
  struct jpeg_source_mgr * datasrc = cinfo->src;

  if (datasrc->bytes_in_buffer == 0) {
    if (! (*datasrc->fill_input_buffer) (cinfo))
//      ERREXIT(cinfo, JERR_CANT_SUSPEND);
      throw std::exception();

  }
  datasrc->bytes_in_buffer--;
  return GETJOCTET(*datasrc->next_input_byte++);
}


METHODDEF(boolean)
print_text_marker (j_decompress_ptr cinfo)
{
  boolean traceit = (cinfo->err->trace_level >= 1);
  INT32 length;
  unsigned int ch;
  unsigned int lastch = 0;

  length = jpeg_getc(cinfo) << 8;
  length += jpeg_getc(cinfo);
  length -= 2;			/* discount the length word itself */

  if (traceit) {
    if (cinfo->unread_marker == JPEG_COM)
      DEBUGLOG( "Comment, length %ld:\n", (long) length);
    else			/* assume it is an APPn otherwise */
      DEBUGLOG( "APP%d, length %ld:\n",
	      cinfo->unread_marker - JPEG_APP0, (long) length);
  }

  while (--length >= 0) {
    ch = jpeg_getc(cinfo);
    if (traceit) {
      /* Emit the character in a readable form.
       * Nonprintables are converted to \nnn form,
       * while \ is converted to \\.
       * Newlines in CR, CR/LF, or LF form will be printed as one newline.
       */
      if (ch == '\r') {
	DEBUGLOG( "\n");
      } else if (ch == '\n') {
	if (lastch != '\r')
	  DEBUGLOG( "\n");
      } else if (ch == '\\') {
	DEBUGLOG( "\\\\");
      } else if (isprint(ch)) {
	putc(ch, stderr);
      } else {
	DEBUGLOG( "\\%03o", ch);
      }
      lastch = ch;
    }
  }

  if (traceit)
    DEBUGLOG( "\n");

  return TRUE;
}

// jpegファイルを読み込んでImageBufferに書き込む
bool JpegImage::deCompress2(const string& fileName, ImageBuffer& output) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  shared_ptr<JpegOutput> dest_mgr;
  FILE * input_file;
  JDIMENSION num_scanlines;

  /* Initialize the JPEG decompression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Insert custom marker processor for COM and APP12.
   * APP12 is used by some digital camera makers for textual info,
   * so we provide the ability to display it as text.
   * If you like, additional APPn marker types can be selected for display,
   * but don't try to override APP0 or APP14 this way (see libjpeg.doc).
   */
  jpeg_set_marker_processor(&cinfo, JPEG_COM, print_text_marker);
  jpeg_set_marker_processor(&cinfo, JPEG_APP0+12, print_text_marker);


  cinfo.err->trace_level = 0;

  /* Open the input file. */
  if ((input_file = fopen(fileName.c_str(), READ_BINARY)) == NULL) {
    DEBUGLOG( "can't open %s\n", fileName.c_str()) ;
    //exit(EXIT_FAILURE);
    jpeg_destroy_decompress(&cinfo);
    return false;
  }

  /* Specify data source for decompression */
  jpeg_stdio_src(&cinfo, input_file);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* Initialize the output module now to let it override any crucial
   * option settings (for instance, GIF wants to force color quantization).
   */
  dest_mgr = JpegOutput::createJpegOutput(cinfo, output);
  if (dest_mgr.get() == 0) {
    // 生成失敗
    return false;
  }

  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);

  /* Write output file header */
  dest_mgr->start_output(&cinfo);

  /* Process data */
  while (cinfo.output_scanline < cinfo.output_height) {
    num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
					dest_mgr->buffer_height);
    dest_mgr->put_pixel_rows(num_scanlines);
  }

  /* Finish decompression and release memory.
   * I must do it in this order because output module has allocated memory
   * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
   */
//  (*dest_mgr->finish_output) (&cinfo, dest_mgr);
  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  /* Close files, if we opened them */
  if (input_file != stdin)
    fclose(input_file);

  /* All done. */
//  exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  return true;			/* suppress no-return-value warnings */
}

  JpegImage::JpegImage() {
  }
  JpegImage::~JpegImage() {
  }

  ImageBuffer::ptr JpegImage::deCompress(const string& fileName, ImageBuffer::PixelFormat fmt /*= ImageBuffer::A8R8G8B8*/) {
    ImageBuffer::ptr img(ImageBuffer::CreateImageBuffer(fmt, 0,0));
    if (deCompress2(fileName, *img)) {
      return img;
    }
    return ImageBuffer::ptr();
  }


  // ファイルに書き込む
  bool JpegImage::compress(const string& /*fileName*/, const ImageBuffer::ptr& /*input*/) {
    return false;
  }


  

}

