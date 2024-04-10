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


class dpdoInsertMgr
{
	public $debug = __CLASS__;

	protected $container = array();

	protected $cache = null;

	protected $error = "";

	function __construct(string $tabname="", bool $replace = false)
	{

		if( strlen($tabname) )
			$this->newInsert($tabname, $replace);

	}

	public function logger(string $_msg, int $_level=3): void
	{

		if( $this->debug )
		{

			new LogMsg($_level, $this->debug, $_msg);

		}

	}

	public function reset()
	{

		$this->container = array();

		$this->cache = null;

	}

	public function getData()
	{

		if( count($this->container) )
		{

			return $this->container;

		}else{

			return false;

		}
	}

	public function setData(array $data)
	{

		if( count($data) )
		{

			$this->container = $data;

			return true;

		}else{

			return false;

		}
	}

	public function getResult()
	{

		if(! count($this->container))
		{
			return $this->error;
		}

		$ret = $this->container;

		foreach($ret as $idx => $insertdata)
		{

			$ret[$idx]["values count"] = count($insertdata["values"]);

		}

		return $ret;
	}

	public function exec(object &$db, bool $delete_on_success = false)
	{
		$ret = true;

		if( is_array($this->container) and count($this->container) )
		{

			$this->error = "";

			foreach($this->container as $idx => $insertdata)
			{
				$stmt = $db->insert($insertdata["tabname"], $insertdata["collist"], $insertdata["vallist"], $insertdata["values"], $insertdata["replace"] ? "replace" : "insert");

				$this->container[$idx]["db_error"] = $stmt->db_error;
				$this->container[$idx]["db_errno"] = $stmt->db_errno;
				$this->container[$idx]["querry"] = $stmt->lastquery;
				$this->container[$idx]["id"] = $stmt->id;

				if( strlen($stmt->db_error) )
				{

					$this->error .= $stmt->db_error."|";

					$ret = false;

				}else{

					if( $delete_on_success )
					{

						unset($this->container[$idx]);

					}
				}
			}

		}else{

			$this->logger("no data to execute", 0);

			return false;

		}

		return $ret;

	}

	protected function prepareCache(): void
	{

		$this->cache = array( "tabname" => "", "replace" => false, "data" => array() );

	}

	public function newInsert(string $tabname, bool $replace = false)
	{
		$tabname = trim($tabname);
		if( strlen($tabname) )
		{
			if( is_null($this->cache) )
				$this->prepareCache();

			$this->cache["tabname"] = $tabname;

			$this->cache["replace"] = $replace;

		}else{

			$this->logger("tabname not set", 0);

			return false;

		}
	}

	public function addMultibleColumns(array $values, string $comma_sep_cols = "")
	{

		if( strlen($comma_sep_cols) == 0 )
		{

			if( count($values) )
			{

				foreach($values as $idx => $val)
				{

					$this->addColumn(trim($idx), $val);

				}
			}else{

				$this->logger("no data in values", 0);

			}
		}else{

			$cols = explode(",", $comma_sep_cols);

			if( count($cols) == count($values) )
			{

				$vals = array();

				foreach($values as $idx => $val)
					$vals[] = $val;

				foreach($cols as $idx => $col)
				{
					$this->addColumn(trim($col), $vals[$idx]);
				}

			}else{

				$this->logger("count difference between columns and values: ".count($cols)." : ".count($values), 0);

			}
		}
	}


	public function addColumn(string $colname, $value)
	{
		if( substr($colname,0,1) == ":" )
			$colname = substr($colname,1);

		$this->cache["data"][$colname] = $value;
	}

	public function commit(): bool
	{
		if( ! is_array($this->cache) or ! isset($this->cache["tabname"]) or strlen($this->cache["tabname"]) == 0 )
		{

			$this->logger("tabname not set", 0);

			return false;

		}

		if( is_array($this->cache) and isset($this->cache["data"]) and  count($this->cache["data"]) )
		{

			$newinsert = array("tabname" => $this->cache["tabname"], "replace" => $this->cache["replace"], "collist" =>array(), "vallist" =>array(), "values" => array());

			foreach($this->cache["data"] as $col => $value)
			{

				$newinsert["collist"][] = $col;

				$newinsert["vallist"][] = ":".$col;

				$newinsert["values"][":".$col] = $value;

			}

			$newinsert["collist"] = implode(", ", $newinsert["collist"]);

			$newinsert["vallist"] = implode(", ", $newinsert["vallist"]);

			$this->container[] = $newinsert;

			$this->cache = null;

		}else{

			$this->logger("array contains no data", 0);

			return false;

		}

		return true;

	}
}

?>