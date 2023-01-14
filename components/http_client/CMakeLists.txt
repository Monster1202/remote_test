set(srcs "src/http_client.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp-tls esp_http_client decoder_ijg main jpegdec para_list)
