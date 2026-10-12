cmake_minimum_required(VERSION 2.8)
project(MyTinySTL)

set(CMAKE_CXX_STANDARD 11)



add_executable(MyTinySTL main.cpp
        MyTinySTL/type_traits.h
        MyTinySTL/util.h
        algo.h
        MyTinySTL/basic_string.h
        MyTinySTL/memory/construct.h
        MyTinySTL/memory/alloc.h
        MyTinySTL/memory/uninitialized.h
        MyTinySTL/iterator.h
        MyTinySTL/alloc.h
        MyTinySTL/allocator.h
        MyTinySTL/construct.h
        MyTinySTL/algobase.h
)


add_executable(test_00 test/test00.cpp)
add_executable(test_01_traits test/test_01_traits.cpp)
add_executable(test_02_move test/test_02_move.cpp)

add_executable(test_03_forward test/test_03_forward.cpp)
add_executable(test_04_swap test/test_04_swap.cpp)

add_executable(test_05_new test/test_05_new.cpp)
