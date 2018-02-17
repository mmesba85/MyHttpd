#!/bin/bash

#first argument is the executable
#second argument is the server config file
#if [ "$#" -ne 2 ]; then
#  echo "Usage : ./test.sh [executable] [server_config]"
#fi

echo -e "\e[33mTesting a get request with curl and a good port\e[0m"
echo "$ curl -i -v http://127.0.0.1:4242"
curl http://127.0.0.1:4242 2> /dev/null
echo -e "\e[32mExited with value : $?\e[0m"

echo -e "\e[33mTesting a get request with curl\e[0m"
echo "$ curl -i -v http://127.0.0.1:4242/home.html"
curl http://127.0.0.1:4242/home.html 2> /dev/null
echo -e "\e[32mExited with value : $?\e[0m"

echo -e "\e[33mTesting a get request with curl; a cgi request\e[0m"
echo "$ curl -i -v http://127.0.0.1/script.php?var1=abc&var2=4"
curl http://127.0.0.1/script.php?var1=abc&var2=4 2> /dev/null
echo -e "\e[32mExited with value : $?\e[0m"




