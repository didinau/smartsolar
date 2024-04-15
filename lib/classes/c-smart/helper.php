<?php

class helper //extends smartsql
{
	public $smart=false;

	public $prognose=array();

	public $queries=array();

	public $memusage=0;


	function __construct(&$_smart)
	{

		$this->smart=&$_smart;

		$this->sql_dbopen();

		$this->DB_fallback_init();

	}

	function __destruct()
	{

		//if(file_exists($this->smart->CONFIG["dataFile-JSON"])) unlink($this->smart->CONFIG["dataFile-JSON"]);

	}

	static function arrayreplace(&$_ziel, &$_quelle,$_replacevars, $_float = false)
	{
		foreach($_replacevars as $to => $from)
		{

			if( isset($_quelle[$from]) )
			{

				$_float ? $_ziel[$to] = floatval($_quelle[$from]) : $_ziel[$to] = $_quelle[$from];

			}else{

				new LogMsg(1, __FUNCTION__, "Wert ".$w." nicht gesetzt!");

			}
		}
	}

	static function arraynum(&$_array, $_werte)
	{
		foreach($_werte as $w)
		{
			isset($_array[$w]) ? $_array[$w."F"] = mf::format_num($_array[$w], 1) : new LogMsg(1, "Warn", __FUNCTION__." Wert ".$w." nicht gesetzt!");
		}
	}

	public function setPublData( $_data, &$insertContainer )
	{

		$ret = false;

		if( $_data["publishP"] )
		{

			$insertContainer->newInsert($this->smart->CONFIG["tabname.power"], DPDO::REPLACE);


			$insertContainer->addMultibleColumns( array(
				"pdevice" => $_data["pdevice"],
				"pzeit" => $_data["T"],
				"pturn" => $_data["pturn"],
				"ppv" => $_data["ppv"],
				"ppv1" => $_data["ppv1"],
				"ppv2" => $_data["ppv2"],
				"ppv3" => $_data["ppv3"],
				"ppv4" => $_data["ppv4"],
				"puser" => $_data["puser"],
				"pload" => $_data["pload"],
				"pgrid" => $_data["pgrid"],
				"pcharge" => $_data["pcharge"],
				"pdischarge" => $_data["pdischarge"],
				"paccharge" => $_data["paccharge"],
				"soc" => $_data["soc"],
				"ubat" => $_data["vbat"],
				"fgrid" => $_data["fac"],
				"ugrid" => $_data["vac"],
				));

			if( count($_data["more"]) )
				$insertContainer->addColumn("more", mf::json_enc($_data["more"]));

			$insertContainer->commit();

			$ret = true;

		}
		if( $_data["publishE"] )
		{

			$insertContainer->newInsert($this->smart->CONFIG["tabname.harvest"], DPDO::REPLACE);

			$insert = array(
				"edatum" => $_data["D"],
				"edevice" => $_data["edevice"],
				"dcharge" => $_data["dcharge"],
				"ddischarge" => $_data["ddischarge"],
				"dgridcharge" => $_data["dgridcharge"],
				"dload" => $_data["dload"],
				"dpv" => $_data["dpv"],
				"dgrid" => $_data["dgrid"],
				"dfromgrid" => $_data["dfromgrid"],
				"dprognose" => $_data["dprognose"],
				"dprognextd" => $_data["dprognextd"],
				"tpv" => $_data["tpv"],
				"tgrid" => $_data["tgrid"],
				"tfromgrid" => $_data["tfromgrid"],
				"tdischarge" => $_data["tdischarge"],
				"tcharge" => $_data["tcharge"],
				"tload" => $_data["tload"],
				"tgridcharge" => $_data["tgridcharge"],
				);

 			if( $_data["dmore"] )
				$insert["dmore"] = mf::json_enc($_data["dmore"]);

			$insertContainer->addMultibleColumns($insert);

			$insertContainer->addColumn("thour", $_data["H"]);

			$insertContainer->commit();

			$insertContainer->newInsert($this->smart->CONFIG["tabname.harvest"], DPDO::REPLACE);

			$insertContainer->addMultibleColumns($insert);

			$insertContainer->addColumn("thour", -1);

			$insertContainer->commit();

			$ret = true;

		}

		if( $_data["publishT"] )
		{

			$insertContainer->newInsert($this->smart->CONFIG["tabname.temperature"], DPDO::REPLACE);

			$insertContainer->addMultibleColumns( array(
				"tdevice" => $_data["device"],
				"tzeit" => $_data["T"],
				"tturn" => $_data["pturn"],
				));

			if($_data["a_temperature"])
			{
				$insertContainer->addColumn("tdata", mf::json_enc($_data["a_temperature"]));
			}else{
				$insertContainer->addColumn("tdata", null);
			}

			$insertContainer->commit();

			$ret = true;

		}

		return $ret;

	}



	function dev_getMaxValues($time, $dev)
	{
		if( !isset($this->smart->STATIC["dev_maxdata"][$dev]["max"]["ts"]) or $this->smart->STATIC["dev_maxdata"][$dev]["max"]["ts"] < $time )
		{

			$this->smart->STATIC["dev_maxdata"][$dev]["max"]=array();

			$res=$this->sql_query("select max(ppv) mpv, max(ppv1) mpv1, max(ppv2) mpv2, max(pgrid) mgrid, max(pload) mload from ".$this->smart->CONFIG["tabname.power"]." where pdevice='".$dev."'");

			if($res->rows)
			{

				foreach($res->fetch() as $index => $idata)
				{

					$this->smart->STATIC["dev_maxdata"][$dev]["max"]["data"][$index]=(float) $idata;

				}
			}

			$res=$this->sql_query("select max(dpv) mdpv, max(dgrid) mdgrid, max(dfromgrid) mdfromgrid from ".$this->smart->CONFIG["tabname.harvest"]." where edevice='".$dev."' and thour=-1");

			if($res->rows)
			{

				foreach($res->fetch() as $index => $idata)
				{

					$this->smart->STATIC["dev_maxdata"][$dev]["max"]["data"][$index]		=(float) $idata;

				}
			}else{

				$this->smart->STATIC["dev_maxdata"][$dev]["max"]["data"]["mdpv"]		=
					$this->smart->STATIC["dev_maxdata"][$dev]["max"]["data"]["mdgrid"]		=
					$this->smart->STATIC["dev_maxdata"][$dev]["max"]["data"]["mdfromgrid"]	= floatval(0);

			}

			$this->smart->STATIC["dev_maxdata"][$dev]["max"]["ts"] = $time;
			$this->smart->STATIC["dev_maxdata"][$dev]["max"]["ts"] += intval(
				isset($this->smart->CONFIG["secs_devmax_validity"]) ? $this->smart->CONFIG["secs_devmax_validity"] : sec_per_hour);

		}

		return $this->smart->STATIC["dev_maxdata"][$dev]["max"];

	}

	function dev_getData($today, $dev, $data)
	{

		$ertrag =
			$leistung = array();

		$res = $this->sql_query("select * from ".$this->smart->CONFIG["tabname.harvest"]." where edatum=date('".$today."') and edevice='".$dev."' and thour=-1");

		if( $res->rows )
			$ertrag = $res->fetch();

		$res = $this->sql_query("select * from ".$this->smart->CONFIG["tabname.power"]." where pzeit='".$data["T"]."' and pdevice='".$dev."'");

		if( $res->rows )
		{

			$leistung = $res->fetch();

			if( $leistung["pturn"] < 0 )
				$leistung = dataprovider($dev);

			$leistung["vbat"] = $leistung["ubat"];

			if( isset($leistung["more"]) and strlen($leistung["more"]) )
				$leistung["more"] = mf::json_dec($leistung["more"]);

		}else{

				$leistung = dataprovider($dev);

		}

		return array_merge($ertrag, $leistung);

	}

	function write_json(&$insertContainer)
	{
		static $gc_time;

		$time = time();

		switch( $this->smart->CONFIG["typeDataStorage"] )
		{
			case "sql":

				$dataTime = "";

				$today = date("Y-m-d");

				if(! isset($this->smart->STATIC["last"]) or !count($this->smart->STATIC["last"]) )
				{
					$dev = $this->smart->DB->fetchall(
						"select max(pzeit) T, pdevice as device, date(T) D from ".$this->smart->CONFIG["tabname.power"]." group by pdevice"
						);

					$devices = array();

					if( $dev->rows > 0)
					{

						foreach( $dev->data as $erg )
						{

							$devices[$erg["device"]] = array();

							$devices[$erg["device"]]["T"] = $erg["T"];

						}

					}else{

						return false;

					}

					unset($dev);

				}else{

					$devices = $this->smart->STATIC["last"];

				}

				if(! isset($this->smart->STATIC["dev_maxdata"]) )
					$this->smart->STATIC["dev_maxdata"] = array();

				if( count($devices) )
				{

					$all = array();


					foreach( $devices as $dev => $data )
					{

						if( $data["T"]>$dataTime )
							$dataTime = $data["T"];

						$dev_pointer = $this->dev_getMaxValues($time, $dev);

						$all[$dev] = array_merge(
							dataprovider(),
							$this->dev_getData($today, $dev, $data),
							$dev_pointer["data"],
							array("device" => $dev, "pdevice" => $dev, "edevice" => $dev)
							);

							// Von Growatt auf andere übertragbar?
						$all[$dev]["dproduktion"] = $all[$dev]["dpv"] + $all[$dev]["ddischarge"] - $all[$dev]["dcharge"];

						$all[$dev]["dselbst"] = $all[$dev]["dproduktion"] - $all[$dev]["dgrid"];

						$all[$dev]["dload"] > 0 ? $all[$dev]["dbezugp"] = $all[$dev]["dfromgrid"] * 100 / $all[$dev]["dload"] : $all[$dev]["dbezugp"]="0";

						$all[$dev]["dproduktion"] > 0 ? $all[$dev]["dselbstp"] = $all[$dev]["dselbst"] * 100 / $all[$dev]["dproduktion"] : $all[$dev]["dselbstp"] = "0";

						if( $all[$dev]["vbat"] > 0 )
							$all[$dev]["ibat"] = floatval(($all[$dev]["pcharge"]-$all[$dev]["pdischarge"]) / $all[$dev]["vbat"]);

						$this->arraynum($all[$dev],
							[ "mdpv","mdgrid","mdfromgrid","dcharge","ddischarge","dload","dpv","tpv","dgrid","dgridc","dproduktion","dfromgrid" ],
							1
							);

						$this->arraynum($all[$dev],
							[ "dgridcharge","dload","dselbst","dbezugp","dselbstp","mdpv","mdgrid","mdfromgrid" ],
							1
							);

						$this->arraynum($all[$dev], [ "mpv","mgrid","mload","ibat","vbat","soc","pcharge","paccharge","pdischarge" ]);
					}

				}

				$res = $this->sql_query("select max(pid) maxpid from ".$this->smart->CONFIG["tabname.power"]." where pturn = 0");

				if( $res->rows )
				{

					$m = $res->fetch();

				}else{

					$m = array("maxpid" => -1);

				}

				unset($res);

				$erg = array();

				if( isset($this->smart->prognose["heute"]) )
				{

					$erg[0]["dprognose"] = $this->smart->prognose["heute"];
					$erg[0]["dprognextd"] = $this->smart->prognose["morgen"];

				}else{

					$erg[0]["dprognose"] = 0;
					$erg[0]["dprognextd"] = 0;

				}

				$this->arraynum($erg[0], array("dprognose","dprognextd"));

				$erg[0]["sonnenaufgang"] = $this->smart->STATIC["sun"]["sunrise"];
				$erg[0]["sonnenzenit"] = $this->smart->STATIC["sun"]["transit"];
				$erg[0]["sonnenuntergang"] = $this->smart->STATIC["sun"]["sunset"];
				$erg[0]["sonnenscheindauer"] = "(".$this->smart->STATIC["sun"]["duration"]." h)";

				$erg[0]["maxpid"] = intval($m["maxpid"]);
				$erg[0]["dataTime"] = $dataTime;

				$erg[0]["configid"] = $this->smart->STATIC["config_id"];


				foreach( $all as $idx => $data )
				{

					$erg[] = $data;

				}

				$erg[0]["devicecount"] = count($all);

				unset($all);

				$erg[0]["x"] = "net";

				$this->smart->json = mf::gzip_enc( mf::json_enc($erg) );

				$erg[0]["x"]="db";

				$json = mf::json_enc($erg);

				$insertContainer->newInsert($this->smart->CONFIG["tabname.data"], DPDO::REPLACE);

				$insertContainer->addMultibleColumns( array(
					"data_type" => 'live data',
					"data_ts" => time(),
					"data_raw" => $json,
					));

				$insertContainer->commit();

				if( intval($gc_time + 600) < $time )
				{

					new LogMsg(1, "MemC", mf::GC_stat($this->smart->CONFIG["mail"], "smart-obj=".$this->smart->getSize()));
					new LogMsg(1, "MemC", "smart-obj=".$this->smart->getSize());
					$gc_time = $time;

				}

				new LogMsg(1, "Snap", $erg[0]["devicecount"]." devices");

				break;

			default:

				new LogMsg(0,"Snap","unknown storage type");

				break;
		}

	}

	public function sql_dbopen()
	{

		$this->smart->DB = new dpdo_mysql($this->smart->CONFIG["dbhost"], $this->smart->CONFIG["dbuser"], $this->smart->CONFIG["dbpwd"], $this->smart->CONFIG["dbname"]);

		$this->checkDatabase();

	}

	public function sql_dbclose()
	{
		$this->smart->DB = null;
	}

	public function checkDatabase()
	{
		if( isset($this->smart->CONFIG["dbrepair.tables"]) and strlen($this->smart->CONFIG["dbrepair.tables"]) )
		{
			$tables = explode(",", $this->smart->CONFIG["dbrepair.tables"]);
			foreach( $tables as $tab )
			{
				$tab = trim($tab);
				if( strlen($tab) )
				{
					$sql = "check table " . $tab;
					$res = $this->sql_query($sql);
					$err = $res->db_errno . " / " . $res->db_error;
					$res = $res->fetchall();
					foreach( $res as $row => $data )
					{
						if( $data["Op"] == "check" and strtolower($data["Msg_text"]) != "ok" )
						{
							$sql = "repair table " . $tab;
							$repair = $this->sql_query($sql);
							$repair = $repair->fetchall();
							new LogMsg(0, "HLPR", __function__ . " " . $sql . ": " . print_r($repair,true));
						}else{
							new LogMsg(0, "HLPR", __function__ . " check ok: table " . $tab);
						}
					}
				}
			}
		}
	}


	public function sql_query($_query)
	{
		// Standart-Query
		// Prüfe, ob die Datenbankverbindung herstellt wurde!

		if( $this->smart->DB )
		{

			if( $this->smart->DB->connection_time() > $this->smart->CONFIG["lifetime_db_obj"] )
			{

				if( $this->smart->DB->reconnect() )
					$this->checkDatabase();

			}

		}else{

			$this->sql_dbopen();

		}

		$res=$this->smart->DB->getAll($_query);

		return $res;
	}

	public function INSERT_Container(&$insertContainer): void
	{

		// Sind Daten in der Fallback-DB vorhanden, dann zunächst neue Daten an die Fallback-DB anhängen und Retry aufrufen
		// Wenn nicht, Daten in DB einfügen und im Fehlerfall an die Fallback-DB anhängen

		$spoolerCnt = $this->DB_fallback_getCmdCount(true, true);

		if( $spoolerCnt == 0 )
		{

			// Keine Daten in der Fallback-DB - normales Einfügen versuchen
			$erg = $insertContainer->exec($this->smart->DB, true);

			if(! $erg )
			{

				// Insert ist fehlgeschlagen, in der Fallback-DB speichern
				new LogMsg(0,"Publ",__function__." Error save data into db");

				$insertData = mf::json_enc($insertContainer->getData());

				// Was tun, wenn die Daten da auch nicht eingefügt werden können
				if(! $this->DB_fallback_insert("", $insertData) )
				{

					new LogMsg(0,"Publ",__function__." Error save data into FALLBACK-db");

				}

			}

		}else{

			// In der Fallback-DB speichern (die enthaltenen Daten haben Vorrang)
			$insertData = mf::json_enc($insertContainer->getData());

			if(! $this->DB_fallback_insert("", $insertData) )
			{

				new LogMsg(0,"Publ",__function__." Error save data into FALLBACK-db");

			}

			// Versuchen die Fallback-Daten in die DB zu transferieren
			$this->INSCONretry();

		}


		// Reset der Containers (Mehrweg)
		$insertContainer->reset();

	}

	public function INSCONretry(): void
	{

		if(! $this->smart->DBFB ) $this->DB_fallback_init();

		// Abfrage generieren
		$stmt = $this->smart->DBFB->query("select * from sqlcmds where data <> '' order by cmd_id");

		// Erstellen des Insert-Containers
		$insertContainer=new dpdoInsertMgr();

		// Ausführen, solange Daten vorhanden
		while( $data = $stmt->fetch() )
		{

			$idata = mf::json_dec($data["data"]);

			if($idata != false)
			{

				// Einfügen in den InsertContainer
				$insertContainer->setData($idata);


				// Insert ausführen
				if( $insertContainer->exec($this->smart->DB, true) )
				{

					// Erfolg!
					new LogMsg(0,"Retry",substr($data["command"],0,28)."...:".substr($data["data"],0,28)."...");

					// Aus Fallback-DB löschen
					$this->smart->DBFB->query("delete from sqlcmds where cmd_id = ".$data["cmd_id"]);

				}else{


					// Insert fehlgeschlagen
					if(! $this->smart->DB )
					{

						// Wenn die DB null, dann neu initialisieren
						$this->sql_dbopen();

					}else{

						// Ansonsten neu verbinden
						$this->smart->DB->reconnect();

					}

					// Bei Fehlschlag abbrechen
					return;
				}
			}else{

				new LogMsg(0,"Retry","Bad data: " . print_r($data, true));

				// Aus Fallback-DB löschen
				//$this->smart->DBFB->query("delete from sqlcmds where cmd_id = ".$data["cmd_id"]);

			}

		}	/* Ende while */

	}

	public function DB_fallback_init()
	{

		$this->smart->DBFB = new dpdo_sqlite;

		if( isset($this->smart->CONFIG['dataFile-FALLBACK']) )
		{
			$this->smart->DBFB->init($this->smart->CONFIG['dataFile-FALLBACK']);
		}else{
			$this->smart->DBFB->db_error = "Config: PATH for Fallback-DB not set! (dataFile-FALLBACK)";
		}

		if( $this->smart->DBFB->db_error )
		{

			new LogMsg(0,"DBfb","Exit on fatal error: ".__function__.": ".$this->smart->DBFB->db_error);

			sleep(10);

			exit(1);

		}


		if( isset($this->smart->CONFIG['FB-DB-INIT']) )
		{

			$sql = is_array($this->smart->CONFIG['FB-DB-INIT']) ? $this->smart->CONFIG['FB-DB-INIT'] : explode(";",$this->smart->CONFIG['FB-DB-INIT']);

			foreach($sql as $command)
				if( strlen($command) )
					$this->smart->DBFB->query($command);

		}

	}

	public function DB_fallback_insert(string $_cmd, string $_values = ""): bool
	{

		new LogMsg(0,"Store",substr($_cmd,0,28)."...:".substr($_values,0,28)."...");

		if(! $this->smart->DBFB )
			$this->DB_fallback_init();


		$iM=new dpdoInsertMgr("sqlcmds");

		$iM->addColumn("timestamp", time());
		$iM->addColumn("command", $_cmd);
		$iM->addColumn("data", $_values);

		$iM->commit();

		if(! $iM->exec($this->smart->DBFB) )
		{

			new LogMsg(0,"FBin",print_r($iM->getResult(),true));

			return false;
		}

		return true;

	}

	public function DB_fallback_getCmdCount($INCLUDE_empty_COMMANDfield = false, $INCLUDE_empty_DATAfield=true)
	{

		$where = "";

		if( isFalse($INCLUDE_empty_COMMANDfield) )
		{

			if( strlen($where) )
				$where .= " and";

			$where .= " command <> ''";

		}


		if( isFalse($INCLUDE_empty_DATAfield) )
		{

			if( strlen($where) )
				$where .= " and";

			$where .= " data <> ''";

		}

		if( strlen($where) )
		{

			$where = " where".$where;

		}

		$stmt = $this->smart->DBFB->query("select count(*) as cnt from sqlcmds");

		if(! $stmt->db_error )
		{

			$cnt = $stmt->fetch();

			if( $cnt["cnt"] > 0 )
				new LogMsg(2, "DBFB", $cnt["cnt"]." Lines...");

			return $cnt["cnt"];

		}else{

			return false;

		}

	}
}

?>