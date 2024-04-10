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

$config_file = __DIR__."/../var/conf.ini";

$application = "Smart Daemon";

function dataprovider($_device = "")
{
	return array(
			"edevice" => $_device, "device" => $_device, "dcharge" => 0, "ddischarge" => 0, "dload" => 0, "dpv" => 0, "dgrid" => 0,
			"dprognose" => 0, "dprognextd" => 0, "dfromgrid" => 0,"dproduktion" => 0, "dbezugp" => 0, "dselbstp" => 0, "dgridcharge" => 0, "dgridc" => 0,
			"dselbst" => 0,

			"tpv" => 0, "tgrid" => 0, "tdischarge" => 0, "tcharge" => 0, "tload" => 0, "tgridcharge" => 0, "tfromgrid" => 0,

			"pdevice" => $_device,
			"ppv" => 0, "ppv1" => 0, "ppv2" => 0, "ppv3" => 0, "ppv4" => 0, "puser" => 0, "pload" => 0, "pgrid" => 0, "pcharge" => 0, "pdischarge" => 0,
			"pturn" => null,
			"paccharge" => 0,

			"soc" => 0,"vbat" => 0, "ibat" => 0,
			"fac" => -1, "vac" => -1, "temp" => 0,

			"mdpv" => 0, "mdgrid" => 0, "mdfromgrid" => 0, "mpv" => 0, "mgrid" => 0, "mload" => 0,"more" => [],"dmore" => [],

			"a_temperature" => null,

			"publishE" => false, "publishP" => false, "publishT" => false,

			"T" => date("Y-m-d H:i:s"), "S" => date("H:i:s"), "D" => date("Y-m-d"), "H" => date("G"),

			"edatum" => date("Y-m-d"), "pzeit" => date("Y-m-d, H:i:s"),

		);
}

?>