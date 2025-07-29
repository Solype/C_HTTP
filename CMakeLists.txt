cmake_minimum_required(VERSION 3.10)

project(ServerHTTPHTTPS LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
    src/http_server_init.c
    src/http_server_run.c
    src/request.c
    src/response.c
    src/header.c
    src/utils/http_method.c
    src/utils/http_log.c
    src/router/router.c
    src/router/router_get_handler.c
    src/router/router_utils.c
)

# ==============================================
# Trouver et inclure Python automatiquement
# ==============================================
find_package(Python3 REQUIRED COMPONENTS Development)

# ==============================================
# Création de l'exécutable "server"
# ==============================================
add_executable(server src/exec/main.c ${SOURCES})
target_include_directories(server PRIVATE include)

# Ajout des options de compilation (sécurité + debug)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic -g3)
endif()

enable_testing()

# ==============================================
# Ajout d'une bibliothèque partagée pour Python
# ==============================================
set(PYTHON_LIB_SOURCES src/python/wrapper.c)

add_library(pythonlib SHARED ${PYTHON_LIB_SOURCES})
target_include_directories(pythonlib PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(pythonlib PRIVATE ${Python3_LIBRARIES})

set_target_properties(pythonlib PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(pythonlib PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic -g3)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(server PRIVATE /W4 /WX /Zi)
endif()
