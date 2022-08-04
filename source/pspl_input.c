#include "psplib.h"

#include <math.h>
#include <string.h>
#include <pspctrl.h>

static const pspl_btn keyMap[] =
{
	PSP_CTRL_UP,
	PSP_CTRL_LEFT,
	PSP_CTRL_DOWN,
	PSP_CTRL_RIGHT,
	PSP_CTRL_CROSS,
	PSP_CTRL_CIRCLE,
	PSP_CTRL_SQUARE,
	PSP_CTRL_TRIANGLE,
	PSP_CTRL_START,
	PSP_CTRL_SELECT,
	PSP_CTRL_LTRIGGER,
	PSP_CTRL_RTRIGGER
};

static SceCtrlData state, oldState;
static int holdDuration[PSPL_BTN_MAX];
static int analogDeadzone = PSPL_ANALOG_DEFAULT_DEADZONE;

void pspl_input_init(bool enableAnalogSampling)
{
	sceCtrlSetSamplingMode(enableAnalogSampling ? PSP_CTRL_MODE_ANALOG : PSP_CTRL_MODE_DIGITAL);

	pspl_input_clear();
}

void pspl_input_update()
{
	oldState = state;
	sceCtrlReadBufferPositive(&state, 1);

	int i;
	for (i = 0; i < PSPL_BTN_MAX; ++i)
	{
		if (pspl_btn_down((pspl_btn)i))
		{
			holdDuration[i]++;
		}
		else
		{
			holdDuration[i] = 0;
		}
	}
}

void pspl_input_clear()
{
	memset(&state, 0, sizeof(SceCtrlData));
	memset(&oldState, 0, sizeof(SceCtrlData));
	memset(holdDuration, 0, sizeof(holdDuration));
}

float _normalize_stick(int value)
{
	int shifted = value - 127;
	if (abs(shifted) <= analogDeadzone)
	{
		return 0.0f;
	}

	return (float)(shifted) / 128.0f;
}

void _get_stick_delta(int* dx, int* dy)
{
	*dx = 0;
	*dy = 0;

	int x = state.Lx - oldState.Ly;
	int y = state.Ly - oldState.Ly;

	if (abs(x) > analogDeadzone)
	{
		*dx = state.Lx > oldState.Ly ? 1 : -1;
	}

	if (abs(y) > analogDeadzone)
	{
		*dy = state.Ly > oldState.Ly ? 1 : -1;
	}
}

bool pspl_btn_down(pspl_btn button)
{
	if (button < 0 || button >= PSPL_BTN_MAX)
	{
		return false;
	}

	if (button < PSPL_STICK_START)
	{
		return (state.Buttons & keyMap[button]);
	}

	float ax, ay;
	pspl_analog_get_float(&ax, &ay);

	switch (button)
	{
	case PSPL_STICK_UP:
		return ay < 0.0f;
	case PSPL_STICK_DOWN:
		return ay > 0.0f;

	case PSPL_STICK_LEFT:
		return ax < 0.0f;
	case PSPL_STICK_RIGHT:
		return ax > 0.0f;

	default:
		break;
	}

	return false;
}

bool pspl_btn_pressed(pspl_btn button)
{
	if (button < 0 || button >= PSPL_BTN_MAX)
	{
		return false;
	}

	if (button < PSPL_STICK_START)
	{
		return (state.Buttons & keyMap[button]) && !(oldState.Buttons & keyMap[button]);
	}

	return false;
}

bool pspl_btn_released(pspl_btn button)
{
	if (button < 0 || button >= PSPL_BTN_MAX)
	{
		return false;
	}

	if (button < PSPL_STICK_START)
	{
		return !(state.Buttons & keyMap[button]) && (oldState.Buttons & keyMap[button]);
	}

	return false;
}

int pspl_get_hold_duration(pspl_btn button)
{
	if (button < 0 || button >= PSPL_BTN_MAX)
	{
		return 0;
	}

	return holdDuration[button];
}

void pspl_analog_set_deadzone(int deadzone)
{
	if (deadzone > 0)
	{
		analogDeadzone = deadzone;
	}
}

void pspl_analog_get(int* x, int* y)
{
	if (x == NULL || y == NULL)
	{
		return;
	}

	*x = state.Lx;
	*y = state.Ly;
}

void pspl_analog_get_float(float* x, float* y)
{
	if (x == NULL || y == NULL)
	{
		return;
	}

	*x = _normalize_stick(state.Lx);
	*y = _normalize_stick(state.Ly);
}