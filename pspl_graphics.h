/*
* pspl_graphics.h
*
*
**/

#ifndef PSPLIB_GRAPHICS_H
#define PSPLIB_GRAPHICS_H

#include <stdbool.h>

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

// Screen size
#define PSPL_SCRW (480)
#define PSPL_SCRH (272)

#define PSPL_DISPLAY_LIST_SIZE (262144 * 2)
#define PSPL_AVAIL_VRAM_OFFSET (0x154000) // 0x88000 + 0x88000 + 0x44000
#define PSPL_TEXTURE_SIZE_MAX (512) // Hardware limit of 512x512
#define PSPL_TEXTURE_POOL_SIZE (256)

#define PSPL_RGBA(r,g,b,a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

#ifdef __cplusplus
extern "C" {
#endif

	typedef unsigned int pspl_color;

	typedef enum
	{
		PSPL_PF_8888 = GU_PSM_8888,
		PSPL_PF_FORMAT_4444 = GU_PSM_4444,
		PSPL_PF_FORMAT_5551 = GU_PSM_5551,
	} pspl_pixel_format;

	typedef enum
	{
		PSPL_BLEND_NONE,
		PSPL_BLEND_ALPHA,
		PSPL_BLEND_ADD,
		PSPL_BLEND_MUL,
	} pspl_blend_mode;

	typedef struct pspl_texture
	{
		pspl_pixel_format format;
		unsigned int imgWidth; // actual size, no padding
		unsigned int imgHeight;
		unsigned int texWidth; // po2 size
		unsigned int texHeight; 
		unsigned int byteSize;
		bool isSwizzled;
		void* data;
	} pspl_texture;

	typedef struct pspl_color_vertex
	{
		unsigned int color;
		float x, y, z;

	} pspl_color_vertex;

	typedef struct pspl_texture_vertex
	{
		float u, v;
		unsigned int color;
		float x, y, z;

	} pspl_texture_vertex;

	typedef struct
	{
		int x;
		int y;
		int width;
		int height;
		float pivotX;
		float pivotY;
		float rotation;
		float scaleX;
		float scaleY;
		short texY;
		short texX;
		short texWidth;
		short texHeight;
		pspl_color color;
	} pspl_sprite;

	typedef struct
	{
		float x;
		float y;
		float width;
		float height;
	} pspl_rect;

	/* Initialization */
	bool pspl_gfx_init();
	bool pspl_gfx_quit();

	/* Render states */
	void pspl_gfx_set_blend_mode(pspl_blend_mode blendMode);
	void pspl_gfx_set_texture(pspl_texture* texture); // no mipmap
	void pspl_gfx_set_texture_smooth(bool linearFilter);
	void pspl_gfx_set_texture_wrap(bool wrap);
	void pspl_gfx_set_scissor_rect(int x, int y, int width, int height);

	/* Matrices */
	void pspl_gfx_set_ortho(float x, float y, float width, float height, float zNear, float zFar);
	void pspl_gfx_set_perspective(float fov, float zNear, float zFar);
	void pspl_gfx_reset_ortho();
	void pspl_gfx_set_view2D(float x, float y, float z, float rotZ, float scaleX, float scaleY);
	void pspl_gfx_reset_view();
	void pspl_gfx_reset_model();

	/* Drawing */
	void pspl_gfx_clear(pspl_color color);
	void pspl_gfx_start_drawing(pspl_texture* target); // TODO: render targets
	void pspl_gfx_end_drawing(); // gu finish an sync
	void pspl_gfx_swap_buffers(); // end frame: vsync and swap
	void* pspl_get_current_buffer();
	void* pspl_get_display_list();

	// Draws using GU_TRIANGLE
	void* pspl_gfx_get_memory(int sizeInBytes); // returns a memory region in the display list
	void pspl_gfx_draw_vertex_color(pspl_color_vertex* vertices, unsigned int count);
	void pspl_gfx_draw_vertex_texture(pspl_texture_vertex* vertices, unsigned int count, pspl_texture* texture);
	void pspl_gfx_draw_rect(pspl_rect* rect, pspl_color color);
	void pspl_gfx_draw_rects(pspl_rect* rect, unsigned int count, pspl_color color);
	void pspl_gfx_draw_sprite(pspl_sprite* sprite, pspl_texture* texture);
	void pspl_gfx_draw_sprites(pspl_sprite* sprite, unsigned int count, pspl_texture* texture);	
	void pspl_gfx_sprite_set_texture(pspl_sprite* sprite, pspl_texture* texture);

	/* Simple drawing functions */
	void pspl_gfx_draw_xy(pspl_texture* texture, float x, float y, pspl_color color);
	void pspl_gfx_draw_xy_size(pspl_texture* texture, float x, float y, float width, float height, pspl_color color);

	/* Texture loading */
	pspl_texture* pspl_gfx_create_texture(unsigned int width, unsigned int height, pspl_pixel_format format);
	void pspl_gfx_free_texture(pspl_texture* texture);
	void pspl_gfx_free_all_textures();
	pspl_texture* pspl_gfx_load_png_file(const char* file, pspl_pixel_format format);
	pspl_texture* pspl_gfx_load_png_buffer(void* fileData, unsigned int size, pspl_pixel_format format);
	pspl_texture* pspl_gfx_load_swizzled_file(const char* file, pspl_pixel_format format);
	pspl_texture* pspl_gfx_load_swizzled_buffer(void* fileData, unsigned int size, pspl_pixel_format format);

#ifdef __cplusplus
}
#endif

#endif 
