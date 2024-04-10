<?php

/*

Erstens ist ein Wechselrichter, wie Sie wissen, ein leistungselektronisches Gerät oder eine Schaltung, die Gleichstrom (DC) aus Quellen wie Batterien oder Brennstoffzellen in
Wechselstrom (AC) umwandelt. Die Eingangsspannung, die Ausgangsspannung, die Frequenz und die Gesamtbelastbarkeit hängen vom Design des jeweiligen Geräts oder Schaltkreises ab.
Der Wechselrichter erzeugt keinen Strom; Die Stromversorgung erfolgt über die Gleichstromquelle. Wechselrichter werden hauptsächlich in elektrischen Energieanwendungen eingesetzt,
bei denen hohe Ströme und Spannungen vorhanden sind.
******---------*******
Bei der Blindleistung handelt es sich um die Leistung, die benötigt wird, um den Stromfluss aufrechtzuerhalten, und um die Aufrechterhaltung der Spannungsniveaus zu unterstützen,
die für die Systemstabilität erforderlich sind. Genauso wie wir uns die Speicherung realer Energie in einer Batterie vorstellen können, ist es sinnvoll, sich Blindleistung als im
elektrischen Feld eines Kondensators oder im Magnetfeld einer Induktivität gespeichert vorzustellen. In dem Maße, in dem ein Stromkreis die Phase des Stroms zur Spannung verschiebt
(und dies gilt nur für Wechselstromkreise), spricht man von einer Blindkomponente seiner Impedanz. Daher speichert jeder Stromkreis, einschließlich eines Wechselrichters, der eine
induktive oder kapazitive Komponente in seiner Impedanz hat, vorübergehend Blindleistung in einer oder mehreren seiner Komponenten. Ein Wechselrichter verfügt häufig über einen
Transformator als Teil des Stromkreises, der an einen Wechselstromeingang angeschlossen wird, und stellt daher aus Sicht des Energieversorgers häufig eine induktive Last dar.
******---------*******
Es gibt noch einen weiteren Punkt, der berücksichtigt werden muss: Erneuerbare Energiequellen wie Solarenergie liefern nicht nur Strom, sondern können auch zur Erzeugung von
Blindleistung genutzt werden. Um Stromausfälle zu verhindern, benötigen erneuerbare Energiesysteme außerdem intelligente Wechselrichter, um den Energiefluss zu steuern und die
passive Leistung der Stromnetze zu verwalten. Blindleistung erledigt nicht die Arbeit von Elektrizität, die wir alle kennen, wie das Anzünden einer Lampe oder das Erhitzen von
Wasser. Stattdessen verbraucht das Wechselstromsystem Blindleistung, um den Stromfluss aufrechtzuerhalten. Mit zunehmender Strommenge, die auf einer Leitung fließt, steigt auch
die Menge an Blindleistung, die erforderlich ist, um den zusätzlichen Strom zu transportieren und die richtige Spannung aufrechtzuerhalten. Je weiter die Strecke zurückgelegt wird,
desto mehr Blindleistung wird verbraucht. Wenn die Blindleistung nicht ausreicht, sinkt die Spannung. Sinkt sie weiter, werden Schutzeinrichtungen betroffene Kraftwerke und
Leitungen abschalten, um sie vor Schäden zu schützen.
******---------*******
Was den Hauptpunkt Ihres Problems betrifft:
„Wie Wechselrichter Blindleistung erzeugen“ ... können wir Folgendes sagen:
Zur Blindleistungserzeugung können wir einen Wechselrichter nutzen. Um nur Blindleistung zu liefern, muss die über eine Reaktanz mit dem Hauptnetz verbundene Spannungsquelle eine
Spannung erzeugen, die in Phase mit der Netzspannung ist, jedoch eine höhere Amplitude aufweist. Um Blindleistung zu absorbieren, wird eine Spannung erzeugt, die immer noch in Phase
mit der Netzspannung ist, jedoch eine geringere Amplitude aufweist. Wechselrichter erzeugen Blindleistung mithilfe der Freilaufdioden an jedem Leistungsschalter. Die induktive Natur
der Last führt dazu, dass sie auch dann noch Strom ziehen möchte, wenn der Netzschalter ausgeschaltet wurde. Die Last wird als Induktionsmotor dargestellt, kann aber auch die
Primärseite eines Dreiphasentransformators oder einer anderen Wechselstromlast sein. Damit ein Wechselrichter Blindleistung verarbeiten kann, muss er in der Lage sein, rückwärts zu
laufen und die Blindenergie aufzunehmen. Diese Technologie wird als Vierquadrantenbetrieb bezeichnet. Der Wechselrichter kann Blindleistung basierend auf der gesamten Größe des
Wechselrichters bereitstellen, nicht nur auf der Ebene der Erzeugung. Wenn also bei bewölktem Himmel die Solarenergieerzeugung von 100 Prozent auf 10 Prozent sinkt, kann der
Wechselrichter die anderen 90 Prozent seiner verbleibenden Kapazität zur Bereitstellung von Blindleistungsunterstützung und zur Verbesserung der Stromqualität des öffentlichen
Stromnetzes nutzen. Die Blindleistungsregelung kann auf verschiedene Arten implementiert werden: Wechselrichter können entweder so eingestellt werden, dass sie ein bestimmtes
Verhältnis von Wirk- zu Blindleistung liefern, oder sie können so eingestellt werden, dass sie sich dynamisch an den Blindleistungsbedarf der Last anpassen, wenn sich dieser im
Laufe der Zeit ändert. Die Scheinleistungskapazität des Wechselrichters wird in Voltampere (VA) gemessen. Wenn Wechselrichter in der Vergangenheit mit einem Leistungsfaktor von
eins betrieben wurden, entsprach die Scheinleistung (VA) eines Wechselrichters im Wert der Wirkleistung (W). Daher wurden diese Begriffe oft synonym verwendet. In der Welt der
Blindleistungsregelung ist dies jedoch nicht mehr der Fall, sodass der Zusammenhang zwischen Wechselrichterkapazität und Wirkleistung klar verstanden werden muss. Glücklicherweise
handelt es sich um einen relativ einfachen Zusammenhang, wie die folgende Formel zeigt: Wechselrichterkapazität x Leistungsfaktor = Wirkleistung Traditionell wurden PV-Wechselrichter
bewusst so konzipiert, dass sie so viel Wirkleistung (kW) einspeisen, wie bei einem Leistungsfaktor von Eins in den Punkt der gemeinsamen Kopplung (PCC) verfügbar war. In jüngerer
Zeit haben Versorgungsunternehmen und unabhängige Energieversorger großes Interesse an der Fähigkeit des Dreiphasen-Wechselrichters gezeigt, auch Blindleistung Q (kVAR) vom und zum
Netz aufzunehmen und bereitzustellen. In über 95 % der Fälle läuft ein PV-Wechselrichter bei der Umwandlung von Gleichstrom-Solarstrom in Wechselstrom-Wirkstrom unter seinem
Nennausgangsstrom. Die ungenutzte Kapazität des Wechselrichters kann dann zur Erzeugung von Blindleistung genutzt werden. Der Ausgang eines intelligenten PV-Wechselrichters weist
sowohl Blind- als auch Wirkwechselströme auf, die sich geometrisch zur Scheinleistung addieren, die durch die Nennstromstärke des Wechselrichters begrenzt wird.

*/




/*

		--	SOC und ExportLimit beim SPH einstellen	--

Doc:

;	Parameter:
;	port=ttyUSB1							| \
;	addr=1									| - Gerät das ausgelesen werden soll
;	debug=1									| /

;	soc.1=65,soc.2=55 bis soc.12=65			| Tabelle mit den Vorgaben für den Mindest SOC des jeweiligen Monats

;	prog.nextdaythreshold=15				| Ab welcher Prognose für den Folgetag soll der Min.-SOC abgesenkt werden
;	prog.correction=8						| Wert der Absenkung
;	minsoc=20								| Diser Wert darf nie unterschritten werden
;	minsoc.overwrite=GBLI6532can|FELI12mb	| Für die Füllstandsanzeige der Batterien
;	minsoc.permittedhours=14|10|11			| zu welchen Stunden darf (außer zur Nachtzeit) der minsoc geändert werden

;	exportlimit.power=800							| max.800 Watt exportieren (exportlimit.maxpower ist erforderlich)
;	exportlimit.maxpower=4600							| SPH erzeugt max. 4600 Watt
;											| oder
;	exportlimit.percent=50							| setze ExportLimit auf 50 %

;	powerfactor=-0.9						| -0.8 (inductive) bis 1 bis 0.8 (cpacitive)

Holdingregister:

89	PFModel		Eg：207 is Int(16bits)
	Set PF function Model
	0: PF=1
	1: PF by set
	2: default PF line
	3: User PF line
	4: UnderExcited (Inda)
	Reactive Power
	5: OverExcited(Capa)
	Reactive Power
	6：Q(v)model
	7：Direct Control mode
	8. Static capacitive QV
	mode
	9. Static inductive QV
	mode

02	PF CMD Set Holding memory	register3,4,5,99 CMD state will be memory or not(1/0),
	if not, these settings are the initial value.W0or10Means these settings will be
	acting or not when next power on

03	Active Rate
	W0-100 or % 255255255: power is not be limited
	P Inverter Max output
	active power percent

04	Reactive Rate
	Inverter max output reactive power percent -100-100 % or 255

05 Power factor Inverter output power
	factor’s 10000 timesW0-20000, 0-10000 is underexcited, other is overexcited


*/


class special_SPHsettings extends sensor
{
	protected $SOCTab			=array(	1=>40, 2=>40, 3=>40, 4=>40, 5=>40, 6=>40, 7=>40, 8=>40, 9=>40, 10=>40, 11=>40, 12=>40	);

	protected $MinSOCAbs		=	20;

	protected $MinSOCRegister	=array(
									0=>608,
									1=>1091,
									2=>1071,
									);

	protected $SOCRegister		=	1014;

	protected $ACChargeRegister	=	1128;

	protected $modData			=array(
									"minSocSet"=>false,
									"minSocShouldSet"=>false,
									"ProgNeed"=>15,
									"ProgRed"=>8,
									"ProgKorr"=>0,
									);

	protected $ExportEnRegister	=	122;
	protected $ExportRegister	=	123;

	protected $PFRegister		=	5;
	protected $PFModelRegister	=	89;
	protected $PFModel			=array(
									0=>"PF=1",
									1=>"PF by set",
									2=>"default PF line",
									3=>"User PF line",
									4=>"UnderExcited (Inda)",
									5=>"OverExcited(Capa)",
									6=>"Q(v)model",
									7=>"Direct Control mode",
									8=>"Static capacitive QV",
									9=>"Static inductive QV",
									);

	protected $Monat			=	false;
	protected $Stunde			=	false;

	protected $protomsgs		=array(
									"wr"=>"SPH write register",
									"0"=>"SPH standby",
									"1"=>"normal operation"
									);


	function __construct($_port,$_addr,$_res)
	{

		$this->Monat=date("n");
		$this->Stunde=date("G");

		parent::__construct($_port,$_addr,$_res);
	}

	function __destruct()
	{
		parent::__destruct();
	}

	public static function pf_reg2val($_regval)
	{
		if( $_regval == 20000 )
			return 1;
		return ($_regval - 10000) / 10000;
	}

	public static function pf_val2reg($_val)
	{
		if( $_val == 1 )
			return 20000;
		return ($_val * 10000) + 10000;
	}

	protected function checkDevDate()
	{

		// WR Datum und Zeit
		$mbDatum=$this->mbReadHoldingReg(45,7);

		if(! strlen($mbDatum["err"]) )
		{
			// [0] => jahr 2023  [1] => monat 8  [2] => tag 9  [3] => stunde 11  [4] => minute 59  [5] => sekunde 16  [6] => wochentag (Montag=1) 3
			// Stimmt das WR-Datum mit unserem überein?
			$wrdatum = mf::format_datemysql($mbDatum["regs"][0], $mbDatum["regs"][1], $mbDatum["regs"][2]);
		}

		if( $wrdatum != $this->result["D"] )
		{
			$this->logger("WR-Datum entspricht nicht dem Query-Datum");
			return false;
			return sensor::RET_NODATA;
		}

		return true;
	}

	protected function checkExportLimit()
	{

		//	Exportlimitation

		//	Reg 122: 0/1	disabled / enabled
		//	Reg 123: 0-1000 (0,1%)

		// ist in der Config exportlimit.enabled, ist $this->opt["exportlimit.enabled"] NULL, ansonsten TRUE oder FALSE

		$mbExport = $this->mbReadHoldingReg($this->ExportEnRegister, 2);

		$this->modData["ExpOn"] = $mbExport["regs"][0];

		$this->modData["ExpLimIst"] = $mbExport["regs"][1];

		$this->modData["ExpLimSoll"] =
		$this->modData["ExpLimSet"] = false;

		// Option zum Exportlimit nicht gesetzt!
		if(  is_null($this->opt["exportlimit.enabled"]) )
		{

			$this->modData["ExpLimSet"] = false;

			return;

		}

		if( isTrue($this->opt["exportlimit.enabled"]) )
		{

			// Export limitieren


			// Config: Prozentwert hat Vorrang vor dem Leistungswert

			if( $this->opt["exportlimit.percent"] )
			{

				$this->modData["ExpLimSoll"]=$this->opt["exportlimit.percent"];

			}else{

				// Prozentwert aus .power und .maxpower berechnen

				if( $this->opt["exportlimit.power"] and $this->opt["exportlimit.maxpower"] )
				{

					$this->modData["ExpLimSoll"] = intval(1000 * $this->opt["exportlimit.power"] / $this->opt["exportlimit.maxpower"] );

				}else{

					$this->modData["ExpLimSoll"] = false;

				}
			}

			if( $this->modData["ExpLimSoll"] and $this->modData["ExpLimIst"] !== $this->modData["ExpLimSoll"] )
			{

				$this->modData["ExpLimSet"] = $this->modData["ExpLimSoll"];

			}else{

				$this->modData["ExpLimSet"] = false;
			}

		}else{

			// Kein Exportlimit!
			// prüfen, ob das Register 122 auf 0 steht

			if( $this->modData["ExpOn"] != 0 )
			{

				$this->modData["ExpLimSet"] = true;

			}
		}
	}

	protected function checkPF()
	{

		$mbPF = $this->mbReadHoldingReg($this->PFModelRegister, 1);

		$this->modData["PFModel"] = $this->PFModel[ $mbPF["regs"][0] ];

		$mbPF = $this->mbReadHoldingReg($this->PFRegister, 1);

		$this->modData["PFist"] = $this->pf_reg2val($mbPF["regs"][0]);

		$this->modData["PFSet"] = intval($this->opt["powerfactor"] * 100 ) / 100;

		if( $this->modData["PFSet"] == $this->modData["PFist"] )
		{

			$this->modData["PFSet"] = false;

		}

		if(
			$this->modData["PFSet"]
			and
			(
				(
					$this->modData["PFSet"] < 0
					and
					(
						$this->modData["PFSet"] > -0.8
						or
						$this->modData["PFSet"] < -0.9999
					)
				)
				or
				(
					$this->modData["PFSet"] > 0
					and
					(
						$this->modData["PFSet"] < 0.8
						or
						$this->modData["PFSet"] > 1
					)
				)
			)
			)
		{

			$this->logger("Powerfactor in Config (".$this->modData["PFSet"].") ist ungültig");

			$this->modData["PFSet"] = false;

		}
	}

	protected function checkMinSOC()
	{
		//param=prog.nextdaythreshold=15,prog.correction=8,minsoc=20,minsoc.overwrite=GBLI6532can;FELI12mb,minsoc.permittedhours=14;10


			/*
				data=mbgrowattSPH/ttyUSB1/1

				3144. Today discharge energy Priority: 0 LoadFirst, 1 BatteryFirst, 2 GridFirst

				1071.	GridFirstStopS Stop Discharge soc when Grid First	(0-100) %	Stop Discharge soc when Grid First
				1091.	wBatFirst stop Stop Charge soc when Bat First		(0-100) %	Stop Charge soc when Bat First
				3037.	GridFirstStopS Stop Discharge soc when Grid First	(1-100) %
				3048.	wBatFirst stop Stop Charge soc when Bat First		(1-100) %
				608.	bLoadFirstStopSoc When LoadFirst R/W				(13-100)%
			*/

		$mb = $this->mbReadHoldingReg(3144, 1);

		$this->modData["minSocWorkMode"] = $mb["regs"][0];

		if( isset($this->MinSOCRegister[$this->modData["minSocWorkMode"]]) )
		{

			$mb = $this->mbReadHoldingReg($this->MinSOCRegister[$this->modData["minSocWorkMode"]], 1);

		}else{

			$this->logger(__CLASS__." Unbekannter WorkMode");

			$this->modData["minSocSet"] = false;

			return;

		}

		$this->modData["minSocIst"]  = $mb["regs"][0];

		$this->modData["minSocSoll"] = $this->SOCTab[$this->Monat] - intval($this->modData["ProgKorr"]);

		$mb = $this->mbReadInputReg($this->SOCRegister, 1);

		$this->modData["soc"] = $mb["regs"][0];

		$mb = $this->mbReadInputReg($this->ACChargeRegister, 2);

		$this->modData["accharge"] = intval($this->ulng32($mb["data"], $mb["reg2idx"]->idx($this->ACChargeRegister), 10));		//R 1128+1129

		$this->modData["minSocSet"] = $this->modData["minSocIst"];


		if( $this->modData["soc"] > $this->modData["minSocSoll"] )
		{
			$this->modData["minSocSet"] = $this->modData["minSocSoll"];
		}

		$this->logger("Check 1: ".$this->modData["minSocSet"]);


		//SOC verringern
		if($this->modData["minSocSoll"] < $this->modData["minSocIst"])
		{
			$this->modData["minSocSet"] = $this->modData["minSocSoll"];
		}

		$this->logger("Check 2: ".$this->modData["minSocSet"]);


		//Wenn AC-Ladung, den SOC nicht erhöhen
		if($this->modData["minSocSet"] > $this->modData["minSocIst"] and $this->modData["accharge"] > 0)
		{
			$this->modData["minSocSet"] = $this->modData["minSocIst"];
		}

		$this->logger("Check 3: ".$this->modData["minSocSet"]);


		//nicht unter den Mindestwert
		if($this->modData["minSocSet"] < $this->MinSOCAbs)
		{
			$this->modData["minSocSet"] = $this->MinSOCAbs;
		}

		//Wert ist identisch, nichts tun
		$this->logger("Check 4: ".$this->modData["minSocSet"]);


		if( $this->modData["minSocSet"] == $this->modData["minSocIst"] )
		{
			$this->modData["minSocSet"] = false;
		}

		$this->logger("Ergebnis ".$this->modData["minSocSet"]);


		if( $this->modData["minSocSet"] and ( ($this->modData["minSocWorkMode"] == 0 and $this->modData["minSocSet"] < 13) or $this->modData["minSocSet"] > 100 ) )
		{
			$this->modData["minSocSet"] = false;
		}

		if( $this->modData["minSocSet"] )
		{

			if( in_array($this->Stunde, $this->modData["minSocSetHours"]) )
			{

				$this->logger("Check 5: final ".$this->modData["minSocSet"]);

			}else{

				$this->modData["minSocSet"] = false;

				$this->logger("Check 5: ".$this->Stunde." (".implode(";", $this->modData["minSocSetHours"]).") MinSoc nur zur erlaubten Zeit ändern!");

			}
		}

		return;
	}

	public function getconf()
	{

		$this->getParam();

		for( $i = 1; $i < 13; $i++)
		{
			if(
				isset($this->opt["soc.".$i])
				and
				intval($this->opt["soc.".$i])
				)
			{

				$this->SOCTab[$i]=intval($this->opt["soc.".$i]);
			}
		}

		$dbg = $this->isEnabled("debug");

		if( isTrue($dbg) )
		{

			$this->debug = "Debg: SPHsettings";

		}

		$idx = "minsoc.overwrite";
		if( isset($this->opt[$idx]) )
		{

			$devs = explode(";",$this->opt["$idx"]);

			$this->modData["minSocOwrt"] = mf::cleanArray($devs,"s");

		}


		$idx = "prog.nextdaythreshold";
		if(! isset($this->opt[$idx]) )
			$this->opt[$idx] = $this->modData["ProgNeed"];

		if( isset($this->smart->prognose["date"])
			and isset($this->smart->prognose["morgen"])
			and $this->smart->prognose["date"] == date("Ymd")
			and floatval($this->smart->prognose["morgen"]) > $this->opt[$idx] )
		{

			$this->modData["ProgKorr"] = $this->opt["prog.correction"];

		}


		if(! isset($this->opt["prog.correction"]) )
			$this->opt["prog.correction"] = $this->modData["ProgRed"];

		if( isset($this->opt["minsoc.permittedhours"]) )
		{

			$this->modData["minSocSetHours"] = array();

			$a = explode(";", $this->opt["minsoc.permittedhours"]);

			$this->modData["minSocSetHours"] = mf::cleanArray($a, "i");

		}else{

			$this->modData["minSocSetHours"] = array();

		}


		$idx = "exportlimit.percent";
		if( isset($this->opt[$idx]) )
		{

			$this->opt[$idx] = intval( (float) $this->opt[$idx] * 10);

			if( $this->opt[$idx] < 0 or $this->opt[$idx] > 1000 )
			{

				$this->opt[$idx] = false;

			}

		}else{

			$this->opt[$idx] = false;

		}

		$idx="exportlimit.maxpower";
		if( isset($this->opt[$idx]) )
		{

			$this->opt[$idx] = intval($this->opt[$idx]);

			if( intval($this->opt[$idx]) < 0 )
				$this->opt[$idx] = false;

		}else{

			$this->opt[$idx] = false;

		}

		$idx = "exportlimit.power";
		if( isset($this->opt[$idx]) )
		{

			$this->opt[$idx] = intval($this->opt[$idx]);

			if( $this->opt[$idx] < 0 or $this->opt[$idx] > $this->opt[$idx] )
			{

				$this->opt[$idx] = false;
			}

		}else{

			$this->opt[$idx] = false;

		}

		$idx = "exportlimit.enabled";

		$this->opt[$idx] = $this->isEnabled($idx);

	}

	protected function SPHstandby($_on)
	{
		if( $_on )
		{

			// normal >> standby
			$res=$this->mbWriteHoldingReg(0,0);

			$GLOBALS["smart"]->proto($this->protomsgs["0"], $this->ID,"reg 0 -> 0");

		}else{

			mf::Sleep(1);

			// standby >> normal
			$res = $this->mbWriteHoldingReg(0,1);

			$GLOBALS["smart"]->proto($this->protomsgs["0"], $this->ID,"reg 0 -> 1");

		}
	}

	public function setExportLimit($_en,$_val)
	{

		// ExportLimit enablen
		if( $_en === 1 )
		{

			$res = $this->mbWriteHoldingReg($this->ExportEnRegister, $_en);

			$GLOBALS["smart"]->proto($this->protomsgs["wr"], $this->ID, "reg ".$this->ExportEnRegister." -> ".$_en);

		}

		if( $_en === 0 )
		{
			$res = $this->mbWriteHoldingReg($this->ExportEnRegister, $_en);

			$GLOBALS["smart"]->proto($this->protomsgs["wr"], $this->ID, "reg ".$this->ExportEnRegister." -> ".$_en);
		}


		if( isNotFalse($_val) )
		{
			// limit schreiben
			$res = $this->mbWriteHoldingReg($this->ExportRegister, $_val);

			$GLOBALS["smart"]->proto($this->protomsgs["wr"], $this->ID, "reg ".$this->ExportRegister." -> ".$_val);

			$this->logger(__CLASS__." Set ExportLimit: Register 122/123 ".$_en."/".$_val);
		}

	}

	protected function setSOC($_WorkMode,$_soc)
	{
		$soc = intval($_soc);

		if(isset($this->MinSOCRegister[$_WorkMode]))
		{

			$res = $this->mbWriteHoldingReg($this->MinSOCRegister[$_WorkMode], array(0,$soc));

			$GLOBALS["smart"]->proto($this->protomsgs["wr"], $this->ID, "reg ".$this->MinSOCRegister[$_WorkMode]." -> ".$_soc);

			$this->logger(__CLASS__." Set SOC: Register ".$this->MinSOCRegister[$_WorkMode].", SOC ".$soc);

		}
	}

	public function setPF($_pf)
	{

		$pf=$this->pf_val2reg($_pf);

		$res=$this->mbWriteHoldingReg($this->PFRegister, $pf);

		$GLOBALS["smart"]->proto($this->protomsgs["wr"], $this->ID, "reg ".$this->PFRegister." -> ".$pf);

		$this->logger(__CLASS__." Set PF: Register ".$this->PFRegister.", ".$pf." (".$this->pf_reg2val($pf).")");

	}

	public function query()
	{

		$isStandby = false;

		$this->getconf();

		$this->port = $this->opt["port"];
		$this->addr = $this->opt["addr"];

		if( $this->mbOpen() )
		{

			if(! $this->checkDevDate() )
				return sensor::RET_NODATA;


			$this->checkMinSOC();

			$this->checkPF();

			$this->checkExportLimit();

			$this->logger("SOC=".$this->modData["soc"].", minSOC-Soll=".$this->modData["minSocSoll"].", minSOC-Ist=".$this->modData["minSocIst"].", ACCharge=".$this->modData["accharge"].
				", Prognose: (".( isset($this->smart->prognose["date"]) ? $this->smart->prognose["date"] : "???" ).") ".( isset($this->smart->prognose["morgen"]) ? $this->smart->prognose["morgen"] : "???" ).", ProgKorr=".$this->modData["ProgKorr"]);
			$this->logger("\t".($this->modData["minSocSet"] ? "SetSOC=".$this->modData["minSocSet"] : "MinSoc nicht geändert"));

			$this->logger("LimitAktiviert=".$this->modData["ExpOn"].", LimitIst=".($this->modData["ExpLimIst"]/10)."%, LimitSoll=".($this->modData["ExpLimSoll"]/10)."%");
			$this->logger("\t".($this->modData["ExpLimSet"] ? "SetExportLimit=".$this->modData["ExpLimSet"] : "ExportLimit nicht geändert"));

			$this->logger("PFModel=".$this->modData["PFModel"].", PowerfactorIst=".$this->modData["PFist"].", PowerfactorSoll=".$this->opt["powerfactor"]);
			$this->logger("\t".($this->modData["PFSet"] ? "SetPF=".$this->modData["PFSet"] : "PF nicht geändert"));


			if( $this->modData["ExpLimSet"] or $this->modData["PFSet"] )
			{
				$this->SPHstandby(true);
				$isStandby = true;
			}


			if( isTrue($this->modData["ExpLimSet"]) )
			{
				// Nur Register 122 auf 0 setzen = Disable ExportLimit

				$this->setExportLimit(($this->modData["ExpOn"] == 0 ? false : 0), false);

			}else{

				if( $this->modData["ExpLimSet"] )
				{

					// Enable ExportLimit

					$this->setExportLimit(($this->modData["ExpOn"] == 1 ? false : 1), $this->modData["ExpLimSet"]);

				}

			}

			if( $this->modData["minSocSet"] )
			{

				$this->logger("set minSOC ".$this->modData["minSocSet"]);

				$this->setSOC($this->modData["minSocWorkMode"], $this->modData["minSocSet"]);

			}else{

				$this->modData["minSocSet"] = $this->modData["minSocIst"];

			}

			if( $this->modData["PFSet"] )
			{

				$this->setPF($this->modData["PFSet"]);

			}

			if( $isStandby )
			{
				$this->SPHstandby(false);
			}

			if( is_array($this->modData["minSocOwrt"]) and count($this->modData["minSocOwrt"]) )
			{

				foreach($this->modData["minSocOwrt"] as $dev)
				{
					configOverwrite("conf/batteries/".$dev."/minsoc",$this->modData["minSocSet"]);
				}
			}

		}
		return sensor::RET_NODATA;
	}

}

?>