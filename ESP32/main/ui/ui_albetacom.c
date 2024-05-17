// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.6
// Project name: albetacom

#include "ui_albetacom.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////

// SCREEN: ui_ha_alarm_keypad
void ui_ha_alarm_keypad_screen_init(void);
void ui_event_ha_alarm_keypad(lv_event_t *e);
lv_obj_t *ui_ha_alarm_keypad;
lv_obj_t *ui_ha_alarm_label;
lv_obj_t *ui_Container1;
void ui_event_Button1(lv_event_t *e);
lv_obj_t *ui_Button1;
lv_obj_t *ui_Label1;
void ui_event_Button2(lv_event_t *e);
lv_obj_t *ui_Button2;
lv_obj_t *ui_Label2;
void ui_event_Button3(lv_event_t *e);
lv_obj_t *ui_Button3;
lv_obj_t *ui_Label3;
void ui_event_Button4(lv_event_t *e);
lv_obj_t *ui_Button4;
lv_obj_t *ui_Label4;
void ui_event_Button5(lv_event_t *e);
lv_obj_t *ui_Button5;
lv_obj_t *ui_Label5;
void ui_event_Button6(lv_event_t *e);
lv_obj_t *ui_Button6;
lv_obj_t *ui_Label6;
void ui_event_Button7(lv_event_t *e);
lv_obj_t *ui_Button7;
lv_obj_t *ui_Label7;
void ui_event_Button8(lv_event_t *e);
lv_obj_t *ui_Button8;
lv_obj_t *ui_Label8;
void ui_event_Button9(lv_event_t *e);
lv_obj_t *ui_Button9;
lv_obj_t *ui_Label9;
void ui_event_Button11(lv_event_t *e);
lv_obj_t *ui_Button11;
lv_obj_t *ui_Label11;
void ui_event_Button10(lv_event_t *e);
lv_obj_t *ui_Button10;
lv_obj_t *ui_Label10;
void ui_event_Button12(lv_event_t *e);
lv_obj_t *ui_Button12;
lv_obj_t *ui_Label12;
lv_obj_t *ui_LabelCode;
lv_obj_t * ui_LabelCode1;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP != 0
#error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_ha_alarm_keypad(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SCREEN_LOADED)
    {
        AlarmCodeScreenLoadedAction(e);
    }
}
void ui_event_Button1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button1ClickedAction(e);
    }
}
void ui_event_Button2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button2ClickedAction(e);
    }
}
void ui_event_Button3(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button3ClickedAction(e);
    }
}
void ui_event_Button4(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button4ClickedAction(e);
    }
}
void ui_event_Button5(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button5ClickedAction(e);
    }
}
void ui_event_Button6(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button6ClickedAction(e);
    }
}
void ui_event_Button7(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button7ClickedAction(e);
    }
}
void ui_event_Button8(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button8ClickedAction(e);
    }
}
void ui_event_Button9(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button9ClickedAction(e);
    }
}
void ui_event_Button11(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ButtonClearClickedAction(e);
    }
}
void ui_event_Button10(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        Button0ClickedAction(e);
    }
}
void ui_event_Button12(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        ButtonSendClickedAction(e);
    }
}

///////////////////// SCREENS ////////////////////

void ui_albetacom_init(void)
{
    ui_ha_alarm_keypad_screen_init();
    sensor_add(ui_LabelCode1, "alarmStatus", StatusCallback);
    switch_add(ui_ha_alarm_keypad, ui_Button1, "button1", IHAC_SWITCH_TYPE_PUSHBUTTON);
}
