<?php

	class JSONHandlebars {

		private $tokenCollection = [];

		function __construct() {

		}

		public function addToken($name, $value) {
			$this->tokenCollection[$name] = $value;
		}

		public function loadJSONFile($filename) {
			$tmp = file_get_contents($filename);

			foreach ($tokenCollection as $token => $value) {
				$tmp = str_replace($token, $value, $tmp);
			}

			return json_decode($tmp);

		}

	}

?>