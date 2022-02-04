#!/bin/bash
ON_A_CALL=0
SLEEP_TIME=15

HUE_IP="" # The IP Address of your esp board (should display on LED matrix on start up)
HUE_API_KEY="" # Philips Hue Username/API key (https://developers.meethue.com/develop/get-started-2/)
URL_SUFFIX="/api/${HUE_API_KEY}/lights/31/state" # 31 (in my case) being the light Id to update
URL_FULL="http://${HUE_IP}${URL_SUFFIX}"

BLUE_LIGHT_DATA='{"on":true,"sat":254,"bri":254,"hue":45000}'
GREEN_LIGHT_DATA='{"on":true,"sat":254,"bri":254,"hue":27500}'
ORANGE_LIGHT_DATA='{"on":true,"sat":254,"bri":254,"hue":5000}'

PROGRAM="BlueJeans|zoom.us"



echo "starting up..."
curl -X PUT $URL_FULL -H "Content-Type: application/json" -d $ORANGE_LIGHT_DATA

while true
do
	numUDP=`lsof -iUDP | grep -E $PROGRAM | wc -l`
	echo "...."
	if (($numUDP > 1)) #on a call
	then
		if (($ON_A_CALL != 1))
		then
			echo "Just joined a call"
			curl -X PUT $URL_FULL -H "Content-Type: application/json" -d $BLUE_LIGHT_DATA

			ON_A_CALL=1
		fi
	else
		if (($ON_A_CALL != 0))
		then
			echo "Just hung up... $numUDP"
			curl -X PUT $URL_FULL -H "Content-Type: application/json" -d $ORANGE_LIGHT_DATA

			ON_A_CALL=0
		fi
	fi
	sleep $SLEEP_TIME;
done;