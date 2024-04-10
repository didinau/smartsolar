#!/bin/bash

wdir="/usr/local/energie"
wlog="$wdir/log/smart.log"

cd $wdir/log

if [ "$1" != "daemon" ]; then
	su -c "$0 daemon" myadmin
else
	(
	exec 2>&1
	exec 1>> $wlog
	exec 0</dev/null

	export PATH="/usr/local/bin:$PATH"

	#PHP="php-dist"
	PHP="php"

	while true
	do
		if [ -s $wdir/log/smart.log ]; then
			cp $wlog ${wlog}.old
		fi
		>$wlog
		$PHP $wdir/bin/smart.php 1>>$wlog 2>&1
		sleep 10
	done
	) &
fi
