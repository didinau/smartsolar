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


class can extends bin_dev_functions
{
	public $debug = false;
	public $can_port = false;
	public $can_portstate = false;
	public $can_portspeed = false;
	public $can_linkset_sudo = true;
	public $can_havecanext = false;

	function __construct()
	{

//error_log("konstruktor php: ".phpversion().", canbus:".extension_loaded("canbus")."\n");

		if( extension_loaded("canbus") )
		{

//error_log("CanBusExt found");

			$this->can_havecanext = true;
		}
		parent::__construct();
	}

	public function can_setport($_port, $_speed, $_sudo)
	{
		$this->can_port = $_port;
		$this->can_portspeed = $_speed;
		$this->can_linkset_sudo = $_sudo;
	}

	public function can_getiplink()
	{
		$cmd = "sudo ip link show ".$this->can_port;

		$rc = bin_dev_functions::secured_exec($cmd, $ret);

		if( $rc != 0 )
		{

			new LogMsg(0, __FUNCTION__, "call ".$cmd." rc ".$rc.": ".print_r($ret,true));

			return null;

		}else{

			if(strpos($ret[0],"state DOWN"))
			{

				return false;

			}
		}

		return true;
	}

	public function can_getifstate()
	{
		$netifs = net_get_interfaces();

		if( isFalse($this->can_port) or isset($netifs[$this->can_port]) == false )
		{

			new LogMsg(0, __FUNCTION__, "interface ".$this->can_port." not set!");

			return false;

		}else{

			if(! isset($netifs[$this->can_port]["up"]) )
			{

				$this->can_portstate = $this->can_getiplink() === true ? true : false;

			}else{

				$this->can_portstate=$netifs[$this->can_port]["up"];

			}
		}

		return true;
	}

	public function can_setlink($_UPorDOWN)
	{
		$cmd = ( $this->can_linkset_sudo ? "sudo " : "" )."ip link set dev ".$this->can_port." ".$_UPorDOWN." type can bitrate ".$this->can_portspeed;

		$ret = array();

		$rc = bin_dev_functions::secured_exec($cmd, $ret);

		if( $rc != 0 )
		{

			new LogMsg(0, __FUNCTION__, "call ".$cmd." rc ".$rc.": ".print_r($ret,true));

			return false;
		}
	}

	public function can_getdump($_pktcnt, $_timeout)
	{
		$can = array();

		$cmd = ( $this->can_linkset_sudo ? "sudo " : "" )."candump -L -n ".$_pktcnt." -T $_timeout ".$this->can_port;

		$ret = array();

		$rc = bin_dev_functions::secured_exec($cmd, $ret);

		$lines = explode("\n", $ret[0]);

		foreach($lines as $line)
		{
			$data = explode(" ", $line);

			if( count($data) == 3 and $data[1] == $this->can_port )
			{

				$cdata = explode("#",$data[2]);
				if( strlen($cdata[0]) and strlen($cdata[1]) )
				{

					$data = array_map("ord", str_split(hex2bin($cdata[1])));

					$can[hexdec($cdata[0])] = $data;

				}
			}
		}

		return $can;
	}

	public function readdump($_port, $_speed=500000, $_ifupdown=true, $_sudo=true, $_pktcnt=50, $_timeout=1000)
	{
		# ip link set dev can0 up type can bitrate 500000  # ??? restart-ms 100
		# candump -n 1 can0,315:fff
		# ip link set dev can0 down

		//$start=microtime(true);

		$ret = array();
		$need_ifdown = false;

		$this->can_setport($_port, $_speed, $_sudo);

		if(! $this->can_getifstate() )
			return false;

		if( $_ifupdown )
		{

			if(! $this->can_portstate )
			{

				$need_ifdown = true;

				if(! $this->can_setlink("up") )
					return false;
			}
		}

		$can = $this->can_getdump($_pktcnt, $_timeout);

		if( $_ifupdown and $need_ifdown )
		{

			$this->can_setlink("down");

		}

		//error_log("readdump dauerte ".microtime(true)-$start." msec");

		return $can;
	}

/*
 * $_port:			"can0"
 * $_speed
 * $_ifupdown:		set can port up and also down
 * $_sudo:			use sudo for ip-cmd
 * $_pktcnt:		not used here
 * $_timeout:		timeout (msec)
 * $_index:			the can-ids $a=array(0x311,0x312,0x313)
 *
*/
	public function readindexed($_index, $_port, $_speed=500000, $_ifupdown=true, $_sudo=true, $_pktcnt=50, $_timeout=1000)
	{

		//$start=microtime(true);

		$this->can_setport($_port, $_speed, $_sudo);

		if(! $this->can_getifstate() )
			return false;

		$can = array();

		$ids = array();

		$need_ifdown = false;

		if(! is_array($_index) or !count($_index) )
		{

			new LogMsg(0, __FUNCTION__, "indexarray not valid! ".print_r($_index, true));

			return false;
		}

		foreach($_index as $id)
		{

			$ids[$id] = true;
		}

		if( $_ifupdown )
		{
			if(! $this->can_portstate )
			{

				$need_ifdown = true;

				if(! $this->can_setlink("up") )
					return false;
			}
		}

		$canBus = new CanBus($this->can_port);

		// no block
		if( isFalse($canBus->init(false)) )
		{

			new LogMsg(0, __FUNCTION__, "canbus-ext: init failed! ".$this->can_port);

			return false;
		}

//		error_log("Debug: ".print_r($canBus,true));

		$timeout = microtime(true) + $_timeout / 1000 ; //*1000;

		while( microtime(true) < $timeout )
		{

			if(! count($ids) )
				break;

			$frame = $canBus->read();

			if( isNotFalse($frame) )
			{

				if( in_array($frame->id, $ids) )
				{

					$can[$frame->id] = $frame->data;

					unset($ids[$frame->id]);
				}
			}else{

				//error_log("CAN no frame");
				usleep(1000);
			}
		}

		unset($canBus);
		// socket descriptor stays open ???

		if( $_ifupdown and $need_ifdown )
		{

			$this->can_setlink("down");
		}

		//error_log("readindex takes ".microtime(true)-$start." msec");

		return $can;
	}
}
?>