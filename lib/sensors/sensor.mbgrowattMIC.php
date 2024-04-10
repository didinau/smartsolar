<?php

class mbgrowattMIC extends sensor
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

		if( $this->mbOpen() )
		{

			// WR Datum und Zeit
			$mbD = $this->mbReadHoldingReg(45, 7);

			if(! strlen($mbD["err"]) )
			{
				// [0] => jahr 2023  [1] => monat 8  [2] => tag 9  [3] => stunde 11  [4] => minute 59  [5] => sekunde 16  [6] => wochentag (Montag=1) 3
				// Stimmt das WR-Datum mit unserem überein?

				$wrdatum = mf::format_datemysql($mbD["regs"][0], $mbD["regs"][1], $mbD["regs"][2]);

				if($wrdatum != $this->result["D"])
				{

					new LogMsg(0, __class__."::".__function__, "WR-Datum entspricht nicht dem Query-Datum");

					return sensor::RET_NODATA;
				}

			}else{

				$this->log_NoResponse(__class__."::".__function__, $this->port);

				return sensor::RET_NODATA;

			}

			$mbA = $this->mbReadInputReg(0, 63);
			$mbB = $this->mbReadInputReg(63, 62);
			$mbC = $this->mbReadInputReg(1000, 64);
			$mbD = $this->mbReadInputReg(1120, 64);
			$mbE = $this->mbReadInputReg(200, 64);


			if(! strlen($mbA["err"] . $mbB["err"] . $mbC["err"] . $mbD["err"]) )
			{

				$data = array_merge($mbA["data"], $mbB["data"]);
				$dataidx = $mbA["reg2idx"];
				$dataidx->regcnt = $mbA["regcnt"] + $mbB["regcnt"];

				unset($mbA, $mbB);

				$this->result["T"]			= date("Y-m-d H:i:s");
				$this->result["D"]			= date("Y-m-d");


				$this->result["tgrid"]		= $this->ulng32($mbC["data"], $mbC["reg2idx"]->idx(1050), 10);


//*************	Getestet und Produktiv:
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


				$this->result["fac"]		= $this->uint16($data, $dataidx->idx(37), 100);						//R 37
				$this->result["vac"]		= $this->uint16($data, $dataidx->idx(38), 10);						//R 38

				$this->result["publishP"] = true;
				$this->result["publishE"] = true;

				return sensor::RET_DATA;
			}
		}

		return sensor::RET_NODATA;
	}
}

?>