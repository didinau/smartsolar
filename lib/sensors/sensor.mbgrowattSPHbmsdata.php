<?php

class mbgrowattSPHbmsdata extends sensor
{

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

		if($this->mbOpen())
		{

			$mbA=$this->mbQuery(4,1108,16,true,true);
			$mbB=$this->mbQuery(4,1082,16,true,true);

			if(!strlen($mbA["err"].$mbB["err"]))
			{
				foreach($mbA["regs"] as $r=>$v)
					$mbA["regs"][$r]=(float) $v/1000;



				$this->result["more"]=array(
					"cellvoltage"				=>$mbA["regs"],
					"temp"						=>array(
													(float) $mbB["regs"][7],
													),
					);
				$this->result["publishP"]		=true;
				$this->result["ibat"]			=(float) $mbB["regs"][6]/100;
				$this->result["vbat"]			=(float) $mbB["regs"][5]/100;
				$this->result["soc"]			=(float)  $mbB["regs"][4];

				if($this->result["ibat"]<0)
				{
					$this->result["pdischarge"]	=(float) $this->result["ibat"] * $this->result["vbat"] * -1;
				}else{
					$this->result["pcharge"]	=(float) $this->result["ibat"] * $this->result["vbat"];
				}
				return true;

			}else{

				$this->log_NoResponse(__class__."::".__function__, $this->port);

			}
		}else{

			new LogMsg(0, __class__."::".__function__, "error open port /dev/".$this->port);

		}

		return false;
	}
}

?>