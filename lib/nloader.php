<?php

$include = __DIR__."/small-fw.inc";

if( is_readable($include) and (include $include ) !== true )
{

	error_log("Error including necessary files!");

	sleep(20);

	exit(1);

}


//*D*E*C*L*A*R*E*//


class ProjectIncluderConfig
{

	public static $SearchDirectories = array(".", "../bin");

	public static $FileExtensions = array("php", "inc");

	public static $dataStorage = __dir__."/nloader.json";	public static $suitableObjects = array("class","interface","trait");

	public static $essential=array(

		__dir__."/config.inc",

		__dir__."/classes/c-smart/smart_func.inc",

		__dir__."/sensors/sensor.php",

		);
}

class ProjectIncluder
{
	public static $AllClasses;

	public static $LearnedIncludes;

	public const CSEARCHER = __dir__."/nloader.2nd";

	public static function RequestForInclude($_class)
	{

		if(! isset(self::$AllClasses[$_class]) )
			getClasses();

		if( isset(self::$AllClasses[$_class]) )
		{

			$includeFile = __dir__ . "/" . self::$AllClasses[$_class];

			if( is_readable($includeFile) )
			{

				require_once $includeFile;

			}else{

				new LogMsg(0, __class__."::".__function__, "File ".$includeFile." not found or not readable!");

				exit(1);
			}

		}else{

			new LogMsg(0, __class__."::".__function__, "Class ".$_class." not found!");

			exit(1);

		}
	}
}

function getClasses()
{

	try
	{
		if( is_readable(ProjectIncluder::CSEARCHER) )
		{

			require_once ProjectIncluder::CSEARCHER;

			$worker = new ClassSearch();

			ProjectIncluder::$AllClasses = $worker->find_classes(__dir__."/", ProjectIncluderConfig::$SearchDirectories, ProjectIncluderConfig::$FileExtensions);

			if( is_writeable(ProjectIncluderConfig::$dataStorage) or is_writeable(__dir__) )
			{
				file_put_contents(ProjectIncluderConfig::$dataStorage, $worker->get_json(ProjectIncluder::$AllClasses));
			}
		}
	}catch( throwable $e){

			new ErrorAndMore($e, 0 ,__class__."::".__function__);

	}

}


//*E*X*E*C*//


if( file_exists(ProjectIncluderConfig::$dataStorage) and is_readable(ProjectIncluderConfig::$dataStorage) )
{

	$test = json_decode(file_get_contents(ProjectIncluderConfig::$dataStorage));

	if( is_array($test) and count($test) )
	{

		ProjectIncluder::$AllClasses = $test;

	}

}else{

	getClasses();
}


spl_autoload_register( array('ProjectIncluder', 'RequestForInclude') );


if( is_array(ProjectIncluderConfig::$essential) and count(ProjectIncluderConfig::$essential) )
{

	foreach(ProjectIncluderConfig::$essential as $file)
	{

		if( is_readable($file) )
		{

			require_once $file;

		}else{

			error_log("include essential file ".$file.": not found!");

		}
	}
}


?>