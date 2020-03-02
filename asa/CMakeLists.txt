cmake_minimum_required(VERSION 3.0)

project(assemble)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -ffast-math -DNDEBUG -ftree-vectorize -fstrict-aliasing -msse3 -mtune=native -mfpmath=sse")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wold-style-cast -Wstrict-aliasing -Wno-long-long")

set(CMAKE_REQUIRED_FLAGS -std=c++17)
set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Threads REQUIRED)

add_executable(assemble main.cpp
                        graph.cpp
                        bloom_filter.cpp
                        subtask_a.cpp
                        traversal_state.cpp
                        bfs_state.cpp
                        contig_writer.cpp
                        solidify.cpp
                        nucleotides.cpp
                        )
set_property(TARGET assemble PROPERTY CXX_STANDARD 17)
target_link_libraries(assemble stdc++fs ${CMAKE_THREAD_LIBS_INIT})

