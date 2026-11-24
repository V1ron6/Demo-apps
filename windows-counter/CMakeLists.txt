cmake_minimum_required(VERSION 3.20)

project(CounterApp LANGUAGES CXX)

set(COUNTER_VERSION "1.0.0" CACHE STRING "x.y.z of this build; must match what you publish")

# Triskelion update library (the repo is a sibling folder: Demo-apps-main/triskelion).
# Needs OpenSSL: build.ps1 sets it up through vcpkg.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../triskelion/sdk-windows triskelion-build)

add_executable(CounterApp WIN32 src/main.cpp)
target_compile_features(CounterApp PRIVATE cxx_std_17)
target_compile_definitions(CounterApp PRIVATE COUNTER_VERSION="${COUNTER_VERSION}")
target_link_libraries(CounterApp PRIVATE triskelion_win)
