idf_component_register(SRCS "esp32-i2c-lsm6dsox.c"
                            "esp32-i2c-lis3mdl.c"
                       REQUIRES driver
                       INCLUDE_DIRS ".")
