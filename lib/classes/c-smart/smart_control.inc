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


class ControlData
{
	public $CONFIG;
	public $STATIC;
	public $json;
	public $DB;
	public $DBFB;
	public $newData;
	public $prognose;
	public $WEATHER;

	function __sleep()
	{
		return array("CONFIG", "STATIC", "json", "newData", "prognose","WEATHER",);
	}

	public function __set($_name, $_value)
	{

		$this->logger("set unknown property ".$_name, 0,__class__);

		return false;

	}

	public function getSize()
	{
		return strlen(serialize($this));
	}
}


class ControlDatacenter implements ArrayAccess
{
	private $debug = __class__;
	private $container	=array();
	private $db			=null;
	private $dbfb		=null;

	function __construct(&$_db = null, &$_dbfb = null)
	{

		$this->db = &$_db;
		$this->dbfb = &$_dbfb;

	}

	function __destruct()
	{

	}

	function __sleep()
	{
		return array("container");
	}

	private function logger(string $_msg, int $_level=3): void
	{

		if( $this->debug )
			new LogMsg($_level, $this->debug, $_msg);

	}


	public function offsetExists (mixed $offset):bool
	{

		return isset($this->container[$offset]);

	}

	public function &offsetGet (mixed $offset):mixed
	{

		if(! isset($this->container[$offset]) )
			$this->container[$offset]=array();

		return $this->container[$offset];
	}


	public function offsetSet (mixed $offset, mixed $value):void
	{

		if( is_null($offset) )
		{

			$this->container[] = $value;

		}else{

			$this->container[$offset] = $value;

		}

	}

	public function offsetUnset (mixed $offset):void
	{

		if( isset($this->container[$offset]) )
		{

			unset($this->container[$offset]);

		}

	}

	public function save($_ts)
	{

		$iM=new dpdoInsertMgr("data", DPDO::REPLACE);

		$iM->addColumn("data_type", "staticdata");

		$iM->addColumn("data_ts", $_ts);

		$iM->addColumn("data_raw", mf::json_enc($this->container));

		$iM->commit();

		if( $this->db )
		{

			$check = $iM->exec($this->db);

			if(! $check )
			{
				print_r($iM);

				$this->logger("error replace static data", 0);

			}
		}

		if( $this->dbfb )
		{

			$check = $iM->exec($this->dbfb);

			if(! $check )
			{
				print_r($iM);

				$this->logger("error replace static data in fallback-db", 0);

			}
		}

		return true;

	}

	public function load()
	{

		$query="select data_raw from data where data_type='staticdata'";

		$line = array();

		if(! is_null($this->dbfb) )
		{

			$stmt = $this->dbfb->query($query);

			if(! $stmt->db_error )
			{

				$line = $stmt->fetch();

			}

		}

		if( (! isset($line["data_raw"]) ) and (! is_null($this->dbfb)) )
		{

			$stmt = $this->db->query($query);

			if(! $stmt->db_error )
			{

				$line = $stmt->fetch();

			}
		}

		if( isset($line["data_raw"]) )
		{

			$this->container = mf::json_dec($line["data_raw"]);

			return true;
		}

		return false;
	}


}

?>
