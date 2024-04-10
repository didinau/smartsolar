<?php
/*
 * network.classes.php
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

/** * Netzwerk-Funktionen
	* (C) Dieter Naujoks (dieter@naujoks.homeip.net) 26.02.2015
**/


class sockserver_tcp
{
	public $lasterror = "";

	public $lasterrno = 0;

	public $debug = false;

	public $ident = "socks";

	public function notify_close($sock)
	{

		$this->store_error($sock);

		$this->close($sock);

	}

	public function store_error($sock)
	{

		$this->lasterrno = socket_last_error($sock);

		$this->lasterror = socket_strerror($this->lasterrno);

		if($this->debug)
			new LogMsg(0, __class__."::".__function__, "Errno ".$this->lasterrno.": ".$this->lasterror);
	}

	public function close($sock)
	{

		@socket_shutdown($sock);

		@socket_close($sock);

	}

	public function connect($ADDR, $PORT, $_localadr=null)
	{

		$sock=false;

		if ( ! $fd = @socket_create(AF_INET, SOCK_STREAM, 0))
		{

			$this->lasterror = "error create socket";

		}else{

			if( $_localadr !== null )
			{

				socket_bind($fd, $_localadr);

			}

			if( @socket_connect($fd, $ADDR, $PORT) != true )
			{

				$this->notify_close($fd);

			}else{

				$sock = $fd;

			}
		}

		return $sock;
	}

	public function listen($LISTEN_PORT, $LISTEN_ADDR="0.0.0.0")
	{

		$sock = false;

		if(! $fd = @socket_create(AF_INET, SOCK_STREAM, 0) )
		{

			$this->lasterror = "error create socket";

		}else{

			if ( ($ret = @socket_set_option($fd, SOL_SOCKET, SO_REUSEADDR, 1)) < 0 )
			{

				$this->notify_close($fd);

			}else{

				if ( ($ret = @socket_bind($fd, $LISTEN_ADDR, $LISTEN_PORT)) < 0 )
				{

					$this->notify_close($fd);

				}else{

					if( ($ret = @socket_listen($fd, 64)) < 0 )
					{

						$this->notify_close($fd);

					}else{

						$sock = $fd;

					}
				}
			}
		}

		return $sock;
	}

	public function waitconnect($sock)
	{

		return $this->accept($sock);

	}

	public function accept($sock)
	{

		$newsock = false;

		if( is_resource($sock) or is_object($sock) )
		{

			$newsock = @socket_accept($sock);

			if( isFalse($newsock) )
			{

				$this->store_error($sock);

			}
		}

		return $newsock;
	}

	public function remoteaddr($sock)
	{

		$addr = "";

		$port = 0;

		$ret = false;

		if( is_resource($sock) or is_object($sock) )
		{

			if(@socket_getpeername($sock, $addr, $port))
			{

				$ret = array("addr" => $addr, "port" => $port);

			}
		}

		return $ret;
	}

	public function nonblock($sock)
	{

		if( is_resource($sock) or is_object($sock) )
		{

			$ret = @socket_set_nonblock($sock);

			if( isFalse($ret) )
			{

				$this->store_error($sock);

			}
		}
	}

	public function block($sock)
	{
		if( is_resource($sock) or is_object($sock) )
		{
			$ret = @socket_set_block($sock);

			if( isFalse($ret) )
			{

				$this->store_error($sock);

			}
		}
	}

	public function _timeout(&$sock, $opt, $millisec)
	{

		if( is_resource($sock) or is_object($sock) )
		{

			$d = array("sec" => floor($millisec / 1000), "usec" => ($millisec % 1000) * 1000);

			$ret = socket_set_option($sock, SOL_SOCKET, $opt, $d);

			if( isFalse($ret) )
			{

				$this->store_error($sock);

			}
		}
	}

	public function _opt(&$sock, $opt, $data)
	{
		if( is_resource($sock) or is_object($sock) )
		{

			$ret = socket_set_option($sock, SOL_SOCKET, $opt, $data);

			if( isFalse($ret) )
			{
				$this->store_error($sock);
			}
		}
	}

	public function rcv_timeout(&$sock, $millisec = 1000)
	{

		return $this->_timeout($sock, SO_RCVTIMEO, $millisec);

	}

	public function snd_timeout(&$sock, $millisec = 1000)
	{

		return $this->_timeout($sock, SO_SNDTIMEO, $millisec);

	}

	public function read($sock)
	{

		$ret = false;

		if( is_resource($sock) or is_object($sock) )
		{

			$ret = @socket_read($sock, 4096, PHP_BINARY_READ);

			if( isFalse($ret) )
			{

				$this->store_error($sock);

			}
		}

		return $ret;
	}

	public function read_nonblock($sock)
	{

		$ret = false;

		if( is_resource($sock) or is_object($sock) )
		{

			$rcvopt = @socket_get_option($sock, SOL_SOCKET, SO_RCVTIMEO);

			$this->rcv_timeout($sock, 200);

			$ret = @socket_read($sock, 4096, PHP_BINARY_READ);

			if( isFalse($ret) )
			{

				$this->store_error($sock);

			}

			@socket_set_option($sock, SOL_SOCKET, SO_RCVTIMEO, $rcvopt);
		}

		return $ret;
	}

	public function write_raw($sock, $data)
	{

		if( is_resource($sock) or is_object($sock) )
		{

			$cnt = @socket_write($sock, $data);

			if( isFalse($cnt) )
			{

				$this->store_error($sock);

			}
		}
	}

	public function write($sock, &$data)
	{

		$ret = false;

		if( is_resource($sock) or is_object($sock) )
		{

			$tcnt = strlen($data);

			while($tcnt > 0)
			{

				$cnt = @socket_write($sock, $data);
				if( isFalse($cnt) )
				{

					$this->store_error($sock);

					break;

				}

				$tcnt -= $cnt;

				if( $tcnt > 0 )
				{

					$data = substr($data, $cnt);

				}
			}
		}
	}

	public function write_secure($_sock, &$_data)
	{

		return $this->write($_sock, $_data);

	}

	public function read_timeout($sock, $timeout = 1000)
	{

		$ret = false;

		if( is_resource($sock) or is_object($sock) )
		{

			$this->rcv_timeout($sock, $timeout);

			$ret = $this->read($sock, 4096);

		}

		return $ret;
	}

	public function write_timeout($sock, $data, $timeout = 4000)
	{

		if( is_resource($sock) or is_object($sock) )
		{

			$this->snd_timeout($sock, $timeout);

			$ret = $this->write($sock, $data);

		}

		return $ret;
	}
}

?>