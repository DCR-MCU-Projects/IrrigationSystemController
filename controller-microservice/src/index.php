<?php

	$controller = new RESTApiController();

	$controller->registerEndpoint(RESTApiController::GET, "/info", function() {
		echo "HELLO!";
	});

	print_r($_SERVER);

	$controller->parse();
	
	class RESTApiController
	{

		const GET = 0;
		const POST = 1;
		const PUT = 2;

		private $endPointCollection = [];

		function __construct()
		{

		}

		public function registerEndpoint($method, $endpoint, $callbackFunction)
		{
			$this->endPointCollection[$method] = ['endpoint' => $endpoint, 'callback' => $callbackFunction];
		}

		public function parse($query)
		{

		}

	}


?>