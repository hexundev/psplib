#include "pspl_graphics.h"

#define DISP_BUFFER_WIDTH (512) // Buffer width must be po2, 480->512

const float PROJECTION_RATIO = 16.0f / 9.0f;

typedef struct
{
	float x;
	float y;
	float fov;
	float width;
	float height;
	float zNear;
	float zFar;
}_pspl_projection;

typedef struct
{
	ScePspFVector3 pos;
	ScePspFVector3 scale;
	ScePspFVector3 rotation;
}_pspl_view;

typedef struct
{
	_pspl_projection proj;
	_pspl_view view;
}_pspl_state;

static bool initialized = false;
static _pspl_state state;
static unsigned int vramOffset;
static unsigned int __attribute__((aligned(16))) displayList[PSPL_DISPLAY_LIST_SIZE];

/* Public functions */

bool pspl_gfx_init(pspl_pixel_format frameBufferFormat)
{
	if (initialized)
	{
		return false;
	}

	sceGuInit();
	sceGuStart(GU_DIRECT, displayList);

	int pixelSize = (frameBufferFormat == PSPL_PF_8888) ? 4 : 2;
	unsigned int dispBufferSize = DISP_BUFFER_WIDTH * PSPL_SCRH * pixelSize;
	unsigned int zBufferSize = DISP_BUFFER_WIDTH * PSPL_SCRH * 2;

	vramOffset = (dispBufferSize * 2) + zBufferSize;

	void* frameBuffer = (void*)(0);
	void* doubleBuffer = (void*)(dispBufferSize);
	void* zBuffer = (void*)(dispBufferSize * 2);

	sceGuDrawBuffer(frameBufferFormat, frameBuffer, DISP_BUFFER_WIDTH);
	sceGuDispBuffer(PSPL_SCRW, PSPL_SCRH, doubleBuffer, DISP_BUFFER_WIDTH);
	sceGuDepthBuffer(zBuffer, DISP_BUFFER_WIDTH);

	sceGuOffset(2048 - (PSPL_SCRW / 2), 2048 - (PSPL_SCRH / 2));
	sceGuViewport(2048, 2048, PSPL_SCRW, PSPL_SCRH);

	sceGuClearDepth(0);
	sceGuDepthRange(65535, 0);
	sceGuColor(0xFFFFFFFF);
	sceGuAmbientColor(0xFFFFFFFF);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuEnable(GU_DITHER);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuTexScale(1.0f, 1.0f);
	sceGuAlphaFunc(GU_GREATER, 20, 0xFF);

	sceGuDepthFunc(GU_GEQUAL);
	sceGuDepthMask(GU_FALSE);

	pspl_gfx_set_texture(NULL);
	pspl_gfx_set_blend_mode(PSPL_BLEND_ALPHA);
	pspl_gfx_set_texture_wrap(false);
	pspl_gfx_set_texture_smooth(true);
	pspl_gfx_set_scissor_rect(0, 0, PSPL_SCRW, PSPL_SCRH);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	initialized = true;
	return true;
}

bool pspl_gfx_quit()
{
	if (!initialized)
	{
		return false;
	}

	sceGuDisplay(GU_FALSE);
	sceGuTerm();

	pspl_gfx_free_all_textures();

	return true;
}

unsigned int pspl_gfx_get_vram_offset()
{
	return vramOffset;
}


void* pspl_get_display_list()
{
	return (void*)displayList;
}


void pspl_gfx_set_blend_mode(pspl_blend_mode blendMode)
{
	switch (blendMode)
	{
	case PSPL_BLEND_NONE:
		sceGuDisable(GU_BLEND);
		break;

	case PSPL_BLEND_ALPHA:
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
		break;

	case PSPL_BLEND_ADD:
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_FIX, 0, 0xFFFFFF);
		break;

	case PSPL_BLEND_MUL:
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_DST_COLOR, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
		break;

	default:
		break;
	}
}

void pspl_gfx_set_texture(pspl_texture* texture)
{
	if (texture == NULL || texture->data == NULL ||
		texture->texWidth == 0 || texture->texHeight == 0)
	{
		sceGuDisable(GU_TEXTURE_2D);
		return;
	}

	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(texture->format, 0, 0, texture->isSwizzled ? 1 : 0);
	sceGuTexImage(0, texture->texWidth, texture->texHeight, texture->texWidth, texture->data);
}

void pspl_gfx_set_texture_smooth(bool linearFilter)
{
	int filter = linearFilter ? GU_LINEAR : GU_NEAREST;
	sceGuTexFilter(filter, filter);
}

void pspl_gfx_set_texture_wrap(bool wrap)
{
	int mode = wrap ? GU_REPEAT : GU_CLAMP;
	sceGuTexWrap(mode, mode);
}

void pspl_gfx_set_scissor_rect(int x, int y, int width, int height)
{
	sceGuScissor(x, y, width, height);
}


void pspl_gfx_set_ortho(float x, float y, float width, float height, float zNear, float zFar)
{
	state.proj.x = x;
	state.proj.y = y;
	state.proj.width = width;
	state.proj.height = height;
	state.proj.zNear = zNear;
	state.proj.zFar = zFar;

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(x, width, height, y, zNear, zFar);

}
void pspl_gfx_set_perspective(float fov, float zNear, float zFar)
{
	state.proj.fov = fov;
	state.proj.zNear = zNear;
	state.proj.zFar = zFar;

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(fov, PROJECTION_RATIO, zNear, zFar);
}

void pspl_gfx_reset_ortho()
{
	pspl_gfx_set_ortho(0.0f, 0.0f, (float)PSPL_SCRW, (float)PSPL_SCRH, -10.0f, 10.0f);
}

void pspl_gfx_set_view2D(float x, float y, float z, float rotZ, float scaleX, float scaleY)
{
	state.view.pos.x = -x;
	state.view.pos.y = -y;
	state.view.pos.z = z;

	state.view.rotation.x = 0.0f;
	state.view.rotation.y = 0.0f;
	state.view.rotation.z = rotZ;

	state.view.scale.x = scaleX;
	state.view.scale.y = scaleY;
	state.view.scale.z = 1.0f;

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumTranslate(&state.view.pos);
	sceGumRotateXYZ(&state.view.rotation);
	sceGumScale(&state.view.scale);
}

void pspl_gfx_reset_view()
{
	pspl_gfx_set_view2D(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

void pspl_gfx_reset_model()
{
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}