<?PHP

// Version : 2.0 	+ 3 juin 2015
// Version : 2.1 	+ Many change, including fileupload
// Version : 2.2 	+ Recast of the HELP page
// Version : 2.3 	+ Add the concept of X-API-RequestID in Headers and RequestURI.
//					+ Modification of the return object splitting the RequestData and DataData
//					+ Add the concept of X-API-DEBUG to run in debug mode and receive all global error.


	error_reporting(E_ALL);
	//error_reporting(0);
	//error_reporting(E_ALL & ~E_NOTICE & E_WARNING & ~E_DEPRECATED);		
	
	
	header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
	header("Cache-Control: post-check=0, pre-check=0", false);
	header("Pragma: no-cache");
	header("Content-type: application/json; charset=UTF-8");
	
	try {

		include_once "classes/class.xlog.php";
		
		define("FILE_CLASS_DEFINITION", "config/config.webapi.xml");
		define("CLASSES_FOLDER", "ressources/");
		
		$log = new XLog(null, "RestAPI_PACOMessageBoard");
		
		// Populate local variables
		$request_headers = getallheaders();
		$request_uri = "/".$_GET['URI'];
		$request_method = $_SERVER['REQUEST_METHOD'];
		$request_body = file_get_contents("php://input");
		$file_upload = false;

		if (isset($request_headers['X-API-DEBUG']))
			error_reporting(E_ALL);
		
		// Before doing anything, check that the user want to communicate using JSON only. If not, throw an exception accordantly.
			// Check if method = GET and ContentType = application/json
		if (($request_method != "GET") && ($_SERVER['CONTENT_TYPE'] != "application/json"))
			if (($request_method == "POST") && (strpos($_SERVER['CONTENT_TYPE'],"multipart/form-data") !== false))
				$file_upload = true;
			else
			throw new Exception("This WebAPI can only exchange data in JSON type (application/json), you are using: ".$_SERVER['CONTENT_TYPE']." please use Content-Type header accordantly.", 415);
			// Check if ACCEPT application/json or */*
		if ((strpos($_SERVER['HTTP_ACCEPT'], "application/json") === false) && (strpos($_SERVER['HTTP_ACCEPT'], "*/*") === false))
			throw new Exception("This WebAPI can only exchange data in JSON type (application/json), please use Accept header accordantly.", 406);		

		$log->WriteEntry("New request $request_uri");
		
		if (isset($request_headers["X-API-SID"])) {
			include_once "/classes/class.session.php";
			$session = new Session($request_headers["X-API-SID"]);
		}
	
		// Load the authorized functions details
		$config = simplexml_load_file(FILE_CLASS_DEFINITION);

		$class_filename_list = initClassArrayFromConfig($config);
		$func_info = findURIFromConfig($config, $request_method, $request_uri);

		if (!$func_info) 
			throw new Exception("The function was not found for $request_uri", 404);

		//$log->WriteEntry("Fnc found: " . print_r($func_info, 1));
		
		$tmp_class_name = (string)$func_info['node']['class'];
		$tmp_func_name = (string)$func_info['node']['func'];

		if (!isset($class_filename_list[$tmp_class_name]))
			throw new Exception("Class '$tmp_class_name' that it is use by this function is not found.", 404);

			
		if ($file_upload) {
			
			require_once CLASSES_FOLDER.$class_filename_list[$tmp_class_name];
			
			$tmp = new $tmp_class_name;			
			$ret = call_user_func(array($tmp, $tmp_func_name), array_merge($_FILES, $_REQUEST));
			
		} 
		else {
			$request_body_json = json_decode($request_body, true);
			
			if ($request_body != "" && $request_body_json == null)
				throw new Exception("The JSON object can not be decode", 409);
			
			require_once CLASSES_FOLDER.$class_filename_list[$tmp_class_name];

			$args = is_array($request_body_json) ? array_merge($func_info['var'], $request_body_json) : $func_info['var'];
			
			$tmp = new $tmp_class_name;
			
			$log->WriteEntry(sprintf("Calling: Class: %s; Function: %s; Args %s", $tmp_class_name, $tmp_func_name, print_r($args, true)));
			
			try {
				$ret['return_data'] = call_user_func(array($tmp, $tmp_func_name), $args);
			} catch (Exception $ex) {
				throw $ex;
				//throw new Exception("The function ($tmp_func_name) configure in the API isn't part of the class ($tmp_class_name). (".$ex->getMessage().")", 404);
			}
			
		}
		
		$log->WriteEntry(print_r($ret, true));
		
		
		$ret['request_data']['uri'] = (string)$request_uri;
		$ret['request_data']['method'] = (string)$request_method;
		$ret['request_data']['file_upload'] = (bool)$file_upload;
		if (isset($request_headers['X-API-DEBUG']))
			$ret['request_data']['running_debug_mode'] = (bool)true;
		$ret['request_data']['id'] = (isset($request_headers["X-API-RequestID"])) ? $request_headers["X-API-RequestID"] : 0;
		
		if ($request_method == "POST")
			http_response_code(201);
		else {
			if ((ob_get_length() <= 0) && ($ret == "" || $ret == null))
				http_response_code(204);
			else
				http_response_code(200);
		}
		
		
	} catch (Exception $e) {		
		http_response_code($e->getCode());
		$ret = array("ErrorMessage" => $e->getMessage());
		$log->WriteEntry(sprintf("Error running function %s from class %s. See server log for more details.", $tmp_func_name, $tmp_class_name), $e->getCode());
		
		while ($e = $e->getPrevious()) {
			$log->WriteEntry(sprintf("\t=> %s %s %s", $e->getFile(), $e->getLine(), $e->getMessage()), $e->getCode());
		}
	}
	
	if ($ret != "" && $ret != null)		
		print(json_encode($ret));
	

	function initClassArrayFromConfig($config) {
		$class = array();
		$xml_class = $config->xpath("//classes/class");
		foreach ($xml_class as $xc)
			$class[(string)$xc['name']] = (string)$xc['filename'];
		
		return $class;
	}

	// Check if the uri match any function stored in the config file. If true return the function 
	// structure including class file and any relevent informations.
	function findURIFromConfig($config, $method, $uri) {	
		foreach($config->xpath("//function/action[@method=\"$method\"]") as $c) {
			$selectedURI = (string)$c->xpath("..")[0]['uri'];
			
			$pattern = preg_replace("/\{([^\}]*)\}/i", "(?'\\1'[^/]+)", $selectedURI);

			$matchs = array();
			if (@preg_match("|^".$pattern."$|im", $uri, $matchs) > 0) {				
				return array("node" => $c, "var" => $matchs);
			}
			
		}
		return false;
	}


	
	/* Function: includeClassFileFromConfig($classFolder, $config)
	Description: Make a include_once for each file entry in the config file.
	Return:	void
	*/
	function includeClassFileFromConfig($classFolder, $config) {
		foreach($config->class as $c)
			include_once($classFolder.$c['file']);	
	}

	
	/* Function: findFunctionFromConfig($functionName, $config)
	Description: Look for a given function name in all class from config file.
	Return:	Class Name that containe the function or FALSE if nothing found.
	*/
	function findFunctionFromConfig($functionName, $config) {
		foreach($config->uricollection->uri as $c) {
			if (in_array($functionName, get_class_methods((string)$c['name'])))
			return (string)$c['name'];
		}
		return false;
	}

	function checkIfClassAndFunctionExists($class, $function) {
		if (class_exists($class))
			if (method_exists($class, $function))
				return [true, "all"];
			else
				return [false, "function"];
			
		return [false, "class"];
	}

	
?>