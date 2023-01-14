
#include "key.h"

static const char *TAG = "key";

esp_err_t key_init(void)
{
    ESP_LOGI(TAG, "init key gpio");
    gpio_pad_select_gpio(GPIO_KEY);
    gpio_set_direction(GPIO_KEY, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_KEY, GPIO_PULLUP_ONLY);
    return ESP_OK;
}

int key_get_value(void)
{
    return gpio_get_level(GPIO_KEY);
}

// __attribute__((weak)) 
void wake_btn_handler(void *arg)
{
    if (0 == gpio_get_level(GPIO_KEY)) {
        // printf("button pressed/n");
    }
}

//按键中断方式
esp_err_t key_init_isr(void)
{
    gpio_config_t io_conf_key;
    io_conf_key.intr_type = GPIO_INTR_NEGEDGE;
    io_conf_key.mode = GPIO_MODE_INPUT;
    io_conf_key.pin_bit_mask = 1ULL << GPIO_KEY;
    io_conf_key.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_key.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf_key));

    /* Install GPIO ISR service to enable GPIO ISR callback */
    gpio_install_isr_service(0);
    ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_KEY, wake_btn_handler, NULL));

    return ESP_OK;
}

