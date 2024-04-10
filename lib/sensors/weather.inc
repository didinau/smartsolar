<?php

abstract class weather_base
{
	public $options = false;

	public $forecast = false;

	public $admdata = false;

	public $forecast_tpl = array( "requestTime" => false, "rain%" => false, "clouds%" => false, "sunshineMinutes" => false, "harvestKWH" => false, );



	public function delOldForecasts()
	{
		if( is_array($this->forecast) and count($this->forecast) )
		{

			$ref = date("Ymd");

			foreach($this->forecast as $date => $data)
			{

				if( $date < $ref ) unset($this->forecast[$date]);
			}
		}
	}

	abstract function performRequest(&$_master);

}




class weather_solarprognose extends weather_base
{

	public $getvars = array( 'access-token', 'item', 'type', 'id', 'START_DAY', 'END_DAY',);

	public function __construct()
	{

		// Hack für alte Funktion
		if(! isset($GLOBALS["prognose"]) )
			$GLOBALS["prognose"] = array( "date" => date("Ymd"), "ts" => time(), "status" => 0, "heute" => 0, "morgen" => 0, );

	}


	public function performRequest(&$_master)
	{

		if( (! isset($this->admdata["nextRequest"]) ) or ( isset($this->admdata["nextRequest"]) and time() > $this->admdata["nextRequest"] ) )
		{

			$this->admdata["status"] = false;

			$GET=array();

			foreach( $this->getvars as $idx )
			{
				if( isset($this->options["option"][$idx]) )
				{

					$GET[$idx] = $this->options["option"][$idx];

				}else{

					new LogMsg(0, __class__."::".__function__, "notwendige HTTP-Get-Variable '".$idx."' nicht gesetzt");

					return false;

				}
			}

			if( $data = $_master->master_request($this->options["url"], false, $GET, false, false, false, false) )
			{

				new LogMsg(2, __class__."::".__function__, "updateRequest ok\n".print_r($data, true));

				$this->admdata["Response"] = ""; //$data;

				$this->admdata["nextRequest"] = time() + $this->options["interval"] * 3600;

				if( isset($data["preferredNextApiRequestAt"]["epochTimeUtc"]) )
				{

					$ts = (int) $data["preferredNextApiRequestAt"]["epochTimeUtc"];

					if( $ts > 0 and $ts < $this->admdata["nextRequest"] )
					{

						$this->admdata["nextRequest"] = $ts + 3600;
						$this->admdata["nextRequestTime"] = date("Ymd His", $this->admdata["nextRequest"]);

					}
				}


				$this->admdata["lastRequest"] = date("Ymd His");

				if( isset($data["status"]) and $data["status"] == 0 )
				{
					$controldates= array( date("Ymd"),);

					if( isset($this->options["forecast_days"]) )
					{
						$end = intval($this->options["forecast_days"]) + 1;

						for( $i = 1; $i < $end; $i++ )
							$controldates[] = date("Ymd",strtotime("+".$i." day"));

					}

					if( isset($data["data"]) and is_array($data["data"]) and count($data["data"]) )
						foreach( $data["data"] as $idx => $dat )
					{

						if( in_array($dat[0], $controldates) )
						{

							$this->forecast[$dat[0]] = $this->forecast_tpl;

							$this->forecast[$dat[0]]["harvestKWH"] = floatval($dat[1]);

							$this->forecast[$dat[0]]["requestTime"] = $this->admdata["lastRequest"];

						}
					}

					$this->admdata["status"] = true;
				}
			}else{

				new LogMsg(0, __class__."::".__function__, "updateRequest failed");

			}

		}

		$this->delOldForecasts();


		// Hack für alte Funktion

		$h = date("Ymd");
		$m = date("Ymd",strtotime("+1 day"));

		if( isset($this->forecast[$h]["harvestKWH"]) and isset($this->forecast[$m]["harvestKWH"]) )
		{

			$GLOBALS["prognose"]["date"] = $h;
			$GLOBALS["prognose"]["ts"] = time();

			$GLOBALS["prognose"]["heute"] = $this->forecast[$h]["harvestKWH"];
			$GLOBALS["prognose"]["morgen"] = $this->forecast[$m]["harvestKWH"];

		}

	}
}

class weather_meteoblue extends weather_base
{

	public $getvars = array( 'apikey', 'lat', 'lon', 'asl', 'format', 'forecast_days',);


	public function performRequest(&$_master)
	{

		if( (! isset($this->admdata["nextRequest"]) ) or ( isset($this->admdata["nextRequest"]) and time() > $this->admdata["nextRequest"] ) )
		{

			$this->admdata["status"] = false;

			$GET=array();

			foreach( $this->getvars as $idx )
			{
				if( isset($this->options["option"][$idx]) )
				{

					$GET[$idx] = $this->options["option"][$idx];

				}else{

					new LogMsg(0, __class__."::".__function__, "notwendige HTTP-Get-Variable '".$idx."' nicht gesetzt");

					return false;

				}
			}

			if( $data = $_master->master_request($this->options["url"], false, $GET, false, false, false, false) )
			{

				new LogMsg(2, __class__."::".__function__, "updateRequest ok\n".print_r($data, true));


				$this->admdata["Response"] = $data; //""//

				$this->admdata["nextRequest"] = time() + $this->options["interval"] * 3600;

				$this->admdata["nextRequestTime"] = date("Ymd His", $this->admdata["nextRequest"]);

				$this->admdata["lastRequest"] = date("Ymd").date(" His");

				if( isset($data["data_day"]) and is_array($data["data_day"]) and count($data["data_day"]) and
					isset($data["data_day"]["time"]) and is_array($data["data_day"]["time"]) and count($data["data_day"]["time"]) )
				{

					$controldates= array( date("Ymd"),);

					if( isset($this->options["forecast_days"]) )
					{
						$end = intval($this->options["forecast_days"]) + 1;

						for( $i = 1; $i < $end; $i++ )
							$controldates[] = date("Ymd",strtotime("+".$i." day"));

					}


					$misc_dates = array();


					foreach($data["data_day"]["time"] as $idx => $day)
					{
						$daydate = mf::onlyNumbers($day);

						if( in_array($daydate, $controldates) )
						{

							$misc_dates[$idx] = $daydate;

							$this->forecast[$daydate] = $this->forecast_tpl;

							$this->forecast[$daydate]["requestTime"] = $this->admdata["lastRequest"];

							if( isset($data["data_day"]["totalcloudcover_mean"][$idx]) )
								$this->forecast[$daydate]["clouds%"] = intval($data["data_day"]["totalcloudcover_mean"][$idx]);

							if( isset($data["data_day"]["sunshine_time"][$idx]) )
								$this->forecast[$daydate]["sunshineMinutes"] = intval($data["data_day"]["sunshine_time"][$idx]);
						}
					}


					$misc = array();

					foreach( $data["data_day"] as $categorie => $data )
					{

						if( $categorie != "time" )
						{

							foreach( $misc_dates as $didx => $daydate )
							{
								$this->forecast[$daydate]["misc"][$categorie] = $data[$didx];
							}

						}

					}

					$this->admdata["status"] = true;

				}
			}else{

				new LogMsg(0, __class__."::".__function__, "updateRequest failed");

			}
		}

		$this->delOldForecasts();

	}
}


class weatherforecast extends sensor
{
	private const ID = "weatherforecast";

	public $devicename = false;

	public $models = [];

	private $controlData = false;

	function __construct($_port, $_addr, $_res)
	{

		parent::__construct($_port, $_addr, $_res);

	}

	function __sleep()
	{
		return array();
	}

	public function refreshModels($_models)
	{

		if(! isset($this->controlData->STATIC[self::ID]) )
		{
			$this->controlData->STATIC[self::ID] = array();
		}

		foreach( $_models as $model => $data)
		{

			$class = "weather_".$model;

			if( class_exists($class) )
			{

				$options = array( "option" => array() );

				foreach( $data as $optidx => $optdata )
				{

					if( $optidx == "option" )
					{

						if(! is_array($optdata) )
							$optdata = array( $optdata );

						foreach( $optdata as $i )
						{
							$o = mf::split_on_first($i, "=", true);

							if( isset($o[1]) )
								$options["option"][$o[0]] = $o[1];
						}
					}else{

						$options[$optidx] = $optdata;

					}
				}

				$this->models[$model] = new $class();

				$this->models[$model]->options = $options;

				if(! isset($this->controlData->STATIC["modules"][$this->devicename][$model]) )
					$this->controlData->STATIC["modules"][$this->devicename][$model] = array();

				$this->models[$model]->admdata = &$this->controlData->STATIC["modules"][$this->devicename][$model];


				if(! isset($this->controlData->STATIC[self::ID][$model]) )
					$this->controlData->STATIC[self::ID][$model] = array();

				$this->models[$model]->forecast = &$this->controlData->STATIC[self::ID][$model];

			}
		}

	}

	public function query()
	{

		$this->devicename = $this->result["device"];

		$this->controlData = &$this->smart;

		$this->refreshModels($this->controlData->CONFIG["weather"]);

		if(! isset($this->controlData->STATIC[self::ID]) ) $this->controlData->STATIC[self::ID] = array();

		try
		{

			if( is_array($this->models) and count($this->models) )
			{

				foreach($this->models as $model => $class)
				{

					$class->performRequest($this);

				}

				$this->save();

			}

		}catch(throwable $e){

			new ErrorAndMore($e, 0, __class__."::".__function__);

		}

		return sensor::RET_NODATA;

	}

	public function save()
	{

		if(! isset($this->controlData->STATIC[self::ID]) )
			return;

		$iM=new dpdoInsertMgr("data", DPDO::REPLACE);

		$iM->addColumn("data_type", self::ID);

		$iM->addColumn("data_ts", time());

		$iM->addColumn("data_raw", mf::json_enc($this->controlData->STATIC[self::ID]));

		$iM->commit();

		if( isset($this->controlData->DB) and $this->controlData->DB )
		{

			$check = $iM->exec($this->controlData->DB);

			if(! $check )
			{
				print_r($iM);

				new LogMsg(0, __class__."::".__function__, "error replace ".self::ID." in db");

			}
		}

		if( isset($this->controlData->DBFB) and $this->controlData->DBFB )
		{

			$check = $iM->exec($this->controlData->DBFB);

			if(! $check )
			{
				print_r($iM);

				new LogMsg(0, __class__."::".__function__, "error replace ".self::ID." in fallback-db");

			}
		}

		return;

	}



	public function master_request(
		$_url,
		$_postvars,
		$_getvars = false,
		$_cookie_file = false,
		$_referer = false,
		$_useragent = false,
		$_post = true,
		$_curlopt = false,
		$_cookies = false,
		$_auth = false)
	{

		$response = mf::curl_request($_url, $_postvars, $_getvars, $_cookie_file, $_referer, $_useragent, $_post, $_curlopt, $_cookies, $_auth);

		$data = false;

		if( isset($response["status"]) and $response["status"] == 200 and isset($response["data"]) )
		{

				$data = mf::json_dec($response["data"]);

		}

		return $data;

	}

}

/*
Was ist der Unterschied zwischen DNI, DIF und GHI?

Der Begriff Sonneneinstrahlung bezeichnet die Leistung der Sonne, die pro Flächeneinheit eine Oberfläche erreicht.
Direktstrahlung ist der Teil der Sonnenstrahlung, der direkt eine Oberfläche erreicht; diffuse Strahlung ist der Teil,
der von der Atmosphäre gestreut wird; Die Globalstrahlung ist die Summe der diffusen und direkten Komponenten, die
dieselbe Oberfläche erreichen.

Andererseits stellt der Begriff Sonneneinstrahlung die Summe der Energie dar, die pro Flächeneinheit über einen
bestimmten Zeitraum von der Sonne aufgenommen wird. Im Global Solar Atlas stellen wir drei Größenordnungen im
Zusammenhang mit der Sonneneinstrahlung bereit:

GHI, Globale horizontale Strahlung
DNI, direkte Normalbestrahlung
DIF, diffuse horizontale Strahlung

GHI und DIF beziehen sich auf eine Oberfläche horizontal zum Boden, während  sich DNI auf eine Oberfläche senkrecht
zur Sonne bezieht. Höhere Werte des DIF/GHI-Verhältnisses bedeuten ein häufigeres Auftreten von Wolken, eine höhere
Luftverschmutzung oder einen höheren Wasserdampfgehalt.


                                    [units] => Array
                                        (
                                            [time] => YYYY-MM-DD hh:mm
                                            [predictability] => percent
                                            [precipitation_probability] => percent
                                            [cloudcover] => percent
                                            [pressure] => hPa
                                            [relativehumidity] => percent
                                            [visibility] => m
                                            [radiation] => Wm-2
                                            [ghi_total] => Whm-2
                                            [gni_total] => Whm-2
                                            [dni_total] => Whm-2
                                            [dif_total] => Whm-2
                                            [extraterrestrialradiation_total] => Whm-2
                                            [co] => ug/m3
                                            [temperature] => C
                                            [winddirection] => degree
                                            [precipitation] => mm
                                            [windspeed] => ms-1
                                        )

                                            [totalcloudcover_mean] => Array
                                                (
                                                    [0] => 56
                                                    [1] => 54
                                                    [2] => 73
                                                )

                                            [sunshine_time] => Array
                                                (
                                                    [0] => 285
                                                    [1] => 299
                                                    [2] => 115
                                                )

					Die Sonneneinstrahlung, die auf die Erde trifft, wird auch als Globalstrahlung bezeichnet. Das ist
					die solare Strahlung, die auf eine definierte horizontale Fläche auftrifft.
					Angegeben wird sie in Kilowattstunden pro Jahr pro Quadratmeter.
					Die Globalstrahlung setzt sich aus direkter und diffuser sowie reflektierter Strahlung zusammen.

					Die Solarstrahlung besteht aus diffuser und direkter Strahlung.
					Bei der diffusen Strahlung wird das Sonnenlicht aufgrund von Nebel, Dunst oder Wolken gestreut und
					trifft es aus verschiedenen Richtungen auf die Erdoberfläche auf.


                                            [ghi_total] => Array
                                                (
                                                    [0] => 2502
                                                    [1] => 3076
                                                    [2] => 2368
                                                )

                                            [gni_total] => Array
                                                (
                                                    [0] => 6030
                                                    [1] => 6783
                                                    [2] => 5878
                                                )

                                            [dni_total] => Array
                                                (
                                                    [0] => 4192
                                                    [1] => 4297
                                                    [2] => 4164
                                                )

                                            [dif_total] => Array
                                                (
                                                    [0] => 969
                                                    [1] => 1471
                                                    [2] => 814
                                                )


					Außerirdische Strahlung (Ra)
					Die auf eine horizontale Oberfläche oberhalb der Erdatmosphäre einfallende Sonnenstrahlung wird als
					außerirdische Strahlung bezeichnet und ist eine Funktion von Breitengrad, Jahreszeit und Tageszeit
					und kann mit den folgenden Parametern berechnet werden (Vaziri et al., 2008).

                                            [extraterrestrialradiation_total] => Array
                                                (
                                                    [0] => 4683
                                                    [1] => 4763
                                                    [2] => 4844
                                                )


*/
?>