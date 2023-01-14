idf_component_register(
    SRC_DIRS
        "." 
    INCLUDE_DIRS
        "include"
    PRIV_REQUIRES
        i2c_devices mqtt_app para_list gpio_ctrl)
