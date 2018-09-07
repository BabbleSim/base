#! /usr/bin/env bash
# Stop all BabbleSim processes, finding them from their lock files.
# Also remove the folder used to keep the locks and fifos
#
# A simulation identification string may be provided as the first paramter to
# this script to limit it to only clean up for that simulation  

s=$1
FOLDER=/tmp/bs_$USER/$s/

if [ ! -d "$FOLDER" ]; then
	echo "Not even the folder $FOLDER was found"
	exit 0
fi

function remove_folder() {
	rm -rf -v $FOLDER 
}

FOLDER=`realpath $FOLDER`

LOCKS=`find $FOLDER -name "*.lock"`

if [ ! -n "$LOCKS" ]; then
	echo "No BabbleSim processes found"
	remove_folder
	exit 0
fi

for LOCK in $LOCKS; do
	if [ ! -f  $LOCK ]; then
		echo "$LOCK dispeared as we cleaned up"
		#As we kill components of a simulation other many get unblocked and clean up on their own
		continue
	fi
	PID=`head -n1 $LOCK`
	START=`tail -n1 $LOCK`
	DEAD=0
	kill -0 $PID 2> /dev/null
	if [[ $? -ne 0 ]] ; then
		DEAD=1
	fi
	if [[ $DEAD -eq 0 ]]; then
		OTHERSTART=`cat /proc/$PID/stat | cut -d')' -f2 | cut -d ' ' -f21`
		if [[ ! $OTHERSTART -eq $START ]]; then
			DEAD=1
		fi
	fi
	if [[ $DEAD -eq 1 ]]; then
		echo "Found dead process $PID"
	else
		echo -n "Found "
		ps --no-headers --pid $PID --format pid,lstart,command
		echo "Killing it"
		kill -9 $PID
	fi
	rm -v $LOCK
done

remove_folder
