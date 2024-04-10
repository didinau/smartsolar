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


class COM_modbus_registerindex
{
	public $start = 0;
	public $regcnt = 0;

	function __construct($_start, $_regcnt)
	{

		$this->start = $_start;
		$this->regcnt = $_regcnt;

	}

	function idx($_register)
	{

		$idx=$_register-$this->start;

		if( $idx > $this->regcnt ) new LogMsg(0, __class__ . "::" . __function__, "index > cnt");

		return $idx * 2;

	}

}

?>