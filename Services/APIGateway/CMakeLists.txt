# ---------------------------------------------------------------------------------------
#                         Copyright (©) Carl Zeiss Meditec AG
#                               - All Rights Reserved -
#
#                     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF
#                                  CARL ZEISS MEDITEC AG
#
#                       This copyright notice does not evidence any
#                    actual or intended publication of such source code.
# ---------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)
project(APIGateway VERSION 0.1)

# Option controlled by Conan to build tests
option(WITH_TESTS "Build the tests" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/generators" "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/generators"
                        ${CMAKE_MODULE_PATH}
)
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators" "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/generators"
                        ${CMAKE_PREFIX_PATH}
)

message(STATUS "Building : ${PROJECT_NAME}")
message(STATUS "CONAN_LIBS_DROGON: ${CONAN_LIBS_DROGON}")

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Drogon REQUIRED)
find_package(log4cplus REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio-grpc REQUIRED)
find_package(jwt-cpp REQUIRED)

# Add the source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/GatewayController.cpp
)

# Add the header files for MOC
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/GatewayController.h
    # ${PROJECT_SOURCE_DIR}/include/Logger.h
    # ${PROJECT_SOURCE_DIR}/include/LoggingHelpers.h
)

add_library(lib${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(lib${PROJECT_NAME}
    PUBLIC log4cplus::log4cplus 
    PUBLIC Drogon::Drogon 
    PUBLIC nlohmann_json::nlohmann_json 
    PUBLIC asio-grpc::asio-grpc 
    PUBLIC jwt-cpp::jwt-cpp
    rt)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})

# Conditional compilation of tests
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    add_executable(${PROJECT_NAME}_tests tests/test_main.cpp)
    target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE lib${PROJECT_NAME}
        PRIVATE gtest::gtest
    )
    add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_tests)
endif()