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

class tpl extends tpl_static
{

	public $Path = null;

	public $data = "";

	public $predef_array = [];

	public $predef_raw = [];

	public $addComments = true;



	public function __construct($_path = null, $_addComments = true)
	{

		$this->Path = $_path;

		if( isFalse($_addComments) ) $this->addComments = false;

	}

	public function NewStage()
	{

		return new tplStage($this);

	}

	public function setPredefs(array $_array): void
	{

		$this->predef_array = $_array;

	}

	public function setPredefEntry(string $_key, string $_value): void
	{

		$this->predef_array[$_key] = $_value;

	}

	public function setPredefRaw(string $_key, string $_value): void
	{

		$this->predef_raw[$_key] = $_value;

	}

	public function getPredefs(): array
	{

		return $this->predef_array;

	}

	// Template-datei anhängen
	public function addTpl(string $_filename): bool
	{

		if(! is_null($this->Path) )

			$fn = $this->Path."/".$_filename;

		else

			$fn = $_filename;


		$this->data .= $this->tpl_load($fn, $this->addComments);

		return true;

	}

	// Ersetzte Template-datei anhängen
	public function addTranslatedTpl(string $_filename, array $_data, array $_raw = []): bool
	{

		$this->data .= $this->translateTpl ($_filename, $_data, $_raw);

		return true;

	}

	// Daten anhängen
	public function addToTpl(string $_data): bool
	{

		$this->data .= $_data;

		return true;

	}

	public function getPath(): string
	{

		return $this->Path;

	}

	// Ausgabe des Großen Ganzen
	public function finalOut(array $_data = []): string
	{

		$_data["_time_"] = time();

		return $this->tpl_replace($this->data, array_merge($this->predef_array, $_data));

	}

	// Zurücksetzen
	public function clear(): void
	{

		$this->data = "";

		$this->predef_array = [];

		$this->predef_raw = [];

	}

	// Template einlesen, ersetzen und Ergebnis zurück geben
	public function translateTpl (string $_filename, array $_data, array $_raw = []): string
	{

		if(! is_null($this->Path) )

			$fn = $this->Path."/".$_filename;

		else

			$fn = $_filename;

		return tpl::tpl_translate($fn, array_merge($this->predef_array, $_data), array_merge($this->predef_raw, $_raw), $this->addComments);
	}

	public function compileTpl(array $_data, array $_raw = [])
	{
		$_data["_time_"] = time();



	}

}

class tpl_static
{
	public static function html_enc()
	{
		return function($_value)
			{
				return htmlspecialchars($_value, ENT_NOQUOTES, 'UTF-8');
			};
	}

	public static function indexArray2replaceArray(array &$_array, string $_pre = "", string $_suf = ""): array
	{

		$ret = array();

		foreach($_array as $idx => $data)
		{

			$ret[ $_pre.$idx.$_suf ] = $data;

		}

		return $ret;

	}

	public static function tpl_replace(string $_tpl, array $_data, array $_raw = []): string
	{

		if( count($_raw) ) $_tpl = strtr($_tpl, $_raw );

		return strtr($_tpl, array_map( self::html_enc(), $_data ));

	}

	public static function tpl_translate (string $_filename, array $_data, array $_raw = [], bool $_addComment = false): string
	{

		$tpl = self::tpl_load($_filename, $_addComment);

		return self::tpl_replace($tpl, $_data, $_raw);

	}

	public static function tpl_load (string $_filename, bool $_addComment = false): string
	{

		if( $_addComment )
		{
			$commentb = "<!-- begin tpl:".basename($_filename)." -->\n";
			$commente = "\n<!-- end tpl -->\n";
		}else{
			$commentb = "";
			$commente = "";
		}


		if(! $data = file_get_contents($_filename) )
			throw new Exception("Die Vorlage ".$_filename." konnte nicht gelesen werden!");

		return $commentb . trim($data) . $commente;

	}

}


class tplStage extends tpl_static
{

	private $stage = "";

	private $form = "";

	private $parent = false;


	function __construct(&$_parent)
	{

		$this->parent = &$_parent;

	}


	public function NewStage()
	{

		return new tplStage($this);

	}

	public function stageFormLoad(string $_filename): void
	{

		if(! is_null($this->parent->Path) )

			$fn = $this->parent->Path."/".$_filename;

		else

			$fn = $_filename;


		$this->form = $this->tpl_load($fn, $this->parent->addComments);

	}

	// Stage-Form übersetzen
	public function stageFormCompile(array $_data, array $_raw = []): void
	{

		$_data["_time_"] = time();

		$this->stage .= tpl::tpl_replace($this->form, array_merge($this->parent->predef_array, $_data), array_merge($this->parent->predef_raw, $_raw));

	}

	public function stageAddData(string $_string): void
	{

		$this->stage .= $_string;

	}

	// Stage-File einlesen
	public function stageAdd(string $_filename): void
	{


		if(! is_null($this->parent->Path) )

			$fn = $this->parent->Path."/".$_filename;

		else

			$fn = $_filename;


		$this->stage .= $this->tpl_load($fn, $this->parent->addComments);

	}

	// Stage-File übersetzen
	public function stageCompile(array $_data, array $_raw = []): void
	{

		$_data["_time_"] = time();

		$this->stage = tpl::tpl_replace($this->stage, array_merge($this->parent->predef_array, $_data), array_merge($this->parent->predef_raw, $_raw));

	}

	// Stage-File abschließen
	public function stageMigrate(): void
	{

		if( isset($this->parent->data) )

			$this->parent->data .= $this->stage;

		else

			$this->parent->stage .= $this->stage;

		$this->stage = "";

	}

	// Stage Clear
	public function stageClear(): void
	{
		$this->stage = "";
		$this->form = "";
	}

}

?>