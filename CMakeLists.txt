cmake_minimum_required(VERSION 3.20)
project(midi_printer)

set(CMAKE_CXX_STANDARD 14)

add_executable(midi_printer main.cpp midi_file_printer.cpp midi_file_printer.h)
