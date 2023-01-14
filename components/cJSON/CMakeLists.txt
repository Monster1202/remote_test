set(srcs "src/cJSON.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include")
