cmake_minimum_required(VERSION 3.13)
project(cxx_json_serialize)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -save-temps")

set(SRC_CXX_JSON_SERIALIZE
        utils/json/cJSON.c
        utils/json/cJSON.h
        utils/json/cxxJson.cpp
        utils/json/cxxJson.h)

set(SRC_TEST
        test/base/tstbase.cpp
        test/base/tstbase.h
        test/lsblk/infolsblk.h
        test/lsblk/tstlsblk.cpp
        test/lsblk/tstlsblk.h)

add_executable(cxx_json_serialize
                main.cpp
                ${SRC_CXX_JSON_SERIALIZE}
                ${SRC_TEST})
