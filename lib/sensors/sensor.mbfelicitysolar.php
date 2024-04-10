<?php



class mbfelicitysolar extends sensor
{

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

		// $this->newday !!

		if( $this->mbOpen() )
		{

			$mbA = $this->mbReadHoldingReg(0x1302, 0x0a);

			if(! $mbA["err"] )
			{

				$mbB = $this->mbReadHoldingReg(0x132A, 16);

				foreach( $mbB["regs"] as $r => $v )
					$mbB["regs"][$r] = floatval($v/1000);

				$this->result["more"]	= array(
					"cellvoltage"		=> $mbB["regs"],
					"temp"				=> array( floatval($mbA["regs"][8]), ),
					);

				$this->result["publishP"]	= true;
				$this->result["ibat"]		= floatval($mbA["regs"][5]/-10);
				$this->result["vbat"]		= floatval($mbA["regs"][4]/100);
				$this->result["soc"]		= floatval($mbA["regs"][9]);
				$this->result["temp"]		= floatval($mbA["regs"][8]);

				if( $this->result["ibat"] < 0 )
				{

					$this->result["pdischarge"]	= floatval($this->result["ibat"] * $this->result["vbat"] * -1);

				}else{

					$this->result["pcharge"]	= floatval($this->result["ibat"] * $this->result["vbat"]);

				}

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