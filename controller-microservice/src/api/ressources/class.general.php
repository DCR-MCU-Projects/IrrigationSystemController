<?PHP	

class General {
	function ShowVisualHomeScreen() {
		include_once "help.webapi.php";		
	}
	
	function GetAPIInfo() {
		$config = simplexml_load_file(FILE_CLASS_DEFINITION);
		$return['name'] = (string)$config->xpath("/webapi/information/title")[0];
		$return['version'] = (string)$config->xpath("/webapi/information/version")[0];
		$return['build'] = (string)$config->xpath("/webapi/information/build")[0];		
		//$return['returncodes'] = $config->xpath("/webapi/information/returncodes/return");
		$return['notes'] = str_replace("\t", "", (string)$config->xpath("/webapi/information/notes")[0]);
				
		return $return;	
	}
	function GetAPIVersion() {
		$config = simplexml_load_file(FILE_CLASS_DEFINITION);		
		$return['version'] = (string)$config->xpath("/webapi/information/version")[0];
		$return['build'] = (string)$config->xpath("/webapi/information/build")[0];		
				
		return $return;
	}
}

?>