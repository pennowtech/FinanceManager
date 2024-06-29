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
project(FinanceManager)

# set(CMAKE_PREFIX_PATH "/lib/x86_64-linux-gnu/cmake" "${CMAKE_BINARY_DIR}/Debug/generators/" ${CMAKE_PREFIX_PATH})
# set(CMAKE_PREFIX_PATH "/home/sds/.conan2/p/qt0b8d5dc773b00/p/lib/cmake" ${CMAKE_PREFIX_PATH})

message(STATUS "Started building project: ${PROJECT_NAME}")

# Optionally set the C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++-14")
add_compile_options(-ftemplate-backtrace-limit=0)

# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()
option(USE_CONAN "Use Conan for dependencies" ON)
message(STATUS "${PROJECT_NAME}: USE_CONAN: \"${USE_CONAN}\"")

# Include the Utils directory
include_directories(${PROJECT_SOURCE_DIR}/Utils/include)

# Include each service
add_subdirectory(Utils)
add_subdirectory(Services/APIGateway)
# add_subdirectory(Services/AuthAPI)
# add_subdirectory(Services/DBManager)
# add_subdirectory(Services/Expenses)
# add_subdirectory(Services/Item)
# add_subdirectory(Services/NotificationService)
# add_subdirectory(Services/Reports)
# add_subdirectory(Services/Search)
add_subdirectory(Services/UI)

# If there are any global compiler options or definitions, set them here
add_compile_options(-Wall -Wextra -Werror)

# target_link_libraries(lib${PROJECT_NAME} gtest::gtest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "${PROJECT_NAME}: CMAKE_EXPORT_COMPILE_COMMANDS: \"${CMAKE_EXPORT_COMPILE_COMMANDS}\"")
message(STATUS "${PROJECT_NAME}: CMAKE_CXX_COMPILER: \"${CMAKE_CXX_COMPILER}\"")

# BUILD_OPTION can also be supplied through command line -DBUILD_TESTS=ON
option(BUILD_TESTS "Build Unit Tests" OFF)
message(STATUS "${PROJECT_NAME}: BUILD_TESTS: \"${BUILD_TESTS}\"")

option(COVERAGE "Build with coverage" OFF)
message(STATUS "${PROJECT_NAME}: COVERAGE: \"${COVERAGE}\"")

if(BUILD_TESTS)
  enable_testing()
endif()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/starting_processes
    COMMAND bash ${CMAKE_SOURCE_DIR}/manage_services.sh run
    DEPENDS ${CMAKE_SOURCE_DIR}/manage_services.sh
    COMMENT "Starting processes..."
    VERBATIM
)
add_custom_target(
    start_processes
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/starting_processes
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/stopping_processes
    COMMAND bash ${CMAKE_SOURCE_DIR}/manage_services.sh killall
    DEPENDS ${CMAKE_SOURCE_DIR}/manage_services.sh
    COMMENT "Stopping processes..."
    VERBATIM
)

add_custom_target(
    stop_processes
    COMMAND bash ${CMAKE_SOURCE_DIR}/manage_services.sh killall
    COMMENT "Stopping processes..."
)
