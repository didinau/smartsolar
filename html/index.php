<?php

include __DIR__."/../login-data.php";
include __DIR__."/../helper.php";
include __DIR__."/html-lib.php";


$mode=strtolower(trim(isset($_GET["mode"])?$_GET["mode"]:""));
if($mode=="") $mode=strtolower(trim(isset($_POST["mode"])?$_POST["mode"]:""));

$fn=strtolower(trim(isset($_GET["fn"])?$_GET["fn"]:""));
if($fn=="") $fn=strtolower(trim(isset($_POST["fn"])?$_POST["fn"]:""));

switch($mode)
{
	case "pdata":
		include __DIR__."/pdata.php";
		break;

	case "conf":
		print php2js2($growatt["conf"],"conf");
		print php2js2($growatt["bez"],"bez");
		/*
		print "\n// --DynConf--\nConf={\n";
		foreach($growatt["conf"] as $idx=>$wert)
		{
			if(is_string($wert))
				print "\"".$idx."\": \"".$wert."\",\n";
			else
				print "\"".$idx."\": ".$wert.",\n";
		}
		print "}\n// --ENDE--\n";*/
		//break;
		$fn="js/conf.js";

	case "file":
		if(strlen($fn))
		{
			//error_log("FILE=".__DIR__."/".$fn);
			if(file_exists(__DIR__."/".$fn))
			{
				$c=file_get_contents(__DIR__."/".$fn);
				$m=_mime::get_mimetype($fn);
				header("Content-Length: ".strlen($c));
				if(strlen($m)) header("Content-type: ".$m);
				print $c;
			}else{
				html_header404();
			}
		}else{
			html_header404();
		}
		break;

	case "verbrauch_i":
		$_POST["mygroup"]=true;
		$_POST["datumbeginn"]=date("01.m.Y");
		//no break;
	case "verbrauch":
		html_start();
		$datvon="";
		$datbis="";

		$group=isset($_POST["mygroup"]) ? true : false;

		if(isset($_POST["datumbeginn"]))
		{
			$datvon=extract_date($_POST["datumbeginn"]);
		}
		if($datvon=="") $datvon=date("d.m.Y");

		if(isset($_POST["datumende"]))
		{
			$datbis=extract_date($_POST["datumende"]);
		}
		if($datbis=="") $datbis=date("d.m.Y");
		print html_formverbrauch($datvon,$datbis,$group);
//		print html_tabend();
		$js=sel_verbrauch($datvon,$datbis,$group);
		print html_tabend();
		html_finish($js.js_verbrauch());
		break;

	case "singlegrid":
		html_start();
		print html_pagesinglegrid();
		print html_tabend();
		print js_jssinglegrid();
		html_finish();
		break;

	case "singlepv":
		html_start();
		print html_pagesinglepv();
		print html_tabend();
		print js_jssinglepv();
		html_finish();
		break;

	default:
		html_start();
		$devices=array();
		$obj_DB=new cool_mysqldbc($growatt["dbhost"],$growatt["dbuser"],$growatt["dbpwd"],$growatt["dbname"]);
		$obj_res=$obj_DB->query("select distinct pdevice as device from leistung");
		$arr_Latest=$obj_DB->fetch($obj_res);
		if($obj_res["rows"])
		{
			while($arr_Erg=$obj_DB->fetch($obj_res))
			{
				$devices[]=$arr_Erg["device"];
			}
		}

//		print html_page_aktuell($devices);

		print html_page_aktuell($growatt);
		print html_tabend();
		print js_jsaktuell();
		html_finish();
		break;
}

?>