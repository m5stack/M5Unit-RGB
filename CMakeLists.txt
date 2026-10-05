# ESP-IDF component manifest for M5Unit-RGB.
# Upstream: https://github.com/m5stack/M5Unit-RGB
#
# The public headers include only M5UnitComponent (M5UnitUnified). M5Utility and the RMT driver
# are used by the .cpp files only.
file(GLOB_RECURSE SRCS "src/*.cpp")
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "src"
    REQUIRES M5UnitUnified
    PRIV_REQUIRES M5Utility driver
)
