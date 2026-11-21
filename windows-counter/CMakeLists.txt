cmake_minimum_required(VERSION 3.20)

project(CounterApp LANGUAGES CXX)

add_executable(CounterApp WIN32 src/main.cpp)

target_compile_features(CounterApp PRIVATE cxx_std_17)
