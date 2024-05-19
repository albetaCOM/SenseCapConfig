#include "indicator_view_ha.h"
#include "ui.h"

#include "../model/indicator_ha_config.h"
#include "../ui/screens/ui_screen_ha_templates.h"

static const char *TAG = "view-ha";

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    lv_port_sem_take();
    switch (id)
    {
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

        lv_label_set_text(all_sensors[p_data->index].data, p_data->value);

        if (all_sensors[p_data->index].callback != NULL) {
            all_sensors[p_data->index].callback(p_data->value);
        }
        break;
    }
    case VIEW_EVENT_HA_MQTT_CONNECTED:
    {
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
                ESP_LOGI(TAG, "Switch[%d].states[%d].state_value = '%s'", p_data->index,i,all_switches[p_data->index].states[i].state_value);
                if ((all_switches[p_data->index].states[i].state_value[0] != 0) && (all_switches[p_data->index].value[0] != 0) &&
                    (strcmp(all_switches[p_data->index].value,all_switches[p_data->index].states[i].state_value) == 0) ){
                     ESP_LOGW(TAG, "MQTT State found %s\n", all_switches[p_data->index].states[i].state_value);

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
                                                             
}