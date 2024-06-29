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
project(personal-finance VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set(CMAKE_PREFIX_PATH "/lib/x86_64-linux-gnu/cmake" "${CMAKE_BINARY_DIR}/Debug/generators/" ${CMAKE_PREFIX_PATH})
# set(CMAKE_PREFIX_PATH "/home/sds/.conan2/p/qt0b8d5dc773b00/p/lib/cmake" ${CMAKE_PREFIX_PATH})


message(STATUS "SDSINGH: The value of CMAKE_PREFIX_PATH is: ${CMAKE_PREFIX_PATH}")
# message(FATAL_ERROR "Stopping execution after printing the variable value.")

set(QML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
file(GLOB_RECURSE QML_SOURCE_FILES "${QML_SOURCE_DIR}/*.qml")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Qt6 COMPONENTS Core Quick Widgets Charts REQUIRED)
find_package(redis++ REQUIRED)
find_package(cassandra-cpp-driver REQUIRED)
find_package(libpqxx REQUIRED)
find_package(Drogon REQUIRED)
find_package(httplib REQUIRED)
find_package(nlohmann_json REQUIRED)

find_package(cpp-jwt REQUIRED)
find_package(GTest REQUIRED)

# find_package(cpp-httplib REQUIRED)

# Add the source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/RedisDB.cpp
    ${PROJECT_SOURCE_DIR}/src/CassandraDB.cpp
    ${PROJECT_SOURCE_DIR}/src/DBManager.cpp
    ${PROJECT_SOURCE_DIR}/src/FinanceManager.cpp
)

# Add the header files for MOC
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/RedisDB.h
    ${PROJECT_SOURCE_DIR}/include/CassandraDB.h
    ${PROJECT_SOURCE_DIR}/include/DBManager.h
    ${PROJECT_SOURCE_DIR}/include/FinanceManager.h
    # ${PROJECT_SOURCE_DIR}/include/Logger.h
    # ${PROJECT_SOURCE_DIR}/include/LoggingHelpers.h
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Quick Qt6::Widgets Qt6::Charts
    PRIVATE redis++::redis++_static
    PRIVATE cassandra-cpp-driver::cassandra-cpp-driver
    PRIVATE libpqxx::pqxx
    PRIVATE Drogon::Drogon
    PRIVATE httplib::httplib
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE cpp-jwt::cpp-jwt
    rt)

# Add custom target to copy QML files
add_custom_target(
  CopyQmlFilesOnBuild ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${QML_SOURCE_DIR}
          ${CMAKE_BINARY_DIR}/qml/
)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Make sure the custom target runs before the main executable
add_dependencies(${PROJECT_NAME} CopyQmlFilesOnBuild)

# target_link_libraries(lib${PROJECT_NAME} gtest::gtest)
