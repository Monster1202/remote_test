set(srcs "src/mqtt_app.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES nvs_flash mqtt cJSON para_list gpio_ctrl wifi)
