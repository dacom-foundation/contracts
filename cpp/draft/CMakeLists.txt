cmake_minimum_required(VERSION 3.5)

# Проект
project(draft)

find_package(cdt)

# Добавление контракта
add_contract(draft draft draft.cpp)
target_compile_definitions(draft PUBLIC IS_TESTNET=${IS_TESTNET})
# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
