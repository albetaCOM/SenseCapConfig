SET(SOURCES screens/ui_ha_alarm_keypad.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_font0.c
    fonts/ui_font_font2.c
    fonts/ui_font_font3.c
    fonts/ui_font_font4.c)

add_library(ui ${SOURCES})
