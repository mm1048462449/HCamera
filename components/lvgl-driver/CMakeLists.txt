set(COMPONENT_SRCS 
 lv_port_disp.c
 lv_port_indev.c
)
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_REQUIRES newlib)
set(COMPONENT_REQUIRES lvgl esp-lcd esp-slaves)
register_component()