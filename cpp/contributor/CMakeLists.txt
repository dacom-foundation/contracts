cmake_minimum_required(VERSION 3.5)

# Проект
project(contributor)

find_package(cdt)

# Добавление контракта
add_contract(contributor contributor contributor.cpp)
