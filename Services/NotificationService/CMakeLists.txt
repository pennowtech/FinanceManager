

cmake_minimum_required(VERSION 3.21)
project(UI VERSION 0.1)

set(CMAKE_PREFIX_PATH "/opt/qt/6.7.1/gcc_64/lib/cmake" ${CMAKE_PREFIX_PATH})

# Option controlled by Conan to build tests
option(WITH_TESTS "Build the tests" OFF)

message(STATUS "Building : ${PROJECT_NAME}")

set(QML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
file(GLOB_RECURSE QML_SOURCE_FILES "${QML_SOURCE_DIR}/*.qml")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add the include directory
include_directories(${CMAKE_SOURCE_DIR}/include ${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS})

find_package(Qt6 COMPONENTS Core Gui Quick Widgets Charts Test REQUIRED)
find_package(Drogon REQUIRED)
find_package(nlohmann_json REQUIRED)

find_package(cpp-jwt REQUIRED)
find_package(GTest REQUIRED)
message(STATUS "Using Qt version: ${Qt6_VERSION}")

# Add the source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/dummy.cpp
)

# Add the header files for MOC
set(HEADERS
    # ${PROJECT_SOURCE_DIR}/include/Logger.h
    # ${PROJECT_SOURCE_DIR}/include/LoggingHelpers.h
)

add_library(lib${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(lib${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::Widgets Qt6::Charts Qt6::Test
    PRIVATE Drogon::Drogon
    PRIVATE nlohmann_json::nlohmann_json

    PRIVATE cpp-jwt::cpp-jwt
    rt)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})

# Add custom target to copy QML files
add_custom_target(
  CopyQmlFilesOnBuild ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${QML_SOURCE_DIR}
          ${CMAKE_BINARY_DIR}/qml/
)

# Make sure the custom target runs before the main executable
add_dependencies(${PROJECT_NAME} CopyQmlFilesOnBuild)

# Conditional compilation of tests
if(BUILD_TESTS)
    add_executable(${PROJECT_NAME}_tests tests/test_main.cpp)
    target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE lib${PROJECT_NAME}
        PRIVATE gtest::gtest
    )
    add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_tests)
endif()

if(BUILD_QT_TESTS)
    add_executable(${PROJECT_NAME}_qt_tests tests/test_qt_main.cpp)
    target_link_libraries(${PROJECT_NAME}_qt_tests
        PRIVATE lib${PROJECT_NAME}
        PRIVATE Qt6::Test)
    add_test(NAME ${PROJECT_NAME}_qt_test COMMAND ${PROJECT_NAME}_qt_tests)
endif()

if(BUILD_QML_TESTS)
    qt6_add_resources(QT_RESOURCES qml.qrc)
    add_executable(${PROJECT_NAME}_qml_test_runner main.cpp ${QT_RESOURCES})
    target_link_libraries(${PROJECT_NAME}_qml_test_runner Qt6::Quick Qt6::Test)
endif()
