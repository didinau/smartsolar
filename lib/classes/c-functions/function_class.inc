<?php

/*
 *
 *
 * Copyright 2024 Dieter Naujoks <dnaujoks@naujoks.homeip.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


class mf
{

	public static function getUserIP()
	{
		$client  = @$_SERVER['HTTP_CLIENT_IP'];
		$forward = @$_SERVER['HTTP_X_FORWARDED_FOR'];
		$remote  = $_SERVER['REMOTE_ADDR'];

		if(filter_var($client, FILTER_VALIDATE_IP))
		{
			$ip = $client;
		}
		elseif(filter_var($forward, FILTER_VALIDATE_IP))
		{
			$ip = $forward;
		}else{
			$ip = $remote;
		}

		return $ip;
	}

	public static function curl_request($_url, $_postvars, $_getvars = false,
			$_cookie_file = false, $_referer = false, $_useragent = false, $_post = true, $_curlopt = false, $_cookies = false, $_auth = false)
	{
		$debug=false;
		$postvars=is_array($_postvars)?http_build_query($_postvars):$_postvars;
		$getvars=is_array($_getvars)?http_build_query($_getvars):$_getvars;
		$headers = array(
			"Content-type: application/x-www-form-urlencoded;charset=\"utf-8\"",
			"Cache-Control: no-cache",
			"Pragma: no-cache",
			"Content-length: ".strlen($postvars));

		if($ch = curl_init($_url.(strlen($getvars)?"?".$getvars:"")))
		{
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
			curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

			if($_post and is_array($_postvars))
			{
				curl_setopt($ch, CURLOPT_POST, $_post);
				curl_setopt($ch, CURLOPT_POSTFIELDS, $postvars);
			}

			curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
			curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
			curl_setopt($ch, CURLOPT_SSLVERSION, 1);

			curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);

			if($_cookie_file)
			{
				curl_setopt($ch, CURLOPT_COOKIEJAR, $_cookie_file);
				curl_setopt($ch, CURLOPT_COOKIEFILE, $_cookie_file);
			}

			if(is_array($_cookies))
			{
				$server=parse_url($_url);
				$server=$server["host"];
				$cook="";
				foreach($_cookies as $name=>$value)
				{
					if($debug) print "SetCookie:\n".$server."\tFALSE\t/\tFALSE\t0\t".$name."\t".urlencode($value)."\n";
					curl_setopt($ch, CURLOPT_COOKIELIST, $server."\tFALSE\t/\tFALSE\t0\t".$name."\t".urlencode($value));
				}
			}

			if($_referer)
			{
				curl_setopt($ch, CURLOPT_REFERER, $_referer);
			}

			if($_useragent)
			{
				curl_setopt($ch, CURLOPT_USERAGENT, $_useragent);
			}

			if(is_array($_auth))
			{
				if(isset($_auth["user"]) and isset($_auth["pw"]))
				{
					curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
					curl_setopt($ch, CURLOPT_USERPWD, $_auth["user"].":".$_auth["pw"]);
				}
			}

			$data=curl_exec($ch);
			if($debug)
			{
				$cooklist=curl_getinfo($ch, CURLINFO_COOKIELIST);
				print_r($cooklist);
			}
			$stat=curl_getinfo($ch, CURLINFO_HTTP_CODE);
			curl_close($ch);

			if($_cookie_file)
			{
				chmod($_cookie_file, 0666);
			}

		return array(
			"data"=>$data,
			"status"=>(int)$stat
			);
		}else{
			return false;
		}
	}

	public static function get_vartype($_v)
	{
		if(!is_array($_v))
			$v=strtolower(trim($_v));
		else
			$v="";
		if(is_bool($_v) or $v=="true" or $v=="false")
			return "bool";
		elseif(is_null($_v) or $v=="null")
			return "null";
		elseif(is_numeric($_v))
			return "num";
		elseif(is_string($_v))
			return "string";
		elseif(is_array($_v))
			return "array";
		return "unknown";
	}

	public static function settype_bool($_v)
	{
		if( $_v === true or strtolower(trim($_v)) === "true")
			return true;
		return false;
	}

	public static function get_vartype_by_cont($_v)
	{
		$ret=null;
		switch(self::get_vartype($_v))
		{
			case "bool":
				$ret=self::settype_bool($_v);
				break;
			case "null":
				$ret=null;
				break;
			case "num":
				$ret=(double) $_v;
				break;
			case "string":
				$ret=$_v;
				break;
			default:
				new LogMsg(0, __class__."::".__function__, " VAR ".print_r($_v,true).": unknown type!");
		}
		return $ret;
	}

	public static function php2js2($_array, $_name)
	{
		$ret = "\n".$_name."=Array();\n";

		if( is_array($_array) )
			foreach($_array as $idx=>$wert)
		{

			switch(self::get_vartype($wert))
			{
				case "bool":

					if($wert)
						$ret .= $_name."[\"".$idx."\"]=true;\n";
					else
						$ret .= $_name."[\"".$idx."\"]=false;\n";

					break;

				case "null":
					$ret.=$_name."[\"".$idx."\"]=null;\n";
					break;
				case "num":
					$ret.=$_name."[\"".$idx."\"]=".(double)$wert.";\n";
					break;
				case "string":
					$ret.=$_name."[\"".$idx."\"]=\"".$wert."\";\n";
					break;
				case "array":
					$ret.=self::php2js2($wert,$_name."[\"".$idx."\"]");
					break;
				default:
					new LogMsg(0, __class__."::".__function__, "VAR ".print_r($_v,true).": unknown type!");
			}
		}
		return $ret;
	}

	public static function fileFinder($_path,$_rekursiv=false)
	{
		$ret=array();
		if(is_dir($_path))
		{
			if($dh=opendir($_path))
			{
				while(($fn=readdir($dh)))
				{
					if(is_file($_path."/".$fn))
					{
						if( isNotFalse($pos = strrpos($fn,".") ) )
						{
							$fext=substr($fn,$pos+1);
						}else{
							$fext="";
						}
						$ret[$_path."/".$fn]=array(
							"name"=>$fn,
							"mtime"=>filemtime($_path."/".$fn),
							"size"=>filesize($_path."/".$fn),
							"fext"=>$fext,
							);
					}else{
						if($fn!="." and $fn!="..")
						{
							if($_rekursiv and is_dir($_path."/".$fn))
							{
								$ret=array_merge($ret,self::fileFinder($_path."/".$fn,true));
							}
						}
					}
				}
				closedir($dh);
			}
		}
		return $ret;
	}

	public static function split_on_first($_string, $_needle, $_trim = false, $_back = false)
	{
		$ret = array();

		if( isNotFalse( $pos = $_back ? strrpos($_string, $_needle) : strpos($_string, $_needle) ) )
		{

			$l = substr($_string, 0, $pos);
			$r = substr($_string, $pos + strlen($_needle));

			if( isFalse($_trim) )

				$ret = array($l, $r);

			else

				$ret = array(trim($l), trim($r));

		}else{

			$ret = array($_string);

		}

		return $ret;
	}

	public static function split_on_last($_string, $_needle, $_trim=false, $_back=false)
	{

		return self::split_on_first($_string, $_needle, $_trim, true);

	}

	public static function json_enc($_array)
	{
		return json_encode($_array,JSON_UNESCAPED_SLASHES | JSON_HEX_APOS | JSON_HEX_QUOT | JSON_HEX_AMP);
	}

	public static function json_dec($_string)
	{
		return json_decode($_string,true);
	}

	public static function gzip_enc(string $_string, int $_clevel = 3)
	{

		return gzencode($_string, $_clevel);

	}

	public static function gzip_dec(string $_string)
	{

		return gzdecode($_string);

	}

	public static function bzip_enc(string $_string, int $_clevel = 1)
	{

		return bzcompress($_string, $_clevel);

	}

	public static function bzip_dec(string $_string)
	{

		return bzdecompress($_string);

	}


	public static function extract_date($ina)
	{
		$ret="";
		$in=trim($ina);
		//Punkte?
		$dot=preg_match ("/\./",$in);
		if ($dot==0)
		{
			switch (strlen($in))
			{
			case 6:
				if(substr($in,4,2)>date("y")) {$c="19";}else{$c="20";}
                $ret=substr($in,0,2).".".substr($in,2,2).".".$c.substr($in,4,2);
				break;
			case 8:
				$ret=substr($in,0,2).".".substr($in,2,2).".".substr($in,4,4);
				break;
			default:
				break;
			}
        }else{
			if((substr($in,2,1)==".") and (substr($in,5,1)=="."))
			{
				switch (strlen($in))
				{
				case 8:
					if(substr($in,6,2)>date("y")) {$c="19";}else{$c="20";}
					$ret=substr($in,0,2).".".substr($in,3,2).".".$c.substr($in,6,2);
					break;
				case 10:
					$ret=substr($in,0,2).".".substr($in,3,2).".".substr($in,6,4);
					break;
				}
			}
		}
		return($ret);
	}

	public static function extract_time($ina)
	{
		$ret="";
		$in=trim($ina);
		//Punkte?
		$dot=preg_match ("/\:/",$in);
		if ($dot==0)
		{
			switch (strlen($in))
			{
			case 4:
				$ret=substr($in,0,2).":".substr($in,2,2);
				break;
			case 6:
				$ret=substr($in,0,2).":".substr($in,2,2).":".substr($in,4,4);
				break;
			default:
				break;
			}
		}else{
			if(substr($in,2,1)==":")
			{
				$ret=substr($in,0,2).":".substr($in,3,2);
			}
		}
		return($ret);
	}

	public static function arr2mysql_in($_arr)
	{
		$ret="";
		if(is_array($_arr) and count($_arr))
			foreach($_arr as $item)
			{
				if(strlen($ret))
					$ret.=", ";
				$ret.="'".$item."'";
			}
		return $ret;
	}

	public static function monatjahr_mysql2de($_dat,$_format="num")
	{
		$monate_k=array("Jan","Feb","Mär","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez");
		$monate_l=array("Januar","Februar","März","Mai","Juni","Juli","August","September","Oktober","November","Dezember");
		$jahr=substr($_dat,0,4);
		$monat=substr($_dat,4,2);
		$monatint=intval($monat);
		$ret="";
		switch($_format)
		{
			case "kurz": $ret.=$monate_k[$monatint];
				break;
			case "lang": $ret.=$monate_l[$monatint];
				break;
			default: $ret.=$monat." -";
		}
		return $ret." ".$jahr;
	}


	public static function datum_de2mysql($_dat)
	{
		return substr($_dat,6,4)."-".substr($_dat,3,2)."-".substr($_dat,0,2);
	}

	public static function datum_mysql2de($_dat)
	{
		return substr($_dat,8,2).".".substr($_dat,5,2).".".substr($_dat,0,4);
	}

	public static function format_datemysql($_j,$_m,$_t)
	{
		return str_pad($_j,4,"0",STR_PAD_LEFT)."-".str_pad($_m,2,"0",STR_PAD_LEFT)."-".str_pad($_t,2,"0",STR_PAD_LEFT);
	}

	public static function format_num($_summe,$_nk=2,$_tsd="")
	{
		return number_format(floatval($_summe),$_nk,",",$_tsd);
	}

	public static function onlyNumbers($_string)
	{

		$ret = "";

		for($i=0; $i<strlen($_string); $i++)
		{

			$needle = substr($_string, $i, 1);

			if( isNotFalse(strpos("0123456789", $needle)) )
				$ret .= $needle;

		}

		return $ret;
	}

	public static function globals_overview()
	{
		$erg=array();
		foreach($GLOBALS as $idx=>$data)
			$erg[$idx]	=gettype($GLOBALS[$idx]);

		asort($erg);

		foreach($erg as $idx=>$data)
			new LogMsg(2,"Info","GLOBALS['".$idx."']"." ".$data);
	}

	public static function format_bytes3($B, $D=2)
	{
		$S = 'kMGTPEZY';
		$F = floor((strlen($B) - 1) / 3);
		return sprintf("%.{$D}f", $B/pow(1024, $F)).' '.@$S[$F-1].'B';
	}


	public static function format_bytes($bytes, $force_unit = NULL, $format = NULL, $si = TRUE)
	{
		// Format string
		$format = ($format === NULL) ? '%01.2f %s' : (string) $format;

		// IEC prefixes (binary)
		if( isNotFalse($si) OR isNotFalse(strpos($force_unit, 'i')) )
		{
			$units = array('B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB');
			$mod   = 1024;
		}
		// SI prefixes (decimal)
		else
		{
			$units = array('B', 'kB', 'MB', 'GB', 'TB', 'PB');
			$mod   = 1000;
		}

		// Determine unit to use
		if ( isFalse($power = array_search((string) $force_unit, $units)) )
		{
			$power = ($bytes > 0) ? floor(log($bytes, $mod)) : 0;
		}

		return sprintf($format, $bytes / pow($mod, $power), $units[$power]);
	}

	public static function pmap($_pid)
	{
		#$x=new bin_dev_functions;
		$a=array();

		bin_dev_functions::secured_exec("pmap ".$_pid,$a);

		$a=explode("\n",$a[0]);

		if(count($a))
			while( !strlen(trim($total=array_pop($a))) )
				if(!count($a))	break;

		$total=explode(" ",mf::str_allrepl("  "," ",trim($total)));

		if(isset($total[1])) return($total[1]);

		return false;
	}

	public static function GC_stat($_maildata=false, $_additionalText = "")
	{
		static $mem;

		$memusage=memory_get_usage(true);

		gc_enable();
		gc_collect_cycles();
		$ret="GC: ";
		$gcmc=gc_mem_caches();
		$st=gc_status();
		foreach($st as $idx=>$data)
		{
			$ret.=$idx.": ".$data.", ";
		}
		$ret.="Mem: ".mf::format_bytes3($memusage).", max. ".mf::format_bytes3(memory_get_peak_usage()).", pmap: ".mf::pmap(posix_getpid());
		gc_disable();

		if( isNotFalse($_maildata) and $memusage > (int)$mem and (int)$mem > 0)
		{
			try
			{
				$m=new cool_mail( $_maildata["mailsender"], (isset($GLOBALS["application"])?$GLOBALS["application"]." ":"Smart ")."Mem Usage", $_maildata["mailsendername"] );
				$m->addAddr( $_maildata["mailrecipient"],$_maildata["mailrecipientname"] );
				$m->addText("Speicherverbrauch steigt von ".mf::format_bytes3($mem)." auf ".mf::format_bytes3($memusage).".\n\nautomatic mail\n".
					"GC memcaches: ".print_r($gcmc,true)."\n".
					"GC status: ".$ret."\n");
				$m->addText("\n".$_additionalText."\n");
				$m->send();
				unset($m);
			}catch (throwable $e){
				new LogMsg(0, __class__."::".__function__, "send mail: ".$e->getMessage().", ".str_replace("\n",", ",print_r($_maildata,true)));
			}
		}
		$mem=$memusage;

		memory_reset_peak_usage();

//		mf::globals_overview();

		return $ret;
	}

	//Alle Zeichen(folgen) in einem String ersetzen
	public static function str_allrepl($_from,$_to,$_str)
	{
		$cnt=1;
		while ($cnt)
			$_str=str_replace($_from,$_to,$_str,$cnt);
		return $_str;
	}
	public static function get_config()
	{
		$ret = [];

		$files = [];

		foreach(func_get_args() as $file)
		{

			if( is_array($file) )
			{

				if( count($file) )
					foreach($file as $filename)
						$files[] = $filename;

			}else{

				$files[] = $file;

			}
		}

		asort($files);

		foreach($files as $file)
		{

			if( is_file($file) )
			{

				$ret = array_replace_recursive($ret, mf::parse_config(file_get_contents($file)));

			}

		}

		return $ret;
	}

	public static function parse_config($_str,$_with_remarks=false)
	{
		if(empty($_str))
			return false;
		$lines=explode("\n", $_str);
		$ret=array();
		$inside_section=false;
		$comment=0;
		foreach($lines as $orgline)
		{
			$line=trim($orgline);
			if(!$line or $line[0] == "#" or $line[0] == ";" or ( isFalse(strpos($line, '=')) and $line[0]!="[") )
			{
				continue;
			}
			if($line[0]=="[" and $endIdx=strpos($line, "]"))
			{
				$inside_section=substr($line, 1, $endIdx-1);
				$saddr=&$ret;

				$sub=explode("/",$inside_section);

				if(is_array($sub) and  count($sub))
					foreach($sub as $part)
					{
						$part=trim($part);
						if(!isset($saddr[$part]))
						{
							$saddr[$part]=array();
						}
						$saddr=&$saddr[$part];
					}
				continue;
			}
			if(!strpos($line,'=')) continue;

			$tmp=explode("=",$line,2);

			$key=rtrim($tmp[0]);
			$value=self::get_vartype_by_cont(trim($tmp[1]));

			if($inside_section)
			{

				if( isset($saddr[$key]) )
				{

					if(! is_array($saddr[$key]) )
					{

						$saddr[$key]=array($saddr[$key]);

					}

					$saddr[$key][]=$value;

				}else{

					$saddr[$key]=$value;

				}
			}else{
				$ret[$key]=$value;
			}
		}
		return $ret;
	}

	public static function Sleep($seconds)
	{
		$stop=time() + (int) $seconds;
		while(time() < $stop)
			usleep(99999);
	}



	public static function cleanArray($_array, $_fmt="s")
	{

		$_fmt=strtolower(substr($_fmt,0,1));

		$ret = [];

		if( is_array($_array) and count($_array) )
		{
			foreach( $_array as $item )
			{
				switch( $_fmt )
				{
					case "i":
						$ret[] = intval(trim($item));
						break;
					case "f":
					case "d":
						$ret[] = floatval(trim($item));
						break;
					default:
						$ret[] = trim($item);
				}
			}
		}

		if(! count($ret) )
			$ret = false;

		return $ret;
	}
}


?>