cmake_minimum_required(VERSION 3.14)
project(project1DFA)

set(CMAKE_CXX_STANDARD 17)

add_executable(project1DFA main.cpp BigUnsigned.cc BigUnsigned.h BigUnsignedInABase.cc BigUnsignedInABase.h
        NumberlikeArray.h)