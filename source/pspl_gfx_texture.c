#include "pspl_graphics.h"
#include "psplib.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lodepng.h"
#include "swizzle_fast.h"

static pspl_texture texturePool[PSPL_TEXTURE_POOL_SIZE];

static const u16 RGBA_5551(u16 r, u16 g, u16 b, u16 a)
{
	r = ((r*31)/255)&31;
	g = ((g*31)/255)&31;
	b = ((b*31)/255)&31;
	a = (a>0?1:0);

	return (u16)((a<<15) | ((b << 10)) | ((g << 5)) | (r));
}

static pspl_texture *_get_texture()
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


void _rgba32_to_rgba16(u16* dest, const u8* data32, int width, int height)
{
	if (dest==NULL||data32==NULL)
	{
		return;
	}

	int data32Length = width * height * 4;
	for (int i = 0; i < data32Length; i += 4)
	{
		u8 r = data32[i + 0];
		u8 g = data32[i + 1];
		u8 b = data32[i + 2];
		u8 a = data32[i + 3];

		dest[i/4] = RGBA_5551(r,g,b,a);
	}	
}

pspl_texture *pspl_gfx_create_texture(unsigned int width, unsigned int height, pspl_pixel_format format)
{
	if (width == 0 || height == 0)
	{
		return NULL;
	}

	pspl_texture *tex = _get_texture();
	if (tex == NULL)
	{
		return NULL;
	}

	memset(tex, 0, sizeof(pspl_texture));
	
	int bpp;
	switch (format)
	{
	case PSPL_PF_8888:
		bpp = 4;
		break;

	case PSPL_PF_4444:
	case PSPL_PF_5551:
	default:
		bpp = 2;
		break;
	}

	tex->format = format;
	tex->imgWidth = width;
	tex->imgHeight = height;
	tex->texWidth = width;
	tex->texHeight = height;
	tex->byteSize = width * height * bpp;
	tex->isSwizzled = false;
	tex->data = malloc(tex->byteSize);

	return tex;
}

pspl_texture *pspl_gfx_load_png_file(const char *file, pspl_pixel_format format)
{
	pspl_log("Loading PNG %s", file);

	// Load PNG file
	unsigned error;
	unsigned char *img = 0;
	unsigned width, height;
	error = lodepng_decode32_file(&img, &width, &height, file);
	if (img == NULL)
	{
		pspl_log("Failed to load texture: %s %u", file, error);
		return NULL;
	}

	pspl_log("Loaded texture: %d %d", width, height);

	// Create texture
	pspl_texture *tex = pspl_gfx_create_texture(width, height, format);
	if (tex == NULL)
	{
		pspl_log("Failed to create %dx%d texture", width, height);
		free(img);
		return NULL;
	}

	tex->isSwizzled = true;
	if (format == PSPL_PF_8888)
	{
		_swizzle_fast((u8*)tex->data, (const u8*)img, width * 4, height);
		tex->isSwizzled = true;
	}
	else
	{
		static u16 tempBuffer[512*512*2]={0};

		_rgba32_to_rgba16(tempBuffer, img, width, height);		
		_swizzle_fast((u8*)tex->data, (const u8*)tempBuffer, width * 2, height);		
	}

	free(img);
	return tex;
}

void pspl_gfx_free_texture(pspl_texture *texture)
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

pspl_texture *pspl_gfx_load_png_buffer(void *fileData, unsigned int size, pspl_pixel_format format)
{
	return NULL;
}