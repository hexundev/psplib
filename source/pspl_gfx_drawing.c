#include "pspl_graphics.h"

#include <math.h>
#include <string.h>

const int QUAD_VERTEX_COUNT = 6;
const int COLOR_VERTEX_FORMAT = GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D;
const int TEXTURE_VERTEX_FORMAT = GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D;

void* currentDisplayBuffer = (void*)0;

#define obj objects[index]
#define vxRot(_x,_y) ((_x * cosAngle - _y * sinAngle) + obj.x);
#define vyRot(_x,_y) ((_x * sinAngle + _y * cosAngle) + obj.y);
static void _setTextureQuad(pspl_texture_vertex* verts, pspl_sprite* objects, int index, float texWidth, float texHeight)
{
	// UVs
	const float u1 = obj.texX * texWidth;
	const float v1 = obj.texY * texHeight;
	const float u2 = u1 + obj.texWidth * texWidth;
	const float v2 = v1 + obj.texHeight * texHeight;

	// Coordinates
	const float cx = obj.pivotX * obj.width;
	const float cy = obj.pivotY * obj.height;
	const float x1 = -cx * obj.scaleX;
	const float y1 = -cy * obj.scaleY;
	const float x2 = (obj.width - cx) * obj.scaleX;
	const float y2 = (obj.height - cy) * obj.scaleY;

	float cosAngle;
	float sinAngle;
	sincosf(obj.rotation, &sinAngle, &cosAngle);

	/* Tri 1 */

	// 0, 0
	verts[0].color = obj.color;
	verts[0].u = u1;
	verts[0].v = v1;
	verts[0].x = vxRot(x1, y1);
	verts[0].y = vyRot(x1, y1);
	verts[0].z = 0.5f;

	// 1, 0
	verts[1].color = obj.color;
	verts[1].u = u2;
	verts[1].v = v1;
	verts[1].x = vxRot(x2, y1);
	verts[1].y = vyRot(x2, y1);
	verts[1].z = 0.5f;

	// 1, 1
	verts[2].color = obj.color;
	verts[2].u = u2;
	verts[2].v = v2;
	verts[2].x = vxRot(x2, y2);
	verts[2].y = vyRot(x2, y2);
	verts[2].z = 0.5f;

	/* Tri 2 */

	// 0, 0
	verts[3].color = obj.color;
	verts[3].u = u1;
	verts[3].v = v1;
	verts[3].x = vxRot(x1, y1);
	verts[3].y = vyRot(x1, y1);
	verts[3].z = 0.5f;

	// 1,1
	verts[4].color = obj.color;
	verts[4].u = u2;
	verts[4].v = v2;
	verts[4].x = vxRot(x2, y2);
	verts[4].y = vyRot(x2, y2);
	verts[4].z = 0.5f;

	// 0,1
	verts[5].color = obj.color;
	verts[5].u = u1;
	verts[5].v = v2;
	verts[5].x = vxRot(x1, y2);
	verts[5].y = vyRot(x1, y2);
	verts[5].z = 0.5f;
}

void pspl_gfx_clear(pspl_color color)
{
	sceGuClearColor(color);
	sceGuClear(GU_COLOR_BUFFER_BIT);
}

void pspl_gfx_start_drawing(pspl_texture* target)
{
	// todo: render targets
	sceGuStart(GU_DIRECT, pspl_get_display_list());

	pspl_gfx_reset_ortho();
	pspl_gfx_reset_view();
	pspl_gfx_reset_model();

	pspl_gfx_set_texture(NULL);
	pspl_gfx_set_blend_mode(PSPL_BLEND_ALPHA);
}

void pspl_gfx_end_drawing()
{
	sceGuFinish();
	sceGuSync(0, 0);
}

void pspl_gfx_swap_buffers()
{
	sceDisplayWaitVblankStart();
	currentDisplayBuffer = sceGuSwapBuffers();
}

void* pspl_get_current_buffer()
{
	return currentDisplayBuffer;
}

void* pspl_gfx_get_memory(int sizeInBytes)
{
	return sceGuGetMemory(sizeInBytes);
}

void pspl_gfx_draw_vertex_color(pspl_color_vertex* vertices, unsigned int count)
{
	if (vertices == NULL || count == 0)
	{
		return;
	}

	unsigned int size = count * sizeof(pspl_color_vertex);
	void* mem = sceGuGetMemory(size);
	if (mem == NULL)
	{
		return;
	}

	//memcpy(mem, vertices, size);

	pspl_gfx_set_texture(NULL);

	sceGumDrawArray(
		GU_TRIANGLES,
		COLOR_VERTEX_FORMAT,
		count,
		NULL,
		vertices);
}

void pspl_gfx_draw_rect(pspl_rect* rect, pspl_color color)
{

}

void pspl_gfx_draw_rects(pspl_rect* rect, unsigned int count, pspl_color color)
{

}

void pspl_gfx_draw_vertex_texture(pspl_texture_vertex* vertices, unsigned int count, pspl_texture* texture)
{
	if (vertices == NULL || count == 0)
	{
		return;
	}

	pspl_gfx_set_texture(texture);

	sceGumDrawArray(
		GU_TRIANGLES,
		TEXTURE_VERTEX_FORMAT,
		count,
		NULL,
		vertices);
}

void pspl_gfx_draw_sprite(pspl_sprite* sprite, pspl_texture* texture)
{
	pspl_gfx_draw_sprites(sprite, 1, texture);
}

void pspl_gfx_draw_sprites(pspl_sprite* sprites, unsigned int count, pspl_texture* texture)
{
	if (sprites == NULL || count == 0)
	{
		return;
	}

	unsigned int vertexCount = QUAD_VERTEX_COUNT * count;

	pspl_texture_vertex* vertices = sceGuGetMemory(vertexCount * sizeof(pspl_texture_vertex));
	if (vertices == NULL)
	{
		return;
	}

	float texWidth = 0.0f;
	float texHeight = 0.0f;
	if (texture != NULL && texture->texWidth > 0 && texture->texHeight > 0)
	{
		texWidth = 1.0f / (float)texture->texWidth;
		texHeight = 1.0f / (float)texture->texHeight;
	}

	int i;
	if (texture != NULL)
	{
		for (i = 0; i < count; ++i)
		{
			_setTextureQuad(vertices, sprites, i, texWidth, texHeight);
		}

		pspl_gfx_draw_vertex_texture(vertices, vertexCount, texture);
	}
	else
	{

	}
}