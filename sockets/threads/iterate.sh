#!/bin/bash

COUNTER=50
MAX_COUNTER=450
COUNTER_INCREMENT=50
PORT=23456
SERVER="localhost"
CLIENTS=50
touch result.csv
echo $CLIENTS" clients" >> result.csv
echo "max concurrent clients,iterations,time" >> result.csv
while [ $COUNTER -lt $MAX_COUNTER ]; do
	./launchClient $CLIENTS $COUNTER $SERVER $PORT
	TEMP=$(tail -1 "test/Client_"$CLIENTS"_Messages_"$COUNTER"/launch_info")
	TEMP=$(echo $TEMP | cut -d ' ' -f 2)
	TEMP1=$(tail -n2 "test/Client_"$CLIENTS"_Messages_"$COUNTER"/launch_info" | head -n1)
	TEMP1=$(echo $TEMP1 | cut -d ':' -f 2)
	echo $TEMP1","$COUNTER","$TEMP >> result.csv
	let COUNTER=COUNTER+$COUNTER_INCREMENT
done

grep -rnw './test' -e 'Error'
