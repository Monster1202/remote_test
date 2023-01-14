set(srcs "src/native_ota_example.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES app_update esp_http_client esp_https_ota nvs_flash para_list)
