<?PHP
/*
	Class Name:		Database
	Class Version:	2.2
	Last Modify:	Octtober 20 2014
	
	Author:			David Cuerrier
	Contact:		david.cuerrier@gmail.com
	
	Description:	This class has been desing to be reuse by any personal project that use
					mysql database has a base class  (abstract) that can be  extend in your 
					project.  It's contain 4 main function, connect, disconnect and querys.
					
	History:		1.0		March  8th 2012		Original Code
					2.0		April 10th 2013		Refresh code and add log part. Since this 
												version, all function use rawQuery one way 
												or an other.
					2.1		April 22th 2013		Put back RAW query as a public function.
					2.2		Octtober 20 2014	Add protected function visibility
	
*/

error_reporting(E_ALL & ~E_NOTICE & ~E_WARNING & ~E_DEPRECATED);

// Set true or false if you want to log every query into log file.
define("QUERY_LOG", true);
define("QUERY_LOG_FILE", "databaseQuery.log");

/***
	@ClassName: 	Database	
	@Description:	This class has been desing to be reuse by any personal project that use mysql database has a base class (abstract) that can be  extend in your project.  It's contain 4 main function, connect, disconnect and querys.
	@ClassVersion:	1.0
***/

abstract class Database extends PDO {
	
	/***
		@FunctionName:	__construct
		@Description:	Initialise the class with empty var.
	***/	
	function __construct($configFile) {
		
		$config = simplexml_load_file($configFile);	
		parent::__construct("mysql:dbname={$config->database};host={$config->server}", $config->username, $config->password);
	}

	/***
		@FunctionName:	noReturnQuery
		@Description:	Make a SQL query that do not return a result.
		@Param:			$query	string	[A SQL query string to be execute]
	***/
	protected function noReturnQuery($query) {		
				
		$this->rawQuery($query);

		if ($this->errorCode() != "0000")
			throw new Exception('Error with the last query of MySQL '. print_r($this->errorInfo(), true), 500);
	}

	/***
		@FunctionName:	selectQuery
		@Description:	Make a SQL query that return a array of result Array(row, field)
		@Param:			$query	string	[A SQL query string to be execute]
						$includeFieldName	bool	[Indicate whatever we want the field name include as the first record]
						$mysqlResultType	MYSQL_ASSOC, MYSQL_NUM, MYSQL_BOTH.
		@Return:		Array[Row, Filed]
	***/
	protected function selectQuery($query, $mysqlResultType = MYSQL_ASSOC) {
		
		$return = array();
		$result = $this->rawQuery($query);

		foreach($result as $line)
			$return[] = $line;

		if ($this->errorCode() != "0000")
			throw new Exception('Error with the last query of MySQL'. print_r($this->errorInfo(), true), 500);

		return $return;
	}
	
	/***
		@FunctionName:	insertQuery
		@Description:	Make a SQL insert query that return the last incremented ID.
		@Param:			$query	string	[A SQL insert query string to be execute]
		@Return:		int Last inserted ID.
	***/
	protected function insertQuery($query) {	

		$this->rawQuery($query);		
		
		if ($this->errorCode() != "0000")
			throw new Exception('Error with the last query of MySQL'. print_r($this->errorInfo(), true), 500);
			
		return $this->lastInsertId();
	}

	/***
		@FunctionName:	execPreparedQuery
		@Description:	
		@Param:			
		@Return:		
	***/
	protected function execPreparedQuery($query, $values) {	

		$this->writeLog($query);
		$this->writeLog(print_r($values, true));
	
		$s = $this->prepare($query);
		
		if (is_array($values[0])) {
			foreach ($values as $val)
				$err = $s->execute($val);
		} else
			$err = $s->execute($values);
		
		if ($this->errorCode() != "0000")
			throw new Exception('Error with the last query of MySQL'. print_r($this->errorInfo(), true), 500);		
		
		if (strtoupper(explode(" ", $query)[0]) == "INSERT")
			return $this->lastInsertId();
		else
			true;
	}
	
	/***
		@FunctionName:	insertQuery
		@Description:	Make a SQL insert query that return the last incremented ID.
		@Param:			$query	string	[A SQL insert query string to be execute]
		@Return:		int Last inserted ID.
	***/
	protected function updatePreparedQuery($query) {	

		$this->rawQuery($query);		
		
		if ($this->errorCode() != "0000")
			throw new Exception('Error with the last query of MySQL'. print_r($this->errorInfo(), true), 500);
			
		return $this->lastInsertId();
	}
	
	/***
		@FunctionName:	rawQuery
		@Description:	Run a raw query by using the ressource of this class.
		@Param:			$query	string	[A SQL query string to be execute]
	***/	
	protected function rawQuery($query) {
				
		$query = str_replace(array("\t", "\r\n"), " ", $query);
		
		$this->writeLog($query);
				
		return $this->query($query);
	}

	private function writeLog($text) {
		if (QUERY_LOG)
			file_put_contents(QUERY_LOG_FILE, date("d-m-o H:i:s", time())."\t\t".$_SERVER['PHP_SELF']."\t".$text."\n", FILE_APPEND);
	}
	
}

?>