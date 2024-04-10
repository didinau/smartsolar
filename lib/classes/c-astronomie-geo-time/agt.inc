<?php

/* Astro-, Geo- und Zeitfunktionen
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


class agt
{

	public static function sonnendaten($_latitude,$_longitude,$_time = false): array
	{

		if( isFalse($_time) )	$_time = time();

		$ret = date_sun_info($_time,$_latitude,$_longitude);

		foreach($ret as $idx => $data)
		{
			//$ret[$idx]=date("d.m.Y, H:i:s",$data);

			$ret[$idx] = date("H:i:s", $data);

			$ret["ts_".$idx] = $data;

		}

		$ret["date"] = date("d.m.Y",$_time);

		$ret["duration"] = self::format_timeinterval($ret["ts_sunset"] - $ret["ts_sunrise"], false);


		return $ret;
	}


	public static function format_timeinterval($_seconds, $_withSeconds = true, $_withDays = false)
	{
		$ret = "";

		$day = false;
		$hou = 0;
		$min = 0;
		$sec = false;

		$min = intval($_seconds / 60);

		$hou = intval($min / 60);

		$min = $min % 60;


		if( isNotFalse($_withDays) )
		{
			$day = intval($hou / 24);
			$hou = $hou % 24;
			$ret = $day.":";
		}

		$ret .= str_pad($hou,2,"0",STR_PAD_LEFT).":".str_pad($min,2,"0",STR_PAD_LEFT);

		if( isNotFalse($_withSeconds) )
		{
			$sec = $_seconds % 60;
			$ret .= ":".str_pad($sec,2,"0",STR_PAD_LEFT);
		}

		return $ret;

	}


/**
 * Parse various time formats into standard H:i:s format (e.g. "2:30pm" -> "14:30:00").
 *
 * from https://gist.github.com/curtisharvey
 *
 * @param string  $timeString     Time to parse
 * @param boolean $includeSeconds Optionally include seconds, default = true
 *
 * @return string|null Null if unable to parse time
 */
	public static function parseTime($timeString, $includeSeconds = true)
	{
		$timePattern = '/^\s*(?<hour>[01]?[0-9]|2[0-3])(?::(?<min>[0-5][0-9]))?(?::(?<sec>[0-5][0-9]))?\s*(?<meridiem>[ap](?:\.?m\.?)?)?\s*$/i';
		if (preg_match($timePattern, $timeString, $matches) === 1)
		{
			$hour = (int) $matches['hour'];
			$min = isset($matches['min']) ? (int) $matches['min'] : 0;
			if ($hour < 12 && isset($matches['meridiem']) && ($matches['meridiem'][0] === 'p' || $matches['meridiem'][0] === 'P'))
			{
				$hour += 12;
			}
			$str = str_pad($hour, 2, '0', STR_PAD_LEFT) . ':' . str_pad($min, 2, '0', STR_PAD_LEFT);
			if ($includeSeconds)
			{
				$sec = isset($matches['sec']) ? (int) $matches['sec'] : 0;
				$str .= ':' . str_pad($sec, 2, '0', STR_PAD_LEFT);
			}

			return $str;
		}

		return null;
	}


/**
 * Convert time string to minutes since midnight.
 *
 * from https://gist.github.com/curtisharvey
 *
 * @param string  $timeString     Time to parse
 * @param boolean $includeSeconds Optionally include seconds and return as float, default = false
 *
 * @return int|float|null Null if unable to parse time
 */
	public static function timeToMinutes($timeString, $includeSeconds = false)
	{
		$time = self::parseTime($timeString, $includeSeconds);
		if (isset($time))
		{
			$parts = explode(':', $time);

			return ($parts[0] * 60) + $parts[1] + ($includeSeconds ? ($parts[2] / 60) : 0);
		}

		return null;
	}


	public static function timeToSeconds($timeString, $includeSeconds = false)
	{
		$time = self::parseTime($timeString, $includeSeconds);
		if (isset($time))
		{

			$parts = explode(':', $time);

			return ($parts[0] * 3600) + $parts[1] * 60 + intval($includeSeconds ? $parts[2]  : 0);
		}

		return null;
	}


}

?>