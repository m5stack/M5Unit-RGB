# Dependencies (M5Unit-RGB, M5UnitUnified, M5Unified, ...) are pulled in via
# main/idf_component.yml and auto-added as requirements by the component manager.
# The unit chain is fixed in the source, so there is no Kconfig variant to select.
idf_component_register(
    SRCS "ViaPbHub.cpp"
    INCLUDE_DIRS ".")
