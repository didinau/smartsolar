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

Query regulär:

$result = $DB->query("select * from x where a = 1 and b = 2");
oder
$result = $DB->query("select * from x where a = :werta and b = :wertb", [ ":werta" => 1 , ":wertb" => 2 ] );

$result ist Objekt dpdo_result

Daten weden gelesen mit $result->fetch() oder $result->fetchall()


Query mit Datenobjekt dpdoDO:

$result = $DB->getAll("select * from x where a = 1 and b = 2"); ($DB->getAll("select * from x where a = :werta and b = :wertb", [ ":werta" => 1 , ":wertb" => 2 ] );)





Insert-Manager:

$iM=new dpdoInsertMgr("data", true);

	# Entweder so:
		$iM->addMultibleColumns([ "staticdata", $_ts, $raw_data) ], "data_type, data_ts, data_raw");

	# oder so:
		$iM->addMultibleColumns( [ "data_type" => "staticdata", "data_ts" => $_ts, "data_raw" => $raw_data ] );

	# oder einzeln:

	$iM->addColumn( "data_type", "staticdata" );
	$iM->addColumn( "data_ts", $_ts );
	$iM->addColumn( "data_raw", $raw_data );

	# Datensatz festschreiben:

	$iM->commit();



	# Insert durchführen:

	$check = $iM->exec($this->db);

	# $check === true: alles OK, ansonsten ist $check === false

	//print_r($iM->getResult());

*/



class DPDO
{
	public const REPLACE = true;
	public const INSERT = false;

	public const MYSQL = "1";
	public const MARIADB = "1";
	public const SQLITE = "2";
	public const POSTGRES = "3";
}

class dpdo_init
{

	protected $stmt				 = null;

	protected $dsn				= false;
	protected $udsn				= false;
	protected $dbtype			= false;
	protected $dboptions		= array();


	protected $semkey			= null;
	protected $sem				= null;

	protected $errormsgs		=array(
								0 => "all fine",
								2 => "no template for insert or replace",
								6 => "dpdoDO query failed",
								7 => "db not connected",
								8 => "Statement not set",
								);

	protected $def_options		= array(
								PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
								PDO::ATTR_EMULATE_PREPARES => false,
								PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
								);

	public $rows				= 0;	// Letzte Anzahl Treffer
	public $id					= false;

	public $lastquery			= false;
	public $args				= false;


	public $db_errno			= false;	// Letzte Datenbank-Fehlernummer
	public $db_error			= false;	// Letzte Datenbank-Fehlermeldung

	public $debug				= __CLASS__;
	public $verbose				= false;


	public function __set($_name, $_value)
	{

		$this->logger("set unknown property ".$_name, 0);

		return false;

	}

	public function __get($_name)
	{
		switch($_name)
		{
			case "lastinsertid":
				return $this->id;
				break;

			default:
				$this->logger("unknown property ".$_name, 0);
				return false;
		}
	}

	protected function checkPDO($pdo)
	{

		if( is_null($pdo) )
		{
			$this->StopAndThrow(7);
		}

		return true;
	}

	protected function checkSTMT($stmt)
	{

		if( is_null($stmt) )
		{
			$this->StopAndThrow(7);
		}

		return true;
	}

	public function StopAndThrow($err)
	{
		throw new Exception($this->errormsgs[$err], $err);
	}

	public function logger($_msg, $_level=3)
	{

		if( $this->debug )
		{

			new LogMsg($_level, $this->debug, $_msg);

		}

	}

	public function close()
	{
		$this->stmt = null;
		$this->pdo  = null;
	}

	public function close_query()
	{
		$this->stmt = null;
	}

	public function setDBError($e)
	{
		$this->db_errno = intval($e->getCode());
		$this->db_error = $e->getMessage();

		if( $this->db_error and isFalse($this->db_errno) )
		{
			$this->db_errno = -4;
		}

		if( strlen($this->lastquery) )
			$this->logger("Query: ".substr($this->lastquery, 0, 80),4);
		$this->logger("dpdo-error errno: ".$this->db_errno.", ".$this->db_error,0);
	}

}

?>