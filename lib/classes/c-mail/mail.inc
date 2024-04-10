<?php
/*
 * mail.class.php
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

class cool_mail
{
	// Letzte Änderung 09.05.08
	// scheint fehlerfrei zu sein!?
	// Dies, kaum zu glaben, hat sich Dieter selbst erarbeitet!!!

	//$m=new cool_mail("dieter@hbc-service.de", "ein test", "Dieter Naujoks");
	//$m->addAddr("dieter@naujoks.homeip.net","Dieter Naujoks","bcc");
	//$m->addText("Dies ist ein Test\n\nmit zwei Zeilen");
	//$m->addFile("", "/etc/shadow", "shadow.txt");
	//$m->addData("", "/etc/shadow", "file.txt");
	//$m->addText("Dies ist ein Test\n\nmit noch zwei Zeilen");
	//$m->send();
	//Empfangsbestätigung: Header: Disposition-Notification-To: <email>

	public $to = array("to" => "", "cc" => "", "bcc" => "");		// Empfänger

	public $data = "";

	public $subject = "";

	public $mime_boundary = "";

	public $headers = "";

	public $userheaders = "";

	public $to_count = 0;

	//public $eol="\r\n";

	public $eol = "\n";



	function __construct($senderaddr, $subject, $sendername="", $reply="")
	{
		$this->mime_boundary = md5(time());

		if(! strlen($sendername) )
		{

			$sendername = "''";

		}else{

			$sendername = mb_encode_mimeheader($sendername, "UTF-8", "Q");

		}

		if(! strlen($subject) )
		{

			$subject = "ohne";

		}

		$this->subject = mb_encode_mimeheader($subject, "UTF-8", "Q");

		$this->headers .= "From: ".$sendername." <".$senderaddr.">".$this->eol;

		$this->senderaddr = $senderaddr;

		if( strlen($reply) )
		{

			$this->headers .= "Reply-To: <".$reply.">".$this->eol;
			$this->headers .= "Return-Path: <".$reply.">".$this->eol;

		}else{

			$this->headers .= "Reply-To: ".$sendername." <".$senderaddr.">".$this->eol;
			$this->headers .= "Return-Path: ".$sendername." <".$senderaddr.">".$this->eol;

		}

		$this->headers .= "Message-ID: <".$this->mime_boundary."@".$senderaddr.$this->eol;

		//$this->headers .= "Subject: ".$subject.$this->eol;

		$this->headers .= "Content-Type: multipart/mixed; boundary=\"".$this->mime_boundary."\"".$this->eol;

		$this->headers .= 'MIME-Version: 1.0';
	}


	public function addAddr($addr, $name="", $art="to")
	{

		switch( strtolower($art) )
		{

			case "bcc":

				if( strlen($this->to["bcc"]) )
				{

					$this->to["bcc"].=", ";

				}

				if( strlen($name) )
				{

					$this->to["bcc"] .= "\"".mb_encode_mimeheader($name, "UTF-8", "Q")."\" <".$addr.">";

				}else{

					$this->to["bcc"] .= $addr;

				}

				$this->to_count++;

				break;

			case "cc":

				if( strlen($this->to["cc"]) )
				{

					$this->to["cc"] .= ", ";

				}

				if( strlen($name) )
				{

					$this->to["cc"] .= "\"".mb_encode_mimeheader($name, "UTF-8", "Q")."\" <".$addr.">";

				}else{

					$this->to["cc"] .= $addr;

				}

				$this->to_count++;

				break;

			default:
				if(strlen($this->to["to"]))
				{
					$this->to["to"].=", ";
				}
				if (strlen($name))
				{
					$this->to["to"].="\"".mb_encode_mimeheader($name, "UTF-8", "Q")."\" <".$addr.">";
				}else{
					$this->to["to"].=$addr;
				}
				$this->to_count++;
				break;
		}
	}

	public function nextPart($encoding, $name="", $filename="")
	{
		if( strlen($this->data) )
			$this->data .= $this->eol.$this->eol;

		$this->data .= "--".$this->mime_boundary.$this->eol;
		$this->data .= "Content-transfer-encoding: ".$encoding.$this->eol;

		if( strlen($filename) )
		{

			$this->data .= "Content-Disposition: attachment; filename=\"".$filename."\"".$this->eol;

		}
	}

	public function addText($text)
	{

		if( strlen($text) )
		{

			$this->nextPart("8BIT");

			$this->data .= "Content-type: text/plain; charset=\"iso-8859-15\"".$this->eol.$this->eol;
			$this->data .= $text;

		}
	}

	public function addHTML($text, $mime = "text/html", $charset = "iso-8859-15")
	{

		if( strlen($text) )
		{

			$this->nextPart("8BIT");

			$this->data .= "Content-type: ".$mime."; charset=\"".$charset."\"".$this->eol.$this->eol;
			$this->data .= $text;

		}
	}

	public function addData($mime, $data, $dataname = "")
	{

		if( strlen($data) )
		{

			if( strlen($dataname) )
			{

				$this->data .= "Content-Disposition: attachment; filename=\"".$dataname."\"".$this->eol;

			}

			if(! strlen($mime) )
			{

				$mime = "application/octet-stream";

			}

			$this->nextPart("base64", $dataname, $dataname);

			$this->data .= "Content-type: ".$mime.$this->eol.$this->eol;
			$this->data .= chunk_split(base64_encode($data));

		}
	}

	public function addFile($mime, $file, $filename="")
	{

		if( strlen($file) )
		{

			if( $handle = fopen($file, 'rb') )
			{

				if(! strlen($filename) )
				{

					$filename = basename($file);

				}

				if(! strlen($mime) )
				{

					$mime = "application/octet-stream";

				}

				$this->nextPart("base64", $filename, $filename);

				$this->data .= "Content-type: ".$mime.$this->eol.$this->eol;
				$this->data .= chunk_split(base64_encode(fread($handle, filesize($file))));

				fclose($handle);
			}
		}
	}

	public function addMsg($file, $filename = "")
	{

		if( strlen($file) )
		{

			if( $handle = fopen($file, 'rb') )
			{

				if(! strlen($filename) )
				{

					$filename = basename($file);

				}

				$this->nextPart("8bit", $filename, $filename);
				//$this->data.="Content-Disposition: attachment; filename=\"".$filename."\"".$this->eol;

				$this->data .= "Content-type: message/rfc822; name=\"".$filename."\"".$this->eol.$this->eol;
				$this->data .= fread($handle, filesize($file));

				fclose($handle);
			}
		}
	}

	public function addHeaders($headerlines)
	{

		foreach( explode("\n", str_replace("\r\n","\n",$headerlines)) as $line )
		{

			$newheaders = "";

			if( strlen(trim($line)) )
			{

				$newheaders .= trim($line).$this->eol;

			}

			if( strlen($newheaders) )
			{

				$this->userheaders = $newheaders.$this->userheaders;

			}
		}
	}

	public function get_maildata()
	{

		$sendheaders = "";

		if( strlen($this->to["cc"]) )
		{

			$sendheaders .= "Cc: ".$this->to["cc"].$this->eol;

		}

		if( strlen($this->to["bcc"]) )
		{

			$sendheaders .= "Bcc: ".$this->to["bcc"].$this->eol;

		}

		$this->data .= $this->eol.$this->eol."--".$this->mime_boundary."--".$this->eol;

		$h = explode($this->eol, $this->userheaders);

		$h[] = "Subject: ".$this->subject;

		$h = array_merge($h, explode($this->eol, $sendheaders), explode($this->eol, $this->headers));

		//print "\nHeaders:\n";

		//print_r($this->headers);

		//print "\nh:\n";

		//print_r($h);

		for( $i = 0; $i < count($h); $i++ )
		{

			if( $h[$i] == "" )
				unset($h[$i]);

		}

		$h = implode($this->eol, $h);

		return $h.$this->eol.$this->eol.$this->data;
	}

	public function send()
	{

		if( $this->to_count )
		{

			$sendheaders = "";

			if( strlen($this->to["cc"]) )
			{

				$sendheaders .= "Cc: ".$this->to["cc"].$this->eol;

			}

			if( strlen($this->to["bcc"]) )
			{

				$sendheaders .= "Bcc: ".$this->to["bcc"].$this->eol;

			}

			$this->data .= $this->eol.$this->eol."--".$this->mime_boundary."--".$this->eol;

			//$sendmail="/usr/sbin/sendmail -ti";

			$sendmail = "/usr/sbin/sendmail -Uti -f'".$this->senderaddr."'";

			if( isFalse($fh = popen($sendmail, 'w')) )
			{

				mail($this->to["to"], $this->subject, $this->data, $this->userheaders.$sendheaders.$this->headers);

			}else{

				if( strlen($this->to["to"]) )
				{

					$sendheaders = "To: ".$this->to["to"].$this->eol.$sendheaders;

				}

				$sendheaders .= "Subject: ".$this->subject.$this->eol;

				fwrite($fh, $this->userheaders.$sendheaders.$this->headers.$this->eol.$this->eol.$this->data);

				pclose($fh);

/*				if($fh=fopen("/tmp/testemail","w"))
				{
					fwrite($fh, $this->userheaders.$sendheaders.$this->headers.$this->eol.$this->eol.$this->data);
					fclose($fh);
				}	*/
			}

			return $this->userheaders.$sendheaders.$this->headers.$this->eol.$this->eol.$this->data;

		}
	}

	// Hinzugefügte Email-Lowlevel-Funktione (ehemals charset)
	// alle folgenden Funktionen sind statisch!
	// 2020-01-22

	#public static $allowed="+-/.' !#\$%&*=?^_`{|}~";

	// From Header-Zeile dekodieren, auswerten und prüfen:
	public static function decode_from_header_and_check($_in, $_fallback="")
	{

		$ret=array(
			"header" => "",
			"org" => $_in,
			"newQ" => "",
			"newB" => "",
			"utf8" => "",
			"iso" => "",
			"email" => "",
			"c_schema" => "",
			"c_org" => "",
			);

		$ret = array_merge($ret, self::struct_addressheader($_in, $_fallback));

		if( self::detectUTF8($ret["utf8"]) )
		{
			#$ret["iso"] = iconv("utf-8","ISO-8859-15//IGNORE",$ret["utf8"]);

			$ret["iso"] = iconv("utf-8", "ISO-8859-15//TRANSLIT", $ret["utf8"]);

		}else{

			$ret["iso"] = $ret["utf8"];

		}

		if( $ret["iso"] == $ret["email"] )
		{
			// wenn From nur eine Adresse enthält, braucht
			// nichts kodiert werden!

			$ret["newQ"] = "From: ".$ret["iso"];
			$ret["newB"] = $ret["newQ"];

		}else{

			if( $ret["iso"] == $ret["utf8"] )
			{

				// keine codierung notwendig???
				$new = $ret["before8"];
				if( strlen($new) and strlen($ret["after8"]) )
					$new .= " ";

				$new .= $ret["after8"];

				if( strlen($new) )
					$new .= " ";

				$new .= "<".$ret["email"].">";

				$ret["newQ"] = "From: ".$new;

				$ret["newB"] = $ret["newQ"];

			}else{
				// nun die dekodierten Werte wieder codieren:

				$new = $ret["before8"];

				if( strlen($new) and strlen($ret["after8"]) )
					$new .= " ";

				$new .= $ret["after8"];

				$schema = strlen($ret["c_schema"]) ? $ret["c_schema"] : "Q";

				$ocs = strlen($ret["c_org"]) ? $ret["c_org"] : "utf-8";

				$ret["newQ"] = self::mail_header_encode($new, $ret["header"], "utf-8", $ocs, $schema, 76)." <".$ret["email"].">";

				$ret["newB"] = self::mail_header_encode($new, $ret["header"], "utf-8", "utf-8", "B", 76)." <".$ret["email"].">";;

			}
		}

		return $ret;
	}

	// einen Mime-Header erzeugen
	public static function mail_header_encode($_in, $_headername, $_INPUTCHARSET = "utf-8", $_OUTPUTCHARSET = "utf-8", $_SCHEMA = "Q", $_LENGTH = 76)
	{
		$pref = array(
			"scheme" => $_SCHEMA,
			"input-charset" => $_INPUTCHARSET,
			"output-charset" => $_OUTPUTCHARSET,
			"line-length" => $_LENGTH,
			"line-break-chars" => "\n"
			);

		$ret = iconv_mime_encode($_headername, $_in, $pref);

		if( strtoupper($_SCHEMA) == "Q" )
		{
			$ret = str_replace("\"", "=22", $ret);
			$ret = str_replace("=20", "_", $ret);
			$ret = str_replace(",", "=2C", $ret);
			$ret = str_replace("<", "=3C", $ret);
			$ret = str_replace(">", "=3E", $ret);
		}
		$ret = utf8_decode($ret);

		return $ret;
	}



	// Prüfen, ob im String Multibyte-Zeichen vorhanden sind:
	// Wenn keine da, sollte auch nicht utf8 hin und her kodiert werden?
	public static function detectUTF8($_in)
	{
		// non-ascii multibyte ?
		return preg_match('%(?:
			[\xC2-\xDF][\x80-\xBF]				# non-overlong 2-byte
			|\xE0[\xA0-\xBF][\x80-\xBF]			# excluding overlongs
			|[\xE1-\xEC\xEE\xEF][\x80-\xBF]{2}	# straight 3-byte
			|\xED[\x80-\x9F][\x80-\xBF]			# excluding surrogates
			|\xF0[\x90-\xBF][\x80-\xBF]{2}		# planes 1-3
			|[\xF1-\xF3][\x80-\xBF]{3}			# planes 4-15
			|\xF4[\x80-\x8F][\x80-\xBF]{2}		# plane 16
			)+%xs', $_in);
	}

	public static function range_valid_chars($_string, $_stringvaildchars = "", $_start = 0, $_end = 0)
	{
		if(! is_string($_string) or ! is_string($_stringvaildchars) )
			return false;

		if(! $_end )
			$end = strlen($_string);

		for($i = $_start; $i < $_end; $i++)
		{

			if( isFalse(strpos($_stringvaildchars, substr($_string,$i,1))) )
			{

				return false;

			}
		}

		return true;
	}

	public static function search_str($_string, $_tripple, $_startpos = 0, $_validchars = array())
	{
		$ret = array(
			"status" => false,
			"result" => "",
			"tripple" => array(),
			);

		// check: par ist string(3)
		if( is_string($_tripple) and strlen($_tripple) > 2 )
		{

			for($i = 0; $i < 3; $i++)
			{

				$ret["tripple"][$i] = substr($_tripple, $i, 1);

			}

		}else{

			// check: par ist array(3)
			if( is_array($_tripple) and count($_tripple) > 2 )
			{

				$new = array_merge($_tripple);

				for($i = 0; $i < 3; $i++)
				{

					$t = array_shift($new);

					if( is_string($t) )
					{

						$ret["tripple"][$i] = $t;
					}
				}

				unset($new);
			}
		}

		// check: par ok
		if( count($ret["tripple"]) != 3 )
		{

			$ret["result"] = "err: tripple to short";

			return $ret;

		}

		//valid chars (= array(0=>string,1=>string))
		$vc = array(0 => "", 1 => "");

		for($i = 0; $i < 2; $i++)
		{

			$vc[$i] = array_shift($_validchars);

			if(! is_string($vc[$i]) )
			{

				$vc[$i] = "";

			}
		}

		// starte such an pos _startpos
		$lpos = $_startpos;

		while(true)
		{
			// tripple[0]

			$pos = strpos($_string, $ret["tripple"][0], $lpos);

			if( isNotFalse($pos) )
			{
				// nächste pos, falls nicht gefunden wird

				$lpos = $pos;

				// tripple[1] vorhanden?

				$etpos = strpos($_string, $ret["tripple"][1], $pos + 1);

				// vor tripple[1] ein schliessendes tripple[2] ?

				if( isNotFalse($etpos)
					and isNotFalse($tpos = strpos($_string, $ret["tripple"][2], $pos + 1))
					and $tpos < $etpos )
				{
					// ja, dann ungültig

					$etpos = false;

					$lpos = $tpos;

				}

				// sind im linken part nur gültige Zeichen?

				if( isNotFalse($etpos) and strlen($vc[0]) )
				{

					if(! self::range_valid_chars($_string, $vc[0], $pos + 1, $etpos) )
					{
						// nein! ungültige Zeichen: nächste such-pos pos von tripple[1]
						#print $_string."\n".substr($_string,$pos+1)."\n";
						#print $vc[0]."\n";
						#exit;

						$lpos = $etpos;

						$etpos = false;

					}
				}

				if( isNotFalse($etpos) )
				{
					// suche nach dem tripple[2]

					$epos = strpos($_string, $ret["tripple"][2], $etpos + 1);

					if( isNotFalse($epos) and strlen($vc[1]) )
					{
						// sind im rechten part nur gültige Zeichen?

						if(! self::range_valid_chars($_string, $vc[1], $etpos + 1, $epos) )
						{
							// ungültige zeichen gefunden: nächste such-pos pos von tripple[1]

							$lpos = $etpos;

							$etpos = false;

						}
					}
				}
				if( isNotFalse($etpos) )
				{
					// Suche war erfolgreich

					$ret["result"] = substr($_string, $pos + 1, $epos -$pos - 1);
					$ret["status"] = true;

					return $ret;
				}

				$lpos += 1;

			}else{

				break;

			}
		}

		return $ret;
	}

	public static function struct_addressheader($_headerline, $_fallback = "")
	{
		$ret = array(
			"header" => "",
			"utf8" => "",
			"email" => "",
			"email_hide" => true,
			"before8" => "",
			"after8" => "",
			"c_schema" => "",
			"c_org" => "",

			);

		$domok = "abcdefghijklmnopqrstuvwxyz0123456789.-";
		$locok = "+/' !#\$%&*=?^_`[{|}]~\":";
		$locnok = "+/'!#\$%&*=?^_`[{|}]~\":";
		$domok = $domok.mb_convert_case($domok, MB_CASE_UPPER, "utf-8");


		$vchars = array(
			0 => $domok.$locok,
			1 => $domok,
			2 => $domok.$locnok,
			);

		if( isNotFalse($pos = strpos($_headerline, ": ")) )
		{

			$ret["header"] = substr($_headerline, 0, $pos);

			$_headerline = substr($_headerline, $pos + 2);

		}

		$res = self::search_str($_headerline, "<@>", 0, $vchars);
		if( $res["status"] )
		{

			$ret["email_hide"] = false;

		}else{

			$res=self::_search_email($_headerline, array(0 => $vchars[2], 1 => $vchars[1]), 0);

			if( $res["status"] )
				$ret["email_hide"] = false;

		}


		// ist der String mime-code?
		if( isNotFalse($pos = strpos($_headerline, "=?")) )
		{
			//suche nach der Endeposition für den Zeichensatz

			if( isNotFalse($epos = strpos($_headerline, "?", $pos + 2)) )
			{
				// Länge des Zeichensatznamens

				$len = $epos - $pos - 2;

				// Zeichensatz speichern

				$ret["c_org"] = substr($_headerline, $pos + 2, $len);

				// nach Schema suchen

				if( substr($_headerline, $epos + 2, 1) == "?" )
				{
					// Schema speichern

					$ret["c_schema"] = substr($_headerline, $epos + 1, 1);
				}
			}
		}

		$res = imap_mime_header_decode($_headerline);

		//Schritt 1: Alle Objekte durchlaufen und nach utf8
		//umwandeln
		$out = "";

		foreach($res as $i => $v)
		{

			$cset = strtolower($v->charset);

			switch($cset)
			{
				case "utf-8":
					$t = $v->text;
					break;

				case "default":
					$t = utf8_encode($v->text);
					break;

				default:
					$t = iconv($v->charset, "utf-8", $v->text);
					break;

			}

			$out .= str_replace(array("\r", "\n", "\t"), array("", "", " "), $t);

		}

		$out = trim($out);
		//ist nun ein string utf-8

		$ret["utf8"] = $out;


		//suche und entferne "..." und (...)
		$bl = array();
		$pos = 0;

		while( isNotFalse($pos = strpos($out, "\"", $pos)) )
		{
			// abschliessendes " vorhanden ?

			if( isFalse($epos = strpos($out, "\"", $pos + 1)) or substr($out, $epos + 1, 1) == "@" )
			{

				$pos++;

			}else{

				$bl[$pos] = $epos;

				$pos = $epos + 1;

			}
		}

		while( isFalse($pos = strpos($out, "(", $pos)) )
		{
			// abschliessendes " vorhanden ?
			if( isFalse($epos = strpos($out, ")", $pos+1)) )
			{

				$pos++;

			}else{

				$bl[$pos] = $epos;
				$pos = $epos + 1;

			}
		}

		$bl[strlen($out)] = strlen($out);

		asort($bl);

		$spos = 0;
		$foundpos = false;

		foreach($bl as $pos => $epos)
		{

			if( $pos - $spos > 0 )
			{

				$sstring = substr($out, $spos, $pos - $spos);
				#print "suche von $spos bis $pos: '".$sstring."'\n";

				$res = self::search_str($sstring, "<@>", 0, $vchars);

				if(! $res["status"] )
				{

					$res = self::search_str($sstring, "<@ ", 0, $vchars);

				}

				if( $res["status"] )
				{

					$ret["email"] = $res["result"];

					$foundpos = $spos + strpos($sstring, $ret["email"]);

					break;

				}
			}
			$spos = $epos + 1;
		}

		if( isNotFalse($foundpos))
		{

			$ret["before8"] = trim(substr($out, 0, $foundpos - 1));
			$ret["after8"] = trim(substr($out, $foundpos + 2 +strlen($ret["email"])));

		}else{

			$ret=array_merge($ret, self::_search_email($out, array(0 => $vchars[2], 1 => $vchars[1]), 0));
			/*// Nadel im Heuhaufen suchen
			$spos=0;
			while(($pos=strpos($out,"@",$spos))!==false)
			{
				$foundpos=false;
				for($i=$pos-1;$i>=0;$i--)
				{
					#print "test: ".substr($out,$i,1)."\n";
					if(strpos($vchars[2],substr($out,$i,1))===false)
					{
						break;
					}else{
						$foundpos=$i;
					}
				}
				#print "gefunden: '".substr($out,$foundpos)."' (".$foundpos.")\n";
				if($foundpos!==false)
				{
					for($i=$pos+1;$i<strlen($out);$i++)
					{
						#print "test: ".substr($out,$i,1)."\n";
						if(strpos($vchars[1],substr($out,$i,1))===false)
						{
							break;
						}
					}
					if($i>$pos+1)
					{
						$ret["email"]=substr($out,$foundpos,$i-$foundpos);
						$ret["before8"]=trim(substr($out,0,$foundpos));
						$ret["after8"]=trim(substr($out,$foundpos+strlen($ret["email"])));
						break;
					}
				}
				$spos=$pos+1;
			}*/
		}

		if( strlen($ret["email"]) == 0 )
		{
			if( strlen($ret["before8"]) == 0 )
			{

				$ret["before8"] = $ret["utf8"];

			}

			$ret["email"] = trim($_fallback);

			if( strlen($ret["email"]) == 0 )
			{

				$ret["email"] = "unkn@wn";

			}
		}

		return $ret;
	}

	public function _search_email($_string, $_validchars = array(), $_startpos = 0)
	{
		// Nadel im Heuhaufen suchen
		$ret = array(
			"status" => false,
			"email" => "",
			"before8" => "",
			"after8" => "",
			);

		// Falsche Parameter
		if(! is_array($_validchars) or !isset($_validchars[0]) or !isset($_validchars[1]) )
			return($ret);

		$vchars = array(
			1 => $_validchars[1],
			2 => $_validchars[0],
			);

		$spos=(int) $_startpos;

		while( isNotFalse($pos = strpos($_string, "@", $spos)) )
		{

			$foundpos = false;

			for($i = $pos - 1; $i >= 0; $i--)
			{
				#print "test: ".substr($out,$i,1)."\n";

				if( isFalse(strpos($vchars[2], substr($_string, $i, 1))) )
				{

					break;

				}else{

					$foundpos = $i;

				}
			}
			#print "gefunden: '".substr($out,$foundpos)."' (".$foundpos.")\n";

			if( isNotFalse($foundpos) )
			{

				for($i = $pos + 1; $i < strlen($_string); $i++)
				{
					#print "test: ".substr($out,$i,1)."\n";

					if( isFalse(strpos($vchars[1], substr($_string, $i, 1))) )
					{

						break;

					}
				}

				if( $i > $pos + 1 )
				{
					$ret["status"] = true;

					$ret["email"] = substr($_string, $foundpos, $i - $foundpos);

					$ret["before8"] = trim(substr($_string, 0, $foundpos));

					$ret["after8"] = trim(substr($_string, $foundpos + strlen($ret["email"])));

					break;
				}
			}

			$spos = $pos + 1;

		}

		return $ret;
	}
}


?>