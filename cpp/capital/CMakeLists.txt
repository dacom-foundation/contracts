cmake_minimum_required(VERSION 3.5)

# Проект
project(capital)

find_package(cdt)

# Добавление контракта
add_contract(capital capital capital.cpp)
