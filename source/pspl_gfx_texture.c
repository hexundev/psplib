#include "pspl_graphics.h"
#include "pspl_debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pspkernel.h>

#include "lodepng.h"
#include "swizzle_fast.h"

static pspl_texture texturePool[PSPL_TEXTURE_POOL_SIZE];

// Color conversion
typedef u32(*ColConvertFunc)(u16, u16, u16, u16);

static inline u32 RGBA_5551(u16 r, u16 g, u16 b, u16 a)
{
	r = ((r * 31) / 255) & 31;
	g = ((g * 31) / 255) & 31;
	b = ((b * 31) / 255) & 31;
	a = (a > 0 ? 1 : 0);

	return (u32)((a << 15) | ((b << 10)) | ((g << 5)) | (r));
}

static inline u32 RGBA_4444(u16 r, u16 g, u16 b, u16 a)
{
	r = ((r * 15) / 255) & 15;
	g = ((g * 15) / 255) & 15;
	b = ((b * 15) / 255) & 15;
	a = ((a * 15) / 255) & 15;

	return (u32)((a << 12) | ((b << 8)) | ((g << 4)) | (r));
}

static void _convert_color(void* dest, const u8* rgba32Data, u32 size, ColConvertFunc convertFunc)
{
	if (dest == NULL || rgba32Data == NULL || size == 0 || convertFunc == NULL)
	{
		return;
	}

	u16* out = (u16*)dest;
	for (u32 i = 0; i < size; i += 4)
	{
		out[i >> 2] = (u16)(*convertFunc)(
			rgba32Data[i + 0],
			rgba32Data[i + 1],
			rgba32Data[i + 2],
			rgba32Data[i + 3]
			);
	}
}

static pspl_texture* _get_texture()
{
	for (int i = 0; i < PSPL_TEXTURE_POOL_SIZE; ++i)
	{
		if (texturePool[i].data == NULL)
		{
			return &texturePool[i];
		}
	}

	return NULL;
}

pspl_texture* pspl_gfx_create_texture(unsigned int width, unsigned int height, pspl_pixel_format format)
{
	if (width == 0 || height == 0)
	{
		return NULL;
	}

	pspl_texture* tex = _get_texture();
	if (tex == NULL)
	{
		return NULL;
	}

	memset(tex, 0, sizeof(pspl_texture));

	unsigned char bpp;
	switch (format)
	{
	case PSPL_PF_4444:
	case PSPL_PF_5551:
		bpp = 2;
		break;

	case PSPL_PF_8888:
	default:
		bpp = 4;
		break;
	}

	tex->format = format;
	tex->imgWidth = width;
	tex->imgHeight = height;
	tex->texWidth = width;
	tex->texHeight = height;
	tex->bpp = bpp;
	tex->byteSize = width * height * bpp;
	tex->isSwizzled = false;
	tex->data = malloc(tex->byteSize);

	return tex;
}

pspl_texture* pspl_gfx_load_rgba(void* rgbaData, unsigned int width, unsigned int height, pspl_pixel_format format)
{
	if (rgbaData == NULL || width == 0 || height == 0)
	{
		return NULL;
	}

	pspl_texture* tex = pspl_gfx_create_texture(width, height, format);
	if (tex == NULL)
	{
		pspl_log("Failed to create %dx%d texture", width, height);
		return NULL;
	}

	// Always use swizzled textures
	tex->isSwizzled = true;

	// Color conversion
	ColConvertFunc convertFunc;
	switch (format)
	{
	case PSPL_PF_5551:
		convertFunc = &RGBA_5551;
		break;

	case PSPL_PF_4444:
		convertFunc = &RGBA_4444;
		break;

	case PSPL_PF_8888:
	default:
		convertFunc = NULL;
		break;
	}

	if (convertFunc != NULL)
	{
		u16* tempBuffer = (u16*)malloc(width * height * 2);
		u32 imageSize = width * height * 4;

		_convert_color(tempBuffer, rgbaData, imageSize, convertFunc);
		_swizzle_fast((u8*)tex->data, (const u8*)tempBuffer, width * tex->bpp, height);

		free(tempBuffer);
	}
	else
	{
		_swizzle_fast((u8*)tex->data, (const u8*)rgbaData, width * tex->bpp, height);
	}

	sceKernelDcacheWritebackAll();

	return tex;
}

pspl_texture* pspl_gfx_load_png_file(const char* file, pspl_pixel_format format)
{
	pspl_log("Loading PNG %s", file);

	// Load PNG file
	unsigned error;
	unsigned char* img = 0;
	unsigned width, height;
	error = lodepng_decode32_file(&img, &width, &height, file);
	sceKernelDcacheWritebackAll();

	if (img == NULL)
	{
		pspl_log("Failed to decode PNG file: %s %u", file, error);
		return NULL;
	}

	pspl_log("Loaded texture: %dx%d", width, height);

	// Create texture
	pspl_texture* tex = pspl_gfx_load_rgba(img, width, height, format);

	free(img);
	return tex;
}

pspl_texture* pspl_gfx_load_png_buffer(void* fileData, unsigned int size, pspl_pixel_format format)
{
	if (fileData == NULL || size == 0)
	{
		return NULL;
	}

	pspl_log("Loading PNG buffer");

	// Load PNG
	unsigned error;
	unsigned char* img = 0;
	unsigned width, height;
	error = lodepng_decode32(&img, &width, &height, (const unsigned char*)fileData, (size_t)size);
	sceKernelDcacheWritebackAll();

	if (img == NULL)
	{
		pspl_log("Failed to decode PNG buffer: %u", error);
		return NULL;
	}

	pspl_log("Loaded texture: %dx%d", width, height);

	// Create texture
	pspl_texture* tex = pspl_gfx_load_rgba(img, width, height, format);

	free(img);
	return tex;
}

void pspl_gfx_free_texture(pspl_texture* texture)
{
	if (texture == NULL || texture->data == NULL)
	{
		return;
	}

	free(texture->data);
	memset(texture, 0, sizeof(pspl_texture));
}

void pspl_gfx_free_all_textures()
{
	for (int i = 0; i < PSPL_TEXTURE_POOL_SIZE; ++i)
	{
		if (texturePool[i].data != NULL)
		{
			free(texturePool[i].data);
		}
	}

	memset(texturePool, 0, sizeof(texturePool));
}
