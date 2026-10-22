# ESP-IDF component registration for M5Unit-RGB.
# Upstream: https://github.com/m5stack/M5Unit-RGB
#
# The public headers pull in both M5UnitUnified and M5Utility (button status and the
# mmh3 literal). The RMT driver is used by the .cpp files only.
file(GLOB_RECURSE SRCS "src/*.cpp")
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "src"
    REQUIRES M5UnitUnified M5Utility
    PRIV_REQUIRES driver
)
