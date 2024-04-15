<?php

/* 240414
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

class cangrowattgbli6532 extends sensor
{
	public $can = null;

	function __construct($_port, $_addr, $_res)
	{

		parent::__construct($_port, $_addr, $_res);

	}

	function __destruct()
	{

		parent::__destruct();

	}

	public function query()
	{

		$readids = array();

		if( $this->can = new can() )
		{

			$canconfig = array(
				"speed"		=> 500000,
				"ifupdown"	=> 1,
				"sudo"		=> 1,
				"pktcnt"	=> 50,
				"timeout"	=> 1000,
				);

			if( strlen($this->addr) )
			{
				$portconf = explode(",",$this->addr);
				if( count($portconf) )
				{
					foreach( $portconf as $line )
					{
						$cline = explode("=", $line);
						if( count($cline) > 0 )
						{
							switch( $cline[0] )
							{
								case "speed":
									$canconfig["speed"] = intval($cline[1]);
									break;

								case "pktcnt":
									$canconfig["pktcnt"] = intval($cline[1]);
									break;

								case "timeout":
									$canconfig["timeout"] = intval($cline[1]);
									break;

								case "sudo":
									$canconfig["sudo"] = ( $cline[1] == "false" or $cline[1] == "0" ) ? false : true;
									break;

								case "ifupdown":
									$canconfig["ifupdown"] = ( $cline[1] == "false" or $cline[1] == "0" ) ? false : true;
									break;

								default:

									new LogMsg(0, __class__."::".__function__, "unknown option ".$cline[0]." for config");

									break;
							}
						}
					}
				}
			}

			$scanids = array(0x311,0x312,0x313,0x314,0x315,0x316,0x317,0x318);

			if(
				$this->can->can_havecanext
				and
				$this->smart->CONFIG["use_canextension"]
				)
			{

				$cdata = $this->can->readindexed($scanids, $this->port, $canconfig["speed"], $canconfig["ifupdown"], $canconfig["sudo"],
					$canconfig["pktcnt"], $canconfig["timeout"]);

			}else{

				$cdata = $this->can->readdump($this->port, $canconfig["speed"], $canconfig["ifupdown"], $canconfig["sudo"],
					$canconfig["pktcnt"], $canconfig["timeout"]);

			}

			if( is_array($cdata) and count($cdata) )
			{
				$this->result["more"] = array(
					"cellvoltage"		=>array(),
					"temp"				=>array(),
					);

				foreach( $cdata as $id => $data )
				{
					switch( $id )
					{
						case 0x311:
							$this->result["more"]["ubatcharge"]			= $this->uint16($data,0,10);
							$this->result["more"]["ibatchargelimit"]	= $this->uint16($data,2,10);
							$this->result["more"]["ibatdischargelimit"]	= $this->uint16($data,4,10);
							$this->result["more"]["batstatus"]			= sprintf("%08b",ord($data[6]))." ".sprintf("%08b",ord($data[6]));
							$readids[$id] = $id;
							break;

						case 0x312:
							$this->result["more"]["stat1"]		= sprintf("%08b",ord($data[0]));
							$this->result["more"]["stat2"]		= sprintf("%08b",ord($data[1]));
							$this->result["more"]["stat3"]		= sprintf("%08b",ord($data[2]));
							$this->result["more"]["stat4"]		= sprintf("%08b",ord($data[3]));
							$this->result["more"]["cellcnt"]	= $data[7];
							$readids[$id] = $id;
							break;

						case 0x313:
							$this->result["vbat"]				= $this->sint16($data,0,100);
							$this->result["ibat"]				= $this->sint16($data,2,10);
							$this->result["soc"]				= $data[6];
							$this->result["more"]["temp"][0]	= $this->sint16($data,4,10);
							$this->result["more"]["soh"]		= $data[7] & 0x7F;
							$readids[$id] = $id;
							break;

						case 0x314:
							$this->result["more"]["cbat"]				= $this->uint16($data,0,10);
							$this->result["more"]["cbatfull"]			= $this->uint16($data,2,10);
							$this->result["more"]["celldeltavoltage"]	= $this->uint16($data,4,1);
							$this->result["more"]["cyclecount"]			= $this->uint16($data,6,1);
							$readids[$id] = $id;
							break;

						case 0x315:
							$this->result["more"]["cellvoltage"][0]		= $this->uint16($data,0,1);
							$this->result["more"]["cellvoltage"][1]		= $this->uint16($data,2,1);
							$this->result["more"]["cellvoltage"][2]		= $this->uint16($data,4,1);
							$this->result["more"]["cellvoltage"][3]		= $this->uint16($data,6,1);
							$readids[$id] = $id;
							break;

						case 0x316:
							$this->result["more"]["cellvoltage"][4]		= $this->uint16($data,0,1);
							$this->result["more"]["cellvoltage"][5]		= $this->uint16($data,2,1);
							$this->result["more"]["cellvoltage"][6]		= $this->uint16($data,4,1);
							$this->result["more"]["cellvoltage"][7]		= $this->uint16($data,6,1);
							$readids[$id] = $id;
							break;

						case 0x317:
							$this->result["more"]["cellvoltage"][8]		= $this->uint16($data,0,1);
							$this->result["more"]["cellvoltage"][8]		= $this->uint16($data,2,1);
							$this->result["more"]["cellvoltage"][10]	= $this->uint16($data,4,1);
							$this->result["more"]["cellvoltage"][11]	= $this->uint16($data,6,1);
							$readids[$id] = $id;
							break;

						case 0x318:
							$this->result["more"]["cellvoltage"][12]	= $this->uint16($data,0,1);
							$this->result["more"]["cellvoltage"][13]	= $this->uint16($data,2,1);
							$this->result["more"]["cellvoltage"][14]	= $this->uint16($data,4,1);
							$this->result["more"]["cellvoltage"][15]	= $this->uint16($data,6,1);
							$readids[$id] = $id;
							break;

						default:
							break;
					}
				}

				if( isset($readids[0x313]) )
				{

					$this->result["publishP"] = true;

					if( $this->result["vbat"] > 0 )
					{

						if( $this->result["ibat"] < 0 )
						{

							$this->result["pdischarge"] = floatval($this->result["ibat"] * $this->result["vbat"] * -1);

						}else{

							$this->result["pcharge"]	= floatval($this->result["ibat"] * $this->result["vbat"]);

						}
					}

					return sensor::RET_DATA;
				}

				new LogMsg(0, __class__."::".__function__, "error required id 313 not set ".$this->port);

			}else{

				new LogMsg(0, __class__."::".__function__, "error no can data dev ".$this->port);

			}
		}

		return sensor::RET_NODATA;
	}
}

?>
