<?php

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
