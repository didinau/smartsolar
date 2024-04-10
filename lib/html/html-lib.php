<?php

//
//	Bereitstellung von HTML- und JS-Code (anwendungsspezifisch)
//

class sHTMLlib extends sJSlib
{

	public function html_header404()
	{
		header("HTTP/1.0 404 Not Found", true, 404);
	}

	public function outFile($fn, $subdir="", $temp="")
	{
		if( strlen($fn) )
		{
			if( file_exists(__DIR__."/".$subdir."/".$fn) )
			{
				$c = file_get_contents(__DIR__."/".$subdir."/".$fn);
				$m = _mime::get_mimetype($fn);
				header("Content-Length: ".(strlen($c) + strlen($temp)));
				if( strlen($m) )
					header("Content-type: ".$m);
				print $c;
				if( strlen($temp) )
					print $temp;
			}else{
				$this->html_header404();
			}
		}else{
			$this->html_header404();
		}
	}

	function html_start()
	{
		$this->html_htmlheader();
		$this->html_bodyheader();
	}

	function html_htmlheader()
	{

		$repl = array( "_title_" => $this->CONF["conf"]["HTMLTitle"] );

		$this->tpl->addTranslatedTpl("page_header.htm", $repl);

	}

	function html_bodyheader()
	{

		$repl = array(
			"_headline_" => $this->CONF["conf"]["HTMLHeadline"],
			"_bgcolor_" => $this->CONF["conf"]["HTMLBGColor"],
			);

		#$this->tpl->setPredefRaw("_mainbuttons_", $this->html_mainbuttons());

		$this->tpl->addTranslatedTpl("page_body_start.htm", $repl, array( "_mainbuttons_" => $this->html_mainbuttons()));

	}

	function html_mainbuttons()
	{
		$txt=&$this->CONF["conf"]["txt"];
		return $this->html_button("window.location=\"index.php?mode=aktuell\";",$txt["Blatest"]).
			"&nbsp;".$this->html_button("window.location=\"index.php?mode=singlegrid\";",$txt["Bgridrev"]).
			"&nbsp;".$this->html_button("window.location=\"index.php?mode=singlepv\";",$txt["Bpvrev"]).
			"&nbsp;".$this->html_button("window.location=\"index.php?mode=verbrauch_i\";",$txt["Bconsumpt"]).
			"&nbsp;".$this->html_button("window.location=\"index.php?mode=thermo\";",$txt["Bthermo"]);
	}

	function html_button($_action,$_text)
	{
		return "<button onclick='".$_action."'>".$_text."</button>";
	}

	function html_end()
	{

		$this->tpl->addToTpl("</body></html>");

	}

	function html_tabend()
	{
		$this->tpl->addToTpl("</table>");
	}

	function html_page_aktuell_generator($_dev,$_data)
	{

		$stage = $this->tpl->NewStage();

		$stage->stageAdd("page-akt/03-generator.htm");

		$stage->stageCompile(array( "_dev_" => $_dev));

		$stage->stageMigrate();

	}

	function html_page_aktuell_battery($_dev,$_data)
	{

		$stage = $this->tpl->NewStage();

		$stage->stageAdd("page-akt/05-battery.htm");

		$stage->stageCompile(array( "_dev_" => $_dev));

		$stage->stageMigrate();

	}

	function html_page_aktuell_inverter($_dev, $_data)
	{

		$repl = array( "_dev_" => $_dev );

		$stage = $this->tpl->NewStage();

		$stage->stageAdd("page-akt/10-inverter-top.htm");

		if( isset($_data["meter"]) and $_data["meter"] )
		{

			$stage->stageAdd("page-akt/11-inverter-meter.htm");

		}else{

			$stage->stageAdd("page-akt/11-inverter-no-meter.htm");

		}

//		$stage->stageAdd("html_page_aktuell_inverter_2.htm");

		if( isset($_data["meter"]) and $_data["meter"] )
		{

			if( $this->CONF["conf"]["barBezEinspKombi"] )
			{

				$stage->stageAdd("page-akt/12-inverter-combi.htm");

			}else{

				$stage->stageAdd("page-akt/12-inverter-no-combi.htm");
			}
		}

		if( isset($_data["battery"]) and $_data["battery"] )
		{

			$stage->stageAdd("page-akt/13-inverter-bat-gridcharge.htm");

		}

		$stage->stageAdd("page-akt/14-inverter-mitte.htm");

		if( isset($_data["battery"]) and $_data["battery"] )
		{

			$stage->stageAdd("page-akt/15-inverter-bat-discharge.htm");

		}

		$stage->stageAdd("page-akt/16-inverter-bat-infogridcharge.htm");

		if( isset($_data["gridmeter"]) and $_data["gridmeter"] )
		{

			$stage->stageAdd("page-akt/17-inverter-grid-f+u.htm");

		}

		$stage->stageAdd("trennzeile-3.htm");

		$stage->stageCompile($repl);

		$stage->stageMigrate();

	}

	function html_page_aktuell()
	{

		$this->tpl->addTpl("page-akt/00-prog.htm");

		$this->tpl->addTpl("page-akt/01-wetter.htm");


		if(! count($this->CONF["conf"]["generators"]) )
		{

			$this->tpl->addTpl("tr-nodevices.htm");

		}else
		{
			foreach( $this->CONF["conf"]["generators"] as $dev => $data )
			{

				if( $data["showdevice"] ) $this->html_page_aktuell_generator($dev, $data);

			}
		}

		if(! count($this->CONF["conf"]["batteries"]) )
		{

			$this->tpl->addTpl("tr-nodevices.htm");

		}else foreach( $this->CONF["conf"]["batteries"] as $dev => $data )
		{

			if($data["showdevice"])
				$this->html_page_aktuell_battery($dev, $data);

		}


		if(! count($this->CONF["conf"]["inverters"]) )
		{

			$this->tpl->addTpl("tr-nodevices.htm");

		}else foreach( $this->CONF["conf"]["inverters"] as $dev => $data )
		{

			if($data["showdevice"])
				$this->html_page_aktuell_inverter($dev, $data);

		}


		#$this->tpl->addTpl("page-akt/page-akt-ll.htm");

}

	function html_pagesinglegrid()
	{

		$txt=&$this->CONF["conf"]["txt"];
		return "
			<tr valign='top'><td align='left'><strong>".$txt["griddata"]."</strong></td><td>&nbsp;</td><td>&nbsp;</td></tr>
			<tr valign='top'><td colspan='3'><div class='gauge' id='chart_singleG'></div></td>
			<tr valign='top'><td colspan='3'><div class='gauge' id='chart_singleGU'></div></td>
			<tr valign='top'><td colspan='3'><div class='gauge' id='chart_singleGF'></div></td>
		";
	}

	function html_pagesinglepv_generator($_dev, $_data)
	{

		$stage = $this->tpl->NewStage();

		$stage->stageFormLoad("html_page_singlepv_generator.htm");

		$repl = [];

		$repl["_dev_"] = $_dev;

		$stage->stageFormCompile($repl);

		$stage->stageMigrate();

	}

	function html_pagesinglepv($_date)
	{

		$this->html_formdate("singlepv",$_date["von"], $_date["bis"]);

		if(! count($this->CONF["conf"]["generators"]) )
		{

			$this->tpl->addTpl("tr-nodevices.htm");

		}else foreach($this->CONF["conf"]["generators"] as $dev => $data)
		{

			$this->html_pagesinglepv_generator($dev, $data);

		}

		//return $ret;
	}

	function html_pagethermo($_date)
	{

		$this->html_formdate("thermo", $_date["von"], $_date["bis"]);

		$stage = $this->tpl->NewStage();

		if(!count($this->CONF["conf"]["thermo"]))
		{

			$stage->stageAdd("html-nodevice.htm");

			$stage->stageMigrate();

		}else{

			$stage->stageMigrate();

			foreach($this->CONF["conf"]["thermo"] as $dev=>$data)
			{

				$this->html_pagethermo_thermo($dev, $data);

			}
		}

	}

	function html_pagethermo_thermo($_dev,$_data)
	{

		$repl = [];

		$repl["_dev_"] = $_dev;

		$stage = $this->tpl->NewStage();

		$stage->stageFormLoad("page-thermo.htm");

		$stage->stageFormCompile($repl);

		$stage->stageMigrate();

	}

	function html_formverbrauch($_von, $_bis, $_group)
	{

		$repl = [];

		$repl["_url_"] = "index.php";
		$repl["_von_"] = $_von;
		$repl["_bis_"] = $_bis;
		$repl["_group_"] = $_group ? "checked" : "";

		$stage = $this->tpl->NewStage();

		$stage->stageFormLoad("frm-verbrauch.htm");

		$stage->stageFormCompile($repl);

		$stage->stageMigrate();

	}

	function html_formdate($_mode, $_von, $_bis="")
	{

		$datum["von"]=explode("-",$_von);
		$datum["bis"]=explode("-",$_bis);
		$heute=explode("-",date("Y-m-d"));

		foreach(array("von","bis") as $idx)
		{

			if( isset($datum[$idx][0]) )
			{
				$datum[$idx]["y"]=(int) $datum[$idx][0];
				if( $datum[$idx]["y"] < 2023 or $datum[$idx]["y"] > $heute[0] )
					$datum[$idx]["y"]=$heute[0];
			}else{
				$datum[$idx]["y"]=$heute[0];
			}

			if( isset($datum[$idx][1]) )
			{
				$datum[$idx]["m"]=(int) $datum[$idx][1];
				if( $datum[$idx]["m"] < 1 or $datum[$idx]["m"] > 12 )
					$datum[$idx]["m"]=$heute[1];
			}else{
				$datum[$idx]["m"]=$heute[1];
			}

			if( isset($datum[$idx][2]) )
			{
				$datum[$idx]["d"]=(int) $datum[$idx][2];
				if( $datum[$idx]["d"] < 1 or $datum[$idx]["d"] > 31 )
					$datum[$idx]["d"]=$heute[2];
			}else{
				$datum[$idx]["d"]=$heute[2];
			}
		}

		$repl = [];
		$rrepl = [];

		$repl["_url_"] = "index.php";
		$repl["_mode_"] = $_mode;

		$rrepl["_vtag_"] = $this->html_getSelectNumbersList("vtag", $datum["von"]["d"],1,31);
		$rrepl["_vmonat_"] = $this->html_getSelectNumbersList("vmonat", $datum["von"]["m"],1,12);
		$rrepl["_vjahr_"] = $this->html_getSelectNumbersList("vjahr", $datum["von"]["y"], 2022, $heute[0]);

		$rrepl["_btag_"] = $this->html_getSelectNumbersList("btag", $datum["bis"]["d"],1,31);
		$rrepl["_bmonat_"] = $this->html_getSelectNumbersList("bmonat", $datum["bis"]["m"],1,12);
		$rrepl["_bjahr_"] = $this->html_getSelectNumbersList("bjahr", $datum["bis"]["y"], 2022, $heute[0]);

		$stage = $this->tpl->NewStage();

		$stage->stageFormLoad("frm-date.htm");

		$stage->stageFormCompile($repl, $rrepl);

		$stage->stageMigrate();

	}

	function html_getSelectNumbersList($_id, $_selectedVal, $_from, $_to, $_numberlen=2,$_actions="")
	{
		$sel_chain="<select id='".$_id."' ".$_actions.">\n";
		for($line=$_from; $line<=$_to; $line++)
		{
			$sel_chain.="<option value='".$line."'";
			if($_selectedVal==$line)	$sel_chain.=" selected";
			$sel_chain.=">".str_pad($line, $_numberlen, "0" , STR_PAD_LEFT )."</option>\n";
		}
		$sel_chain.="</select>";
		return $sel_chain;
	}

	function req_getDate($_vonDefault=false, $_bisDefault=false, $_german=false)
	{
		$datvon="";
		$datbis="";

		if(isset($_POST["datvon"]))
		{
			$datvon=trim($_POST["datvon"]);
		}else{
			if(isset($_GET["datvon"]))
			{
				$datvon=trim($_GET["datvon"]);
			}else{
				$datvon=$_vonDefault;
			}
		}

		if( $_german )	$datvon=mf::extract_date($datvon);

		if(isset($_POST["datbis"]))
		{
			$datbis=trim($_POST["datbis"]);
		}else{
			if(isset($_GET["datbis"]))
			{
				$datbis=trim($_GET["datbis"]);
			}else{
				$datbis=$_bisDefault;
			}
		}

		if( $_german )	$datbis=mf::extract_date($datbis);

		return array("von"=>$datvon, "bis"=>$datbis);
	}

}

?>