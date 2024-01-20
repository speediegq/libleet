#!/bin/sh

printf "Enter your username\n> "
read -r username
printf "Enter your password\n> "
read -r password

curl -X POST -H "Content-Type: application/json" -d "{\"username\": \"$username\",\"password\": \"$password\"}" http://localhost:8080/api/login
