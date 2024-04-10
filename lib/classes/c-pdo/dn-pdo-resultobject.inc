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


class dpdo_result extends dpdo_init
{

	public $debug			  = __CLASS__;


	function __construct($type)
	{

		$this->dbtype=$type;

	}

	function __destruct()
	{

		$this->close_query();

	}

	public function query(&$pdo, $text, $args = null)
	{

		$ret=false;

		$this->lastquery = $text;
		$this->args = $args;

		try {

			$this->checkPDO($pdo);

			$this->stmt = $pdo->prepare($text);

			$this->stmt->execute($args);

			$this->id = $pdo->lastInsertId();

			$this->rows = $this->stmt->rowCount();

			$this->db_error		 = false;
			$this->db_errno		 = false;

			if( $this->verbose ) $this->logger("Info Query: type/".$this->dbtype." id/".$this->id." rows/".$this->rows." db_error/".$this->db_error." db_errno/".$this->db_errno, 3);


			$ret=true;

		} catch (Throwable $e) {

			$this->setDBError($e);

		}

		return $ret;
	}



	public function fetch()
	{
		$ret=null;

		try
		{

			$this->checkSTMT($this->stmt);

			$ret = $this->stmt->fetch(PDO::FETCH_ASSOC);

		} catch (Throwable $e) {

			$this->setDBError($e);

		}

		return $ret;

	}

	public function fetchall()
	{
		$ret=null;

		try
		{
			$this->checkSTMT($this->stmt);

			$ret = $this->stmt->fetchall(PDO::FETCH_ASSOC);

			$this->rows = count($ret);

		} catch (Throwable $e) {

			$this->setDBError($e);
		}

		return $ret;

	}

	public function fetchqueryall(&$pdo, $text, $args = null)
	{
		$this->db_errno = false;
		$this->db_error = false;

		try
		{
			$result = array();

			$result["cnt"] = 0;
			$result["data"] = array();

			if(! $this->query($pdo, $text, $args) )
				$this->StopAndThrow(8);

			$result["lastquery"] = $text;
			$result["id"] = $this->id;

			$result["data"] = $this->stmt->fetchAll();
			$this->rows = $result["cnt"] = count($result["data"]);


		} catch (Throwable $e) {

			$this->setDBError($e);

			$result=false;
		}

		$this->close_query();

		return $result;

	}
}

?>