cmake_minimum_required(VERSION 3.5)

# Проект
project(marketplace)

find_package(cdt)

# Добавление контракта
add_contract(marketplace marketplace marketplace.cpp)
target_compile_definitions(marketplace PUBLIC IS_TESTNET=${IS_TESTNET})
# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
