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

class COM_serial_dio extends bin_dev_functions
{

	public $_dev		= false;
	public $_fd			= false;
	public $_stat		= bin_dev_functions::SERIAL_DEVICE_NOTSET;
	public $autoflush	= true;
	public $debug		= false;
	public $error		= "";
	public $_buffer		= "";

	function __destruct()
	{
		$this->devClose();
		if( $this->debug ) new LogMsg(0, __class__."::".__function__, "shutdown");
	}

	function __construct($_debug=false)
	{

		$this->debug=$_debug;

	}

	/**
	* read from device
	*
	* @param string $str          string to be sent to the device
	* @param float  $waitForReply time to wait for the reply (in seconds)
	* NICHT WARTEN!
	*/
	public function debug_timeRead($_secs)
	{
		$place = __class__."::".__function__;

		$stop=microtime(true)+$_secs;

		while(microtime(true)<$stop)
		{
			$res = $this->devRead(4096);
			if( isFalse($res) )
			{
				new LogMsg(0, $place, "error reading from device");
				return false;
			}
			new LogMsg(0, $place, "read  " . strlen($res) . " bytes:\n" . $this->bin2hexfriendly($res));
			usleep(1000000);
		}
		return true;
	}


	public function devOpen($_dev,$_baud,$_conf,$_flow="none",$mode="r+b")
	{
		$place = __class__ . "::" . __function__;

		if( $this->_stat === bin_dev_functions::SERIAL_DEVICE_OPENED )
		{

			new LogMsg(0, $place, "The device is already opened");

			return false;

		}

		return $this->Dio_devOpen($_dev, $_baud, $_conf, $_flow);

	}

	public function Dio_devOpen($_dev, $_baud, $_conf, $_flow = "none")
	{
		$place = __class__ . "::" . __function__;

		$dio = array();

		if( strlen($_conf) !== 3 )
		{

			new LogMsg(0, $place, "The Configuration String " . $_conf . " is not valid!");

			return false;

		}

		$op = intval(substr($_conf, 0, 1));

		if( $op < 5 or $op > 8 )
		{

			new LogMsg(0, $place, "The Character Length " . $op . " is not valid!");

			return false;

		}

		$dio["bits"] = $op;

		$dio["baud"] = $_baud;

		$op = strtolower(substr($_conf, 1, 1));

		switch($op)
		{
			case "n":

			case "0":
				$op = 0;
				break;

			case "o":
			case "1":
				$op = 1;
				break;

			case "e":
			case "2":
				$op = 2;
				break;

			default:
				new LogMsg(0, $place, "The Parity  " . $op . " is not valid!");
				return false;
		}

		$dio["parity"] = $op;

		$op = intval(substr($_conf, 2, 1));

		if( $op < 1 or $op > 2 )
		{

			new LogMsg(0, $place, "The Count of stopbits " . $op . " is not valid!");

			return false;

		}

		$dio["stop"] = $op;

		if( $this->_fd = dio_open($_dev, O_RDWR|O_NOCTTY|O_NONBLOCK) )
		{

			$this->_dev = $_dev;

			if($this->debug) new LogMsg(0, $place, "dio: devOpen");

			if( dio_tcsetattr($this->_fd, $dio) )
			{

				$this->_stat = bin_dev_functions::SERIAL_DEVICE_OPENED;

				if( $this->debug )
				{
					new LogMsg(0, $place, "dev is open: " . $_dev . " baud " . $_baud . " conf " . $_conf . " flow " . $_flow);
				}

				return true;

			}
		}

		$this->_fd = false;

		$this->_stat = bin_dev_functions::SERIAL_DEVICE_NOTSET;

		new LogMsg(0, $place, "Unable to open the device " . $_dev);

		return false;
	}

	public function devClose()
	{

		if( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
			return true;

		dio_close($this->_fd);

		$this->_fd = false;

		$this->_stat = bin_dev_functions::SERIAL_DEVICE_NOTSET;

		return true;
	}

	public function isOpen()
	{

		if ( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
			return false;

		return true;

	}

	/**
	* Flushes the output buffer
	* @return bool
	*/
	public function devFlush()
	{

		$place = __class__ . "::" . __function__;

		if($this->isOpen())
		{

			try {

				$w = dio_write($this->_fd, $this->_buffer);

			} catch ( throwable $e ) {

				$this->error = $e->getMessage();

				$w = false;

			}

			if( isNotFalse($w) )
			{

				if($this->debug) new LogMsg(0, $place, "DEBUG: write " . $w . " Bytes (".$this->error . ")");

				if( $w < strlen($this->_buffer) )
				{

					$this->_buffer = substr($this->buffer, $w);

				}else{

					$this->_buffer = "";

				}

				return true;

			}else{

				if($this->debug) new LogMsg(0, $place, "DEBUG: " . $this->error);

			}
		}

		return false;
	}

	/**
	* Sends a string to the device
	*
	* @param string $str          string to be sent to the device
	* @param float  $waitForReply time to wait for the reply (in seconds)
	* NICHT WARTEN!
	*/
	public function devSend($_str, $waitForReply = 0.1)
	{

		$this->_buffer .= $_str;

		if ( $this->autoflush )
			return $this->devFlush();

		return true;
	}

	/**
	* Reads the port until no new datas are availible, then return the content.
	*
	* @param int $count Number of characters to be read (will stop before
	*                   if less characters are in the buffer)
	* @return string
	*/
	public function devRead($count = 1)
	{
		$place = __class__ . "::" . __function__;

		if( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
		{

			$this->error = "Device must be opened to read it";

			if($this->debug)
				new LogMsg(0, $place, "DEBUG: " . $this->error);

			return false;

		}

		return $this->Dio_devRead($count);

	}

	public function Dio_devRead($count = 1)
	{

		$place = __class__ . "::" . __function__;

		$content = dio_read($this->_fd, $count);

		if( is_null($content) or isFalse($content) )
		{
			$content = false;

			if($this->debug) new LogMsg(0, $place, "dio: read error");

		}else{

			if($this->debug) new LogMsg(0, $place, "dio: read ".strlen($content)." bytes");

		}

		return $content;
	}

}

?>