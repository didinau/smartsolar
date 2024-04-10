<?php

/**
 * Serial port control class
 *
 * THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTIES !
 * USE IT AT YOUR OWN RISKS !
 *
 * @author Rémy Sanchez <remy.sanchez@hyperthese.net>
 * @author Rizwan Kassim <rizwank@geekymedia.com>
 * @thanks Aurélien Derouineau for finding how to open serial ports with windows
 * @thanks Alec Avedisyan for help and testing with reading
 * @thanks Jim Wright for OSX cleanup/fixes.
 * @copyright under GPL 2 licence
 */


class COM_serial_nodio extends bin_dev_functions
{

	public $_dev		= false;
	public $_fd			= false;
	public $_stat		= bin_dev_functions::SERIAL_DEVICE_NOTSET;
	public $autoflush	= true;
	public $debug		= false;
	public $error		= "";
	public $_buffer		= "";
	public $_stty		= array();
	public $_stty_modes	= array(
		"flowcontrol"	=>array(
			"none"		=> "clocal -crtscts -ixon -ixoff",
			"rts/cts"	=> "-clocal crtscts -ixon -ixoff",
			"xon/xoff"	=> "-clocal -crtscts ixon ixoff",
			),
		"bauds"			=>array(
			110			=> 11,
			150			=> 15,
			300			=> 30,
			600			=> 60,
			1200		=> 12,
			2400		=> 24,
			4800		=> 48,
			9600		=> 96,
			19200		=> 19,
			38400		=> 38400,
			57600		=> 57600,
			115200		=> 115200,
			),
		"parity"		=>array(
			"n"			=> "-parenb",
			"o"			=> "parenb parodd",
			"e"			=> "parenb -parodd",
			),
		);

	function __destruct()
	{
		$this->devClose();

		if($this->debug) new LogMsg(0, __class__ . "::" . __function__, "shutdown");
	}

	function __construct($_debug = false)
	{
		$this->_stty = array();

		if( $this->secured_exec("stty --version") !== 0 )
		{

			new LogMsg(0, __class__ . "::" . __function__, "No stty availible, unable to run");

		}

		#	register_shutdown_function(array($this, "devClose"));

		$this->debug = $_debug;

	}

	/**
	* read from device
	*
	* @param string $str          string to be sent to the device
	* @param float  $waitForReply time to wait for the reply (in seconds)
	* NICHT WARTEN!
	*/
	public function debug_timeRead($_secs)
	{
		$place = __class__ . "::" . __function__;

		$stop = microtime(true) + $_secs;

		while( microtime(true) < $stop )
		{

			$res = $this->devRead(4096);

			if( isFalse($res) )
			{

				new LogMsg(0, $place, "error reading from device");

				return false;

			}

			new LogMsg(0, $place, "read  " . strlen($res) . " bytes:\n" . $this->bin2hexfriendly($res));

			usleep(1000000);

		}

		return true;

	}


	public function sttyOpts($_opts)
	{

		foreach(explode(" ",$_opts) as $op)
		{

			$this->_stty[] = $op;

		}

	}

	/**
	* Opens the device for reading and/or writing.
	*
	*
	* @param  device, Baud, Config Line, string $mode Opening mode : same parameter as fopen()
	* @param  e.g. devOpen("/deb/ttyUSB0",9600,"8n1");
	* @return bool
	*/
	public function devOpen($_dev,$_baud,$_conf,$_flow="none",$mode="r+b")
	{

		$place = __class__ . "::" . __function__;

		if( $this->_stat === bin_dev_functions::SERIAL_DEVICE_OPENED )
		{

			new LogMsg(0, $place, "The device is already opened");

			return false;

		}

		// Checks
		$op = intval($_baud);

		if( isset($this->_stty_modes["bauds"][$op]) )
		{

			$this->_stty[]=$this->_stty_modes["bauds"][$op];

		}else{

			new LogMsg(0, $place, "The Baud " . $_baud . " is not valid!");

			return false;

		}

		if( strlen($_conf) !== 3 )
		{

			new LogMsg(0, $place, "The Configuration String " . $_conf . " is not valid!");

			return false;

		}

		$op = intval(substr($_conf, 0, 1));

		if( $op < 5 or $op > 8 )
		{

			new LogMsg(0, $place, "The Character Length " . $op . " is not valid!");

			return false;

		}

		$this->_stty[] = "cs" . $op;

		$op = strtolower(substr($_conf, 1, 1));

		if( isset($this->_stty_modes["parity"][$op]) )
		{

			foreach(explode(" ", $this->_stty_modes["parity"][$op]) as $op)
			{

				$this->_stty[] = $op;

			}

		}else{

			new LogMsg(0, $place, "The Parity " . $op . " is not valid!");

			return false;

		}

		$op = intval(strtolower(substr($_conf,2,1)));

		if( $op = 1 or $op = 2 )
		{

			$this->_stty[] = ( $op==1 ? "-" : "" ) . "cstopb";

		}else{

			new LogMsg(0, $place, "Stopbits " . $op . " is not valid!");

			return false;

		}

		$op = "stty -F " . $_dev . " " . implode(" ", $this->_stty);

		if( $this->debug ) new LogMsg(0, $place, "STTY: " . $op);

		if( $ret = $this->secured_exec($op, $error) === false )
		{

			new LogMsg(0, $place, "Error stty " . $op . " (" . $ret . ")\n" . $out);

			return false;

		}else{

			$this->_stat = bin_dev_functions::SERIAL_DEVICE_OPENED;

			$this->_dev = $_dev;

		}

		if($this->debug) new LogMsg(0, $place, "open device");

		if( $this->_fd = @fopen($this->_dev, $mode ) )
		{

			stream_set_blocking($this->_fd, false);
			// stream_set_timeout($this->_fd, 0, 100);

			$this->_stat = bin_dev_functions::SERIAL_DEVICE_OPENED;

			return true;

		}

		$this->_fd = false;

		$this->_stat = bin_dev_functions::SERIAL_DEVICE_NOTSET;

		new LogMsg(0, $place, "Unable to open the device");

		return false;
	}

	public function devClose()
	{

		$this->_stty = array();

		if( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
			return true;

		fclose($this->_fd);

		$this->_fd = false;

		$this->_stat = bin_dev_functions::SERIAL_DEVICE_NOTSET;

		return true;
	}

	public function isOpen()
	{
		if ( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
			return false;

		return true;
	}

	/**
	* Flushes the output buffer
	* @return bool
	*/
	public function devFlush()
	{

		$place = __class__ . "::" . __function__;

		if( $this->isOpen() )
		{

			try{

				$w = fwrite($this->_fd, $this->_buffer);

			}catch ( throwable $e ){

				$this->error = $e->getMessage();

				$w = false;

			}

			if( isNotFalse($w) )
			{

				if( $this->debug ) new LogMsg(0, $place, "DEBUG: write " . $w . " Bytes (" . $this->error . ")");

				if( $w < strlen($this->_buffer) )
				{

					$this->_buffer = substr($this->buffer, $w);

				}else{

					$this->_buffer = "";

				}

				return true;

			}else{

				if( $this->debug ) new LogMsg(0, $place, "DEBUG: " . $this->error);

			}
		}

		return false;
	}

	/**
	* Sends a string to the device
	*
	* @param string $str          string to be sent to the device
	* @param float  $waitForReply time to wait for the reply (in seconds)
	* NICHT WARTEN!
	*/
	public function devSend($_str, $waitForReply = 0.1)
	{

		$this->_buffer .= $_str;

		if ( $this->autoflush )
			return $this->devFlush();

		return true;
	}

	/**
	* Reads the port until no new datas are availible, then return the content.
	*
	* @param int $count Number of characters to be read (will stop before
	*                   if less characters are in the buffer)
	* @return string
	*/
	public function devRead($count = 1)
	{

		$place = __class__ . "::" . __function__;

		if( $this->_stat !== bin_dev_functions::SERIAL_DEVICE_OPENED )
		{

			$this->error = "Device must be opened to read it";

			if( $this->debug ) new LogMsg(0, $place, "DEBUG: " . $this->error);

			return false;

		}

		$content = "";

		$i = 0;

		if ( $count !== 0 )
		{

			do{

				if( $i > $count )
				{

					$content .= fread($this->_fd, ($count - $i));

				}else{

					$content .= fread($this->_fd, 128);

				}

			}while( ($i += 128) == strlen($content) );

		}else{

			do{

				$content .= fread($this->_fd, 128);

			}while( ($i += 128) == strlen($content) );

		}

		if( $this->debug ) new LogMsg(0, $place, "read " . strlen($content) . " bytes");

		return $content;
	}

}

?>