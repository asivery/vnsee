cmake_minimum_required(VERSION 3.20)
project(VNSee VERSION 1.1.0 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE STRING "" FORCE)
set(LIBVNCSERVER_INSTALL OFF CACHE STRING "" FORCE)
set(WITH_LIBVNCSERVER OFF CACHE STRING "" FORCE)
set(WITH_LIBSSHTUNNEL OFF CACHE STRING "" FORCE)
set(WITH_SYSTEMD OFF CACHE STRING "" FORCE)
set(WITH_FFMPEG OFF CACHE STRING "" FORCE)
set(WITH_TIGHTVNC_FILETRANSFER OFF CACHE STRING "" FORCE)
set(WITH_SDL OFF CACHE STRING "" FORCE)
set(WITH_LZO OFF CACHE STRING "" FORCE)
set(WITH_GTK OFF CACHE STRING "" FORCE)
set(WITH_OPENSSL OFF CACHE STRING "" FORCE)
set(WITH_GNUTLS OFF CACHE STRING "" FORCE)
set(WITH_GCRYPT OFF CACHE STRING "" FORCE)
set(WITH_EXAMPLES OFF CACHE STRING "" FORCE)
set(WITH_SASL OFF CACHE STRING "" FORCE)
set(WITH_XCB OFF CACHE STRING "" FORCE)
set(WITH_TESTS OFF CACHE STRING "" FORCE)
set(WITH_QT OFF CACHE STRING "" FORCE)
add_subdirectory(vendor/libvncserver)

add_subdirectory(vendor/boost-preprocessor)

# Options for building vnsee
option(CHECK_INCLUDES "Run include-what-you-use to check #includes" OFF)
option(CHECK_TIDY "Run clang-tidy linter" OFF)
option(TRACE "Print tracing messages on standard output" OFF)

if(CHECK_INCLUDES)
    find_program(IWYU_PATH include-what-you-use)

    if(NOT IWYU_PATH)
        message(FATAL_ERROR "Could not find include-what-you-use")
    endif()

    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
endif()

if(CHECK_TIDY)
    find_program(TIDY_PATH clang-tidy)

    if(NOT TIDY_PATH)
        message(FATAL_ERROR "Could not find clang-tidy")
    endif()

    set(CMAKE_CXX_CLANG_TIDY ${TIDY_PATH};-fix)
endif()

if(TRACE)
    message(STATUS "Trace mode enabled")
    add_definitions(-DTRACE)
endif()

add_executable(vnsee
    src/app/buttons.cpp
    src/app/virtualkeyboard.cpp
    src/app/client.cpp
    src/app/screen.cpp
    src/app/touch.cpp
    src/main.cpp
    src/rmioc/device.cpp
    src/rmioc/screen.cpp
)

configure_file(src/config.hpp.in src/config.hpp)
target_link_libraries(vnsee PUBLIC rt)
target_include_directories(vnsee PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(vnsee PRIVATE vncclient)
target_include_directories(vnsee PRIVATE
    ${LibVNCServer_BINARY_DIR}
    ${LibVNCServer_BINARY_DIR}/include
    ${LibVNCServer_SOURCE_DIR}/include
)

target_link_libraries(vnsee PUBLIC Boost::preprocessor)

target_include_directories(vnsee PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/rm-appload/backends/qtfb-clients/cpp
)
target_sources(vnsee PRIVATE
    vendor/rm-appload/backends/qtfb-clients/cpp/qtfb-client.cpp
)

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/dist)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/vnsee)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/vnsee)

set_target_properties(vnsee PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist/vnsee
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist/vnsee
)

file(COPY ${CMAKE_SOURCE_DIR}/assets/vnsee-0 DESTINATION ${CMAKE_BINARY_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/assets/vnsee-1 DESTINATION ${CMAKE_BINARY_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/assets/vnsee-2 DESTINATION ${CMAKE_BINARY_DIR}/dist)
file(COPY ${CMAKE_SOURCE_DIR}/assets/vnsee-3 DESTINATION ${CMAKE_BINARY_DIR}/dist)
