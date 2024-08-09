cmake_minimum_required(VERSION 3.5)

# Проект
project(soviet)

find_package(cdt)

# Добавление контракта
add_contract(soviet soviet soviet.cpp)
target_compile_definitions(soviet PUBLIC IS_TESTNET=${IS_TESTNET})

# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
