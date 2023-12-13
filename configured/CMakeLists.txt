configure_file(
    "config.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/config.hpp"
    ESCAPE_QUOTES)

configure_file(
    "Doxyfile.in" "${CMAKE_HOME_DIRECTORY}/docs/Doxyfile"
    ESCAPE_QUOTES)
