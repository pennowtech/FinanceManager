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
project(UI VERSION 0.1)

# Option controlled by Conan to build tests
option(WITH_TESTS "Build the tests" OFF)

message(STATUS "Building : ${PROJECT_NAME}")

set(QML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
file(GLOB_RECURSE QML_SOURCE_FILES "${QML_SOURCE_DIR}/*.qml")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Quick QuickControls2 Widgets Charts Network Test REQUIRED)
find_package(Drogon REQUIRED)
find_package(httplib REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(cxxopts REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(redis++ REQUIRED)

message(STATUS "Using Qt version: ${Qt6_VERSION}")


set(SOURCES
${PROJECT_SOURCE_DIR}/src/FirebaseAuth.cpp
${PROJECT_SOURCE_DIR}/src/FirebaseAuthQt.cpp
${PROJECT_SOURCE_DIR}/src/RedisHandler.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(HEADERS
${PROJECT_SOURCE_DIR}/include/FirebaseAuth.h
${PROJECT_SOURCE_DIR}/include/FirebaseAuthQt.h
${PROJECT_SOURCE_DIR}/include/RedisHandler.h
)

add_library(lib${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(lib${PROJECT_NAME}
    PUBLIC Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickControls2 Qt6::Widgets Qt6::Charts Qt6::Network Qt6::Test
    PUBLIC Drogon::Drogon
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC jwt-cpp::jwt-cpp
    PUBLIC cxxopts::cxxopts
    PUBLIC redis++::redis++_static
    PUBLIC rt
    PUBLIC libUtils
    PUBLIC openssl::openssl
    PUBLIC httplib::httplib INTERFACE OpenSSL::SSL OpenSSL::Crypto
)
qt6_add_resources(QT_RESOURCES ${PROJECT_SOURCE_DIR}/qml/resources.qrc)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Utils/include)
target_sources(${PROJECT_NAME} PRIVATE ${QT_RESOURCES})

# Add custom target to copy QML files
add_custom_target(
  CopyQmlFilesOnBuild ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${QML_SOURCE_DIR}
          ${PROJECT_BINARY_DIR}/qml/
)

# Make sure the custom target runs before the main executable
add_dependencies(${PROJECT_NAME} CopyQmlFilesOnBuild)

# Conditional compilation of tests
# if(BUILD_TESTS)
#     find_package(GTest REQUIRED)
#     add_executable(${PROJECT_NAME}_tests tests/test_main.cpp)
#     target_link_libraries(${PROJECT_NAME}_tests
#         PUBLIC lib${PROJECT_NAME}
#         PUBLIC gtest::gtest
#     )
#     add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_tests)
# endif()

# if(BUILD_QT_TESTS)
#     add_executable(${PROJECT_NAME}_qt_tests tests/test_qt_main.cpp)
#     target_link_libraries(${PROJECT_NAME}_qt_tests
#         PRIVATE lib${PROJECT_NAME}
#         PRIVATE Qt6::Test)
#     add_test(NAME ${PROJECT_NAME}_qt_test COMMAND ${PROJECT_NAME}_qt_tests)
# endif()

# if(BUILD_QML_TESTS)
#     qt6_add_resources(QT_RESOURCES qml.qrc)
#     add_executable(${PROJECT_NAME}_qml_test_runner main.cpp ${QT_RESOURCES})
#     target_link_libraries(${PROJECT_NAME}_qml_test_runner Qt6::Quick Qt6::Test)
# endif()
