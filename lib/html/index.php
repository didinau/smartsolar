<?php

include __DIR__."/../nloader.php";
include __DIR__."/js-lib.php";
include __DIR__."/html-lib.php";
include __DIR__."/html-helper.php";


$HLPR = new html_helper;


$mode=strtolower(trim(isset($_GET["mode"])?$_GET["mode"]:""));
if($mode=="") $mode=strtolower(trim(isset($_POST["mode"])?$_POST["mode"]:""));

$fn=strtolower(trim(isset($_GET["fn"])?$_GET["fn"]:""));
if($fn=="") $fn=strtolower(trim(isset($_POST["fn"])?$_POST["fn"]:""));

$temp="";

switch($mode)
{
	case "pdata":
		include __DIR__."/pdata.php";
		break;

	case "nconf":
		print $HLPR->jsconfig();
		#print mf::php2js2($config["conf"],"conf")."\n";
		break;

	case "conf":

		$fn="conf.js";

		$HLPR->outFile($fn, "js", $HLPR->jsconfig());

		break;

	case "file":

		$HLPR->outFile($fn);

		break;

	case "picture":

		$HLPR->outFile($fn,"pics");

		break;

	case "verbrauch_i":
		$_POST["mygroup"]=true;
		$_POST["datumbeginn"]=date("01.m.Y");

		//no break;

	case "verbrauch":

		$HLPR->outPage_verbrauch();

		break;

	case "singlegrid":
		$HLPR->html_start();
		print $HLPR->html_pagesinglegrid();
		print $HLPR->html_tabend();
		print $HLPR->js_jssinglegrid();
		$HLPR->html_finish();
		break;

	case "singlepv":

		$HLPR->outPage_pv();

		break;

	case "thermo":

		$HLPR->outPage_thermo();

		break;

	default:

		$HLPR->outPage_aktuell();

		break;
}

?>