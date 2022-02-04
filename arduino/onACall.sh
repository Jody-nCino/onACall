#!/bin/bash
ON_A_CALL=0
SLEEP_TIME=15
ESP_IP="insertIPAddressHere" # The IP Address of your esp board (should display on LED matrix on start up)
PROGRAM="BlueJeans|zoom.us"

echo "starting up..."

while true
do
	numUDP=`lsof -iUDP | grep -E $PROGRAM | wc -l`
	echo "...."
	if (($numUDP > 1)) #on a call
	then
		if (($ON_A_CALL != 1))
		then
			echo "Just joined a call"
			curl -X POST http://$ESP_IP/onACall

			ON_A_CALL=1
		fi
	else
		if (($ON_A_CALL != 0))
		then
			echo "Just hung up... $numUDP"
			curl -X POST http://$ESP_IP/offACall

			ON_A_CALL=0
		fi
	fi
	sleep $SLEEP_TIME;
done;