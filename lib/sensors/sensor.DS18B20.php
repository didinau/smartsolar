<?php

/*	240414
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

class DS18B20 extends sensor
{

	public $devids	= "/sys/bus/w1/devices/w1_bus_master1/w1_master_slaves";
	public $devpath	= "/sys/bus/w1/devices/";


	function __construct($_port, $_addr, $_res)
	{

		parent::__construct($_port, $_addr, $_res);

	}

	function __destruct()
	{

		parent::__destruct();

	}

	public function getconf()
	{

		$this->getParam();

		if(! isset($this->opt["avg"]) )

			$this->opt["avg"] = 1;

		else

			$this->opt["avg"] = intval($this->opt["avg"]);

		if( $this->opt["avg"] < 1 ) $this->opt["avg"] = 1;

		if( isset($this->opt["debug"]) and intval($this->opt["debug"]) == 1 )
		{

			$this->debug = "Debg: DS18B20";

		}

	}

	public function query()
	{

		$this->getconf();

		$ds = explode(",", $this->port);

		if( count($ds) )
		{

			$slaves = file_get_contents($this->devids);

			$slaves = explode("\n", $slaves);

			$erg = array();

			foreach( $ds as $devid )
			{

				$devid = trim($devid);

				if( in_array($devid, $slaves) )
				{

					$temp = 0;

					for( $i = 0; $i < $this->opt["avg"]; $i++)
					{

						$temp += intval( file_get_contents($this->devpath . $devid . "/temperature") );

					}

					$temp = $temp / $this->opt["avg"];

					if( isset($this->adjust["temperature"][$devid]) )

						$atemp = $temp + floatval($this->adjust["temperature"][$devid]);

					else

						$atemp = $temp;

					$dev = $devid;

					$erg[$dev]["atemp"]	= floatval(round($atemp / 1000, 1));

					$erg[$dev]["temp"]	= floatval($temp  / 1000);

					$this->logger($dev . ": " . $this->opt["avg"] . " Messung(en): " . $erg[$dev]["temp"] . ", korr. " . $erg[$dev]["atemp"]);

				}else{

					$this->logger(__function__ . ": sensor " . $devid . " not in slave table!");

					return sensor::RET_ERROR;
				}
			}

			if( count($erg) == 0 ) return sensor::RET_ERROR;

			$this->result["a_temperature"]	= $erg;

			$this->result["publishT"]		= true;

			$this->result["publishP"]		= false;

			$this->result["publishE"]		= false;

			return sensor::RET_DATA;

		}else{

			return sensor::RET_NODATA;

		}
	}
}

?>
