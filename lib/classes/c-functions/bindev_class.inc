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


class bin_dev_functions
{
	public $error="";

	public const SERIAL_DEVICE_NOTSET = 0;
	public const SERIAL_DEVICE_SET = 1;
	public const SERIAL_DEVICE_OPENED = 2;


	function __construct()
	{
	}


	// converts 16bit binary number string to integer
	// using two's complement
	public static function _bin16dec($_int16)
	{

		// $num = $bin & 0xFFFF;
		#print "strlen()=".strlen($_int16)."\n";

		$dec=unpack("n",$_int16);
		$dec=$dec[1];

		if(0x8000 & $dec) $dec=- (0x010000 - $dec);

		return $dec;
	}

	public static function a2ulong32($_a,$_idx,$_bigendian=true)
	{

		$dec=(int) ($_a[$_idx] << 8) | $_a[$_idx+1];
		$dec= ($dec << 8) | $_a[$_idx+2];
		$dec= ($dec << 8) | $_a[$_idx+3];

		#if($_signed and (0x8000 & $dec)) $dec=- (0x010000 - $dec);

		return $dec;
	}

	public function ulng32($_array,$_idx,$_divisor=1)
	{
		return $this->a2ulong32($_array,$_idx) / $_divisor;
	}

	public function sint16($_array,$_idx,$_divisor=1,$_signed=true)
	{
		return $this->uint16($_array,$_idx,$_divisor,$_signed);
	}

	public function uint16($_array,$_idx,$_divisor=1,$_signed=false)
	{
		return $this->a2uint16($_array,$_idx,$_signed) / $_divisor;
	}

	public function a2sint16($_a,$_idx)
	{
		return $this->a2uint16($_a,$_idx,true);
	}

	public function uint162a()
	{
		$a="";
		foreach(func_get_args() as $val)
		{
			$a.=pack("n", $val);
		}
		return $a;
	}

	public function a2uint16($_a,$_idx,$_signed=false)
	{

		// $num = $bin & 0xFFFF;
		$dec=(int) ($_a[$_idx] << 8) | $_a[$_idx+1];

		if($_signed and (0x8000 & $dec)) $dec=- (0x010000 - $dec);

		return $dec;
	}

	// Calculate CRC16 (ModBus)
	public function crc16($data)
	{
		$crc = 0xFFFF;
		for ($i = 0; $i < strlen($data); $i++)
		{
			$crc ^=ord($data[$i]);
     		for ($j = 8; $j !=0; $j--)
			{
				if (($crc & 0x0001) !=0)
				{
					$crc >>= 1;
					$crc ^= 0xA001;
				}
				else
				$crc >>= 1;
			}
		}
		$highCrc=floor($crc/256);
		$lowCrc=($crc-$highCrc*256);
		return chr($lowCrc).chr($highCrc);
	}

	public function bin2hexfriendly (string $binString): string
	{
		$hexString=strtoupper(bin2hex($binString));
		$hexString=chunk_split($hexString,2," ");
		$hexString=chunk_split($hexString,132,"\n");
		return $hexString;
	}

	public function bin2sexy (string $binString): string
	{
		$hexString="";
		$p=0;
		$a="";
		for($i=0;$i<strlen($binString);$i++)
		{
			$n=ord(substr($binString,$i,1));
			$hexString.=sprintf("%02s",dechex($n))." ";
			$p++;
			if($n>31)
				$a.=chr($n);
			else
				$a.=".";
			if($p>16)
			{
				$hexString.=" | ".$a."\n";
				$p=0;
				$a="";
			}
		}
		if(strlen($a))
			$hexString.=" | ".$a."\n";
		$hexString.="\n";
		return $hexString;
	}

	public static function secured_exec($cmd, &$out = false)
	{

		$msg = array();

		try
		{

			$retrc = self::__exec($cmd, $msg);

		} catch (throwable $e)
		{

			new ErrorAndMore($e, 0, __class__."::".__FUNCTION__);

			$retrc = 1;

		}

		if( func_num_args() == 2 )
			$out = $msg;

		return $retrc;
	}

	public static function __exec($cmd, &$out=false)
	{
		$desc = array(
			1 => array("pipe", "w"),
			2 => array("pipe", "w"),
			3 => array("pipe", "w"),
			);

		$proc = proc_open($cmd."||echo $? >&3", $desc, $pipes);

		$ret = stream_get_contents($pipes[1]);
		$err = stream_get_contents($pipes[2]);
		$rc = stream_get_contents($pipes[3]);

		fclose($pipes[1]);
		fclose($pipes[2]);
		fclose($pipes[3]);

		proc_close($proc);

		if (func_num_args() == 2)
			$out = array(0=>$ret, 1=>$err);

		return (int) $rc;
	}
}
