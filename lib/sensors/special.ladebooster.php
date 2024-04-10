<?php

/*

Doc:

Parameter in Teil 3 der "data"-Option oder in der "param"-Option:

;	Parameter:
;	alt=mbgrowattMIC/ttyUSB0/1	| Wenn keine Ladung, wir PV auf eine anderes Gerät (WR) geschaltet
;	sensor=PZEM017				| Welcher Sensor misst die Leistung
;	dev_check=NPCUCE1021mb		| Bedingungsparameter von welchem Gerät lesen
;	dev_check.ubat=GBLI6532can	| Batteriedaten lesen von

;	Optionen zum "Aufwecken" des PZEM017, denn bei langsamen Anstieg der Spannung startet dieser nicht selbständig

;	wakeup.gpio=24				|
;	wakeup.on=0					|
;	wakeup.offtime=3			|

;	Boostoptionen:

;	boost.gpio=23				| Setze GPIO-Port
;	boost.on=0					| auf [] zum Akktivieren des Boost
;	boost.delay=480				| Ausschaltverzögerung in Sekunden
;	boost.disabled=0			| Deaktivieren der Funktion


;	Bedingungsoptionen:			| nicht benötigte Optionen können weggelassen werden

;	boost.mincharge=100			| "dev_check" muss min. 100 Watt laden
;	boost.socavg=100			| Durchschnitt SOC der Batterien: kleiner 100
;	boost.accharge=20			| "dev_check" darf mit max. 20 Watt aus dem Netz laden
;	boost.pv=199				| nur wenn "dev_check" PV-Leistung über 199 Watt liegt
;	boost.grid=-1				| nur wenn "dev_check" Einspeisung über -1 Watt liegt (bei -1 immer wahr)
;	boost.ubat=55.2				| Höchste Batteriespannung unter 55,2 Volt
;	boost.user=200				| Netzbezug durch dev_check unter 100 Watt

*/


class special_ladebooster extends sensor
{
	public $ubat			=0;
	public $checkDevData	=false;

	function __construct($_port,$_addr,$_res)
	{
		parent::__construct($_port,$_addr,$_res);
	}

	function __destruct()
	{
		parent::__destruct();
	}

	public function checkBOOST(&$STATIC)
	{

		$uinfo = "UBAT: ";
		$bed   = "check ";

		$batteries = $this->opt["dev_check.ubat"];

		// Keine Sensordaten dürfen älter als 10 Minuten sein
		$time = time();

		$devs = $batteries;

		$devs[] = $this->opt["dev_check"];

		foreach( $devs as $dev )
		{

			if(
				! isset($this->smart->STATIC["lastquery+"][$dev])
				or
				$this->smart->STATIC["lastquery+"][$dev] < ( $time - 600)
				)
			{

				$STATIC["boostdelay"] = 0;

				$this->logger("Daten von Gerät ".$dev." sind zu alt!",1);

				return false;

			}
		}

		#$dev=array_pop($batteries);
		#$this->ubat=(double) $this->smart->STATIC["current"][$dev]["vbat"];
		#$uinfo.=$dev."=".$this->ubat." ";

		$socavg = 0;

		$this->ubat = 0;

		if( count($batteries) )
		{

			foreach( $batteries as $dev )
			{
				$uinfo .= $dev . "=" . $this->smart->STATIC["current"][$dev]["vbat"] . " ";
				$volt1  = floatval($this->smart->STATIC["current"][$dev]["vbat"]);

				if( $volt1 > $this->ubat )
				{

					$this->ubat = $volt1;

				}

				$socavg += floatval($this->smart->STATIC["current"][$dev]["soc"]);

			}

			$socavg = floatval($socavg / count($batteries));

		}

		$this->logger($uinfo);

		if( isset( $this->opt["boost.mincharge"] ))
		{
			$test = $this->checkDevData["pcharge"] > intval($this->opt["boost.mincharge"]);

			$this->logger($bed . "boost.mincharge ist " . ($test ? "" : "un") . "wahr: " . $this->checkDevData["pcharge"] . " > " . $this->opt["boost.mincharge"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.mincharge wird nicht geprüft");

		}

		if( isset( $this->opt["boost.accharge"] ))
		{
			$test = $this->checkDevData["paccharge"] < intval($this->opt["boost.accharge"]);

			$this->logger($bed . "boost.accharge ist " . ($test ? "" : "un") . "wahr: " . $this->checkDevData["paccharge"] . " < " . $this->opt["boost.accharge"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.accharge wird nicht geprüft");

		}

		if( isset( $this->opt["boost.grid"] ))
		{

			$test = $this->checkDevData["pgrid"] > intval($this->opt["boost.grid"]);

			$this->logger($bed . "boost.grid ist " . ($test ? "" : "un") . "wahr: " . $this->checkDevData["pgrid"] . " > " . $this->opt["boost.grid"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.grid wird nicht geprüft");

		}

		if( isset( $this->opt["boost.pv"] ))
		{

			$test=$this->checkDevData["ppv"] > intval($this->opt["boost.pv"]);

			$this->logger($bed . "boost.pv ist " . ($test ? "" : "un") ."wahr: ".$this->checkDevData["ppv"]." > ".$this->opt["boost.pv"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.pv wird nicht geprüft");

		}

		if( isset($this->opt["boost.ubat"]) )
		{

			$test = $this->ubat < floatval($this->opt["boost.ubat"]);

			$this->logger($bed . "boost.ubat ist " . ($test ? "" : "un") . "wahr: (" . $uinfo . ") " . $this->ubat . " < " . $this->opt["boost.ubat"]);

			if(! $test )
			return false;
		}else{
			$this->logger($bed . "boost.ubat wird nicht geprüft");
		}

		if( isset($this->opt["boost.socavg"]) )
		{

			$test = $socavg < floatval($this->opt["boost.socavg"]);

			$this->logger($bed . "boost.socavg ist " . ($test ? "" : "un") . "wahr: " . $socavg . " < " . $this->opt["boost.socavg"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.socavg wird nicht geprüft");

		}

		if( isset($this->opt["boost.user"]) )
		{

			$test = $this->checkDevData["puser"] < intval($this->opt["boost.user"]);

			$this->logger($bed . "boost.user ist " . ($test ? "" : "un") . "wahr: " . $this->checkDevData["puser"] . " < " . $this->opt["boost.user"]);

			if(! $test )
				return false;

		}else{

			$this->logger($bed . "boost.user wird nicht geprüft");

		}

		return true;

	}

	public function getSensorData(&$res, &$GPIO, $_BOOST=false)
	{

		$sensoropt = array();

		$STATIC = &$this->smart->STATIC["modules"][$this->result["device"]];

		if( $this->opt["sensor"] == "PZEM017" )
		{

			if( isset($this->opt["shunt.sensor"]) )
			{

				$sensorOptions = "shunt=" . $this->opt["shunt.sensor"];

			}else{

				$sensorOptions = "";

			}

			$type = array("data" => "PZEM017" . "/" . $this->opt["port"] . "/" . $this->opt["addr"], "options" => $sensorOptions);

			foreach( array("u", "i") as $t )
			{

				if( isset($this->opt["adjust.sensor." . $t]) )
					$type["options"] .= ",adjust." . $t . "=" . floatval($this->opt["adjust.sensor." . $t]);

			}

			// Daten vom Sensor holen

			if(! $sensor_data = querySensor($type, $res, $this->smart) )
			{
				// Sensor antwortet nicht

				if( $this->opt["wakeup.gpio"] )
				{

					$this->logger("Wakeup PZEM017 set " . $this->opt["wakeup.gpio"] . " " . $this->opt["wakeup.on"]);

					$GPIO->gpio_setState($this->opt["wakeup.gpio"], $this->opt["wakeup.on"]);

					mf::Sleep(isset($this->opt["wakeup.offtime"])?$this->opt["wakeup.offtime"] : 2);

					$this->logger("Wakeup PZEM017 set " . $this->opt["wakeup.gpio"] . " " . (!$this->opt["wakeup.on"]) );

					$GPIO->gpio_setState($this->opt["wakeup.gpio"], !$this->opt["wakeup.on"]);

				}

			}else{

				if(! isset($STATIC["totalcount"]) )
				{

					$tcnt = array(	"dpv"			=> 0,
									"start"			=> $res["tpv"],
									"total"			=> 0,
									"totalcharge"	=> 0,
									"dcharge"		=> 0,
									"date"			=> date("Y-m-d"));

				}else{

					$tcnt						=	$STATIC["totalcount"];

					// nur zum Testen:

					$tcnt["pzem-kwh-cnter"]		=	$res["tpv"];

					// wenn der start > als der zähler im pzem ist, diesen übernehmen (PZEM ist z.B. zurückgesetzt)

					if( $tcnt["start"] > $res["tpv"] )
						$tcnt["start"] = $res["tpv"];

					if(! isset($tcnt["date"]) or $tcnt["date"] != date("Y-m-d") )
					{

						$tcnt["dpv"]	= 0;
						$tcnt["dcharge"]= 0;
						$tcnt["date"]	= date("Y-m-d");
						$tcnt["start"]	= $res["tpv"];

					}else{

						$diff = $res["tpv"] - $tcnt["start"];

						$tcnt["dpv"] += $diff;

						if( $_BOOST )
						{

							$tcnt["dcharge"] += $diff;
							$tcnt["totalcharge"]+= $diff;

						}

						$tcnt["total"] += $diff;

						$tcnt["start"] = $res["tpv"];

					}
				}

				$STATIC["totalcount"] = $tcnt;

				if( $_BOOST )
					$res["pcharge"] = floatval($res["ppv"]);
				else
					$res["pcharge"] = 0;

				$res["tpv"]		= floatval($STATIC["totalcount"]["total"]);
				$res["tcharge"]	= floatval($STATIC["totalcount"]["totalcharge"]);
				$res["dpv"]		= floatval($STATIC["totalcount"]["dpv"]);
				$res["dcharge"]	= floatval($STATIC["totalcount"]["dcharge"]);
				$res["vbat"]	= floatval($res["u"]);

			}
		}

		if(! $_BOOST and isset($this->opt["alt"]) )
		{

			// Daten vom alternativen Sensor holen, wenn kein boost

			$x = dataprovider("altdev");

			$type = array("data" => implode("/", explode(";", $this->opt["alt"])));

			if( querySensor($type, $x, $this->smart) )
			{

				$res["ppv1"] = $x["ppv"];

			}

		}

		return $sensor_data;
	}

	public function getconf()
	{

		$STATIC = &$this->smart->STATIC["modules"][$this->result["device"]];

		if(
			! isset($STATIC["opts"])
			or
			! isset($STATIC["configmtime"])
			or
			$this->smart->STATIC["config_ftime"] != $STATIC["configmtime"]
			)
		{

			$this->logger("read new opts");

			$STATIC["configmtime"] = $this->smart->STATIC["config_ftime"];

			$this->getParam();

			if(! isset($this->opt["boost.delay"]) )		$this->opt["boost.delay"] = 30;
			if(! isset($this->opt["boost.disabled"]) )	$this->opt["boost.disabled"] = false;

	#		if(! isset($this->opt["boost.mincharge"]) )	$this->opt["boost.mincharge"]=1000;

			if(! isset($this->opt["dev_replace"]) )		$this->opt["dev_replace"] = "MIC600mb";
			if(! isset($this->opt["dev_check"]) )		$this->opt["dev_check"] = "NPCUCE1021mb";

			if( isset($this->opt["dev_check.ubat"]) )
			{

				$devs = explode(";", $this->opt["dev_check.ubat"]);

				$this->opt["dev_check.ubat"] = mf::cleanArray($devs, "s");

			}else{

			}

			$STATIC["opts"] = $this->opt;

		}else{

			$this->opt = $STATIC["opts"];

		}

		if( !isset($this->opt["dev_check.ubat"]) or !count($this->opt["dev_check.ubat"]) )
			$this->opt["dev_check.ubat"] = array($this->opt["dev_check"]);

		if( isset($this->opt["debug"]) and (int) $this->opt["debug"] == 1)
			$this->debug = "Debg: BOOSTER";

	}

	public function query()
	{


		if(! isset($this->smart->STATIC["modules"][$this->result["device"]]) )
			$this->smart->STATIC["modules"][$this->result["device"]] = array();

		$STATIC = &$this->smart->STATIC["modules"][$this->result["device"]];

		$this->getconf();

		if(! isset($STATIC["boost"]) )
			$STATIC["boost"] = false;


		if(! isset($this->checkDevData) )
			return sensor::RET_NODATA;


		$GPIO= new dgpio();

		$sensor_data = false;

		$res = $this->result;

		$time = time();

		// werte die Optionen aus


		// Lese die gpio für wakeup und boost

		$pins = $GPIO->gpio_readModes();

		if(
			isset($pins[$this->opt["wakeup.gpio"]])
			and
			$pins[$this->opt["wakeup.gpio"]] != "OUT"
			)
		{

			$this->logger("set ".$this->opt["wakeup.gpio"] . " out");

			$GPIO->gpio_setMode($this->opt["wakeup.gpio"], "out");

			$this->logger("set ".$this->opt["wakeup.gpio"] . " " . (!$this->opt["wakeup.on"]));

			$GPIO->gpio_setState($this->opt["wakeup.gpio"], !$this->opt["wakeup.on"]);

		}

		if(
			isset($pins[$this->opt["boost.gpio"]])
			and
			$pins[$this->opt["boost.gpio"]]!="OUT"
			)
		{

			$this->logger("set " . $this->opt["boost.gpio"] . " out");

			$GPIO->gpio_setMode($this->opt["boost.gpio"], "out");

			$this->logger("set " . $this->opt["boost.gpio"] . " " . (!$this->opt["boost.on"]));

			$GPIO->gpio_setState($this->opt["boost.gpio"], !$this->opt["boost.on"]);

			$STATIC["boost"] = false;
		}

		$boost = false;

		// prüfe das Zeitfenster
		if( !$GLOBALS["smart"]->isNight() )
		{

			// sind im Zeitfenster
			$this->logger("Im Zeitfenster");

			// Daten vom Sensor holen
			if( isset($this->ndata[$this->opt["dev_check"]]) )
			{

				$this->checkDevData = $this->ndata[$this->opt["dev_check"]];

			}else{

				$this->checkDevData = $this->smart->STATIC["current"][$this->opt["dev_check"]];

			}

			$sensor_data = $this->getSensorData($res, $GPIO, $STATIC["boost"]);

			// prüfe Bedingungen für booster

			$test = $this->checkBOOST($STATIC);

			if($this->debug)
				$info =
					" ACCharge: " .	$this->checkDevData["paccharge"].
					", pcharge: " .	$this->checkDevData["pcharge"].
					", puser: " .	$this->checkDevData["puser"].
					", ppv: " .		$this->checkDevData["ppv"].
					", pload: " .	$this->checkDevData["pload"].
					", pgrid: " .	$this->checkDevData["pgrid"].
					", SOC: " .		$this->checkDevData["soc"].
					", vbat: " . 	$this->ubat.
					( $sensor_data ? ", sU: ".$res["u"].", sI: ".$res["i"].", sP: ".$res["ppv"] : "");


			if( $test )
			{

				// booster - Bedingungen sind gegeben

				$this->logger(($STATIC["boost"] ? "ist":"wäre")." ok:".$info);

				if(!$this->opt["boost.disabled"])

					$boost = true;

				else

					$this->logger("ist disabled");

			}else{

				$this->logger("nicht angebracht:" . $info);

			}

			// Aktion für BOOST innerhalb des Zeitfensters

			if($boost and $STATIC["boost"] == false)
			{
				// Einschalten BOOSTER

				$this->logger("set " . $this->opt["boost.gpio"] . " " . $this->opt["boost.on"]);

				$GPIO->gpio_setState($this->opt["boost.gpio"], $this->opt["boost.on"]);
				$STATIC["boost"] = true;
				$STATIC["boostdelay"] = $time + $this->opt["boost.delay"];

				// weggeschalteten WR mit 0-Wert versorgen

				$rdata = dataprovider($this->opt["dev_replace"]);

				$rdata["publishP"] = true;
				$rdata["pturn"]    = -2;

				if( isset($this->ndata[$this->opt["dev_replace"]]["last"]) )
					$rdata["last"] = $this->ndata[$this->opt["dev_replace"]]["last"];

				$this->ndata[$this->opt["dev_replace"]] = $rdata;

			}

		}

		// wenn in den Optionen boost disabled ist, muss ein eingeleiteter Boost abgewürgt werden

		if($this->opt["boost.disabled"])
			$boost = false;

		if( $boost == false )
		{

			if(
				$GPIO->gpio_getState($this->opt["boost.gpio"]) == $this->opt["boost.on"]
				or
				$STATIC["boost"] == true
				)
			{
				// Ausschalten BOOSTER

				if( $time > $STATIC["boostdelay"] or $this->opt["boost.disabled"] )
				{

					$this->logger("set " . $this->opt["boost.gpio"] . " " . (!$this->opt["boost.on"]));

					$GPIO->gpio_setState($this->opt["boost.gpio"], !$this->opt["boost.on"]);

					$STATIC["boost"] = false;

				}else{

					// Ausschaltverzögerung ist aktiv

					$this->logger("delayed Off (" . ($STATIC["boostdelay"] - $time) . ")");

				}
			}
		}


		$this->logger("ist " . ($STATIC["boost"] ? "an" : "aus") . "!");

		if( $STATIC["boost"] == true )
		{

			// Daten schreiben

			if($boost) $STATIC["boostdelay"] = $time + $this->opt["boost.delay"];

		}


		if( $sensor_data )
		{

			// gültige Werte vom Sensor übernehmen

			$this->result = $res;
			//foreach($res as $idx=>$data)	$this->result[$idx]=$data;

			$this->logger("Kp: " . $this->result["ppv1"] . ", ppv: " . $this->result["ppv"] . " W, U: " . $this->result["u"] . " V, I: " . $this->result["i"] . " A");

			$this->result["publishP"] = true;
			$this->result["publishE"] = true;

			return sensor::RET_DATA;

		}

		// Rückkehr ohne relevante Daten
		return sensor::RET_NODATA;
	}
}

?>