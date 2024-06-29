#!/bin/bash

function rebuild() {
    echo "Rebuilding all microservices..."
    # # TODO:  Don\t know why I need to setup these paths manually here when they're already specified in my conan profile
    # export CC=/usr/bin/clang-14
    # export CXX=/usr/bin/clang++-14
    if [ -d "build" ]; then
        echo "Removing existing build directory..."
        rm -rf build
    fi
    echo "Installing Conan dependencies..."
    conan install . --profile:build clang-debug-profile --profile:host clang-debug-profile --build=missing -o with_tests=True

    echo "Starting building app now ..."

    mkdir -p build
    cd build
    cmake .. -DBUILD_TESTS=OFF -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake
    echo "Starting building microservice now ..."
    cmake --build .
}

function build() {
    echo "Building all microservices..."
    cd build
    cmake --build .
}

function test() {
    echo "Running tests..."
    cd build
    ctest --output-on-failure
}

function run() {
    echo "Running microservices from $PWD"
    cd build/Debug/
    # ./Services/APIGateway/APIGateway &
    ./Services/UI/UI &
    # ./AuthAPI/AuthAPI &
    # ./NotificationService/NotificationService &
    wait
    echo "Running microservices ended..."
}

function kill_all() {
    echo "Killing all running microservices..."
    kill $(ps aux | grep './APIGateway/APIGateway' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './AuthAPI/AuthAPI' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './DBManager/DBManager' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './Expenses/Expenses' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './Item/Item' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './NotificationService/NotificationService' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './Reports/Reports' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './Search/Search' | awk '{print $2}') 2>/dev/null
    kill $(ps aux | grep './UI/UI' | awk '{print $2}') 2>/dev/null
    echo "All services should be stopped now."
}

# Command line argument parsing
case "$1" in
    rebuild)
        rebuild
        ;;
    build)
        build
        ;;
    test)
        test
        ;;
    killall)
        kill_all
        ;;
    run)
        run
        ;;
    *)
        echo "Usage: $0 {rebuild|build|test|run|killall}"
        exit 1
esac
