cmake_minimum_required(VERSION 3.10)

project(ServerHTTPHTTPS LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(EXECUTABLE_NAME server_http_https)

set(SOURCES
    main.c
    src/http_server_init.c
    src/http_server_run.c
    src/request.c
    src/header.c
    src/http_log.c
)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

find_package(OpenSSL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(${EXECUTABLE_NAME} PRIVATE include)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ENABLE_HTTPS=1)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -Wall -Wextra -Wpedantic -Werror
    )
endif()

enable_testing()
