set(srcs "src/wifi_sta.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES nvs_flash para_list)
