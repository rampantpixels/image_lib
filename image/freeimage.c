/* freeimage.c  -  Image library  -  Public Domain  -  2018 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform image loading library in C11 for projects
 * based on our foundation library.
 *
 * The latest source code maintained by Rampant Pixels is always available at
 *
 * https://github.com/rampantpixels/image_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
 *
 */

#include <foundation/foundation.h>

#include "image.h"

#include "ext/FreeImage.h"

void
image_freeimage_initialize(void);

void
image_freeimage_finalize(void);

static object_t _library_freeimage;

typedef void(DLL_CALLCONV* FreeImage_Initialise_Fn)(BOOL);
typedef void(DLL_CALLCONV* FreeImage_DeInitialise_Fn)(void);
typedef FREE_IMAGE_FORMAT(DLL_CALLCONV* FreeImage_GetFileTypeFromHandle_Fn)(FreeImageIO*, fi_handle,
                                                                            int);
typedef FIBITMAP*(DLL_CALLCONV* FreeImage_LoadFromHandle_Fn)(FREE_IMAGE_FORMAT, FreeImageIO*,
                                                             fi_handle, int);
typedef void(DLL_CALLCONV* FreeImage_Unload_Fn)(FIBITMAP*);

typedef unsigned(DLL_CALLCONV* FreeImage_GetWidth_Fn)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetHeight_Fn)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetPitch_Fn)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetBPP_Fn)(FIBITMAP*);
typedef FREE_IMAGE_TYPE(DLL_CALLCONV* FreeImage_GetImageType_Fn)(FIBITMAP*);
typedef FREE_IMAGE_COLOR_TYPE(DLL_CALLCONV* FreeImage_GetColorType_Fn)(FIBITMAP*);
typedef BYTE*(DLL_CALLCONV* FreeImage_GetBits_Fn)(FIBITMAP*);

static FreeImage_Initialise_Fn _FreeImage_Initialise;
static FreeImage_DeInitialise_Fn _FreeImage_DeInitialise;
static FreeImage_GetFileTypeFromHandle_Fn _FreeImage_GetFileTypeFromHandle;
static FreeImage_LoadFromHandle_Fn _FreeImage_LoadFromHandle;
static FreeImage_Unload_Fn _FreeImage_Unload;

static FreeImage_GetWidth_Fn _FreeImage_GetWidth;
static FreeImage_GetHeight_Fn _FreeImage_GetHeight;
static FreeImage_GetPitch_Fn _FreeImage_GetPitch;
static FreeImage_GetBPP_Fn _FreeImage_GetBPP;
static FreeImage_GetImageType_Fn _FreeImage_GetImageType;
static FreeImage_GetColorType_Fn _FreeImage_GetColorType;
static FreeImage_GetBits_Fn _FreeImage_GetBits;

void
image_freeimage_initialize(void) {
	if (!_library_freeimage)
		_library_freeimage = library_load(STRING_CONST("FreeImage"));
	if (!_library_freeimage)
		_library_freeimage = library_load(STRING_CONST("freeimage"));

	if (_library_freeimage) {
		_FreeImage_Initialise = (FreeImage_Initialise_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_Initialise"));
		_FreeImage_DeInitialise = (FreeImage_DeInitialise_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_DeInitialise"));
		_FreeImage_GetFileTypeFromHandle = (FreeImage_GetFileTypeFromHandle_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetFileTypeFromHandle"));
		_FreeImage_LoadFromHandle = (FreeImage_LoadFromHandle_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_LoadFromHandle"));
		_FreeImage_Unload = (FreeImage_Unload_Fn)library_symbol(_library_freeimage,
		                                                        STRING_CONST("FreeImage_Unload"));
		_FreeImage_GetWidth = (FreeImage_GetWidth_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetWidth"));
		_FreeImage_GetHeight = (FreeImage_GetHeight_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetHeight"));
		_FreeImage_GetPitch = (FreeImage_GetPitch_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetPitch"));
		_FreeImage_GetBPP = (FreeImage_GetBPP_Fn)library_symbol(_library_freeimage,
		                                                        STRING_CONST("FreeImage_GetBPP"));
		_FreeImage_GetPitch = (FreeImage_GetPitch_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetPitch"));
		_FreeImage_GetImageType = (FreeImage_GetImageType_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetImageType"));
		_FreeImage_GetColorType = (FreeImage_GetColorType_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetColorType"));
		_FreeImage_GetBits = (FreeImage_GetBits_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetBits"));
	} else {
		_FreeImage_Initialise = 0;
	}

	if (!_FreeImage_Initialise || !_FreeImage_DeInitialise || !_FreeImage_GetFileTypeFromHandle ||
	    !_FreeImage_LoadFromHandle || !_FreeImage_Unload || !_FreeImage_GetWidth ||
	    !_FreeImage_GetHeight || !_FreeImage_GetPitch || !_FreeImage_GetBPP ||
	    !_FreeImage_GetImageType || !_FreeImage_GetColorType || !_FreeImage_GetBits) {
		_FreeImage_Initialise = 0;
		_FreeImage_DeInitialise = 0;
		_FreeImage_GetFileTypeFromHandle = 0;
		_FreeImage_LoadFromHandle = 0;
		_FreeImage_Unload = 0;
		_FreeImage_GetWidth = 0;
		_FreeImage_GetHeight = 0;
		_FreeImage_GetPitch = 0;
		_FreeImage_GetBPP = 0;
		_FreeImage_GetImageType = 0;
		_FreeImage_GetColorType = 0;
		_FreeImage_GetBits = 0;
	}

	if (_FreeImage_Initialise)
		_FreeImage_Initialise(0);
}

void
image_freeimage_finalize(void) {
	if (_FreeImage_DeInitialise)
		_FreeImage_DeInitialise();
	if (_library_freeimage)
		library_release(_library_freeimage);
	_library_freeimage = 0;
}

static unsigned DLL_CALLCONV
image_freeimage_read(void* buffer, unsigned size, unsigned count, fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;
	return (unsigned)(stream_read(stream, buffer, size * count) / size);
}

static unsigned DLL_CALLCONV
image_freeimage_write(void* buffer, unsigned size, unsigned count, fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;
	return (unsigned)(stream_write(stream, buffer, size * count) / size);
}

static int DLL_CALLCONV
image_freeimage_seek(fi_handle handle, long offset, int origin) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;

	stream_seek_mode_t mode = STREAM_SEEK_CURRENT;
	if (origin == SEEK_END)
		mode = STREAM_SEEK_END;
	else if (origin == SEEK_SET)
		mode = STREAM_SEEK_BEGIN;
	stream_seek(stream, offset, mode);

	return 1;
}

static long DLL_CALLCONV
image_freeimage_tell(fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return -1;

	return (long)stream_tell(stream);
}

int
image_freeimage_load(image_t* image, stream_t* stream) {
	if (!_FreeImage_LoadFromHandle)
		return -1;

	FreeImageIO io = {image_freeimage_read, image_freeimage_write, image_freeimage_seek,
	                  image_freeimage_tell};

	FREE_IMAGE_FORMAT fif = _FreeImage_GetFileTypeFromHandle(&io, (fi_handle)stream, 0);
	if (fif == FIF_UNKNOWN)
		return -1;

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	FIBITMAP* bitmap = _FreeImage_LoadFromHandle(fif, &io, (fi_handle)stream, 0);
	if (!bitmap)
		return -1;

	image_pixelformat_t pixelformat;
	memset(&pixelformat, 0, sizeof(pixelformat));

	unsigned int width = _FreeImage_GetWidth(bitmap);
	unsigned int height = _FreeImage_GetHeight(bitmap);
	unsigned int pitch = _FreeImage_GetPitch(bitmap);
	FREE_IMAGE_TYPE image_type = _FreeImage_GetImageType(bitmap);
	FREE_IMAGE_COLOR_TYPE color_type = _FreeImage_GetColorType(bitmap);

	if ((color_type != FIC_RGB) && (color_type != FIC_RGBALPHA) && (color_type != FIC_CMYK)) {
		log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED,
		          STRING_CONST("Unsupported FreeImage color type: %u"), (unsigned int)color_type);
		goto cleanup;
	}

	if (color_type == FIC_CMYK)
		pixelformat.colorspace = IMAGE_COLORSPACE_LINEAR;
	else
		pixelformat.colorspace = IMAGE_COLORSPACE_sRGB;

	pixelformat.compression = IMAGE_COMPRESSION_NONE;
	pixelformat.premultiplied_alpha = false;

	if (image_type == FIT_BITMAP) {
		// unsigned int bpp = _FreeImage_GetBPP(bitmap);
		log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED,
		          STRING_CONST("Unsupported FreeImage image type: %u"), (unsigned int)image_type);
		goto cleanup;
	} else {
		if ((image_type != FIT_RGB16) && (image_type != FIT_RGBA16) && (image_type != FIT_RGBF) &&
		    (image_type != FIT_RGBAF)) {
			log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED,
			          STRING_CONST("Unsupported FreeImage image type: %u"),
			          (unsigned int)image_type);
			goto cleanup;
		}

		image_datatype_t data_type = IMAGE_DATATYPE_UNSIGNED_INT;
		unsigned int bits_per_channel = 0;
		if (color_type == FIT_RGBA16) {
			bits_per_channel = 16;
			pixelformat.num_channels = 4;
		} else if (color_type == FIT_RGBAF) {
			bits_per_channel = 32;
			pixelformat.num_channels = 4;
			data_type = IMAGE_DATATYPE_FLOAT;
		} else if (color_type == FIT_RGB16) {
			bits_per_channel = 16;
			pixelformat.num_channels = 3;
		} else if (color_type == FIT_RGBF) {
			bits_per_channel = 32;
			pixelformat.num_channels = 3;
			data_type = IMAGE_DATATYPE_FLOAT;
		}
		pixelformat.bits_per_pixel = bits_per_channel * pixelformat.num_channels;
		pixelformat.pitch = width * (pixelformat.bits_per_pixel / 8);

		pixelformat.channel[IMAGE_CHANNEL_RED].bits_per_pixel = bits_per_channel;
		pixelformat.channel[IMAGE_CHANNEL_RED].data_type = data_type;
		pixelformat.channel[IMAGE_CHANNEL_RED].offset = 0;
		pixelformat.channel[IMAGE_CHANNEL_GREEN].bits_per_pixel = bits_per_channel;
		pixelformat.channel[IMAGE_CHANNEL_GREEN].data_type = data_type;
		pixelformat.channel[IMAGE_CHANNEL_GREEN].offset = bits_per_channel;
		pixelformat.channel[IMAGE_CHANNEL_BLUE].bits_per_pixel = bits_per_channel;
		pixelformat.channel[IMAGE_CHANNEL_BLUE].data_type = data_type;
		pixelformat.channel[IMAGE_CHANNEL_BLUE].offset = bits_per_channel * 2;
		if (pixelformat.num_channels == 4) {
			pixelformat.channel[IMAGE_CHANNEL_ALPHA].bits_per_pixel = bits_per_channel;
			pixelformat.channel[IMAGE_CHANNEL_ALPHA].data_type = data_type;
			pixelformat.channel[IMAGE_CHANNEL_ALPHA].offset = bits_per_channel * 3;
		}
	}

	image_allocate_storage(image, &pixelformat, width, height, 1, 1);

	if (image_type == FIT_BITMAP) {
	} else {
		if (color_type == FIT_RGB16) {
			const FIRGB16* line = (const FIRGB16*)_FreeImage_GetBits(bitmap);
			const FIRGB16* source = line;
			uint16_t* dest = (uint16_t*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
				}
				source = (const FIRGB16*)pointer_offset(line, pitch);
			}
		} else if (color_type == FIT_RGBA16) {
			const FIRGBA16* line = (const FIRGBA16*)_FreeImage_GetBits(bitmap);
			const FIRGBA16* source = line;
			uint16_t* dest = (uint16_t*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
					*dest++ = source->alpha;
				}
				source = (const FIRGBA16*)pointer_offset(line, pitch);
			}
		} else if (color_type == FIT_RGBF) {
			const FIRGBF* line = (const FIRGBF*)_FreeImage_GetBits(bitmap);
			const FIRGBF* source = line;
			float32_t* dest = (float32_t*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
				}
				source = (const FIRGBF*)pointer_offset(line, pitch);
			}
		} else if (color_type == FIT_RGBAF) {
			const FIRGBAF* line = (const FIRGBAF*)_FreeImage_GetBits(bitmap);
			const FIRGBAF* source = line;
			float32_t* dest = (float32_t*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
					*dest++ = source->alpha;
				}
				source = (const FIRGBAF*)pointer_offset(line, pitch);
			}
		}
	}

cleanup:
	_FreeImage_Unload(bitmap);

	return -1;
}