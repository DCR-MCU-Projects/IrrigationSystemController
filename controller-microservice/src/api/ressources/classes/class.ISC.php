<?php

class ISC {

	private $BASE_URI;

	function __construct($host, $port, $username = "", $password = "") {
		$this->host = $host;
		$this->port = $port;
		$this->BASE_URI = sprintf("%s://%s:%d", "http", $host, $port);
	}

	function startZone($zoneId) {
		$data = $this->HTTP_POST("/zone/stop?id=$zoneId");
		sleep(10);
		$data = $this->HTTP_POST("/zone/start?id=$zoneId");
		return json_decode($data[1]);
	}

	function stopZone($zoneId) {
		$data = $this->HTTP_POST("/zone/stop?id=$zoneId");
		return json_decode($data[1]);
	}

	function getInfo() {
		$data = $this->HTTP_GET("/stats");
		return json_decode($data[1]);
	}

	
	
	private function HTTP_GET($endpoint, $manage_error = true) {
		
		$d = $this->HTTP_QUERY($this->BASE_URI . $endpoint, "GET");
		
		if ($manage_error)
			if ($d[0] >= 400)
				throw new Exception("The request return an error.", $d[0]);
		
		return $d;
		
	}
	private function HTTP_PUT($endpoint, $json_payload = "{}", $manage_error = true) {
		
		$d = $this->HTTP_QUERY($this->BASE_URI . $endpoint, "PUT", $json_payload);		
		
		if ($manage_error)
			if ($d[0] >= 400)
				throw new Exception("The request return an error.", $d[0]);
		
		return $d;
		
	}
	private function HTTP_POST($endpoint, $json_payload = "{}", $manage_error = true) {
		
		$d = $this->HTTP_QUERY($this->BASE_URI . $endpoint, "POST", $json_payload);		
		
		if ($manage_error)
			if ($d[0] >= 400)
				throw new Exception("The request return an error. [".$d[1]."]", $d[0]);
		
		return $d;
		
	}
	private function HTTP_QUERY($uri, $method, $payload = "") {
		$ch = curl_init();

		$headers = array(
		   "Content-Type: application/json",
		   "Accept: application/json",
		);
		curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);


		curl_setopt($ch, CURLOPT_URL, $uri);
		curl_setopt($ch, CURLOPT_HEADER, 0);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
		
		if ($method == "PUT") {
			curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'PUT');
			curl_setopt($ch, CURLOPT_POSTFIELDS, $payload);
		} else if ($method == "POST") {
			curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'POST');
			curl_setopt($ch, CURLOPT_POSTFIELDS, $payload);
		}
		
		$data = curl_exec($ch);
		$return_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
		curl_close($ch);
		
		return [$return_code, $data];
	}


}

?>