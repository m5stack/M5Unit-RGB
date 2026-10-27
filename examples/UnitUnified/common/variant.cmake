# Map the Kconfig "Target unit / board" choice (see common/Kconfig.variant) to the source-level
# USING_* macro that both the Arduino and ESP-IDF builds use. Include this from an example's
# main/CMakeLists.txt *after* idf_component_register() (it needs ${COMPONENT_LIB}).
# Default (nothing selected): Unit RGB.
if(CONFIG_EXAMPLE_USING_UNIT_PUZZLE)
    set(M5UNIT_VARIANT USING_UNIT_PUZZLE)
elseif(CONFIG_EXAMPLE_USING_UNIT_HEX)
    set(M5UNIT_VARIANT USING_UNIT_HEX)
elseif(CONFIG_EXAMPLE_USING_UNIT_NEO_HEX)
    set(M5UNIT_VARIANT USING_UNIT_NEO_HEX)
elseif(CONFIG_EXAMPLE_USING_UNIT_NECO)
    set(M5UNIT_VARIANT USING_UNIT_NECO)
elseif(CONFIG_EXAMPLE_USING_UNIT_RGBLED15)
    set(M5UNIT_VARIANT USING_UNIT_RGBLED15)
elseif(CONFIG_EXAMPLE_USING_UNIT_RGBLED_STRIP30)
    set(M5UNIT_VARIANT USING_UNIT_RGBLED_STRIP30)
elseif(CONFIG_EXAMPLE_USING_ATOM_MATRIX_RGB)
    set(M5UNIT_VARIANT USING_ATOM_MATRIX_RGB)
elseif(CONFIG_EXAMPLE_USING_HAT_NEOFLASH)
    set(M5UNIT_VARIANT USING_HAT_NEOFLASH)
else()
    set(M5UNIT_VARIANT USING_UNIT_RGB)
endif()
target_compile_definitions(${COMPONENT_LIB} PRIVATE ${M5UNIT_VARIANT})
