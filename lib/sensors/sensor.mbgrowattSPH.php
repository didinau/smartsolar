<?php


/*	240416
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

class mbgrowattSPH extends sensor
{
	public $MinSOCRegister = array(
					0=>608,
					1=>1091,2=>1071
					);

	function __construct($_port,$_addr,$_res)
	{

		parent::__construct($_port,$_addr,$_res);

	}

	function __destruct()
	{

		parent::__destruct();

	}

	public function query()
	{

		if( $this->mbOpen() )
		{

			// WR Datum und Zeit
			$mbD = $this->mbReadHoldingReg(45, 7);
			if(! strlen($mbD["err"]) )
			{
				// [0] => jahr 2023  [1] => monat 8  [2] => tag 9  [3] => stunde 11  [4] => minute 59  [5] => sekunde 16  [6] => wochentag (Montag=1) 3
				// Stimmt das WR-Datum mit unserem überein?
				$wrdatum = mf::format_datemysql($mbD["regs"][0], $mbD["regs"][1], $mbD["regs"][2]);

				if( $wrdatum != $this->result["D"] )
				{

					new LogMsg(0, __class__ . "::" . __function__, "WR-Datum entspricht nicht dem Query-Datum");

					return sensor::RET_NODATA;

				}

			}else{

				$this->log_NoResponse(__class__ . "::" . __function__, $this->port);

				return sensor::RET_NODATA;

			}

			$mb = $this->mbReadHoldingReg(3144, 1);

			$this->result["more"]["workmode"] = $mb["regs"][0];
			$this->result["more"]["minsoc"]   = 0;

			if( isset($this->MinSOCRegister[$this->result["more"]["minsoc"]]) )
			{

				$mb = $this->mbReadHoldingReg($this->MinSOCRegister[$this->result["more"]["workmode"]], 1);
				$this->result["more"]["minsoc"] = $mb["regs"][0];

			}else{

				//$this->logger(__CLASS__." Unbekannter WorkMode");

			}
			unset($mbWorkMode, $mbMinSOC);

			$mbA = $this->mbReadInputReg(0, 63);
			$mbB = $this->mbReadInputReg(63, 62);
			$mbC = $this->mbReadInputReg(1000, 64);
			$mbD = $this->mbReadInputReg(1120, 64);
			$mbE = $this->mbReadInputReg(200, 64);


			if(! strlen($mbA["err"].$mbB["err"].$mbC["err"].$mbD["err"]) )
			{

				$data = array_merge($mbA["data"], $mbB["data"]);
				$dataidx = $mbA["reg2idx"];
				$dataidx->regcnt = $mbA["regcnt"] + $mbB["regcnt"];

				unset($mbA, $mbB);

				$this->result["T"]			= date("Y-m-d H:i:s");
				$this->result["D"]			= date("Y-m-d");



//>>>>>>>>>>>	Tests
				//$this->result["more"]["ExportLimitApparentPower"]	= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1064), 10);		//R 1048+1049
				#$this->result["more"]["AC Charge Power"]			= $this->ulng32($mbD["data"], $mbD["reg2idx"]->idx(1128), 10);		//R 1128+1129

				$this->result["paccharge"]							= $this->ulng32($mbD["data"], $mbD["reg2idx"]->idx(1128), 10);		//R 1128+1129

//*********		Output power
/*
				$this->result["more"]["Output apparent power"]		= $this->ulng32($mbE["data"], $mbE["reg2idx"]->idx(230), 10);		//R 230+231
				$this->result["more"]["Real Output Reactive Power"]	= $this->ulng32($mbE["data"], $mbE["reg2idx"]->idx(232), 10);		//R 232+233
				$this->result["more"]["Nominal Output Reactive Power"]	= $this->ulng32($mbE["data"], $mbE["reg2idx"]->idx(234), 10);		//R 234+235
				$this->result["more"]["Reactive power generation"]	= $this->ulng32($mbE["data"], $mbE["reg2idx"]->idx(236), 10);		//R 236+237
*/
//<<<<<<<<<<<	Tests






//*************	Charging from grid::

				//Register 112 lowbyte =EACharge_Today_H
				//Register 113 lowbyte =EACharge_Today_L
				// =data[ (112 * 2 + 1) = 225]
				// =data[ (113 * 2 + 1) = 227]

				//Register 114 lowbyte =EACharge_Total_H
				//Register 115 lowbyte =EACharge_Total_L
				// =data[ (114 * 2 + 1) = 229]
				// =data[ (115 * 2 + 1) = 231]
/*
				$xdata=array(
					0=>$data[225],1=>$data[227],
					2=>$data[229],3=>$data[231]
					);
				$this->result["more"]["dgridcharge"]=$this->uint16($xdata,0,10);
				$this->result["more"]["tgridcharge"]=$this->uint16($xdata,2,10);
*/

				//oder

				$this->result["dgridcharge"]				= $this->ulng32($mbD["data"], $mbD["reg2idx"]->idx(1124), 10);		//R 1124+1125

//				$this->result["dmore"]["tgridcharge"]		= $this->ulng32($mbD["data"], $mbD["reg2idx"]->idx(1126), 10);		//R 1126+1127

				$this->result["tgridcharge"]				= $this->ulng32($mbD["data"], $mbD["reg2idx"]->idx(1126), 10);		//R 1126+1127
				$this->result["tfromgrid"]					= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1046), 10);
				$this->result["tdischarge"]					= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1054), 10);
				$this->result["tcharge"]					= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1058), 10);
				$this->result["tgrid"]						= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1050), 10);
				$this->result["tload"]						= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1062), 10);

//*************


//*************	Getestet und Produktiv:

				$this->result["dmore"]["dfromgrid"] = $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1044), 10);	//R 1044+1045

				//$this->result["more"]["dgridcharge"]=$this->result["dgridcharge"];

				$this->result["dfromgrid"]	= $this->result["dmore"]["dfromgrid"] + $this->result["dgridcharge"];

				$this->result["dgrid"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1048), 10);		//R 1048+1049
				$this->result["ddischarge"]	= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1052), 10);		//R 1052+1055
				$this->result["dcharge"]	= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1056), 10);		//R 1056+1057
				$this->result["dload"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1060), 10);		//R 1060+1061
				$this->result["dpv"]		= $this->ulng32($data, $dataidx->idx(59), 10)						//R 59+60 MPPT1
												+ $this->ulng32($data, $dataidx->idx(63), 10)					//R 63+64 MPPT2
												+ $this->ulng32($data, $dataidx->idx(67), 10)					//R 67+68 MPPT3
												+ $this->ulng32($data, $dataidx->idx(71), 10);					//R 71+72 MPPT4
				$this->result["tpv"]		= $this->ulng32($data, $dataidx->idx(61), 10)						//R 61+62 MPPT1
												+ $this->ulng32($data, $dataidx->idx(65), 10)					//R 65+66 MPPT2
												+ $this->ulng32($data, $dataidx->idx(69), 10)					//R 69+70 MPPT3
												+ $this->ulng32($data, $dataidx->idx(73), 10);					//R 73+74 MPPT4
				$this->result["ppv"]		= $this->ulng32($data, $dataidx->idx(1), 10);						//R 1+2 PV Power
				$this->result["ppv1"]		= $this->ulng32($data, $dataidx->idx(5), 10);						//R 5+6 PV Power PPT1
				$this->result["ppv2"]		= $this->ulng32($data, $dataidx->idx(9), 10);						//R 9+10 PV Power PPT2
				$this->result["ppv3"]		= $this->ulng32($data, $dataidx->idx(13), 10);						//R 13+14 PV Power PPT3
				$this->result["ppv4"]		= $this->ulng32($data, $dataidx->idx(17), 10);						//R 17+18 PV Power PPT4

//				P local alle drei Phasen addieren oder Inverter gesamt

//				drei Phasen addieren
/*
				$this->result["pload"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1031), 10)		//R 1031+1032
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1033), 10)	//R 1033+1034
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1035), 10);	//R 1035+1036
*/

//				Inverter Gesamt
				$this->result["pload"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1037), 10);		//R 1037+1038

				$this->result["puser"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1015), 10)		//R 1015+1016
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1017), 10)	//R 1017+1018
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1019), 10);	//R 1019+1020
				$this->result["pgrid"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1023), 10)		//R 1023+1024
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1025), 10)	//R 1025+1026
												+ $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1027), 10);	//R 1027+1028
				$this->result["pdischarge"]	= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1009), 10);		//R 1009+1010
				$this->result["pcharge"]	= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1011), 10);		//R 1011+1012
				$this->result["vbat"]		= $this->uint16($mbC["data"], $mbC["reg2idx"]->idx(1013), 10);		//R 1013
				$this->result["soc"]		= $this->uint16($mbC["data"], $mbC["reg2idx"]->idx(1014));			//R 1014
				$this->result["fac"]		= $this->uint16($data, $dataidx->idx(37), 100);						//R 37
				$this->result["vac"]		= $this->uint16($data, $dataidx->idx(38), 10);						//R 38

				if(  $this->result["dload"] == 0 )
				{

					new LogMsg(0, __class__ . "::" . __function__, "WR-Load=0");

					return sensor::RET_ERROR;

				}

				$this->result["publishP"] = true;
				$this->result["publishE"] = true;

				return sensor::RET_DATA;
			}
		}
		return sensor::RET_NODATA;
	}

/*
			$res=$modbusGW->modbusQuery(1,4,0,63,true,true);
			$data=$res["data"];
			$res=$modbusGW->modbusQuery(1,4,63,62,true,true);
			$res["data"]=array_merge($data,$res["data"]);

			print "PV: ".($modbusGW->a2ulong32($res["data"],2)/10)." W\n";
			print "PV1: ".($modbusGW->a2ulong32($res["data"],10)/10)." W\n";
			print "PV2: ".($modbusGW->a2ulong32($res["data"],18)/10)." W\n";

			print "PV1u: ".($modbusGW->a2uint16($res["data"],6)/10)." V\n";
			print "PV1i: ".($modbusGW->a2uint16($res["data"],8)/10)." A\n";

			print "PV2u: ".($modbusGW->a2uint16($res["data"],14)/10)." V\n";
			print "PV2i: ".($modbusGW->a2uint16($res["data"],16)/10)." A\n";


			//Last
			print "? Pload: ".($modbusGW->a2ulong32($res["data"],70)/10)." W\n";
			print "? i load: ".($modbusGW->a2uint16($res["data"],78)/10)." A\n";

			print "fgrid: ".($modbusGW->a2uint16($res["data"],74)/100)." Hz\n";
			print "ugrid: ".($modbusGW->a2uint16($res["data"],76)/10)." V\n";

			print "? Pac: ".($modbusGW->a2ulong32($res["data"],45)/10)." VA\n";

			print "E (Systemprod.) ac genD: ".($modbusGW->a2ulong32($res["data"],106)/10)." kWh\n";
			print "E (Systemprod.) ac genT: ".($modbusGW->a2ulong32($res["data"],110)/10)." kWh\n";

			print "E (PV1) D: ".($modbusGW->a2ulong32($res["data"],118)/10)." kWh\n";
			print "E (PV2) D: ".($modbusGW->a2ulong32($res["data"],126)/10)." kWh\n";

			print "E (PV1) T: ".($modbusGW->a2ulong32($res["data"],122)/10)." kWh\n";
			print "E (PV2) T: ".($modbusGW->a2ulong32($res["data"],130)/10)." kWh\n";

			#print "E charge D: ".($modbusGW->a2ulong32($res["data"],224)/10)." kWh\n";
			#print "E power to local D: ".($modbusGW->a2ulong32($res["data"],232)/10)." kWh\n";

			print "Inv Temp: ".($modbusGW->a2uint16($res["data"],186)/10)." °C\n";

			$res=$modbusGW->modbusQuery(1,4,1000,64,true,false);
			print "P discharge: ".($modbusGW->a2ulong32($res["data"],18)/10)." W\n";
			print "P charge: ".($modbusGW->a2ulong32($res["data"],22)/10)." W\n";

			print "U Batt.: ".($modbusGW->a2uint16($res["data"],26)/10)." V\n";
			print "SOC Batt.: ".($modbusGW->a2uint16($res["data"],28))." %\n";

			print "P ac to user: ".(
				(
				$modbusGW->a2ulong32($res["data"],30)+
				$modbusGW->a2ulong32($res["data"],34)+
				$modbusGW->a2ulong32($res["data"],38)
				)/10
				)." W\n";

			print "P ac to grid: ".(
				(
				$modbusGW->a2ulong32($res["data"],46)+
				$modbusGW->a2ulong32($res["data"],50)+
				$modbusGW->a2ulong32($res["data"],54)
				)/10
				)." W\n";

			print "P localload: ".(
				(
				$modbusGW->a2ulong32($res["data"],62)+
				$modbusGW->a2ulong32($res["data"],66)+
				$modbusGW->a2ulong32($res["data"],70)
				)/10
				)." W\n";

			print "E touser D: ".($modbusGW->a2ulong32($res["data"],88)/10)." kWh\n";
			print "E touser T: ".($modbusGW->a2ulong32($res["data"],92)/10)." kWh\n";

			print "E togrid D: ".($modbusGW->a2ulong32($res["data"],96)/10)." kWh\n";
			print "E togrid T: ".($modbusGW->a2ulong32($res["data"],100)/10)." kWh\n";

			print "E discharge D: ".($modbusGW->a2ulong32($res["data"],104)/10)." kWh\n";
			print "E discharge T: ".($modbusGW->a2ulong32($res["data"],108)/10)." kWh\n";

			print "E charge D: ".($modbusGW->a2ulong32($res["data"],112)/10)." kWh\n";
			print "E charge T: ".($modbusGW->a2ulong32($res["data"],116)/10)." kWh\n";

			print "E load D: ".($modbusGW->a2ulong32($res["data"],120)/10)." kWh\n";
			print "E load T: ".($modbusGW->a2ulong32($res["data"],124)/10)." kWh\n";

*/

}

?>