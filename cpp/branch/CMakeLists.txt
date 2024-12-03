cmake_minimum_required(VERSION 3.5)

# Проект
project(branch)

find_package(cdt)

# Добавление контракта
add_contract(branch branch branch.cpp)
target_compile_definitions(branch PUBLIC IS_TESTNET=${IS_TESTNET})

# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
