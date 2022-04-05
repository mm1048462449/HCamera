set(COMPONENT_SRCS 
fonts/font8.c
fonts/font12.c
fonts/font16.c
fonts/font20.c
fonts/font24.c
lcd_1inch28.c
lcd_gui.c
)
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "")
set(COMPONENT_PRIV_REQUIRES newlib)
set(COMPONENT_REQUIRES esp-slaves)
register_component()