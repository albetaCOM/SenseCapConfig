#include "indicator_ha.h"
#include "cJSON.h"
#include "indicator_ha_config.h"
#include "nvs.h"
#include "ui/screens/ui_screen_ha_auto.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "ha_config.h"
#include "mqtt_client.h"

#define HA_CFG_STORAGE "ha-cfg"
#define MAX_LENGTH_SCREEN_TARGET 50
#define MAX_LENGTH_MESSAGE_TEXT 100

static const char *TAG = "HA";
#define DEBUG_HA 0

static bool net_flag = false;

typedef struct ha_sensor_entity
{
    int index;
    char *key;
    char *topic;
    int qos;
} ha_sensor_entity_t;

typedef struct ha_switch_entity
{
    int index;
    char *key;
    char *topic_set;
    char *topic_state;
    int qos;
} ha_switch_entity_t;

typedef struct
{
    char *topic_event;
    int qos;
} ha_alarm_entity_t;

// ui  {"index": 1, vaule: "27.2"}  <==MQTT==  topic:/xxx/state {"key": "27.2"}  HA
// ha_sensor_entity_t ha_sensor_entites[CONFIG_HA_SENSOR_ENTITY_NUM];
ha_sensor_entity_t *ha_sensor_entites;
int ha_sensor_entites_num;

// ui  {"index": 1, vaule: 1}  <==MQTT==  topic:/xxx/set    {"key": 1}  HA
// ui  {"index": 1, vaule: 1}  ==MQTT==>  topic:/xxx/state  {"key": 1}  HA
// ha_switch_entity_t ha_switch_entites[CONFIG_HA_SWITCH_ENTITY_NUM];
ha_switch_entity_t *ha_switch_entites;
int ha_switch_entites_num;
char *switch_state;

static ha_alarm_entity_t ha_alarm_entity;

static esp_mqtt_client_handle_t mqtt_client;
static bool mqtt_connected_flag = false;

/**
 * The function `ha_entites_init` initializes sensor and switch entities with their respective keys,
 * topics, and quality of service.
 */
static void ha_entites_init(void)
{

    ha_sensor_entites = malloc(sizeof(ha_sensor_entity_t) * all_sensors_count);
    ha_sensor_entites_num = all_sensors_count;

    // loop in all_sensors
    for (int i = 0; i < all_sensors_count; i++)
    {
        ha_sensor_entites[i].index = i;
        ha_sensor_entites[i].key = all_sensors[i].ha_key;
        if (all_sensors[i].ha_topic != NULL) {
            // printf("ha_topic = '%s'\n", all_sensors[i].ha_topic);
            printf("i = %i\n", i);
            printf("ha_key = '%s'\n", all_sensors[i].ha_key);
            printf("ha_topic = not null\n");
            ha_sensor_entites[i].topic = all_sensors[i].ha_topic;
        } else {
            ha_sensor_entites[i].topic = CONFIG_TOPIC_SENSOR_DATA;
        }
        ha_sensor_entites[i].qos = CONFIG_TOPIC_SENSOR_DATA_QOS;
    }

    ha_switch_entites = malloc(sizeof(ha_switch_entity_t) * all_switches_count);
    ha_switch_entites_num = all_switches_count;
    switch_state = malloc(MAX_LENGTH_STATE_STRING * all_switches_count);

    // loop in all_switches
    for (int i = 0; i < all_switches_count; i++)
    {
        ha_switch_entites[i].index = i;
        ha_switch_entites[i].key = all_switches[i].ha_key;
        ha_switch_entites[i].topic_set = CONFIG_TOPIC_SWITCH_SET;
        ha_switch_entites[i].topic_state = CONFIG_TOPIC_SWITCH_STATE;
        ha_switch_entites[i].qos = CONFIG_TOPIC_SWITCH_QOS;
    }

    // Alarm entity
    ha_alarm_entity.topic_event = CONFIG_TOPIC_ALARMO_EVENT;
}

/**
 * The function `ha_ctrl_cfg_restore` restores the configuration settings for a switch state from
 * storage, and if the settings are not found or there is an error, it initializes the switch state to
 * zero.
 */
static void ha_ctrl_cfg_restore(void)
{
    esp_err_t ret = 0;
    size_t len = sizeof(switch_state);
    ret = indicator_storage_read(HA_CFG_STORAGE, (void *)switch_state, &len);

    if (ret == ESP_OK && len == (sizeof(switch_state)))
    {
#ifdef DEBUG_HA
        ESP_LOGI(TAG, "cfg read successful");
#endif
    }
    else
    {
        // err or not find
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
#if DEBUG_HA
            ESP_LOGI(TAG, "cfg not find");
#endif
        }
        else
        {
#if DEBUG_HA
            ESP_LOGI(TAG, "cfg read err:%d", ret);
#endif
        }
        memset(switch_state, 0, sizeof(switch_state));
    }
}

/**
 * The function `ha_ctrl_cfg_save` saves the `switch_state` variable to a storage location called
 * `HA_CFG_STORAGE`.
 */
static void ha_ctrl_cfg_save(void)
{
    esp_err_t ret = 0;
    ret = indicator_storage_write(HA_CFG_STORAGE, (void *)switch_state, sizeof(switch_state));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "cfg write err:%d", ret);
    }
    else
    {
#if DEBUG_HA
        ESP_LOGI(TAG, "cfg write successful");
#endif
    }
}

/**
 * The function logs an error message if the error code is non-zero.
 *
 * @param message A string that describes the error or provides additional information about it.
 * @param error_code The error code that needs to be checked.
 */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/**
 * The function `mqtt_msg_handler` parses a JSON message received over MQTT and extracts data for
 * sensor and switch entities, then posts events with the extracted data.
 *
 * @param p_topic The p_topic parameter is a pointer to a character array that represents the MQTT
 * topic of the received message.
 * @param topic_len The length of the topic string (p_topic).
 * @param p_data The parameter `p_data` is a pointer to the data received in the MQTT message. It is a
 * string containing the payload of the message.
 * @param data_len The parameter `data_len` represents the length of the data received in the MQTT
 * message.
 *
 * @return an integer value.
 */

static int mqtt_msg_handler(const char *p_topic, int topic_len, const char *p_data, int data_len)
{
    cJSON *root = NULL;
    cJSON *cjson_item = NULL;

    struct view_data_ha_sensor_data sensor_data;
    struct view_data_ha_switch_data switch_data;

    memset(&sensor_data, 0, sizeof(sensor_data));
    memset(&switch_data, 0, sizeof(switch_data));

    root = cJSON_ParseWithLength(p_data, data_len);
    if (root == NULL)
    {
        // Analyze the topics that contains data without JSON format

        for (int i = 0; i < ha_sensor_entites_num; i++)
        {
            if (strncmp(ha_sensor_entites[i].key, p_topic, topic_len) == 0)
            {
#if DEBUG_HA
                ESP_LOGI(TAG, "mqtt_msg_handler found item :%s", ha_sensor_entites[i].key);
#endif
                sensor_data.index = i;
                strncpy(sensor_data.value, p_data, data_len);
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SENSOR, &sensor_data, sizeof(sensor_data), portMAX_DELAY);
            }
        }
        for (int i = 0; i < ha_switch_entites_num; i++)
        {
            if (strncmp(ha_switch_entites[i].key, p_topic, topic_len) == 0)
            {
                // ESP_LOGI(TAG, "[mqtt_msg_handler] KEY: %s - topic: %s - ptopic: %.*s", ha_switch_entites[i].key, ha_switch_entites->topic_set, topic_len, p_topic);
                switch_data.index = i;
                strncpy(switch_data.value_str, p_data, data_len);
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, &switch_data, sizeof(switch_data), portMAX_DELAY);
            }
        }
    }
    else
    {
        // ALARM EVENTS
        if (strncmp(ha_alarm_entity.topic_event, p_topic, topic_len) == 0)
        {
            cjson_item = cJSON_GetObjectItem(root, "event");
            if (cjson_item != NULL && cjson_item->valuestring != NULL)
            {
                if (strcmp(cjson_item->valuestring, "FAILED_TO_ARM") == 0)
                {
                    // Obtener el array de sensores
                    cJSON *sensors = cJSON_GetObjectItem(root, "sensors");
                    if (!cJSON_IsArray(sensors))
                    {
                        ESP_LOGW(TAG, "No se encontró el array de sensores\n");
                        cJSON_Delete(root);
                        return NULL;
                    }
                    else
                    {
                        struct view_data_ha_message_data message_data;
                        message_data.seconds = 10;
                        strcpy(message_data.message, "No es pot activar l'alarma:");

                        // Iterar sobre los sensores y calcular la longitud total
                        cJSON *sensor = NULL;
                        cJSON_ArrayForEach(sensor, sensors)
                        {
                            cJSON *name = cJSON_GetObjectItem(sensor, "name");
                            if (cJSON_IsString(name))
                            {
                                strcat(message_data.message, "\n -");
                                strcat(message_data.message, name->valuestring);
                            }
                        }
                        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_MESSAGE, &message_data, sizeof(message_data), portMAX_DELAY);
                    }
                } else if (strcmp(cjson_item->valuestring, "INVALID_CODE_PROVIDED") == 0)
                {
                    struct view_data_ha_message_data message_data;
                    message_data.seconds = 3;
                    strcpy(message_data.message, "Codi incorrecte");
                    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_MESSAGE, &message_data, sizeof(message_data), portMAX_DELAY);
                } 
            }
        }

#if DEBUG_HA
        ESP_LOGI(TAG, "mqtt_msg_handler topic:%s", p_topic);

        // ESP_LOGI(TAG, "mqtt_msg_handler data:%s", p_data);

        ESP_LOGI(TAG, "mqtt_msg_handler number of sensor entites:%d", ha_sensor_entites_num);
#endif
        // SENSORS
        for (int i = 0; i < ha_sensor_entites_num; i++)
        {
            cjson_item = cJSON_GetObjectItem(root, ha_sensor_entites[i].key);
//#if DEBUG_HA
            // ESP_LOGI(TAG, "SENSOR mqtt_msg_handler check key :%s", ha_sensor_entites[i].key);
//#endif
            if (cjson_item != NULL && cjson_item->valuestring != NULL && 0 == strncmp(p_topic, ha_sensor_entites[i].topic, topic_len))
            {
//#if DEBUG_HA
                ESP_LOGI(TAG, "mqtt_msg_handler found item :%s", ha_sensor_entites[i].key);
//#endif
                sensor_data.index = i;
                printf("int = %d, string = %s",cjson_item->valueint,cjson_item->valuestring);
                int num = atoi(cjson_item->valuestring);
                printf("atoi value = %d",num);
                if (num > 99 || num < (-99)) {
                    itoa(num, sensor_data.value, 10);
                } else {
                    strncpy(sensor_data.value, cjson_item->valuestring, sizeof(sensor_data.value) - 1);
                }
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SENSOR, &sensor_data, sizeof(sensor_data), portMAX_DELAY);
                // return 0;
            }
        }
        //SWITCHS
        for (int i = 0; i < ha_switch_entites_num; i++)
        {
            cjson_item = cJSON_GetObjectItem(root, ha_switch_entites[i].key);

//#if DEBUG_HA
            // ESP_LOGI(TAG, "SWITCH mqtt_msg_handler check key :%s", ha_switch_entites[i].key);
//#endif

            if (cjson_item != NULL && 0 == strncmp(p_topic, ha_switch_entites[i].topic_set, topic_len))
            {
                ESP_LOGI(TAG, "[mqtt_msg_handler] KEY: %s - topic: %s - ptopic: %.*s", ha_switch_entites[i].key, ha_switch_entites->topic_set, topic_len, p_topic);
                switch_data.index = i;
                // switch_data.value = cjson_item->valueint;
                // printf("valueint :%d", cjson_item->valueint);
                if (cjson_item->valuestring != NULL)
                {
                    strcpy(switch_data.value_str, cjson_item->valuestring);
                    ESP_LOGI(TAG, "valuestring: %s", cjson_item->valuestring);
                }
                else
                    ESP_LOGW(TAG, "No valuestring");

                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, &switch_data, sizeof(switch_data), portMAX_DELAY);
                // return 0;
            }
        }

        // Added MQTT topic for changinf the screen from home assistant
        if (0 == strncmp(p_topic, "indicator/message", topic_len))
        {
            struct view_data_ha_message_data message_data;
            memset(&message_data, 0, sizeof(message_data));
            cjson_item = cJSON_GetObjectItem(root, "text");
            if (cjson_item != NULL)
            {
                strcpy(message_data.message, cjson_item->valuestring);
            }
            cjson_item = cJSON_GetObjectItem(root, "seconds");
            if (cjson_item != NULL)
            {
                message_data.seconds =cjson_item->valueint;
            }
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_MESSAGE, &message_data, sizeof(message_data), portMAX_DELAY);
            
        }

        // Added MQTT topic for changinf the screen from home assistant
        if (0 == strncmp(p_topic, "indicator/screen", topic_len))
        {
            cjson_item = cJSON_GetObjectItem(root, "target");
            if (cjson_item != NULL)
            {
                ESP_LOGI(TAG, "CAMBIO DE PANTALLA: %s", cjson_item->valuestring);
                ESP_LOGI(TAG, "String size: %d", strnlen(cjson_item->valuestring, MAX_LENGTH_SCREEN_TARGET));

                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SCREEN_CHANGE, cjson_item->valuestring, strnlen(cjson_item->valuestring, MAX_LENGTH_SCREEN_TARGET) + 1, portMAX_DELAY);
            }
        }

        // Added MQTT topic for BEEEEP
        if (0 == strncmp(p_topic, "indicator/beep", topic_len))
        {

            int beep_info[3] = {1, 50, 0};
            cjson_item = cJSON_GetObjectItem(root, "beep_repetitions");
            if (cjson_item != NULL)
            {
                beep_info[0] = cjson_item->valueint;
                ESP_LOGI(TAG, "MQTT -> BEEP repetitions = %d", beep_info[0]);
            }

            cjson_item = cJSON_GetObjectItem(root, "beep_duration");
            if (cjson_item != NULL)
            {
                beep_info[1] = cjson_item->valueint;
                ESP_LOGI(TAG, "MQTT -> BEEP duration = %d", beep_info[1]);
            }

            cjson_item = cJSON_GetObjectItem(root, "beep_repetition_ms");
            if (cjson_item != NULL)
            {
                beep_info[2] = cjson_item->valueint;
                ESP_LOGI(TAG, "MQTT -> BEEP beep_repetition_ms = %d", beep_info[2]);
            }
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_BEEP, &beep_info[0], sizeof(beep_info), portMAX_DELAY);
        }

    prase_end:
        cJSON_Delete(root);
    }
}

/**
 * The function `mqtt_event_handler` handles different MQTT events such as connection, disconnection,
 * subscription, publication, receiving data, and error events.
 *
 * @param handler_args The `handler_args` parameter is a pointer to any additional arguments that you
 * want to pass to the MQTT event handler function. It can be used to provide context or additional
 * data to the event handler. In the given code snippet, it is not used and is set to `NULL`.
 * @param base The base parameter is the event base associated with the event. It represents the
 * category or type of event that occurred. In this case, it is used to identify that the event is
 * related to MQTT (MQTT_EVENT_BASE).
 * @param event_id The event_id parameter represents the type of MQTT event that occurred. It is of
 * type int32_t and is used in a switch statement to handle different types of events.
 * @param event_data The `event_data` parameter is a pointer to the event data structure associated
 * with the MQTT event. In this case, it is of type `esp_mqtt_event_handle_t`, which is a pointer to a
 * structure containing information about the MQTT event.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:

#if DEBUG_HA
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
#endif
        mqtt_connected_flag = true;

        for (int i; i < ha_sensor_entites_num; i++)
        {
            msg_id = esp_mqtt_client_subscribe(client, ha_sensor_entites[i].topic, ha_sensor_entites[i].qos);
#if DEBUG_HA
            ESP_LOGI(TAG, "subscribe:%s, msg_id=%d", ha_sensor_entites[i].topic, msg_id);
#endif
        }

        for (int i; i < ha_switch_entites_num; i++)
        {
            msg_id = esp_mqtt_client_subscribe(client, ha_switch_entites[i].topic_set, ha_switch_entites[i].qos);
            ESP_LOGI(TAG, "subscribe:%s, msg_id=%d", ha_switch_entites[i].topic_set, msg_id);
#if DEBUG_HA
            ESP_LOGI(TAG, "subscribe:%s, msg_id=%d", ha_switch_entites[i].topic_set, msg_id);
#endif
        }

        // New topic subscription for changing the screens from HA (JCL + Alba)
        msg_id = esp_mqtt_client_subscribe(client, "indicator/screen", 0);
        msg_id = esp_mqtt_client_subscribe(client, "indicator/beep", 0);
        msg_id = esp_mqtt_client_subscribe(client, "indicator/message", 0);

        // Subscription to ALARMO topics
        //msg_id = esp_mqtt_client_subscribe(client, CONFIG_TOPIC_ALARMO_STATE, 0);
        msg_id = esp_mqtt_client_subscribe(client, ha_alarm_entity.topic_event, 0);

        //  restore switch state for UI and HA.
        /*struct view_data_ha_switch_data switch_data;
        for (int i; i < ha_switch_entites_num; i++)
        {
            switch_data.index = i;
            //switch_data.value = switch_state[i];
            sprintf(switch_data.value_str,"%s",switch_state[i*MAX_LENGTH_STATE_STRING]);
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, &switch_data, sizeof(switch_data), portMAX_DELAY);
        }*/

        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_MQTT_CONNECTED, NULL, 0, portMAX_DELAY);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_connected_flag = false;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
#if DEBUG_HA
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
#endif
        break;
    case MQTT_EVENT_PUBLISHED:
#if DEBUG_HA
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
#endif
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // printf("DATA=%.*s\r\n", event->data_len, event->data);
        mqtt_msg_handler(event->topic, event->topic_len, event->data, event->data_len);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * The function `mqtt_start` initializes and starts an MQTT client with the provided configuration.
 *
 * @return In this code, the function `mqtt_start` is returning `void`, which means it is not returning
 * any value.
 */
static void mqtt_start(void)
{
    static bool init_flag = false;
    if (init_flag)
    {
        return;
    }
    init_flag = true;
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = __g_ha_config.url,                       // Your HA broker url
        .credentials.username = __g_ha_config.username,                // Your HA username
        .credentials.authentication.password = __g_ha_config.password, // Your HA password
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

/**
 * The function is an event handler that handles different types of events and performs specific
 * actions based on the event type.
 *
 * @param handler_args The `handler_args` parameter is a pointer to any additional arguments that you
 * want to pass to the event handler function. It is typically used to pass context or configuration
 * data to the event handler. In this case, it is not used and is set to `NULL`.
 * @param base The `base` parameter is the event base associated with the event. It is used to identify
 * the event source or category.
 * @param id The `id` parameter is an integer that represents the type of event that occurred. It is
 * used in the switch statement to determine the specific event that occurred and execute the
 * corresponding code block.
 * @param event_data The `event_data` parameter is a pointer to the data associated with the event. The
 * type of data depends on the event ID. In the code snippet provided, there are three different event
 * IDs:
 */
static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id)
    {
    case VIEW_EVENT_WIFI_ST:
    {
#if DEBUG_HA
        ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
#endif
        struct view_data_wifi_st *p_st = (struct view_data_wifi_st *)event_data;
        if (p_st->is_network)
        {
            net_flag = true;
            mqtt_start();
        }
        else
        {
            net_flag = false;
        }
        break;
    }
    case VIEW_EVENT_SENSOR_DATA:
    {
        if (mqtt_connected_flag == false)
        {
            break;
        }
#if DEBUG_HA
        ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA");
#endif

        struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;

        char data_buf[64];
        int len = 0;
        memset(data_buf, 0, sizeof(data_buf));

        switch (p_data->sensor_type)
        {
        case SENSOR_HA:
        {
            //   char *topic_state = ha_switch_entites[p_data->index].topic_state;
            char *key = all_sensors[p_data->ha_data.index].ha_key;

            ESP_LOGI(TAG, "SENSOR: MQTT data to send: %s", p_data->ha_data.value);

            len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": \"%s\"}", key, p_data->ha_data.value);

            ESP_LOGI(TAG, "SENSOR: MQTT data to send (formated): %s", data_buf);

            esp_mqtt_client_publish(mqtt_client, CONFIG_TOPIC_SENSOR_DATA, data_buf, len, 0, 0);

            ESP_LOGI(TAG, "SENSOR: MQTT set sensor %d: %s", p_data->ha_data.index, p_data->ha_data.value);

            break;
        }
        case SENSOR_DATA_CO2:
        {
            len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_CO2_VALUE_KEY, (int)p_data->vaule);
            esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
            break;
        }
        case SENSOR_DATA_TVOC:
        {
            len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_TVOC_VALUE_KEY, (int)p_data->vaule);
            esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
            break;
        }
        case SENSOR_DATA_TEMP:
        {
            len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%.1f\"}", CONFIG_SENSOR_BUILDIN_TEMP_VALUE_KEY, p_data->vaule);
            esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
            break;
        }
        case SENSOR_DATA_HUMIDITY:
        {
            len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_HUMIDITY_VALUE_KEY, (int)p_data->vaule);
            esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
            break;
        }
        default:
            break;
        }
        break;
    }
    case VIEW_EVENT_HA_SWITCH_ST:
    {
        if (mqtt_connected_flag == false)
        {
            struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;
            //printf("MQTT NOT CONNECTED!! Switch %s\n",all_switches[p_data->index].ha_key);
            break;
        }
#if DEBUG_HA
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SWITCH_SET");
#endif

        struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;

        char data_buf[200];
        int len = 0;
        memset(data_buf, 0, sizeof(data_buf));

        //   char *topic_state = ha_switch_entites[p_data->index].topic_state;
        char *key = all_switches[p_data->index].ha_key;

        ESP_LOGI(TAG, "SWITCH: MQTT data to send: %s", p_data->value_str);

        len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": \"%s\"}", key, p_data->value_str);

        ESP_LOGI(TAG, "SWITCH: MQTT data to send (formated): %s", data_buf);

        esp_mqtt_client_publish(mqtt_client, CONFIG_TOPIC_SWITCH_STATE, data_buf, len, 0, 0);

        ESP_LOGI(TAG, "SWITCH: MQTT set switch %d: %s", p_data->index, p_data->value_str);

        /*if (p_data->index < ha_switch_entites_num)
        {
            sprintf(switch_state[p_data->index],"%s",p_data->value_str);
        }
        ha_ctrl_cfg_save(); // save switch state to flash*/
        break;
    }

    case VIEW_EVENT_HA_SWITCH_ACTION:
    {
        if (mqtt_connected_flag == false)
        {
            break;
        }
#if DEBUG_HA
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SWITCH_ACTION");
#endif

        struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;

        char data_buf[100];
        int len = 0;
        memset(data_buf, 0, sizeof(data_buf));

        char *key = all_switches[p_data->index].ha_key;

        len = snprintf(data_buf, sizeof(data_buf), "{\"key\": \"%s\",\"action\": \"%s\"}", key, p_data->value_str);
        esp_mqtt_client_publish(mqtt_client, CONFIG_TOPIC_SWITCH_ACTION, data_buf, len, 0, 0);

        ESP_LOGI(TAG, "MQTT data to send (formated): %s", data_buf);
    }
    break;

    case VIEW_EVENT_HA_ALARM_CODE_CHANGE:
    {
        if (mqtt_connected_flag == false)
        {
            break;
        }
#if DEBUG_HA
        ESP_LOGI(TAG, "event: VIEW_EVENT_HA_ALARM_CODE_CHANGE");
#endif
        char *p_data = (char *)event_data;

        char data_buf[64];
        int len = 0;
        memset(data_buf, 0, sizeof(data_buf));

        len = snprintf(data_buf, sizeof(data_buf), "{\"command\": \"DISARM\",\"code\": \"%s\"}", p_data);
        esp_mqtt_client_publish(mqtt_client, CONFIG_TOPIC_ALARMO_COMMAND, data_buf, len, 0, 0);

        ESP_LOGI(TAG, "MQTT send alarm code: %s", p_data);
        break;
    }
    default:
        break;
    }
}

/**
 * The function `indicator_ha_init` initializes various components and registers event handlers.
 */
int indicator_ha_init(void)
{
    // ha_ctrl_cfg_restore();  Cancel the restore of the switch state from storage
    ha_entites_init();

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST,
                                                             __view_event_handler, NULL, NULL));

    // buildin sensor
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA,
                                                             __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,
                                                             __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_ALARM_CODE_CHANGE,
                                                             __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ACTION,
                                                             __view_event_handler, NULL, NULL));
}
