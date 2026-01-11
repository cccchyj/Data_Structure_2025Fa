# ====== CMakeLists.txt for AVL & RB Tree Dictionary ======
cmake_minimum_required(VERSION 3.10)
project(DictionaryTrees CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")

add_executable(dict
    main.cpp
    avl_tree.cpp
    rb_tree.cpp
)

# Optional: show compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)