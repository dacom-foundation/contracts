cmake_minimum_required(VERSION 3.5)

# Проект
project(fund)

find_package(cdt)

# Добавление контракта
add_contract(fund fund fund.cpp)

# target_include_directories( fund PUBLIC ${CMAKE_SOURCE_DIR}/include )
