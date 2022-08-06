/* 
* pspl_input.h
* 
* Simple functions for input on the PSP.
* 
* - Call pspl_input_init at the start of your program
* - Call pspl_input_update in your loop
* - Call any of the other functions as needed
* 
**/

#ifndef PSPLIB_INPUT_H
#define PSPLIB_INPUT_H

#include <stdbool.h>

#define PSPL_ANALOG_DEFAULT_DEADZONE 5

#ifdef __cplusplus
extern "C" {
#endif

    // Does not include kernel mode buttons
    typedef enum pspl_btn
    {
        PSPL_BTN_UP = 0,
        PSPL_BTN_LEFT,
        PSPL_BTN_DOWN,
        PSPL_BTN_RIGHT,
        PSPL_BTN_CROSS,
        PSPL_BTN_CIRCLE,
        PSPL_BTN_SQUARE,
        PSPL_BTN_TRIANGLE,
        PSPL_BTN_START,
        PSPL_BTN_SELECT,
        PSPL_BTN_L,
        PSPL_BTN_R,
        PSPL_STICK_START,
        PSPL_STICK_UP = PSPL_STICK_START,
        PSPL_STICK_LEFT,
        PSPL_STICK_DOWN,
        PSPL_STICK_RIGHT,
        PSPL_BTN_MAX
    }pspl_btn;

    void pspl_input_init(bool enableAnalogSampling);
    void pspl_input_update();
    void pspl_input_clear();

    bool pspl_btn_down(pspl_btn button);
    bool pspl_btn_pressed(pspl_btn button);
    bool pspl_btn_released(pspl_btn button);
    int  pspl_get_hold_duration(pspl_btn button);

    void pspl_analog_set_deadzone(int deadzone);
    void pspl_analog_get(int* x, int* y); // values: 0-255, neutral is ~127
    void pspl_analog_get_float(float* x, float* y); // values: -1 to 1, neutral is ~0

#ifdef __cplusplus
}
#endif

#endif 
