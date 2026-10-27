cmake_minimum_required(VERSION 2.8)
project(MyTinySTL)

set(CMAKE_CXX_STANDARD 11)



add_executable(MyTinySTL main.cpp
        MyTinySTL/type_traits.h
        MyTinySTL/util.h
        algo.h
        MyTinySTL/basic_string.h
        MyTinySTL/iterator.h
        MyTinySTL/alloc.h
        MyTinySTL/allocator.h
        MyTinySTL/construct.h
        MyTinySTL/algobase.h
        MyTinySTL/uninitialized.h
        MyTinySTL/memory.h
        MyTinySTL/exceptdef.h
        MyTinySTL/vector.h
        MyTinySTL/functional.h
        MyTinySTL/heap_algo.h
        MyTinySTL/algo.h
        MyTinySTL/basic_string.h
)


add_executable(test_00 test/test00.cpp)
add_executable(test_01_traits test/test_01_traits.cpp)
add_executable(test_02_move test/test_02_move.cpp)

add_executable(test_03_forward test/test_03_forward.cpp)
add_executable(test_04_swap test/test_04_swap.cpp)

add_executable(test_05_new test/test_05_new.cpp)
add_executable(test_06_iterator test/test_06_iterator.cpp)

add_executable(test_07_memory test/test_07_memory.cpp)
add_executable(test_08_functional test/test_08_functional.cpp)

add_executable(test_09_heap test/test_09_heap.cpp)


add_executable(STLTest_01_vector test/stl_test/test_01_vector.cpp
        test/stl_test/test.h)

