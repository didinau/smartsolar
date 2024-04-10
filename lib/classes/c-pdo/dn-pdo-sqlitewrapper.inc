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

Sqlite:
$DB = new dpdo_sqlite; $DB->init("FileName");


*/

class dpdo_sqlite extends dpdo_engine
{

	public function __construct($_debug="", $_semaphore=null)
	{

		if( parent::__construct(DPDO::SQLITE, null, $_semaphore, $_debug) )
		{

			return true;

		}else{

			return false;
		}

	}

	public function init($_host_or_filename, $_user=false, $_pass=false, $_dbname=false, $_port=false)
	{

		return parent::init($_host_or_filename);

	}

	public function __destruct()
	{

		parent::__destruct();

	}
}

?>