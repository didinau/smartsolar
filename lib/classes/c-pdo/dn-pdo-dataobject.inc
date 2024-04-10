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

Mysql / MariaDB: $DB = new dpdo_mysql("dbhost", "dbuser", "dbpwd", "dbname"); | Sqlite: $DB = new dpdo_sqlite; $DB->init("FileName");


Query mit Datenobjekt dpdoDO:

$result = $DB->getAll("select * from x where a = 1 and b = 2"); ($DB->getAll("select * from x where a = :werta and b = :wertb", [ ":werta" => 1 , ":wertb" => 2 ] );)


*/


class dpdoDO
{

	public $debug = __CLASS__;
	public $id = false;
	public $lastquery = false;
	public $db_error = false;
	public $db_errno = false;
	public $rows = 0;

	private $data = array();


	function __construct(&$result)
	{
		if( is_array($result) )
		{
			$this->data = $result;
			$result = null;
			$this->rows = count($this->data);
		}else{
			return false;
		}
	}

	function __destruct()
	{

	}

	public function fetchall()
	{
		if( count($this->data) )
		{
			$ret = $this->data;
			$this->data = array();
			$this->rows = 0;
		}else{
			$ret = false;
		}
		return $ret;
	}

	public function fetch()
	{
		if( count($this->data) )
		{
			$ret = array_shift($this->data);
			$this->rows = count($this->data);
		}else{
			$ret = false;
		}
		return $ret;
	}
}

?>