cmake_minimum_required(VERSION 3.10)

# Defina o nome do projeto
project(RealTimeSystem)

# Adicione o diretório de inclusão
include_directories(src/include)

# Adicione o caminho para os módulos CMake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Encontre as bibliotecas necessárias
find_package(Microhttpd REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(json-c REQUIRED)

# Adicionar diretórios de inclusão das bibliotecas
include_directories(${MICROHTTPD_INCLUDE_DIRS})
include_directories(${SQLite3_INCLUDE_DIRS})
include_directories(${JSON-C_INCLUDE_DIRS})

# Defina o diretório de saída para os binários
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Adicione os arquivos fonte
add_executable(GPSServer
    src/server.c
    src/gps.c
    src/utils.c
    src/gps_thread.c
    src/periodic_task_thread.c
    src/network_communication_thread.c
    src/condition_handler_thread.c
    src/device_check_thread.c
    src/gps_satellites.c
    src/http_server_thread.c
)

# Adicione bibliotecas necessárias
target_link_libraries(GPSServer
    ${MICROHTTPD_LIBRARIES}
    ${SQLite3_LIBRARIES}
    json-c
    pthread
)
