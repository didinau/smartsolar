<?php
/*
 *
 * Copyright 2020 Dieter Naujoks <devops@service.istmein.de>
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
abstract class sensor extends bin_dev_functions
{

	public		$smart			=false;

	protected	$modbus			=false;
	protected	$pspeed			=9600;
	protected	$pparam			="8n1";
	protected	$port;
	protected	$addr;
	protected	$debug			=false;

	protected	$opt			=array();

	public		$adjust			=false;
	public		$ID				=false;
	public		$result			=array();
	public		$newday			=false;
	public		$ndata			=false;
	public		$conf			=false;
	public		$type			=false;

	public const RET_NODATA		=false;
	public const RET_ERROR		=-1;
	public const RET_DATA		=true;

	abstract function query();

	public function __construct($_port, $_addr, $_res)
	{

		$this->result	= $_res;
		$this->port		= $_port;
		$this->addr		= $_addr;

	}

	public function __destruct()
	{

		if( $this->modbus )
		{
			$this->modbus = false;
		}

	}

	protected function log_NoResponse(string $class, string $port): void
	{

		new LogMsg(0, $class, "Error Communicating With Device " . $port);

	}

	protected function logger($_msg, $_level=3)
	{

		new LogMsg($_level, $this->debug, $_msg);

	}

	protected function getParam()
	{

		$this->opt = smart::smartGetParam($this->conf);

		return;

	}

	public function init(&$_smart, $_ID, $_type, $_conf)
	{

		$this->smart	= &$_smart;

		$this->type		= $_type;
		$this->conf		= $_conf;
		$this->ID		= $_ID;

		$this->adjust	= $this->smart->CONFIG["adjust"];
		$this->ndata	= &$this->smart->newData;

		if( isset($this->smart->STATIC["last"][$this->ID]["D"]) )
		{

			if( $this->smart->STATIC["last"][$this->ID]["D"] != $this->result["D"] )
				$this->newday = true;

		}else{

			$this->newday = false;

		}
	}

	protected function mbOpen()
	{

		if(! $this->modbus )
		{

			$this->modbus = new COM_modbus();

		}

		return $this->modbus->devOpen("/dev/".$this->port, $this->pspeed, $this->pparam);
	}

	protected function mbQuery($_regm, $_start, $_len, $_read = true, $_setreg = true)
	{

		if( $this->modbus )
		{

			return $this->modbus->modbusQuery($this->addr, $_regm, $_start, $_len, $_read, $_setreg);

		}

		return false;
	}

	protected function mbWriteHoldingReg($_register, $_val)
	{

		return $this->mbQuery(6, $_register, $_val, true, false);

	}

	protected function mbReadHoldingReg($_register, $_anzahl = 1)
	{

		return $this->mbQuery(3, $_register, $_anzahl, true, true);

	}

	protected function mbReadInputReg($_register, $_anzahl = 1)
	{

		return $this->mbQuery(4, $_register, $_anzahl, true, true);

	}

	protected function isEnabled(string $_setting)
	{

		if( isset($this->opt[$_setting]) )
		{

			return isEnabled($this->opt[$_setting]);

		}else{

			return null;

		}
	}


}

function querySensor(&$_conf, &$_res, &$_smart)
{

	// growattSPH_bat_BMSdata/ttyUSB1/1/
	// chema / port / address /

	$type = $_conf["data"];


	$dev = explode("/",$type);

	if(! isset($dev[2]) )
		$dev[2] = "";

	$sclass = $dev[0];

	if($sclass == "special")
	{

		$sclass .= "_".$dev[1];

	}

	if( count($dev) > 1 )
	{

		if( class_exists($sclass) )
		{
			$sensor			= new $sclass($dev[1],$dev[2],$_res);

			$sensor->smart	= &$_smart;

			$sensor->init($_smart, $_res["device"], $type, $_conf);

			if( $sensor->query() )
			{

				$_res = $sensor->result;

				unset($sensor);

				return true;

			}else{

				return false;

			}
		}
	}

	return false;
}

?>
