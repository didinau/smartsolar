<?php

//
//	Bereitstellung der dynamischen Daten aus DB oder aus JSON-Daten-Datei
//
//		Datei wird von index.php inkludiert!
//

if(!isset($HLPR)) die("ungültiger Aufruf");


isset($_GET["type"]) ? $type=$_GET["type"]:$type="";

#$obj_DB=new my_mysqli($config["dbhost"],$config["dbuser"],$config["dbpwd"],$config["dbname"]);

#$obj_DB = new dpdo_mysql($config["dbhost"],$config["dbuser"],$config["dbpwd"],$config["dbname"]);


switch($type)
{

case "singlethermo":

	$devices=array();

	isset($_GET["datvon"]) ? $datvon=$_GET["datvon"]:$datvon=date("Y-m-d");
	isset($_GET["datbis"]) ? $datbis=$_GET["datbis"]:$datbis=date("Y-m-d");

	$datvon=$HLPR->DB->quote($datvon." 00:00:00");
	$datbis=$HLPR->DB->quote($datbis." 23:59:59");

	$query="select distinct tdevice as device from temperature where tzeit between ".$datvon." and ".$datbis;

	$res=$HLPR->DB->getAll($query);

	if($res->rows)
	{
		while($line=$res->fetch())
		{
			$devices[$line["device"]]=$line["device"];
		}
	}

	foreach($devices as $dev)
	{
		$sensor=array();
		$alias=array();

		if( isset($HLPR->CONF["conf"]["thermo"][$dev]) )
		{
			$opt=smart::smartGetParam($HLPR->CONF["conf"]["thermo"][$dev]);
		}else{
			$opt=array();
		}

		if( isset($opt["port"]) )
			$devids=explode(",",$opt["port"]);
		else
			$devids=array();

		if( count($devids) )
		{
			foreach($devids as $devid)
			{
				$sensor[]=$devid;
				if( isset($opt["alias[".$devid."]"]) )
					$alias[]=$opt["alias[".$devid."]"];
				else
					$alias[]=$devid;
			}
		}

		$devices[$dev]=array(
			"cnt"=>count($sensor),
			"alias"=>$alias,
			"sensor"=>$sensor,
			);

		//print_r($devices[$dev]);

	}

	$query="select * from temperature where tzeit between ".$datvon." and ".$datbis." order by tzeit";

	$res=$HLPR->DB->getAll($query);

	$charts=array(
		"devices"=>$devices,
		"cnt"=>$res->rows,
		"query"=>$query,
		"data"=>array()
	);
	if($res->rows)
	{

		while($line=$res->fetch())
		{

			$erg=array($line["tzeit"]);

			$werte=mf::json_dec($line["tdata"]);

			foreach($devices[$line["tdevice"]]["sensor"] as $dev)
			{
				if( isset($werte[$dev]["atemp"]) )
					$erg[]=$werte[$dev]["atemp"];
				else
					$erg[]=0;
			}


			$charts["data"][]=array(
				"device"=>$line["tdevice"],
				"werte"=>$erg,
				);
		}
	}
//	print "<pre>".print_r($charts,ture)."</pre>";

	print mf::json_enc($charts,"json",false);


	break;

case "singlegrid":
case "singlepv":

	$devices=array();

	isset($_GET["datvon"]) ? $datvon=$_GET["datvon"]:$datvon=date("Y-m-d 00:00:00");
	isset($_GET["datvon"]) ? $datvon=$_GET["datvon"]:$datvon=date("Y-m-d 00:00:00", time()-$HLPR->CONF["conf"]["period_review_hours"] * sec_per_hour);
	isset($_GET["datbis"]) ? $datbis=$_GET["datbis"]:$datbis=date("Y-m-d 23:59:59");

	$datvon=$HLPR->DB->quote($datvon." 00:00:00");
	$datbis=$HLPR->DB->quote($datbis." 23:59:59");

	$query="select distinct pdevice as device from ".$HLPR->CONF["tabname.power"]." where pzeit BETWEEN ".$datvon." and ".$datbis;

	$res=$HLPR->DB->getAll($query);

	if($res->rows)
	{
		while($line=$res->fetch())
		{
			$devices[$line["device"]]=$line["device"];
		}
	}

	$query="select * from ".$HLPR->CONF["tabname.power"]." where pturn < 1 and pzeit BETWEEN ".$datvon." and ".$datbis." order by pzeit";

	$res=$HLPR->DB->getAll($query);

	$charts=array(
		"cnt"=>$res->rows,
		"query"=>$query,
		"data"=>array()
	);
	if($res->rows)
	{
		while($line=$res->fetch())
		{
			$charts["data"][]=array(
				"t"=>$line["pzeit"],
				"pdevice"=>$line["pdevice"],
				"soc"=>(float) $line["soc"],
				"ppv"=>(float) $line["ppv"],
				"ppv1"=>(float) $line["ppv1"],
				"ppv2"=>(float) $line["ppv2"],
				"puser"=>(float) $line["puser"],
				"pload"=>(float) $line["pload"],
				"pcharge"=>(float) $line["pcharge"],
				"pgrid"=>(float) $line["pgrid"],
				"fgrid"=>(float) $line["fgrid"],
				"ugrid"=>(float) $line["ugrid"],
				);
		}
	}
//	print "<pre>".print_r($charts,ture)."</pre>";
	print mf::json_enc($charts,"json",false);

	break;

case "charts":

	$startDT = time() - ( isset($_GET["prec"]) ? $HLPR->CONF["conf"]["period_livechart_hours_hd"] : $HLPR->CONF["conf"]["period_livechart_hours"] ) * sec_per_hour;

	isset($_GET["prec"]) ? $add_where = "" : $add_where = " and pturn in (0, -1)";

	$query = "select * from ".$HLPR->CONF["tabname.power"]." where pzeit >'".date("Y-m-d H:i:s",$startDT)."'".$add_where." order by pzeit";

	$res = $HLPR->DB->getAll($query);

	$charts = array(
		"cnt" => $res->rows,
		"query" => $query,
		"data" => array()
	);
	if( $res->rows )
	{
		while($line=$res->fetch())
		{
			$charts["data"][] = array(
				"device" => $line["pdevice"],
				"t" => $line["pzeit"],
				"soc" => (float) $line["soc"],
				"ppv" => (float) $line["ppv"],
				"ppv1" => (float) $line["ppv1"],
				"ppv2" => (float) $line["ppv2"],
				"puser" => (float) $line["puser"],
				"pload" => (float) $line["pload"],
				"pgrid" => (float) $line["pgrid"],
				"pcharge" => (float) $line["pcharge"],
				"pdischarge" => (float) $line["pdischarge"],
				"fgrid" => (float) $line["fgrid"],
				"ugrid" => (float) $line["ugrid"],
				);
		}
	}

	print mf::json_enc($charts, "json", false);

	break;

default:
	if($HLPR->CONF["netport"])
	{
		$net = new sockserver_tcp;

		if( $sock_connect = $net->connect($HLPR->CONF["server_addr"], $HLPR->CONF["netport"]) )
		{

			$net->write_timeout($sock_connect ,mf::getUserIP(), 500);

			$data = "";

			while( $ret = $net->read_timeout($sock_connect) )
			{

				$data .= $ret;

			}

			$net->close($sock_connect);

			unset($net);

			if( strlen($data) )
			{

				print $data;
				exit(0);

			}

		}else{

			new LogMsg(0, __class__."::".__function__, "error net connect raspi: ".$net->lasterror);

		}
	}

	$res = $HLPR->DB->getAll("select * from data where data_type='live data'");

	$data = $res->fetch();

	print $data["data_raw"];

	break;
}

?>