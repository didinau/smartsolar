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


class COM_modbus extends bin_dev_functions
{

	public $COM = null;
	public $_havedio = false;
	public $debug = false;

	function __construct($_debug=false,$_nodio=false)
	{

		if(!$_nodio and function_exists('dio_open')) $this->_havedio = true;

		$this->debug = $_debug;

	}

	function __destruct()
	{

		$this->COM = null;

	}

	public function devOpen($_dev, $_baud, $_conf, $_flow = "none", $mode = "r+b")
	{

		if( $this->_havedio )

			$this->COM = new COM_serial_dio($this->debug);

		else

			$this->COM = new COM_serial_nodio($this->debug);

		return $this->COM->devOpen($_dev, $_baud, $_conf , $_flow, $mode);

	}

	/*
	* Sende Data modbus style
	*
	*/
	public function modbusQuery($_id, $_fc, $_regadr, $_regCountOrData, $_readResponse = true, $_2register = false)
	{

		/*
		 * Functioncode holding register = 0x03
		 * Functioncode input register = 0x04
		 * Function 0x06 Preset single register
		 * Function 0x10 Preset multiple register
		 *
		 */

		$query = "";

		if ( ($_id > 255 ) or ($_id < 0) ) {

			if( $this->debug ) new LogMsg(0, __class__."::".__function__, "invalid slave id " . $_id);

			return false;

		}

		if ( ($_fc > 16 ) or ($_fc < 1) ) {

			if( $this->debug ) new LogMsg(0, __class__."::".__function__, "invalid function code " . $_fc);

			return false;

		}

		$query .= chr($_id) . chr($_fc);

		$l = $_regadr & 255;

		$h = $_regadr >> 8;

		$query .= chr($h) . chr($l);

		if( is_array($_regCountOrData) and count($_regCountOrData) )
		{

			foreach($_regCountOrData as $z)
				$query .= chr($z);

		}else{

			if( is_string($_regCountOrData) and strlen($_regCountOrData) )
			{

				$query .= $_regCountOrData;

			}else{

				$l = $_regCountOrData & 255;

				$h = $_regCountOrData >> 8;

				$query .= chr($h) . chr($l);

			}
		}

		$query .= $this->crc16($query);

		if($this->debug) new LogMsg(0, __class__."::".__function__, "[query]:\n" . $this->bin2hexfriendly($query));

		$resA = array(
			"err"		=> "",
			"id"		=> $_id,
			"fc"		=> $_fc,
			"reg"		=> $_regadr,
			"regcnt"	=> 0,
			"request"	=> $this->bin2hexfriendly($query),
			"response"	=> "",
			"raw"		=> "",
			"data"		=> array(),
			"regs"		=> array(),
			);

		$ret = $this->COM->devSend($query);

		if( isNotFalse($ret) )
		{

			usleep(200000);

		}else{

			$resA["err"] = "error send";

			$resA["raw"] = $ret;

			return $resA;

		}

		if( isFalse($_readResponse) )
		{

			return $resA;

		}

		$res = $this->COM->devRead(4096);

		if( $res )

			$resA["response"] = $this->bin2hexfriendly($res);

		else

			$resA["response"] = "n.A.";

		if( isFalse($res) or (strlen($res)==0))
		{

			$this->error = __FUNCTION__ .": no response from device!";

			$resA["err"] = $this->error;

			$resA["raw"] = $res;

			return $resA;

		}

		if($this->debug) new LogMsg(0, __class__ . "::" . __function__, "resonse:\n".$this->bin2hexfriendly($res));

		$resA["id"] = ord(substr($res, 0, 1));
		$resA["fc"] = ord(substr($res, 1, 1));

		if( $resA["id"] != $_id or $resA["fc"] != $_fc )
		{

			$this->error = __FUNCTION__ . ": response does not match id/fc !";

			$resA["err"] = $this->error;

			$resA["raw"] = $res;

			return $resA;

		}else{


		}

		// CRC Check
		$data = substr($res, 0, strlen($res) - 2);
		$crc = $this->crc16($data);

		if( $crc != substr($res, -2) )
		{

			$this->error = __FUNCTION__ . ": crc check failed want " . $this->bin2hexfriendly($crc) . " got " . $this->bin2hexfriendly(substr($res, -2));

			$resA["err"] = $this->error;

			$resA["raw"] = $res;

			return $resA;

		}

		$resA["regcnt"] = ord(substr($res, 2, 1));

		if( $_2register )
		{
			for($i = 3; $i < strlen($data); $i += 2)
			{
				$resA["regs"][] = $this->_bin16dec(substr($data, $i, 2));
			}
		}

		$resA["raw"] = substr($data, 3, $resA["regcnt"] * 2);
		$resA["data"] = array_map("ord", str_split(substr($data, 3, $resA["regcnt"] * 2)));

		$resA["reg2idx"] = new COM_modbus_registerindex($_regadr, $resA["regcnt"]);

		return $resA;

	}
}

?>