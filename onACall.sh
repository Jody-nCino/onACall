#!/bin/bash
ON_A_CALL=0
SLEEP_TIME=30
SSID="192.168.86.78"

echo "starting up..."

while true
do
	numUDP=`lsof -iUDP | grep BlueJeans | wc -l`
	if (($numUDP > 1)) #on a call
	then
		if (($ON_A_CALL != 1))
		then
			echo "Just joined a call"
			echo "Firing webhook to esp"
			curl -X POST http://$SSID/onACall

			ON_A_CALL=1
		fi
	else
		if (($ON_A_CALL != 0))
		then
			echo "Just hung up... $numUDP"
			echo "Firing webhook to esp"
			curl -X POST http://$SSID/offACall

			ON_A_CALL=0
		fi
	fi
	sleep $SLEEP_TIME;
done;