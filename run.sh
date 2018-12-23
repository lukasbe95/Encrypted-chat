#!/bin/bash
echo "Specify port on which you want to run chat."
read port
export EC_PORT=$port
echo "Choose encryption library. For Botan type 'b', for OpenSLL type 'o'"
read encrypt
if [ "$encrypt" = "b" ] ; then
    cd ./botan/
    xterm ./server &
    xterm ./client &
    echo "Botan"
elif [ "$encrypt" = "o" ] ; then
    cd ./opensll/
    xterm -e ./server.o $port&
    xterm -e ./client.o "localhost" $port&
    echo "OpenSLL"
else
    echo "Wrong input."
fi
