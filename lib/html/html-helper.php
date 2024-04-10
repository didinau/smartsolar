<?php

class html_helper extends sHTMLlib
{

	private $_html_with_filenames_ = true;


	public $CONF = false;

	public $DB = false;

	public $tpl = false;

	public function __construct()
	{

		if(! isset($GLOBALS["config_file"]) )
		{
			print "<pre>no Config!</pre>";
			exit(1);
		}

		$cfg_files = $this->getConfigFiles(dirname($GLOBALS["config_file"])."/conf.d");

		$this->CONF = mf::get_config($GLOBALS["config_file"], $cfg_files);

		switch( strtolower($this->CONF["dbtype"]) )
		{
			case "mysql":
				$this->DB = new dpdo_mysql($this->CONF["dbhost"], $this->CONF["dbuser"], $this->CONF["dbpwd"], $this->CONF["dbname"]);
				break;

			case "sqlite":
				$this->DB = new dpdo_sqlite($this->CONF["dbhost"], $this->CONF["dbuser"], $this->CONF["dbpwd"], $this->CONF["dbname"]);
				break;

			default:

				print "<pre>Wrong DB-Typ!</pre>";
				exit(1);
				break;
		}


		// select Config from DB

		$res = $this->DB->getAll("select * from ".$this->CONF["tabname.data"]." where data_type='config'");

		if($res->rows)
		{

			$conf = $res->fetch();

			$this->CONF = mf::json_dec($conf["data_raw"]);

		}


		if( isset($this->CONF["template-dir"]) and is_dir($this->CONF["template-dir"]) )

			$this->tpl = new tpl($this->CONF["template-dir"], isEnabled($this->CONF["template-filenames"]));

		else

			$this->tpl = new tpl(__DIR__."/templates", isEnabled($this->CONF["template-filenames"]));


		$this->tpl->setPredefs($this->tpl->indexArray2replaceArray($this->CONF["conf"]["txt"], "_t", "_") + $this->tpl->indexArray2replaceArray($this->CONF["conf"]["color"], "_c", "_"));

	}


	public function getConfigFiles($cfg_dir)
	{

		$cfg_files = [];

		if( is_dir($cfg_dir) )
		{

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

	public function outPage_aktuell()
	{

		$this->html_start();

		$this->html_page_aktuell();

		$this->html_tabend();

		$stage = $this->tpl->NewStage();
		$stage->stageFormLoad("page-akt/99-draw.js");
		$stage->stageFormCompile([]);
		$stage->stageMigrate();

		$this->html_end();

		print $this->tpl->finalOut();

	}

	public function outPage_thermo()
	{

		$date=$this->req_getDate(date("Y-m-d",strtotime("-1 day")), date("Y-m-d"));


		$this->html_start();

		$this->html_pagethermo($date);

		$this->html_tabend();

		$stage = $this->tpl->NewStage();
		$stage->stageFormLoad("js_single.js");
		$stage->stageFormCompile(array( "_mode_" => "thermo", "_from_" => $date["von"], "_to_" => $date["bis"] ));
		$stage->stageMigrate();

		$this->html_end();


		print $this->tpl->finalOut();


	}

	public function outPage_pv()
	{

		$date=$this->req_getDate(date("Y-m-d",strtotime("-1 day")), date("Y-m-d"));

		$this->html_start();

		$this->html_pagesinglepv($date);

		$this->html_tabend();

		$stage = $this->tpl->NewStage();
		$stage->stageFormLoad("js_single.js");
		$stage->stageFormCompile(array( "_mode_" => "pv", "_from_" => $date["von"], "_to_" => $date["bis"] ));
		$stage->stageMigrate();

		$this->html_end();


		print $this->tpl->finalOut();

	}

	public function outPage_verbrauch()
	{

		$group=isset($_POST["mygroup"]) ? true : false;

		$date=$this->req_getDate(date("01.m.Y"), date("d.m.Y"), true);

		$this->html_start();

		$this->html_formverbrauch($date["von"], $date["bis"], $group);

		$inverters=$this->getInverters();

		$this->sel_verbrauch($date["von"], $date["bis"], $group, $inverters);

		$this->html_tabend();

		$stage = $this->tpl->NewStage();
		$stage->stageFormLoad("js_verbrauch_general.js");
		$stage->stageFormCompile([]);
		$stage->stageMigrate();

#		$this->tpl->addToTpl($this->js_verbrauch());

		$this->html_end();


		print $this->tpl->finalOut();

	}

	function sel_verbrauch($_von, $_bis, $_group = false, $_devlist = array())
	{
		$stage = $this->tpl->NewStage();

		$js = "";

		$tpv =
		$tload =
		$tgrid =
		$tfromgrid = 0.0;

		$von = mf::datum_de2mysql($_von);

		$bis = mf::datum_de2mysql($_bis);

		$bis = "DATE_ADD('".$bis."', INTERVAL 1 DAY)";

		$query = "select * from ertrag where thour=-1 and edatum between '".$von."' and '".$bis."' order by edatum";

		if( is_array($_devlist) and count($_devlist) )
			$where = " and edevice in (".mf::arr2mysql_in($_devlist).") ";
		else
			$where = "";

		$query = $_group ?
				"select EXTRACT(YEAR_MONTH FROM edatum) as edatum, sum(dcharge) as dcharge, sum(ddischarge) as ddischarge, sum(dpv) as dpv, sum(dload) as dload,
					sum(dfromgrid) as dfromgrid, sum(dgrid) as dgrid from ertrag where thour=-1 and edatum between '".$von."' and ".$bis." ".$where.
					" group by EXTRACT(YEAR_MONTH FROM edatum) order by edatum"
			:
				"select edatum, sum(dcharge) as dcharge, sum(ddischarge) as ddischarge, sum(dpv) as dpv, sum(dload) as dload,
					sum(dfromgrid) as dfromgrid, sum(dgrid) as dgrid from ertrag where thour=-1 and edatum between '".$von."' and ".$bis." ".$where.
					" group by edatum order by edatum";

		$eg = 0;

		$obj_res = $this->DB->getAll($query);

		if( $obj_res->rows )
		{
			// 3-Spaltige Stammtabelle

			$stage->stageAddData("<tr valign='top'><td colspan='3'>");

			// unterteile ich in 2 Spalten: links Datentabelle, rechts Grafik
			$stage->stageAddData("<table width='100%'><tr><td valign='top'>");
			//Datentabelle
			$stage->stageAddData("<table class='ttd'>
				<tr>
				<th align='center'>".($_group ? $this->CONF["conf"]["txt"]["monyear"] : $this->CONF["conf"]["txt"]["date"])."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["charge"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["discharge"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["pv"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["consumption"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["gridfrgrid"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>
				<th align='center'>".$this->CONF["conf"]["txt"]["togrid"]."<br>".$this->CONF["conf"]["bez"]["kwh"]."</th>");

			if( isset($this->CONF["conf"]["EinspBetrag"]) )
			{

				$stage->stageAddData("<th align='center'>".$this->CONF["conf"]["txt"]["fee"]."<br>".$this->CONF["conf"]["bez"]["euro"]."</th>");

				$eg = floatval($this->CONF["conf"]["EinspBetrag"]);

			}

			$stage->stageAddData("</tr>\n");

			$tcharge =
			$tdischarge = 0;

			if( $eg>0 )
			{
				$stage->stageFormLoad("page-verbrauch-line.htm");
			}else{
				$stage->stageFormLoad("page-verbrauch-line-1.htm");
			}

			$repl = [];

			while($line=$obj_res->fetch($obj_res))
			{
				$tcharge += $line["dcharge"];
				$tdischarge += $line["ddischarge"];
				$tload += $line["dload"];
				$tgrid += $line["dgrid"];
				$tfromgrid += $line["dfromgrid"];
				$tpv += $line["dpv"];

				$z = $_group ? mf::monatjahr_mysql2de($line["edatum"],"num") : mf::datum_mysql2de($line["edatum"]);

				$repl["_date_"] = $z;
				foreach($line as $idx => $data)
				{
					$repl["_".$idx."_"] = mf::format_num($data);
				}
				$repl["_entgelt_"] = mf::format_num($line["dgrid"] * $eg / 100);

				$stage->stageFormCompile($repl);

				$js .= "vdata.addRow([ '".$z."', ".number_format($line["dpv"],1).", ".number_format($line["dload"],1).", ".number_format($line["dfromgrid"],1).", ".number_format($line["dgrid"],1)." ]);\n";

			}

			$stage->stageAddData("<tr><td colspan='6'><hr></td></tr>\n");

			$repl["_date_"] = "Gesamt";
			$repl["_dcharge_"] = mf::format_num($tcharge);
			$repl["_ddischarge_"] = mf::format_num($tdischarge);
			$repl["_dpv_"] = mf::format_num($tpv);
			$repl["_dload_"] = mf::format_num($tload);
			$repl["_dfromgrid_"] = mf::format_num($tfromgrid);
			$repl["_dgrid_"] = mf::format_num($tgrid);
			$repl["_entgelt_"] = mf::format_num($tgrid * $eg / 100);

			$stage->stageFormCompile($repl);

			// Ende Datentablle
			$stage->stageAddData("</table>");

			// rechte Spalte Grafik
			$stage->stageAddData("</td><td valign='top'><div class='gauge' id='chart_v'></div></td></tr></table>");

			// Tabellen Ende
			$stage->stageAddData("</td></tr>");

			$stage->stageFormLoad("js_verbrauch.js");
			$stage->stageFormCompile([], array("_adddata_" => $js));

			#$stage->stageAddData($this->js_verbrauch_chartH() . $ret . $this->js_verbrauch_chartF());

		}else{

			$stage->stageAddData("<pre>".print_r($obj_res,true)."</pre>");

		}

		$stage->stageMigrate();

	}

	function getInverters($_use=false,$_showdevice=false)
	{
		$ret = [];

		if( is_array($this->CONF) )
		{

			foreach($this->CONF["conf"]["inverters"] as $dev=>$data)
			{

				$isin = false;
				if( $_use )
				{

					if( $data["use"] )
					{

						$ret[] = $dev;
						$isin = true;

					}

				}else{

					$ret[] = $dev;
					$isin = true;
				}

				if( $_showdevice )
					if( $data["showdevice"] and !$isin )
					{

						$ret[] = $dev;
						$isin = true;

					}
				else
					if(! $isin )
					{

						$ret[] = $dev;
						$isin = true;

					}

			}

		}

		return $ret;
	}

}

?>