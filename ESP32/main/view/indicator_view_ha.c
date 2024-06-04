#include "indicator_view_ha.h"
#include "ui.h"

#include "../model/indicator_ha_config.h"
#include "../ui/screens/ui_screen_ha_templates.h"

static const char *TAG = "view-ha";

static lv_obj_t * ui_message_label = NULL;

static void ClearMessage(void){
    
    if (ui_message_label != NULL) {
        lv_obj_del(ui_message_label);
        ui_message_label = NULL;
    }
}

void ui_event_message_label(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        MessageLabelClicked(e);
    }
}

void MessageLabelClicked(lv_event_t * e)
{
	ClearMessage();
}


// TIMER TO CONTROL BEEPS DURING ALARMO STATES
static esp_timer_handle_t __oneshot_timer_handle;
static void oneshot_timer_callback(void *arg);

static void create_and_start_timer(uint8_t seconds)
{
	const esp_timer_create_args_t timer_args = {
		.callback = &oneshot_timer_callback,
		.name = "clear message"};
	ESP_ERROR_CHECK(esp_timer_create(&timer_args, &__oneshot_timer_handle));

	ESP_LOGI(TAG, "-----------------------------------> TIMER START");
	ESP_ERROR_CHECK(esp_timer_start_once(__oneshot_timer_handle, seconds * 1000000)); // 1s
}

static void oneshot_timer_callback(void *arg)
{
	ESP_LOGI(TAG, "CLEAR MESSAGE");
    ClearMessage();
    ESP_ERROR_CHECK(esp_timer_delete(__oneshot_timer_handle));
}


static void SetMessage(char* text, uint8_t seconds){
    
    if (ui_message_label == NULL) {
        ui_message_label = lv_label_create(lv_scr_act());

        lv_obj_set_width(ui_message_label, LV_SIZE_CONTENT);   /// 1
        lv_obj_set_height(ui_message_label, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_align(ui_message_label, LV_ALIGN_CENTER);
        
        lv_obj_add_state(ui_message_label, LV_STATE_FOCUSED);       /// States
        lv_obj_add_flag(ui_message_label, LV_OBJ_FLAG_CLICKABLE);     /// Flags
        lv_obj_clear_flag(ui_message_label,    LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                                        LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                                        LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
        lv_obj_set_style_text_color(ui_message_label, lv_color_hex(0xF6DB36), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_message_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_message_label, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_message_label, lv_color_hex(0x4B4949), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_message_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_message_label, lv_color_hex(0x9E0107), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(ui_message_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(ui_message_label, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(ui_message_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(ui_message_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(ui_message_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(ui_message_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(ui_message_label, ui_event_message_label, LV_EVENT_ALL, NULL);
    }
    lv_label_set_text(ui_message_label, text);

    if (seconds != 0) {
        create_and_start_timer(seconds);
    }
}




static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    lv_port_sem_take();
    switch (id)
    {
    case VIEW_EVENT_HA_MESSAGE:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SCREEN_CHANGE");
        struct view_data_ha_message_data * message_data = (struct view_data_ha_message_data *)event_data;
        //char *p_data = (char *)event_data;
        ESP_LOGI(TAG, "data: %s",message_data->message);
        if (message_data->message[0] == 0) {
            ClearMessage();
        } else if (message_data->message[0] != 0) {
            SetMessage(message_data->message, message_data->seconds);
        }
        break;
    }

    case VIEW_EVENT_HA_SCREEN_CHANGE:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SCREEN_CHANGE");
        char *p_data = (char *)event_data;
        ESP_LOGI(TAG, "data: %s",p_data);
        
        if(strcmp(p_data,"settings")==0){
            _ui_screen_change(ui_screen_setting, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0);
        }
        else if(strcmp(p_data,"alarm_keypad")==0){
            _ui_screen_change(ui_ha_alarm_keypad, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0);
        }
        else if(strcmp(p_data,"home")==0){
            if (screen_count == 0)
            {
                _ui_screen_change(ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
            }
            else{
                _ui_screen_change(screens[0].page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
            }
        }
        
        break;    
    }

    case VIEW_EVENT_HA_SENSOR:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SENSOR");
        struct view_data_ha_sensor_data *p_data = (struct view_data_ha_sensor_data *)event_data;

        lv_label_set_text(all_sensors[p_data->index].data, (int)(p_data->value));

        if (all_sensors[p_data->index].callback != NULL) {
            all_sensors[p_data->index].callback(p_data->value);
        }
        break;
    }
    case VIEW_EVENT_HA_MQTT_CONNECTED:
    {
        for (int i=0; i<all_sensors_count; i++)
        {
            // lv_event_send((lv_obj_t *)all_switches[i].btn, LV_EVENT_CLICKED, NULL);
            struct view_data_ha_sensor_data sensor_data;
            memset(&sensor_data, 0, sizeof(sensor_data));
            sensor_data.index = i;
            strncpy(sensor_data.value, "unknown", sizeof(sensor_data.value) - 1);
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, &sensor_data, sizeof(sensor_data), portMAX_DELAY);
        }
        for (int i = 0; i<all_switches_count; i++) {
            if (all_switches[i].value[0] == 0) {
                // lv_event_send((lv_obj_t *)all_switches[i].btn, LV_EVENT_CLICKED, NULL);

                struct view_data_ha_switch_data switch_data;
                switch_data.index = i;
                sprintf(switch_data.value_str,"unknown");
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST, &switch_data, sizeof(switch_data), portMAX_DELAY);
            }
        }
    }
    break;


    case VIEW_EVENT_HA_SWITCH_SET:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SWITCH_SET");
        struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;

        ESP_LOGI(TAG, "set switch %d: %s - type: %d", (p_data->index), p_data->value_str, all_switches[p_data->index].type);


        switch (all_switches[p_data->index].type)
        {
        case IHAC_SWITCH_TYPE_TOGGLE:
            if ((strcmp(p_data->value_str,"0") != 0) && (strcmp(p_data->value_str,"0") != 1) ) {
                sprintf(p_data->value_str,"0");
            }
            
            if (strcmp(p_data->value_str,"0") == 0) {
                // lv_obj_add_state(all_switches[p_data->index].btn, LV_STATE_CHECKED);
                lv_obj_add_state(all_switches[p_data->index].data, LV_STATE_CHECKED);
            }
            else{
                // lv_obj_clear_state(all_switches[p_data->index].btn, LV_STATE_CHECKED);
                lv_obj_clear_state(all_switches[p_data->index].data, LV_STATE_CHECKED);
            }
            lv_event_send((lv_obj_t *)all_switches[p_data->index].btn, LV_EVENT_CLICKED, NULL);
            break;

        case IHAC_SWITCH_TYPE_SLIDER:
        {
            int value_int = atoi(p_data->value_str);
            lv_slider_set_value(all_switches[p_data->index].data, value_int, true); // todo check value
            lv_event_send((lv_obj_t *)all_switches[p_data->index].data, LV_EVENT_VALUE_CHANGED, NULL);
            break;
        }
        case IHAC_SWITCH_TYPE_PUSHBUTTON:
        {
            ESP_LOGI(TAG, "set switch %d: '%.10s'", (p_data->index), p_data->value_str);

            strncpy(all_switches[p_data->index].value, p_data->value_str,MAX_LENGTH_STATE_STRING); // TODO


            ESP_LOGI(TAG, "Before loop (%s)", all_switches[p_data->index].value);

            // loop for all possible states
            for (int i = 0; i<MAX_STATES; i++) {
                // ESP_LOGI(TAG, "Switch[%d].states[%d].state_value = '%s'", p_data->index,i,all_switches[p_data->index].states[i].state_value);
                if ((all_switches[p_data->index].states[i].state_value[0] != 0) && (all_switches[p_data->index].value[0] != 0) &&
                    (strcmp(all_switches[p_data->index].value,all_switches[p_data->index].states[i].state_value) == 0) ){
                     ESP_LOGI(TAG, "MQTT State found %s\n", all_switches[p_data->index].states[i].state_value);

                     char * icon = all_switches[p_data->index].states[i].state_icon;
                     lv_img_dsc_t *icon_img = get_icon_img(icon);
                     lv_img_set_src(all_switches[p_data->index].img, icon_img);
                } 
            }
            //lv_event_send((lv_obj_t *)all_switches[p_data->index].btn, LV_EVENT_CLICKED, NULL);
        }
            break;
        case IHAC_SWITCH_TYPE_BUTTON:
            if ((strcmp(p_data->value_str,"0") != 0) && (strcmp(p_data->value_str,"1") != 0)) {
                sprintf(p_data->value_str,"0");
            }
            if (strcmp(p_data->value_str,"0") == 0) {
                lv_obj_add_state(all_switches[p_data->index].btn, LV_STATE_CHECKED);
            }
            else
            {
                lv_obj_clear_state(all_switches[p_data->index].btn, LV_STATE_CHECKED);
            }
            lv_event_send((lv_obj_t *)all_switches[p_data->index].btn, LV_EVENT_CLICKED, NULL);
            break;

        case IHAC_SWITCH_TYPE_ARC:
        {
            int value_int = atoi(p_data->value_str);

            lv_arc_set_value(all_switches[p_data->index].data, value_int); // todo check value
            lv_event_send((lv_obj_t *)all_switches[p_data->index].data, LV_EVENT_VALUE_CHANGED, NULL);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    lv_port_sem_give();
}

int indicator_view_ha_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SENSOR,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SCREEN_CHANGE,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_MQTT_CONNECTED,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_MESSAGE,
                                                             __view_event_handler, NULL, NULL));
    
                
}