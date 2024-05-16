SET(SOURCES screens/ui_ha_alarm_keypad.c
    screens/ui_ha_covers.c
    screens/ui_time.c
    screens/ui_ha_alarm.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_shutter_open_png.c
    images/ui_img_shutter_png.c
    images/ui_img_shield_lock_png.c
    fonts/ui_font_font0.c
    fonts/ui_font_font2.c
    fonts/ui_font_font3.c
    fonts/ui_font_font4.c)

add_library(ui ${SOURCES})
