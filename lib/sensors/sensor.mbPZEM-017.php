<?php

/*
Shunt-Wert einstellen:

01 06 00 03 00 00 79 CA 100A
01 06 00 03 00 01 B8 0A 50A
01 06 00 03 00 02 F8 0B 200A
01 06 00 03 00 03 39 CB 300A
01 06 00 03 00 04 78 09 10A (interner Shunt)


$res=$modbus->modbusQuery(1,6,3,array(0,1),true,false);

//*/

class PZEM017 extends sensor
{
	function __construct($_port, $_addr, $_res)
	{

		parent::__construct($_port, $_addr, $_res);

	}

	function __destruct()
	{

		parent::__destruct();

	}

	private function setShunt($_wert)
	{
		$wert = mf::onlyNumbers($_wert);

		$shuntid = false;

		switch($wert)
		{
			case 10:
				$shuntid = 4;
				break;
			case 50:
				$shuntid = 1;
				break;
			case 100:
				$shuntid = 0;
				break;
			case 200:
				$shuntid = 2;
				break;
			case 300:
				$shuntid = 4;
				break;
		}

		if( isNotFalse($shuntid) )
		{

			$res = $this->mbWriteHoldingReg(3, array(0, $shuntid));

			if( $res["request"] != $res["response"] )
			{
				new LogMsg(0, __class__."::".__function__, "Error set shunt (".$_wert.")\nRequ:".$res["request"]."\nResp:".$res["response"]);
			}else{
				if( $this->debug )
					new LogMsg(0, __class__."::".__function__, "set shunt ".$_wert);
			}

		}

	}

	public function getconf()
	{

		$this->getParam();

	}

	public function query()
	{

		// $this->newday !!

		$this->getconf();

		$this->pparam = "8n2";

		if( $this->mbOpen() )
		{

			if( isset($this->opt["shunt"]) )
				$this->setShunt($this->opt["shunt"]);

			$mbA = $this->mbReadInputReg(0,8);

			if(
				! $mbA["err"]
				and
				count($mbA["regs"]) == 8
				)
			{

				$this->result["publishP"]		= true;

				$this->result["i"]			= floatval($mbA["regs"][1]/100);

				$this->result["u"]			= floatval($mbA["regs"][0]/100);

				$this->result["dpv"]		= floatval($mbA["regs"][4]/1000);
				$this->result["tpv"]		= floatval($mbA["regs"][4]/1000);
				$this->result["ppv"]		= floatval($mbA["regs"][2]/10);

				foreach( array("i","u") as $op )
				{

					if( isset($this->opt["adjust.".$op]) )
						$this->result["r".$op] = $this->result[$op] + floatval($this->opt["adjust.".$op]);
					else
						$this->result["r".$op] = $this->result[$op];
				}

				$this->result["ppv1"]		= floatval($this->result["ri"] * $this->result["ru"]);

				return sensor::RET_DATA;

			}else{

				$this->log_NoResponse(__class__."::".__function__, $this->port);

			}
		}else{

			new LogMsg(0, __class__."::".__function__, "error open port /dev/".$this->port);

		}
		return sensor::RET_NODATA;
	}
}

?>
