<?PHP

require_once("classes/class.ISC.php");

class IrrigationSystemControl {
	
	private $ISC;

	function __construct() {
		$this->ISC = new ISC("192.168.1.173", 80);
		//parent::__construct("ressources/config/config.database.xml");

	}

	function performAction($var) {
		if ($var['action'] == "start") {
			return $this->ISC->startZone($var['zoneId']);
		}
		else if ($var['action'] == "stop") {
			return $this->ISC->stopZone($var['zoneId']);
		} else {
			throw new Exception("Action not found", 409);
		}
	}	

	function getStats() {
		return $this->ISC->getInfo();
	}
	
}
