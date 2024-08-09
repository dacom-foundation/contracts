cmake_minimum_required(VERSION 3.5)

# Проект
project(registrator)

find_package(cdt)

# Добавление контракта
add_contract(registrator registrator registrator.cpp)
target_compile_definitions(registrator PUBLIC IS_TESTNET=${IS_TESTNET})

# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
