<?php
/*
 *
 *
 * Copyright 2020 Dieter Naujoks <dnaujoks@naujoks.homeip.net>
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
class _mime
{
	//Alle Zeichen(folgen) in einem String ersetzen
	public static function str_allrepl($_from, $_to, $_str)
	{
		$cnt = 1;

		while($cnt)
			$_str = str_replace($_from, $_to, $_str, $cnt);

		return $_str;

	}

	//Hilfsfunktion für mimes: Ersetzer für die Mime-Datei
	public static function _replace_pattern()
	{

		return array(
			"f" => array("  ", "\t"),
			"t" => array(" ", " ")
			);
	}


	//Hilfsfunktion für mimes: Suffix eines Dateinamens zurückgeben
	public static function get_ext($_filename)
	{

		$arr = explode(".", trim($_filename));

		return strtolower(array_pop($arr));

	}	// mimetype und get_mimetype geben den Mimetyp eines Dateinames zurück

	public static function get_mimetype($_filename)
	{

		return self::mimetype($_filename);

	}

	public static function mimetype($_filename)
	{

		if(! isset($_SESSION["shared_mimetype::types"]) )
		{

			$_SESSION["shared_mimetype::types"] = self::mimes_load();

		}

		$ext = self::get_ext($_filename);

		return isset($_SESSION["shared_mimetype::types"][$ext]) ? $_SESSION["shared_mimetype::types"][$ext] : "application/octet-stream";
	}

	public static function mimes_load($_file = "/etc/mime.types")
	{

		$r_def = self::_replace_pattern();

		$erg = array();

		$fp = fopen($_file, "r");

		if( $fp )
		{

			while($line = fgets($fp, 16758))
			{

				$line = trim($line);

				if( strlen($line) and substr($line, 0, 1) != "#" )
				{

					$line = self::str_allrepl($r_def["f"], $r_def["t"], $line);

					$line = explode(" ", $line);

					for($i = 1; $i < count($line); $i++)
					{

						$erg[$line[$i]] = $line[0];

					}
				}
			}

			fclose($fp);
		}

		return $erg;
	}
}

?>