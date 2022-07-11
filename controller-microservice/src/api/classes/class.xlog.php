<?PHP
/*
	Class Name:		LOG
	Class Version:	1.0
	Last Modify:	December 28th 2013
	
	Author:			David Cuerrier
	Contact:		david.cuerrier@gmail.com
	
	Description:	
					
					
					
	History:		1.0		March  8th 2012		Original Code
	Add interaction with php.ini
	
					
*/

//error_reporting(E_ERROR | E_WARNING | E_PARSE);

// class XLog {
// 	function __construct($logpath = null, $logname = null, $include_defaultpath = true) {}

// 	function WriteEntry($text, $errno = 0) {
// 		fwrite(STDOUT, $text);
// 	}
// }

class XLog {

	private $_logname;
	private $_logpath;
	
	function __construct() {
		$this->_logpath = "php://stdout";
		$this->_logname = $this->GetOriginClassName();
	}

	/***
		@FunctionName:	__construct
		@Description:	Constructor of log object. This function will set logname and logpath vairables
						used to create the log file.
		@Param:			$logpath	string	[Path to the log folder. If null, ROOT_FOLDER + logs folder will be use.]
						$logname	string	[Name of the log to log into. If null, name of the caller script will be use.]
	***/
	// function __construct($logpath = null, $logname = null, $include_defaultpath = true) {
		
	// 	$defaultpath = $this->GetDefaultLogPath();
		
	// 	if (!$defaultpath)
	// 		die("Error - Default log path hasn't been set in php.ini section XLog key DefaultLogFolder.");
				
		
	// 	// Check for defaulpath, if not exist create it.
	// 	if ($logpath == null) {
	// 		$this->_logpath = $defaultpath;
	// 	} else {
	// 		if ($include_defaultpath)
	// 			$this->_logpath = str_replace("//", "/", str_replace("\\", "/", $defaultpath . "/" . $logpath));
	// 		else
	// 			$this->_logpath = str_replace("//", "/", str_replace("\\", "/", $logpath));
	// 	}
		
	// 	if ($logname == null)
	// 		$this->_logname = $this->GetOriginClassName().".xlog";
	// 	else
	// 		$this->_logname = $logname.".xlog";

	// 	$this->_logname = strtoupper($this->_logname);
		
	// 	$dirinfo = pathinfo($this->_logpath."/".$this->_logname);
	// 	@mkdir($dirinfo['dirname'], 0777, true);
				
		
	// }

	/***
		@FunctionName:	WriteEntry
		@Description:	Write en entry into the log file
		@Param:			$text	string	[Date sent to the log file.]
						$errno	int		[Error number, if not specified, the NA tag will be use as logtype.]
	***/
	function WriteEntry($text, $errno = 0) {

		$e = "[".str_pad($errno, 6, " ", STR_PAD_LEFT)."] ".$text;
		
		if ($errno == 0)
			$this->LogRaw($e, "N/A");
		else	
			$this->LogRaw($e, "ERR");
	}
	
	private function GetDefaultLogPath() {	
		$ini = parse_ini_file(php_ini_loaded_file(), true);
		try {
			return $ini['XLog']['DefaultLogFolder'];
		} catch (Exception $ex) {
			return false;
		}
	}
	
	private function LogRaw($text, $type = "NA") {				
		//file_put_contents($this->_logpath."/".$this->_logname, $this->FormatLog($text, $type), FILE_APPEND);
		file_put_contents($this->_logpath, $this->FormatLog($this->_logname . " -> " . $text, $type), FILE_APPEND);
	}

	private function FormatLog($text, $type = "NA") {
		return date("d-m-o H:i:s", time())."\t\t".str_pad($_SERVER['PHP_SELF'], 30)."\t".str_pad($type, 6)."\t".$text."\n";
	}

	private function GetOriginClassName() {
		$debugInfo = debug_backtrace();
		$className = $debugInfo[count($debugInfo)-1]['class'];
		
		if ($className == "Log")		
				return "default";
		return $debugInfo[count($debugInfo)-1]['class'];
	}
	
}

?>