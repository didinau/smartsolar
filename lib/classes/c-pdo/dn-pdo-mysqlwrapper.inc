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
 /


USAGE:

Mysql / MariaDB:

$DB = new dpdo_mysql("dbhost", "dbuser", "dbpwd", "dbname");


*/


class dpdo_mysql extends dpdo_engine
{

	public function __construct($host="127.0.0.1", $user=false, $pass=false, $db=false, $charset="", $port=3306)
	{
		if( !$db or !$user or !$pass)
		{

			new LogMsg(0, __class__."::".__function__, "username, password or dbname not  set!");

			exit(1);

		}

		parent::__construct(DPDO::MARIADB);

		parent::init($host, $user, $pass, $db);
	}

	public function __destruct()
	{

		parent::__destruct();

	}
}

?>