<?php


/* 240424
 *
 *
 * Copyright 2024 Dieter Naujoks <dnaujoks@naujoks.homeip.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */



function net_connect()
{
	global $net;
	global $smart;

	global $net_busy;

	static $delay;

	if( ($now = microtime(true)) > $delay and !$net_busy )
	{
		$delay = $now + 0.01;

		$net_busy = true;

		if( $net["obj"] and $sock_connect = $net["obj"]->waitconnect($net["socket"]) )
		{

			$remote = $net["obj"]->remoteaddr($sock_connect);
			$userIP = $net["obj"]->read_timeout($sock_connect, 500);

			new LogMsg(3,"Conn",$userIP." via ".$remote["addr"]);

			if( ($smart->controlData->json) )
				$net["obj"]->write_timeout($sock_connect, mf::gzip_dec( $smart->controlData->json ),1000);

			$net["obj"]->close($sock_connect);

			unset($sock_connect);


		}

		$net_busy = false;
	}

}

function sig_handler($signo)
{

	global $smart;

	switch ($signo)
	{
		case SIGTERM:

			new LogMsg(0,"RSIG","SIGTERM received");

			// Aufgaben zum Beenden bearbeiten

			$smart->saveStaticData(true);

			$smart->proto("SIGHUP", "smart", "exit");

			new LogMsg(0,"RSIG","beende...");

			exit(0);

			break;
	}
}

function configOverwrite($_index, $_value)
{

	global $smart;

	if( isset($smart) )
	{

		$smart->configEntryOverwrite($_index, $_value);

		new LogMsg(3, "Cnfg", "overwrite ".$_index);

	}

}

function updatePrognose()
{

	global $smart;

	if( isset($smart) )
	{

		$smart->updatePrognose();

	}
}

?>
