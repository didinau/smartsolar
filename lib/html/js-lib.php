<?php

//
//	Bereitstellung von JS-Code (anwendungsspezifisch)
//

class sJSlib
{

	function js_jssinglegrid()
	{
		return "
			<script type='text/javascript'>
				google.charts.load('current', { 'packages': ['gauge', 'corechart', 'line', 'bar'], 'language': 'de' }).then( function () { single_grid(); } );
			</script>
		";
	}

	function jsconfig()
	{
		$_conf=$this->CONF["conf"];

		foreach(array("thermo","batteries","generators","inverters") as $d)
		{
			if( isset($_conf[$d]) )
			{
				foreach($_conf[$d] as $idx=>$data)
				{
					if( isset($data["options"]) )
						unset($_conf[$d][$idx]["options"]);

					if( isset($data["param"]) )
						unset($_conf[$d][$idx]["param"]);

					if( isset($data["data"]) )
						unset($_conf[$d][$idx]["data"]);
				}
			}
		}
		return mf::php2js2($_conf,"conf");
	}
}

?>