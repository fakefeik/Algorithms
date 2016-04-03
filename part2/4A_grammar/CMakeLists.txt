cmake_minimum_required (VERSION 2.6)
project(4A_grammar)

add_executable(4A_grammar main.c)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER part2)
