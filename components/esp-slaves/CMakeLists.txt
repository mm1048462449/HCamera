set(COMPONENT_SRCS 
 i2c_slave.c
)
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_PRIV_INCLUDEDIRS "")
set(COMPONENT_PRIV_REQUIRES newlib)
set(COMPONENT_REQUIRES pcf8575)
register_component()