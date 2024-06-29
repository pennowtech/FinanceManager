# Copyright (C) 2024 undefined
#
# This file is part of Finance Manager.
#
# Finance Manager is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Finance Manager is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Finance Manager.  If not, see <https://www.gnu.org/licenses/>.



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
find_package(nlohmann_json REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(cxxopts REQUIRED)
# Find and link OpenSSL (required by jwt-cpp)
find_package(OpenSSL REQUIRED)

# Add the source files
set(SOURCES
${PROJECT_SOURCE_DIR}/src/GatewayController.cpp
)

# Add the header files for MOC
set(HEADERS
${PROJECT_SOURCE_DIR}/include/JwtAuthFilter.h
)

add_library(lib${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(lib${PROJECT_NAME}
    PUBLIC Drogon::Drogon
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC jwt-cpp::jwt-cpp
    PUBLIC cxxopts::cxxopts
    PUBLIC rt
    PUBLIC libUtils
)

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
add_custom_target(run${PROJECT_NAME} COMMAND ${PROJECT_NAME} -e /workspaces/qmldevcontainer/.env )
