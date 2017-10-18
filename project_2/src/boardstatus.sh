#!/bin/bash

for f in $1/*
do
    if [[ -d $f ]]
    then
        # $f is a directory
        for file in $f/*; do
        	# If there is the *_Server_PID.txt file check the pid which is written in this file
			if [[ $file == *"_Server_PID.txt" ]]
			then
				# Copy the PID which is written in the file to PID variable
				PID=$(<$file)
				# Check the PID if it is running
				if ps -p $PID > /dev/null
				then
					numberOfActive=$((numberOfActive+1))
					active="$active $f " 
					active="$active with PID: "
					active="$active $PID"
				else
					numberOfInactive=$((numberOfInactive+1))
					inactive="$inactive $f " 
					inactive="$inactive"
				fi
			fi
    	done
    fi
done
echo "$numberOfActive Boards Active"
echo "$active"
echo "$numberOfInactive Boards Active"
echo "$inactive"