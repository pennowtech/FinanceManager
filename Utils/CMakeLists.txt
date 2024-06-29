cmake_minimum_required(VERSION 3.21)
project(Utils VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

find_package(quill REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add the source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/EnvReader.cpp
    ${PROJECT_SOURCE_DIR}/src/LoggerQuill.cpp
)

# Add the header files for MOC
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/Utils/EnvReader.h
    ${PROJECT_SOURCE_DIR}/include/Utils/Logger.h
    ${PROJECT_SOURCE_DIR}/include/Utils/LoggerQuill.h
)

add_library(lib${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(lib${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(lib${PROJECT_NAME}
    PUBLIC quill::quill
    PUBLIC nlohmann_json::nlohmann_json
    rt)
