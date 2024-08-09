#!/bin/bash

# Останавливаем и удаляем контейнер, если он существует
docker stop node > /dev/null 2>&1 && docker rm node > /dev/null 2>&1

# Очищаем папку с данными блокчейна
rm -rf ~/testnet/data/*

echo "Очистка завершена."

