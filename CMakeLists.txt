cmake_minimum_required(VERSION 3.24)
project(school_db_grader C)

set(CMAKE_C_STANDARD 23)

add_library(checkpoint-noammirjani main.c crypt.c)
