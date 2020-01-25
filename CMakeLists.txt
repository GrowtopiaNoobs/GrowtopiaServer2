cmake_minimum_required(VERSION 3.10.2)

project(GrowtopiaServer CXX)

set(CMAKE_CXX_STANDARD 14)
set(LIBS ${CMAKE_SOURCE_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(INCLUDED_LIBS enet pthread)

if (MSVC)
  set(INCLUDED_LIBS enet winmm ws2_32)
endif()

include_directories(GrowtopiaServer PRIVATE ${LIBS}/enet/include)
include_directories(GrowtopiaServer PRIVATE ${LIBS}/json)
add_subdirectory(${LIBS}/enet)

file (GLOB srcfiles
        src/*.c* s/*.h*
        src/*/*.c* s/*/*.h*
        src/*/*/*.c* s/*/*/*.h*
        src/*/*/*/*.c* s/*/*/*/*.h*
        src/*/*/*/*/*.c* s/*/*/*/*/*.h*
        src/*/*/*/*/*/*.c* s/*/*/*/*/*/*.h*
        src/*/*/*/*/*/*/*.c* s/*/*/*/*/*/*/*.h*)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

add_executable(GrowtopiaServer ${srcfiles})

target_compile_options(GrowtopiaServer PRIVATE -shared-libgcc)

target_link_libraries(GrowtopiaServer ${INCLUDED_LIBS} ${CMAKE_DL_LIBS})
