set(srcs "9-dof-test.c")

idf_component_register(SRCS "${srcs}"
                    REQUIRES driver
                    PRIV_REQUIRES esp32-i2c-lsm6dsox-lis3mdl
                    INCLUDE_DIRS ".")
