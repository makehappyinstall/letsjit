cmake_minimum_required(VERSION 2.4)

project(letsjit)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
set(CMAKE_CXX_STANDARD 17)

llvm_map_components_to_libnames(llvm_libs Core
        ExecutionEngine
        Interpreter
        mc
        mcjit
        Support
        nativecodegen
        )

enable_testing()
find_package(GTest REQUIRED)

file(GLOB_RECURSE lib_sources src "*.cpp")
add_library(letsjit ${lib_sources})
target_link_libraries(letsjit PUBLIC ${llvm_libs})
target_include_directories(letsjit PUBLIC include)

file(GLOB test_sources "tests/*.cpp")
# little hack :(
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
cmake_policy(SET CMP0003 OLD)
foreach(file ${test_sources})
    get_filename_component(bin_name ${file} NAME_WE)
    add_executable("${bin_name}" ${file})
    target_include_directories(${bin_name} PUBLIC include)
    target_link_libraries(${bin_name} PUBLIC ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    target_link_libraries(${bin_name} PUBLIC pthread)
    target_link_libraries(${bin_name} PUBLIC ${llvm_libs})
    target_link_libraries(${bin_name} PUBLIC letsjit)
endforeach()


file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
        clang-format
        COMMAND /usr/bin/clang-format-9
        -i
        ${ALL_SOURCE_FILES}
)
