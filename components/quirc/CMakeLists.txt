set(COMPONENT_SRCS 
    quirc/lib/decode.c
    quirc/lib/identify.c
    quirc/lib/quirc.c
    quirc/lib/version_db.c
)
set(COMPONENT_ADD_INCLUDEDIRS "quirc/lib")
set(COMPONENT_PRIV_REQUIRES newlib)
register_component()