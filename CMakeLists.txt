cmake_minimum_required(VERSION 3.10)

project(JankTerm VERSION 0.1)

# Find the VTE package
find_package(PkgConfig REQUIRED)
pkg_check_modules(VTE REQUIRED vte-2.91)

# Include directories
include_directories(${VTE_INCLUDE_DIRS})

# Set the source files
file(GLOB SRC "src/*.c")

# Add the executable
add_executable(term ${SRC})

# Set compiler flags
target_compile_options(term PRIVATE -O3 -Wall)

# Link the VTE library
target_link_libraries(term ${VTE_LIBRARIES})

install(TARGETS term DESTINATION bin)

# Ubuntu-specific packages to install
# install(CODE "execute_process(COMMAND sudo apt-get install libcairo2-dev libgdk-pixbuf2.0-dev)")
# install(CODE "execute_process(COMMAND sudo apt-get install -y libvte-2.91-dev)")
install(CODE "execute_process(COMMAND sudo ./create_term WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src)")

add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/term
    DEPENDS term
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES term)
