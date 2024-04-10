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

# PHP Warning:  getifaddrs() failed 24: Too many open files in /usr/local/energie/lib/can.php on line 53

#	$string = $this->GPIOPATH . $this->return_mode() . "write " . $pin . " " . $state;
#	$string  = $this->GPIOPATH . $this->return_mode() . "mode $pin $mode";

/*

Wenn der GPIO 17 als Ausgang konfiguriert werden soll, dann gilt folgendes Kommando.

gpio -g mode 17 out
Standardmäßig bekommt der Ausgang den Zustand "low" bzw. "0".

Wenn der GPIO 18 als Eingang konfiguriert werden soll, dann gilt folgendes Kommando.

gpio -g mode 18 in
Zustand eines GPIO-Ausgangs setzten
Grundsätzlich kann man nur Ausgänge setzen. Um den Zustand eines GPIO-Ausgangs auf "high" zu setzen reicht folgendes Kommando.

gpio -g write 17 1
Um den Zustand eines GPIO-Ausgangs auf "low" zu setzen reicht folgendes Kommando.

gpio -g write 17 0
GPIO-Zustand ermitteln/lesen
Um den Zustand, also "high" oder "low", an einem GPIO zu ermitteln, reicht folgendes Kommando. Der GPIO kann dabei sowohl ein Eingang als auch ein Ausgang sein.

gpio -g read 18
Die Ausgabe "1" steht für "high" oder "0" für "low".


*/

/*
sysFs:

$ echo 24 >/sys/class/gpio/export
$ echo out >/sys/class/gpio/gpio24/direction
$ echo 1 >/sys/class/gpio/gpio24/value


*/


class dgpio
{
	private $debug = "dgpio";
	private $whitelist = false;
	private const SYSFS_path = "/sys/class/gpio/";
	private const SYSFS_export = "export";
	private const SYSFS_unexport = "unexport";
	private const SYSFS_activlow = "/activ_low";
	private const SYSFS_direction = "/direction";
	private const SYSFS_value = "/value";

	private $gpio_cmd = "gpio -g ";

	function __construct($_whitelist_array = false)
	{

		if( isNotFalse($_whitelist_array) and is_array($_whitelist_array) and count($_whitelist_array) )
			$this->whitelist = $_whitelist_array;

	}

	private function inStatelist($_state)
	{
		if( $_state == 1 or $_state == 0 )
			return true;

		return false;
	}

	private function inModelist($_mode)
	{
		if( $_mode == "in" or $_mode == "out" )
			return true;

		return false;
	}

	private function inWhitelist($_port)
	{
		if( isNotFalse($this->whitelist) )

			if(! in_array($_port, $this->whitelist) )

				return false;

		return true;
	}

	public function open($path, $mode)
	{

		return @fopen($path, $mode);

	}

	public function getContents($_path)
	{

		if( $fh = $this->open($_path, 'r') )
		{

			$contents = @stream_get_contents($fh);

			fclose($fh);

		}else{

			new LogMsg(0, __class__."::".__function__, "error open r ".$_path);

			$contents = false;

		}

		return $contents;
	}

	public function putContents($_path, $_data)
	{

		if( $fh = $this->open($_path, 'w') )
		{

			$write = @fwrite($fh, $_data);

			fclose($fh);

		}else{

			new LogMsg(0, __class__."::".__function__, "error open w ".$_path);

			$wire = false;

		}

		return $write;

	}

	private function sysFs_getState(int $_portNum): bool
	{

		$port = intval(trim($_portNum));

		if(! $this->inWhitelist($port) )
			return false;

		return $this->getContents(self::SYSFS_path . "gpio" . $port . self::SYSFS_value);

	}

	private function sysFs_setState(int $_portNum, int $_state): bool
	{

		$port = intval(trim($_portNum));

		$mode = strtolower($_mode);

		if(! $this->inWhitelist($port) or ! inStatelist($_state) )
			return false;

		$this->putContents(self::SYSFS_path . "gpio" . $port . self::SYSFS_value, $_state);

		return true;
	}

	public function exportGpio(int $_portNum, $_unxport = false): bool
	{

		if( ! file_exists(self::SYSFS_path . "gpio" . $_portNum) )
		{

			$ret = $this->putContents(self::SYSFS_path . (isFalse($_unxport) ? self::SYSFS_export : self::SYSFS_unexport), $_portNum);

			if( isNotFalse($ret) )
				$ret = true;

		}else{

			$ret = true;
		}

		return $ret;
	}

	public function unexportGpio(int $_portNum): bool
	{
		return $this->exportGpio($_portNum, true);
	}

	public function sysFs_register(int $_portNum, string $_mode, int $_state, int $_active_low = null)
	{

		$port = intval(trim($_portNum));

		$mode = strtolower($_mode);

		if(! $this->inModelist($mode) or ! $this->inWhitelist($port) or ! inStatelist($_state) )
			return false;

		$this->exportGpio($port);

		if(! is_null($_active_low) and ! inStatelist($_active_low) )
			return false;

		$this->putContents(self::SYSFS_path . "gpio" . $port . self::SYSFS_direction, $mode);

		if(! is_null($_active_low) )
			$this->putContents(self::SYSFS_path . "gpio" . $port . self::SYSFS_activlow, $_active_low);

		$this->putContents(self::SYSFS_path . "gpio ". $port . self::SYSFS_value, $_state);

		return true;

	}

	public function gpio_setMode(int $_portNum, string $_mode)
	{

		$cmd = $this->gpio_cmd . "mode " . intval($_portNum)." ".$_mode;

		$pipes = array();

		$res = bin_dev_functions::secured_exec($cmd, $pipes);

		if( $res != 0 )
			new LogMsg(0, __class__."::".__function__, "exec error " . $res . ": " . implode(" ",$pipes[1]));

	}

	public function gpio_setState(int $_portNum, int $_state)
	{

		$cmd = $this->gpio_cmd . "write " . intval($_portNum) . " " . intval($_state);

		$pipes = array();

		$res = bin_dev_functions::secured_exec($cmd, $pipes);

		if( $res != 0 )
			new LogMsg(0, __class__."::".__function__, "exec error " . $res . ": " . implode(" ",$pipes));
	}

	public function gpio_getState(int $_portNum): int
	{
		$cmd = $this->gpio_cmd . "read " . intval($_portNum);

		$pipes = array();

		$res = bin_dev_functions::secured_exec($cmd, $pipes);

		if( $res != 0 )
			new LogMsg(0, __class__."::".__function__, "exec error " . $res . ": " . implode(" ",$pipes));


		return intval(trim($pipes[0]));

	}


	public function gpio_readModes()
	{
		$res=array();
		$ra=$this->readall();

		if(strlen($ra))
		{
			$gpio=explode("\n",$ra);
			foreach($gpio as $line)
			{
				$line=preg_split("/[\|]+/",$line);
				if(count($line)>1)
				{
					$v=trim($line[1]);
					$h=trim($line[12]);
					if(is_numeric($v)) $res[(int) $v]=strtoupper(trim($line[4]));
					if(is_numeric($h)) $res[(int) $h]=strtoupper(trim($line[9]));
				}
			}
			if(count($res))
				return $res;
		}
		return false;
	}

	public function readall()
	{

		$cmd  = $this->gpio_cmd."readall";

		$pipes = array();

		$res = bin_dev_functions::secured_exec($cmd, $pipes);

		if( $res != 0 )
			new LogMsg(0, __class__."::".__function__, "exec error " . $res . ": " . implode(" ",$pipes));

		return trim($pipes[0]);
	}
}

?>