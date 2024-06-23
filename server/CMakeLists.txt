cmake_minimum_required(VERSION 3.10)

# Defina o nome do projeto
project(RealTimeSystem)

# Adicione o diretório de inclusão
include_directories(src)
include_directories(${SQLite3_INCLUDE_DIRS})

# Adicione o caminho para os módulos CMake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Encontre a biblioteca libmicrohttpd
find_package(Microhttpd REQUIRED)
find_package(SQLite3 REQUIRED)

# Defina o diretório de saída para os binários
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Adicione os arquivos fonte
add_executable(GPSReader src/main.c src/gps/gps.c src/utils/utils.c)
add_executable(GPSServer src/server.c src/gps/gps.c src/utils/utils.c)

# Adicione bibliotecas necessárias
target_link_libraries(GPSServer ${MICROHTTPD_LIBRARIES} pthread)
target_link_libraries(GPSReader pthread)
target_link_libraries(GPSServer ${SQLite3_LIBRARIES})
