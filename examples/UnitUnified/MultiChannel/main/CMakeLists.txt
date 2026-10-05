# Dependencies (M5Unit-RGB, M5UnitUnified, M5Unified, ...) are pulled in via
# main/idf_component.yml and auto-added as requirements by the component manager.
# The unit types are fixed in the source (UNIT_1_TYPE / UNIT_2_TYPE), so there is no Kconfig variant.
idf_component_register(
    SRCS "MultiChannel.cpp"
    INCLUDE_DIRS ".")
