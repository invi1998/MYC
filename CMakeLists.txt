cmake_minimum_required(VERSION 3.20)
project(MYC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(MYC
    main.cpp
    solution1/Complex.cpp
)

target_include_directories(MYC PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/solution1
)

if(MSVC)
    target_compile_options(MYC PRIVATE /utf-8)
elseif(WIN32)
    target_compile_options(MYC PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
