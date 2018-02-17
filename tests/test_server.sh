#!/bin/bash

#first argument is the executable
#second argument is the server configuration file
if [ "$#" -ne 2 ]; then
  echo "Usage : ./test_server.sh [path to executable] [path to server_config]"
fi

echo -e "\e[33mOpening a server\e[0m"
./"$1" "$2"
echo -e "\e[32mExited with value : "$?"\e[0m"

echo -e "\e[33mOpening a server with the dry_run option\e[0m"
./"$1" --dry-run "$2"
echo -e "\e[32mExited with value : "$?"\e[0m"
