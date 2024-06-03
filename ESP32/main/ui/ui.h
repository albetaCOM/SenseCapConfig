// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: sensecap

#ifndef _SENSECAP_UI_H
#define _SENSECAP_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "screens/ui_screen_ha_auto.h"
#include "ui_albetacom.h"
#include "../model/indicator_ha_config.h"


void up_Animation( lv_obj_t *TargetObject, int delay);

void ui_event_screen_setting( lv_event_t * e);
extern lv_obj_t *ui_screen_setting;
void ui_event_wifi__st_button_3( lv_event_t * e);
extern lv_obj_t *ui_wifi__st_button_3;
extern lv_obj_t *ui_wifi_st_3;
extern lv_obj_t *ui_time3;
extern lv_obj_t *ui_setting_icon;
extern lv_obj_t *ui_setting_title;
void ui_event_setting_wifi( lv_event_t * e);
extern lv_obj_t *ui_setting_wifi;
extern lv_obj_t *ui_setting_wifi_icon;
extern lv_obj_t *ui_setting_wifi_title;
void ui_event_setting_display( lv_event_t * e);
extern lv_obj_t *ui_setting_display;
extern lv_obj_t *ui_setting_display_icon;
extern lv_obj_t *ui_setting_display_title;
void ui_event_setting_time( lv_event_t * e);
extern lv_obj_t *ui_setting_time;
extern lv_obj_t *ui_setting_time_icon;
extern lv_obj_t *ui_setting_time_title;
extern lv_obj_t *ui_scrolldots3;
extern lv_obj_t *ui_scrolldots31;
extern lv_obj_t *ui_scrolldots32;
extern lv_obj_t *ui_scrolldots33;
extern lv_obj_t *ui_screen_display;
extern lv_obj_t *ui_wifi_st_4;
void ui_event_back1( lv_event_t * e);
extern lv_obj_t *ui_back1;
extern lv_obj_t *ui_display_title;
extern lv_obj_t *ui_brighness;
extern lv_obj_t *ui_brighness_cfg;
extern lv_obj_t *ui_brighness_title;
extern lv_obj_t *ui_brighness_icon_1;
extern lv_obj_t *ui_brighness_icon_2;
extern lv_obj_t *ui_screen_always_on;
extern lv_obj_t *ui_screen_always_on_title;
void ui_event_screen_always_on_cfg( lv_event_t * e);
extern lv_obj_t *ui_screen_always_on_cfg;
extern lv_obj_t *ui_turn_off_after_time;
extern lv_obj_t *ui_after;
void ui_event_sleep_mode_time_cfg( lv_event_t * e);
extern lv_obj_t *ui_turn_off_after_time_cfg;
extern lv_obj_t *ui_min;
void ui_event_display_keyboard( lv_event_t * e);
extern lv_obj_t *ui_display_keyboard;
extern lv_obj_t *ui_screen_date_time;
extern lv_obj_t *ui_wifi_st_5;
void ui_event_back2( lv_event_t * e);
extern lv_obj_t *ui_back2;
extern lv_obj_t *ui_date_time_title;
extern lv_obj_t *ui_time_format;
extern lv_obj_t *ui_time_format_title;
extern lv_obj_t *ui_time_format_cfg;
extern lv_obj_t *ui_auto_update;
extern lv_obj_t *ui_auto_update_title;
void ui_event_auto_update_cfg( lv_event_t * e);
extern lv_obj_t *ui_auto_update_cfg;
extern lv_obj_t *ui_date_time;
extern lv_obj_t * ui_time_zone;
extern lv_obj_t *  ui_zone_auto_update_cfg;
extern lv_obj_t *ui_time_zone_title;
extern lv_obj_t *ui_time_zone_num_cfg;
extern lv_obj_t *ui_utc_tile;
extern lv_obj_t *ui_time_zone_sign_cfg_;
extern lv_obj_t *ui_daylight_title;
extern lv_obj_t *ui_daylight_cfg;
extern lv_obj_t *ui_manual_setting_title;
extern lv_obj_t *ui_date_cfg;
extern lv_obj_t *ui_hour_cfg;
extern lv_obj_t *ui_min_cfg;
extern lv_obj_t *ui_sec_cfg;
extern lv_obj_t *ui_time_label1;
extern lv_obj_t *ui_time_label2;
extern lv_obj_t *ui_screen_wifi;
extern lv_obj_t *ui_wifi_st_6;
extern lv_obj_t *ui_wifi_title;
extern lv_obj_t * ui_time_save;
void ui_event_back3( lv_event_t * e);
extern lv_obj_t *ui_back3;

extern lv_obj_t *ui_screen_factory;
extern lv_obj_t *ui_factory_resetting_title;

// extern lv_obj_t *ui_screen_sensor_chart;
// extern lv_obj_t *ui_wifi_st_7;
// extern lv_obj_t *ui_back4;
// extern lv_obj_t *ui_sensor_data_title;
// extern lv_obj_t * ui_sensor_chart_day;
// extern lv_chart_series_t * ui_sensor_chart_day_series;

// extern lv_obj_t * ui_sensor_chart_week;
// extern lv_chart_series_t * ui_sensor_chart_week_series_hight;
// extern lv_chart_series_t * ui_sensor_chart_week_series_low;

// // SCREEN: ui_ha_alarm_keypad
// void ui_ha_alarm_keypad_screen_init(void);
// extern lv_obj_t * ui_ha_alarm_keypad;
// extern lv_obj_t * ui_ha_alarm_label;
// void ui_event_Button1(lv_event_t * e);
// extern lv_obj_t * ui_Button1;
// extern lv_obj_t * ui_Label1;
// extern lv_obj_t * ui_Button2;
// extern lv_obj_t * ui_Label2;
// extern lv_obj_t * ui_Button3;
// extern lv_obj_t * ui_Label3;
// extern lv_obj_t * ui_Button4;
// extern lv_obj_t * ui_Label4;
// extern lv_obj_t * ui_Button5;
// extern lv_obj_t * ui_Label5;
// extern lv_obj_t * ui_Button6;
// extern lv_obj_t * ui_Label6;
// extern lv_obj_t * ui_Button7;
// extern lv_obj_t * ui_Label7;
// extern lv_obj_t * ui_Button8;
// extern lv_obj_t * ui_Label8;
// extern lv_obj_t * ui_Button9;
// extern lv_obj_t * ui_Label9;
// extern lv_obj_t * ui_Button10;
// extern lv_obj_t * ui_Label10;
// void ui_event_Button11(lv_event_t * e);
// extern lv_obj_t * ui_Button11;
// extern lv_obj_t * ui_Label11;
// void ui_event_Button12(lv_event_t * e);
// extern lv_obj_t * ui_Button12;
// extern lv_obj_t * ui_Label12;
// extern lv_obj_t * ui____initial_actions0;



LV_IMG_DECLARE( ui_img_wifi_disconet_png);   // assets/wifi_disconet.png
LV_IMG_DECLARE( ui_img_location_png);   // assets/location.png
LV_IMG_DECLARE( ui_img_temp_1_png);   // assets/temp_1.png
LV_IMG_DECLARE( ui_img_humidity_1_png);   // assets/humidity_1.png
LV_IMG_DECLARE( ui_img_co2_png);   // assets/co2.png
LV_IMG_DECLARE( ui_img_tvoc_png);   // assets/tvoc.png
LV_IMG_DECLARE( ui_img_temp_2_png);   // assets/temp_2.png
LV_IMG_DECLARE( ui_img_humidity_2_png);   // assets/humidity_2.png
LV_IMG_DECLARE( ui_img_setting_png);   // assets/setting.png
LV_IMG_DECLARE( ui_img_wifi_setting_png);   // assets/wifi_setting.png
LV_IMG_DECLARE( ui_img_display_png);   // assets/display.png
LV_IMG_DECLARE( ui_img_time_png);   // assets/time.png
LV_IMG_DECLARE( ui_img_back_png);   // assets/back.png
LV_IMG_DECLARE( ui_img_high_light_png);   // assets/high_light.png
LV_IMG_DECLARE( ui_img_low_light_png);   // assets/low_light.png
//LV_IMG_DECLARE( ui_img_background_png);
LV_IMG_DECLARE( ui_img_location2_png);
LV_IMG_DECLARE( ui_img_ic_temp_png);   // assets/ic_temp.png
LV_IMG_DECLARE( ui_img_ic_hum_png);   // assets/ic_hum.png
LV_IMG_DECLARE( ui_img_ic_tvoc_png);   // assets/ic_tVOC.png
LV_IMG_DECLARE( ui_img_ic_co2_png);   // assets/ic_co2.png
LV_IMG_DECLARE( ui_img_ic_switch2_on_png);   // assets/ic_switch2_on.png
LV_IMG_DECLARE( ui_img_ic_switch1_on_png);   // assets/ic_switch1_on.png
LV_IMG_DECLARE( ui_img_ic_switch2_off_png);   // assets/ic_switch2_off.png
LV_IMG_DECLARE( ui_img_ic_switch1_off_png);   // assets/ic_switch1_off.png
LV_IMG_DECLARE( ui_img_round_png);   // assets/ic_switch2_off.png
LV_IMG_DECLARE( ui_img_shutter_open_png);
LV_IMG_DECLARE( ui_img_shutter_png);
LV_IMG_DECLARE( ui_img_alarmarmedaway_png);
LV_IMG_DECLARE( ui_img_lightoff_png);
LV_IMG_DECLARE( ui_img_lighton_png);
LV_IMG_DECLARE( ui_img_coveropen_png);
LV_IMG_DECLARE( ui_img_coverclosed_png);
LV_IMG_DECLARE( ui_img_up_png);
LV_IMG_DECLARE( ui_img_down_png);
LV_IMG_DECLARE( ui_img_ceilingfanoff_png);
LV_IMG_DECLARE( ui_img_ceilingfanon_png);
LV_IMG_DECLARE( ui_img_wifi_3_png);




LV_FONT_DECLARE( ui_font_font0);
LV_FONT_DECLARE( ui_font_font1);
LV_FONT_DECLARE( ui_font_font2);
LV_FONT_DECLARE( ui_font_font3);
LV_FONT_DECLARE( ui_font_font4);


extern lv_obj_t *ui_screen_last;
void creat_scrolldots(lv_obj_t * parent, int screen_index, int totals);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
