<?php



class special_doppelbat extends sensor
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
		if(isset($this->ndata["NPCUCE1021mb"]) and isset($this->ndata["FELI12.5mb"]))
		{
			//von growatt batterie die feli runterrechnen
			#if($_ndata["NPCUCE1021mb"]["pcharge"]>0)
			$this->result["pcharge"]=$this->ndata["NPCUCE1021mb"]["pcharge"] - $this->ndata["FELI12.5mb"]["pcharge"];
			#if($_ndata["NPCUCE1021mb"]["pdischarge"]>0)
			$this->result["pdischarge"]=$this->ndata["NPCUCE1021mb"]["pdischarge"] - $this->ndata["FELI12.5mb"]["pdischarge"];

			$this->result["soc"]=$this->ndata["NPCUCE1021mb"]["soc"];
			$this->result["vbat"]=$this->ndata["NPCUCE1021mb"]["vbat"];

			$this->result["publishP"]=true;

			return true;
		}
		return false;
	}
}

?>