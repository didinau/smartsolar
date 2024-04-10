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

class dpdo_engine extends dpdo_init
{

	public $debug				= __CLASS__;

	protected $pdo				= null;
	protected $startTS			= false;

	protected $sql_str_insert	= array(

							DPDO::SQLITE => array(
									"i"	=> "insert into _tabnam__ (_fields__) values (_vals__)",
									"r"	=> "insert or replace into _tabnam__ (_fields__) values (_vals__)",
									),

							DPDO::MYSQL => array(
									"i"	=> "insert into _tabnam__ (_fields__) values (_vals__)",
									"r"	=> "replace into _tabnam__ (_fields__) values (_vals__)",
									),
								);

	function __destruct()
	{

		$this->close();

	}

	function __construct($_type=DPDO::MYSQL, $_options = null, $_semaphore = null, $_debug = true)
	{

		if(! $_debug )
			$this->debug = false;

		if(! is_null($_semaphore) )
		{
			$this->sem = $_semaphore;
		}

		$dbtyp = strtolower(trim($_type));

		if(! is_array($_options) )
			$_options=array();

		$this->dboptions = array_replace($this->def_options, $_options);

		switch( $dbtyp )
		{

			case DPDO::MARIADB:
			case DPDO::MYSQL:
			case "mariadb":
			case "mysql":
				$dbtyp=DPDO::MARIADB;
				break;

			case DPDO::SQLITE:
			case "sqlite":
				$dbtyp=DPDO::SQLITE;
				break;

			case DPDO::POSTGRES:
			case "postgres":
				$dbtyp=DPDO::POSTGRES;
				break;

			default:

				$this->logger("nicht unterstützer Datenbanktyp ".$_type, 0);

				return false;
		}

		$this->dbtype = $dbtyp;

	}

	public function reconnect(): bool
	{
		try
		{

			$this->pdo = new PDO($this->dsn, $this->udsn["u"], $this->udsn["p"], $this->dboptions);

			$this->startTS=time();

		} catch (Throwable $e) {

			$this->setDBError($e);

			return false;
		}

		return true;
	}

	public function init($_host_or_filename, $_user=false, $_pass=false, $_dbname=false, $_port=3306)
	{

		$host = trim($_host_or_filename);

		if( strlen($host) == 0 )
		{
			$this->logger("DSN-Host not set",0);

			return false;
		}

		try
		{

			switch( $this->dbtype )
			{

				case DPDO::MARIADB:
				case DPDO::MYSQL:

					$this->dsn = "mysql:host=".$host.";dbname=".trim($_dbname).";port=".trim($_port).";charset=utf8mb4";
					$this->udsn = array( "u" => trim($_user), "p" => $_pass );
					break;

				case DPDO::SQLITE:

					$this->dsn = "sqlite:".$host;
					$this->udsn = array( "u" => null, "p" => null );

					break;

				default:
					$this->logger("nicht unterstützer Datenbanktyp ".$this->dbtype, 0);
					return false;
					break;
			}

			$this->reconnect();

		} catch (Throwable $e) {

			$this->setDBError($e);

			return false;
		}

		return true;

	}

	public function query($text, $args = null)
	{

		if( $this->sem )
		{
			$this->sem->lock();
		}

		$ret = new dpdo_result($this->dbtype);

		$ret->query($this->pdo, $text, $args);

		if( $this->sem )
		{
			$this->sem->release();
		}

		return $ret;
	}

	public function insert($tabname, $fieldlist, $valueslist, $values, $mode="insert")
	{

		$repl = array ("_tabnam__" => $tabname);

		if( is_array($fieldlist) )
			$fieldlist = implode(", ", $fieldlist);

		if( is_array($valueslist) )
			$valueslist = implode(", ", $valueslist);

		$repl["_fields__"] = $fieldlist;
		$repl["_vals__"] = $valueslist;

		$mode = strtolower(substr($mode,0,1));

		if( isset($this->sql_str_insert[$this->dbtype][$mode]) )
		{

			$query = strtr($this->sql_str_insert[$this->dbtype][$mode], $repl);

			$ret = $this->query($query, $values);

		}else{

			$dummy = array();

			$ret = new dpdoDO($dummy);

			$ret->db_errno = 2;

			$ret->db_error = $this->errormsgs[$ret->db_errno];

		}

		return $ret;

	}

	public function replace($tabname, $fieldlist, $valueslist, $values)
	{

		return $this->insert($tabname, $fieldlist, $valueslist, $values, "replace");

	}

	public function getAll($text, $args = null)
	{

		$res = $this->fetchqueryall($text, $args);

		if( isNotFalse($res) )
		{

			$ret = new dpdoDO($res["data"]);

			$ret->lastquery = $res["lastquery"];

			$ret->id = $res["id"];

		}else{

			$dummy = array();

			$ret = new dpdoDO($dummy);

			$ret->db_errno = 6;

			$ret->db_error = $this->errormsgs[$ret->db_errno];

		}

		return $ret;
	}

	public function fetchqueryall($text, $args = null)
	{

		if( $this->sem )
		{
			$this->sem->lock();
		}

		$ret = new dpdo_result($this->dbtype);

		$result = $ret->fetchqueryall($this->pdo, $text, $args);

		if( $this->sem )
		{
			$this->sem->release();
		}

		return $result;
	}

	protected function begin_transaction()
	{

		$this->pdo->beginTransaction();

	}

	protected function end_transaction()
	{

		$this->pdo->commit();

	}

	protected function roll_back()
	{

		$this->pdo->rollBack();

	}

	public function quote($_str)
	{

		try
		{

			$ret =  $this->pdo->quote($_str);

		} catch(Throwable $e) {

			switch($this->dbtype)
			{

				case "mariadb":
					$ret =  "'".addcslashes($_str,"\\\0\n\r'\x1a")."'";
					break;

				default:
					$ret = false;
					break;

			}

		}

		return $ret;
	}

	public function connection_time()
	{
		return intval(time() - $this->startTS);
	}
}

?>