#include "pspl_graphics.h"
#include "psplib.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lodepng.h"
#include "swizzle_fast.h"

static pspl_texture texturePool[PSPL_TEXTURE_POOL_SIZE];

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

	tex->format = format;
	tex->imgWidth = width;
	tex->imgHeight = height;
	tex->texWidth = width;
	tex->texHeight = height;
	tex->byteSize = width * height * 4;
	tex->isSwizzled = false;
	tex->data = malloc(tex->byteSize);

	return tex;
}

pspl_texture* pspl_gfx_load_png_file(const char* file, pspl_pixel_format format)
{
	pspl_log("Loading PNG %s", file);

	unsigned error;
	unsigned char* img = 0;
	unsigned width, height;

	error = lodepng_decode32_file(&img, &width, &height, file);
	if (img == NULL)
	{
		pspl_log("Failed to load texture: %s %u", file, error);
		return NULL;
	}

	pspl_log("Loaded texture: %d %d", width, height);

	pspl_texture* tex = pspl_gfx_create_texture(width, height, format);
	if (tex == NULL)
	{
		pspl_log("Failed to create %dx%d texture", width, height);
		free(img);
		return NULL;
	}

	_swizzle_fast((u8*)tex->data, (const u8*)img, width * 4, height);
	tex->isSwizzled = true;

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

pspl_texture* pspl_gfx_load_png_buffer(void* fileData, unsigned int size, pspl_pixel_format format)
{
	return NULL;
}