<?php

// My little Framework

class LogMsg
{
	function __construct($_level = 0, $_logheader = false, $_logtext = false)
	{

		global $loglevel;

		$firstout = false;

		$_logheader .= ":";

		$loglevelIst = isset($loglevel) ? intval($loglevel) : 200;

		if( intval($_level) <= $loglevelIst)
		{

			foreach(explode("\n", $_logtext) as $text)
			{

				$text = trim($text);

				if( strlen($text) )
				{

					error_log(date("Ymd His")." ".posix_getpid()." | ".$_logheader." ".$text);

					$firstout = true;

				}

				if( isTrue($firstout) )
				{

					$firstout = null;

					$_logheader = "->";

				}
			}
		}
	}
}

class ErrorAndMore
{
	public function __construct($e = false, $level = 0, $ClassOrFunction = false)
	{
		if($e)
		{

			$msg = "Error: ".$e->getMessage()."\nDatei: ".$e->getFile()."\nZeile: ".$e->getLine();
			new LogMsg($level, $ClassOrFunction, $msg);

		}
	}
}

function isFalse($_var)
{
	return $_var === false ? true : false;
}

function isNotFalse($_var)
{
	return ! isFalse($_var);
}

function isNotTrue($_var)
{
	return ! isTrue($_var);
}

function isTrue($_var)
{
	return $_var === true ? true : false;
}

function getLowerVal($_a, $_b)
{
	return ($_a > 0 and $_a < $_b) ? $_a : $_b;
}

function getHigherVal($_a, $_b)
{
	return ($_a > 0 and $_a > $_b) ? $_a : $_b;
}

function isEnabled($_setting)
{

	if( isTrue($_setting) or $_setting == "true" or intval($_setting) == 1 )
	{

		return true;

	}else{

		return false;

	}

}


const sec_per_day = 86400;
const sec_per_hour = 3600;



return true;

?>