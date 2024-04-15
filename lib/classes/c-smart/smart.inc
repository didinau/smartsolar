<?php

class smart
{

	public $controlData = false;

	public $ShouldEnd = 0;

	public $HELPER		= false;

	public $deviceList	= false;

	public $debug		= false;

	public $cfg_file	= false;

	public $cfg_def_ints= [

				"config_ftime"			=> [ "min" => 0,	"max" => null,	"def" => 0		],
				"SaveStaticDataIv"		=> [ "min" => 300,	"max" => null,	"def" => 300	],
				"lifetime_smart_obj"	=> [ "min" => 0,	"max" => null,	"def" => 0		],
				"lifetime_db_obj"		=> [ "min" => 0,	"max" => null,	"def" => 14400	],

				 ];


	function __construct($_cfg, $_debug = false)
	{
		global $loglevel;

		$this->debug = $_debug;

		$this->cfg_file = $_cfg;

		$this->controlData = new ControlData;

		$ft = 0;

		$cfg_files = $this->getConfigFiles(dirname($this->cfg_file)."/conf.d", $ft);

		$this->controlData->CONFIG = mf::get_config($this->cfg_file, $cfg_files);

		if( isset($this->controlData->CONFIG["debug"]) )
			$loglevel = $this->controlData->CONFIG["debug"];

		$this->HELPER = new helper($this->controlData);

		$this->controlData->STATIC = new ControlDatacenter($this->controlData->DB, $this->controlData->DBFB);
		$this->controlData->STATIC->load();

		$this->checkConfig(true);

		$this->updatePrognose();

		$this->controlData->prognose = &$GLOBALS["prognose"];

	}

	public function updatePrognose()
	{

		if(! isset($GLOBALS["prognose"]) and isset($this->controlData->STATIC["weatherforecast"]) )
		{

			// suche ertragsdaten aus irgendeinem Wetterbreicht zur Erstversorgung nach Systemstart

			$h = date("Ymd");
			$m = date("Ymd",strtotime("+1 day"));

			$GLOBALS["prognose"]["date"] = $h;
			$GLOBALS["prognose"]["ts"] = time();

			foreach( $this->controlData->STATIC["weatherforecast"] as $provider => $provdata )
			{
				foreach( $provdata as $day => $daydata )
				{
					if( $day == $h and isset($daydata["harvestKWH"]) and intval($daydata["harvestKWH"]) > 0 )
						$GLOBALS["prognose"]["heute"] = $daydata["harvestKWH"];

					if( $day == $m and isset($daydata["harvestKWH"]) and intval($daydata["harvestKWH"]) > 0 )
						$GLOBALS["prognose"]["morgen"] = $daydata["harvestKWH"];
				}
			}
		}

		$this->controlData->STATIC["sun"] = agt::sonnendaten($this->controlData->CONFIG["latitude"],$this->controlData->CONFIG["longitude"]);

	}

	public function saveStaticData($_immediately = false)
	{
		//SaveStaticData

		$n = time();

		if( $_immediately or ! isset($this->controlData->STATIC["StaticDataSaved"]) or ( $this->controlData->STATIC["StaticDataSaved"] + $this->controlData->CONFIG["SaveStaticDataIv"] ) < $n )
		{

			$this->controlData->STATIC->save($n);

			$this->controlData->STATIC["StaticDataSaved"] = $n;

			new LogMsg(2, "Stat", "saved");

		}
	}

	public function proto($_type, $_device, $_text)
	{
		// Erstellen des Insert-Containers

		$insertContainer=new dpdoInsertMgr($this->controlData->CONFIG["tabname.proto"], DPDO::REPLACE);

		$insertContainer->addMultibleColumns( array(
			"proto_ts" => date("Y-m-d H:i:s",time()),
			"proto_type" => $_type,
			"proto_device" => $_device,
			"proto_text" => $_text,
			));

		$insertContainer->commit();

		$this->HELPER->INSERT_Container($insertContainer);

	}

	public function getStaticData()
	{

		$this->controlData->STATIC->load();

		$this->checkVars();
	}

	public function cleanStaticData($_immediately = false)
	{
		$cleantab = array("turn", "next", "lastquery+", "lastquery", "modules", "current", "last", "dev_maxdata", );

		$time = time();

		if(! isset($this->controlData->STATIC["StaticDataClean"]) )
			$this->controlData->STATIC["StaticDataClean"] = 0;

		if( $_immediately or $this->controlData->STATIC["StaticDataClean"] < $time )
		{

			$this->getDeviceList();

			$devs = array();

			$cleaned = 0;

			if( count($this->deviceList) )
				foreach($this->deviceList as $sn=>$data)
					$devs[] = $sn;

			new LogMsg(2, "StCl",__function__." begin: ".count($devs)." devices");

			if( count($devs) )
			{

				foreach($cleantab as $section)
				{

					if( isset($this->controlData->STATIC[$section]) )
						foreach($this->controlData->STATIC[$section] as $index => $data)
					{

						if(! in_array($index, $devs) )
						{

							unset($this->controlData->STATIC[$section][$index]);

							new LogMsg(2, "StCl",__function__." deleting ".$section."/".$index);

							$cleaned++;

						}
					}
				}

				if( isset($this->controlData->STATIC["CONFIG_OVERWRITE"]) )
					$this->controlData->STATIC["CONFIG_OVERWRITE"] = array();

				if(! isset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"]) )
					$this->controlData->STATIC["CONFIG_OVERWRITE_TIME"] = array();

				foreach($this->controlData->STATIC["CONFIG_OVERWRITE"] as $index => $data)
				{

					if(! isset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$index]) or $time > $this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$index] )
					{
						unset($this->controlData->STATIC["CONFIG_OVERWRITE"][$index]);

						if( isset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$index]) )
							unset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$index]);

						new LogMsg(2, "StCl",__function__." deleting CONFIG_OVERWRITE/".$index);

						$cleaned++;
					}
				}

			}

			new LogMsg(2, "StCl",__function__." end: delete ".$cleaned." entries");

			$this->controlData->STATIC["StaticDataClean"] = $time + ( isset($this->controlData->CONFIG["secs_static_clean"]) ? $this->controlData->CONFIG["secs_static_clean"] : 600 );

			if( isset($this->controlData->STATIC["lastquery"]) )
			{
				unset($this->controlData->STATIC["lastquery"]);
				$cleaned++;
			}

			if( $cleaned )
			{

				$this->saveStaticData();

				$this->checkConfig(true);

				return true;

			}
		}

		return false;
	}

	public function checkVars()
	{

		foreach($this->cfg_def_ints as $idx => $vals)
		{

			if(! isset($this->controlData->CONFIG[$idx]) )
			{

				$this->controlData->CONFIG[$idx] = $vals["def"];

			}else{

				$val = intval( $this->controlData->CONFIG[$idx] );

				if( ( ! is_null($vals["min"]) and $val < $vals["min"] ) or ( ! is_null($vals["max"]) and $val > $vals["max"] ) )
					$this->controlData->CONFIG[$idx] = $vals["def"];
				else
					$this->controlData->CONFIG[$idx] = $val;

			}
		}
	}

	public function configCheckOverwrite()
	{

		if( isset($this->controlData->STATIC["CONFIG_OVERWRITE"]) and count($this->controlData->STATIC["CONFIG_OVERWRITE"]) )
		{
			foreach($this->controlData->STATIC["CONFIG_OVERWRITE"] as $i => $v)
			{

				$i = trim($i);
				if( strlen($i) )
					$this->setConfigValue($i, $v);

			}
			return true;
		}
		return false;
	}

	public function configEntryOverwrite($_index, $_value, $_timeInSec = 3600)
	{
		if(! is_null($this->controlData->STATIC) )
		{
			if(! isset($this->controlData->STATIC["CONFIG_OVERWRITE"]) )
				$this->controlData->STATIC["CONFIG_OVERWRITE"] = array();

			if(! isset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"]) )
				$this->controlData->STATIC["CONFIG_OVERWRITE_TIME"] = array();

			$_index = trim($_index);

			if(! isset($this->controlData->STATIC["CONFIG_OVERWRITE"][$_index]) or ( isset($this->controlData->STATIC["CONFIG_OVERWRITE"][$_index]) and $this->controlData->STATIC["CONFIG_OVERWRITE"][$_index] != $_value) )
			{

				if( is_null($_value) )
				{

					if( isset($this->controlData->STATIC["CONFIG_OVERWRITE"][$_index]) )
						unset($this->controlData->STATIC["CONFIG_OVERWRITE"][$_index]);

					if( isset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$_index]) )
						unset($this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$_index]);

				}else{

					$this->setConfigValue($_index, $_value);

					$this->controlData->STATIC["CONFIG_OVERWRITE"][$_index] = $_value;

				}

			}

			if( isset($this->controlData->STATIC["CONFIG_OVERWRITE"][$_index]) )
			{
				$this->controlData->STATIC["CONFIG_OVERWRITE_TIME"][$_index] = time() + $_timeInSec;
			}

			$this->saveStaticData(true);
			$this->checkConfig(true);

		}
	}

	public function setConfigValue($_index, $_value)
	{
		$cfg = &$this->controlData->CONFIG;

		$idx = explode("/",$_index);
		$word = array_pop($idx);

		foreach($idx as $a)
		{

			if(! isset($cfg[$a]) )
			{
				new LogMsg(2, "Cnfg","set config value: new index '".$a."'");
				$cfg[$a] = array();
			}

			$cfg = &$cfg[$a];

		}

		$cfg[$word] = $_value;

	}

	public function getConfigFiles($cfg_dir, &$ft)
	{

		$cfg_files = [];

		if( is_dir($cfg_dir) )
		{

			clearstatcache(false, $cfg_dir);

			$ft = getHigherVal($ft, filemtime($cfg_dir));

			$files = mf::fileFinder($cfg_dir);

			foreach( $files as $name => $data )
			{

				if( $data["fext"] == "ini" or $data["fext"] == "conf" )
				{

					$cfg_files[] = $name;

				}
			}
		}

		return $cfg_files;
	}

	public function checkConfig($_immediately = false)
	{
		/*
			// Prüfen, ob eine NEUE CONFIG bereitsteht
		*/

		global $loglevel;

		if( is_file($this->cfg_file) )
		{

			clearstatcache(false, $this->cfg_file);

			$ft = filemtime($this->cfg_file);

			$cfg_files = $this->getConfigFiles(dirname($this->cfg_file)."/conf.d", $ft);


			if( $this->controlData->STATIC["config_ftime"] != $ft or $_immediately )
			{
				try
				{

					new LogMsg(2, "Cnfg", "reading".($_immediately ? " enforced":""));

					$newconfig = mf::get_config($this->cfg_file, $cfg_files);

					$this->controlData->STATIC["config_ftime"] = $ft;

					$this->controlData->CONFIG = $newconfig;

					if( isset($this->controlData->CONFIG["debug"]) )
						$loglevel = $this->controlData->CONFIG["debug"];

					$this->configCheckOverwrite();

					$this->configDeleteUnusedDevices($this->controlData->CONFIG);

					$this->configDBStore($this->controlData->CONFIG);

					$this->getDeviceList();

				} catch (Throwable $e) {

					new LogMsg(0,"Cnfg", "Error reading ".$e->getMessage());

				}

				$this->checkVars();

				$this->getDeviceList();

				if( $this->controlData->CONFIG["lifetime_smart_obj"] > 0 and $this->ShouldEnd == 0 )
					$this->ShouldEnd = time() + $this->controlData->CONFIG["lifetime_smart_obj"];

			}
		}
	}

	public function configDeleteUnusedDevices(&$_config)
	{

		foreach(array("thermo","batteries","inverters","generators") as $type)
		{

			if( isset($_config["conf"][$type]) and count($_config["conf"][$type]) )
			{

				foreach($_config["conf"][$type] as $sn => $dev)
				{
					if(! isset($dev["use"]) or !$dev["use"] )
					{

						unset($_config["conf"][$type][$sn]);
						new LogMsg(3, "Cnfg","Delete unused Dev ".$sn);

					}
				}

			}

		}
	}

	public function configDBStore($_configdata)
	{

		if(! $this->controlData->DB )
			$this->HELPER->sql_dbopen();

		$iM=new dpdoInsertMgr("data", true);

		$iM->addColumn("data_type", "config");
		$iM->addColumn("data_ts", $this->controlData->STATIC["config_ftime"]);
		$iM->addColumn("data_raw", mf::json_enc($_configdata) );
		$iM->commit();

		if( $iM->exec($this->controlData->DB) )
		{

			$this->controlData->STATIC["config_id"] = $iM->getResult()[0]["id"];

		}else{

			print_r($iM->getResult());

		}


	}

	public function getDeviceList()
	{
		$devs = array();
		$config = $this->controlData->CONFIG["conf"];

		foreach(array("thermo","batteries","inverters","generators","other") as $type)
		{
			if( isset($config[$type]) and count($config[$type]) )
				foreach($config[$type] as $sn => $dev)
			{

				if( isset($dev["use"]) and $dev["use"] )
				{
					$devs[$sn] = isset($devs[$sn]) ? array_merge($dev,$devs[$sn]) : $dev;

					if(!isset($dev["interval"]) or ( (int) $dev["interval"] < 10) )
						$dev["interval"] = (int) $this->controlData->CONFIG["sleep"];

					if(!isset($dev["intervalnight"]) or ( (int) $dev["intervalnight"] < 10))
						$dev["intervalnight"] = (int) $dev["interval"];

				}else{

					if(! isset($dev["use"]) )
						new LogMsg( 2,"Cnfg","no entry 'use' in config for ".$sn );

				}
			}
		}

		$this->deviceList = $devs;

	}

	public function isNight()
	{

		$tstart	= agt::timeToSeconds($this->controlData->STATIC["sun"]["sunrise"]);

		$tend	= agt::timeToSeconds($this->controlData->STATIC["sun"]["sunset"]);

		$tn		= agt::timeToSeconds(date("H:i"));

		if( $tn >= $tstart and $tn <=$tend )

			return false;

		else

			return true;

	}

	public function getDeviceQuery($_time, &$_devs)
	{
		$devs = $this->deviceList;
		$nextrun = 0;

		$night = $this->isNight();

		foreach($devs as $sn=>$dev)
		{

			// Tag- oder Nachtinterval
			if( $night and isset($dev["intervalnight"]) )
				$interval = $dev["intervalnight"];
			else
				$interval = $dev["interval"];

			if(! isset($this->controlData->STATIC["turn"][$sn]) )
			{

				$this->controlData->STATIC["turn"][$sn] = 0;

				$this->controlData->STATIC["next"][$sn] = 0;

			}else{

				// Ist die Sensorauswertung an der Reihe?

				if( $_time > $this->controlData->STATIC["next"][$sn] )
				{

					$this->controlData->STATIC["turn"][$sn]++;

					if( $this->controlData->STATIC["turn"][$sn] >= $this->controlData->CONFIG["turns"] )
						$this->controlData->STATIC["turn"][$sn] = 0;

					$this->controlData->STATIC["next"][$sn] = $_time + $interval - 1;

					$nextrun = getLowerVal($nextrun, $this->controlData->STATIC["next"][$sn]);

				}else{

					// noch nicht dran

					$nextrun = getLowerVal($nextrun, $this->controlData->STATIC["next"][$sn]);

					unset($devs[$sn]);

				}
			}
		}

		$_devs = $devs;

		return $nextrun;

	}

	public function readFromSensor($time, $sn, $dev)
	{
		$rdata = dataprovider($sn);

		new LogMsg(3, "Sens", $sn);

		if( isset($this->controlData->STATIC["last"][$sn]) )
			$rdata["last"] = $this->controlData->STATIC["last"][$sn];
		else
			$rdata["last"] = false;

		$erg = querySensor($dev, $rdata, $this->controlData);

		if( $erg == sensor::RET_DATA )
		{

			$this->controlData->newData[$sn] = $rdata;

			if($rdata["pturn"] === null )
				$this->controlData->newData[$sn]["pturn"] = $this->controlData->STATIC["turn"][$sn];

			$this->controlData->STATIC["lastquery+"][$sn] = $time;

		}else{

			if( $erg==sensor::RET_ERROR )
				new LogMsg(1, "Sens","NO DATA / ERROR from ".$sn." type ".$dev["data"]);

			if( isset($dev["heartbeat"]) and $dev["heartbeat"] )
			{

				if(! isset($this->controlData->STATIC["lastquery+"][$sn]))
					$this->controlData->STATIC["lastquery+"][$sn]=0;

				if( ( ($time - (int) $this->controlData->STATIC["lastquery+"][$sn]) / 60 ) > $dev["heartbeat"] )
				{

					new LogMsg(3, "Hrtb", $sn);

					$rdata = dataprovider($sn);

					$rdata["publishP"] = true;
					$rdata["pturn"] = -1;

					$this->controlData->newData[$sn] = $rdata;

					$this->controlData->STATIC["lastquery+"][$sn] = $time;

				}
			}
		}

	}

	static function smartGetParam($_conf)
	{

		$opt = array();

		$dev = explode("/", $_conf["data"]);

		if( isset($dev[2]) )
			$opts = trim($dev[2]);
		else
			$opts = "";

		$opt["port"] = trim($dev[1]);

		foreach(array("param","options") as $o)
		{

			if( isset($_conf[$o]) )
			{

				if( is_array($_conf[$o]) )
				{

					foreach($_conf[$o] as $op)
						$opts .= ",".trim($op);

				}else{

					$opts .= ",".trim($_conf[$o]);

				}
			}
		}

		$opts = explode(",", $opts);

		foreach($opts as $op)
		{

			$op = trim($op);

			if( strlen($op) )
			{

				if(! strpos($op, '=') )
				{

					$opt[$op] = 1;

				}else{

					$tmp = explode("=", $op,2);
					$tmp[0] = trim($tmp[0]);

					if( strlen($tmp[0]) )
						$opt[$tmp[0]] = trim($tmp[1]);

				}
			}
		}

		return $opt;
	}


	public function setupPCNTL()
	{

		try
		{

			declare(ticks = 1);

			if( function_exists("pcntl_signal") )
			{

				if(! pcntl_signal(SIGTERM, "sig_handler") )
					new LogMsg(0, "PCNTL", "error sig handler for SIGTERM");

				if(! pcntl_signal(SIGHUP,  "sig_handler") )
					new LogMsg(0, "PCNTL", "error sig handler for SIGHUP");

				if(! pcntl_signal(SIGUSR1, "sig_handler") )
					new LogMsg(0, "PCNTL", "error sig handler for SIGUSR1");

			}else{

				new LogMsg(0, "PCNTL", "Prozesskontrolle nicht verfügbar");

			}

			cli_set_process_title( (isset($GLOBALS["application"]) ? $GLOBALS["application"] : "") ." ".basename(__FILE__));

		}catch(Throwable $e){

			new LogMsg(0, "PCNTL Err", $e->getMessage());

		}
	}

	public function setupSocketServer()
	{

		global $net;

		try
		{
			$net = array("obj" => false, "socket" => false, );

			if( $this->controlData->CONFIG["netport"] )
			{

				$net["obj"] = new sockserver_tcp;

				if( $net["socket"] = $net["obj"]->listen($this->controlData->CONFIG["netport"]) )
				{

					$net["obj"]->nonblock($net["socket"]);

				}else{

					new LogMsg(0, "Sock", "could'nt set up listener socket");

				}

			}else{

				new LogMsg(0, "Sock", "socket server is not configured");

			}

			register_tick_function("net_connect", true);

		}catch(Throwable $e){

			new LogMsg(0, "SockServer Err", $e->getMessage());

		}
	}

	public function main()
	{

		//$growatt_nextrun = 0;

		$other_nextrun = 0;

		$nextrun = 0;


		new LogMsg(0, "Mstr", "ready");


		while( true )
		{

			$time = time();

			$writeJSON = false;

			if( $time > $other_nextrun )
			{

				// Prüfen, ob eine NEUE CONFIG bereitsteht
				$this->checkConfig();

				updatePrognose();

				$other_nextrun = $time + $this->controlData->CONFIG["sleep"];

			}

			/* Zeit für neue CLOUD Daten?
			if( $this->controlData->CONFIG["useGrowattCloud"] and $time > $growatt_nextrun )
			{

				$growatt_nextrun = time() + $this->controlData->CONFIG["cloudsleep"] - 1;

				growatt_query($this->HELPER, $this->controlData->newData, $writeJSON);

			}	//*/


			// Andere Geräte

			if( isFalse($this->controlData->DB) )
				$this->HELPER->sql_dbopen();


			// Auswertung beginnen

			if( $time > $nextrun )
			{

				// Erstellen des Insert-Containers
				$insertContainer=new dpdoInsertMgr();


				// Zusammensuchen der Gerätschaften
				$deviceQuery = array();

				$nextrun = $this->getDeviceQuery($time,$deviceQuery);

				if( count($deviceQuery) )
					foreach( $deviceQuery as $sn => $dev )
				{

					if( $dev["data"] != "cloud" and substr($dev["data"], 0, 5) !== "special/" and $dev["use"] )
					{

						$this->readFromSensor($time, $sn, $dev);

					}
				}

				if( count($deviceQuery) )
					foreach( $deviceQuery as $sn => $dev )
				{

					if( substr($dev["data"], 0, 5) == "special/" and $dev["use"] )
					{

						$this->readFromSensor($time, $sn, $dev);

					}
				}

				if( is_array($this->controlData->newData) and count($this->controlData->newData) )
				{
					foreach( $this->controlData->newData as $sn => $dev )
					{

						if( isset($this->controlData->prognose) )
						{

							$dev["dprognose"]	= $this->controlData->prognose["heute"];
							$dev["dprognextd"]	= $this->controlData->prognose["morgen"];

							if( isset($this->controlData->prognose["umorgen"]) )
								$dev["dprogunextd"]	= $this->controlData->prognose["umorgen"];
						}


						if( $dev["publishP"] or $dev["publishE"] or $dev["publishT"] )
						{

							if( $this->HELPER->setPublData($dev, $insertContainer) )
								$writeJSON = true;

						}

						if( $dev["pturn"] > -1 )
						{

							$this->controlData->STATIC["current"][$sn] = $dev;

							$this->controlData->STATIC["last"][$sn] = array("T" => $dev["T"], "S" => $dev["S"], "D" => $dev["D"], "H" => $dev["H"]);

						}

					}	/* END For */


					$this->HELPER->INSERT_Container($insertContainer);


					// Write JSON bedeutet das Erstellen eines aktuellen Daten-snapshots
					if( $writeJSON )
					{

						$this->HELPER->write_json($insertContainer);

						$writeJSON = false;

						if(! $this->cleanStaticData() )
							$this->saveStaticData();
					}

					// Abspeichern des insertContainer in der Datenbank


					$this->HELPER->INSERT_Container($insertContainer);

					$this->controlData->newData = array();

				}

				if( $this->ShouldEnd > 0 and $time > $this->ShouldEnd )
					return;

			}

			$deviceQuery = array();

			usleep($this->controlData->CONFIG["loopsleep"]);

		}
	}
}

?>