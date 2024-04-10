<?php

/*
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


// let's start

try{

	$loglevel = 0;

	$include = __DIR__."/../lib/nloader.php";

	if( is_readable($include) )
	{

		include $include;

	}else{

		throw new ErrorException("Error: No such file or directory: ".$include, 0, E_ERROR);

	}

	unset($include);

	print "\n\n***************** Start ".__FILE__." *****************\n";

	print "Version: ".phpversion()."\nExtensions:\n".implode(", ",get_loaded_extensions())."\n\n";


	while( true)
	{

		$smart = new smart($config_file);

		$smart->proto("START", "smart", "up");

		$smart->setupPCNTL();

		$smart->setupSocketServer();

		$smart->main();

		unset($smart);

	}

}catch(Throwable $e){

	error_log("Smart-Error: ".$e->getMessage()."\nDatei: ".$e->getFile()."\nZeile: ".$e->getLine());

	sleep(30);

	exit(1);

}

?>