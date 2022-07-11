<?PHP

/*

TODO;
	Add information on the header options like X-API-DEBUG X-API-RequestID

*/

	define("DEBUG", false);
	error_reporting(E_ALL);
	header("Content-type: text/html; charset=UTF-8");
// phpinfo();
// exit();
	//define("FILE_CLASS_DEFINITION", 	"config/config.webapi.xml");
	$config = simplexml_load_file(FILE_CLASS_DEFINITION);
	
	
	// Include all classes so we can see if function existe (FOR DEBUG ONLY)
	if (DEBUG) {
		$class_filename_list = initClassArrayFromConfig($config);
		foreach($class_filename_list as $k => $v) {
			if (!class_exists($k))
				require_once(CLASSES_FOLDER.$v);
		}
	}
	
	$title = (string)$config->xpath("/webapi/information/title")[0];
	$version = (string)$config->xpath("/webapi/information/version")[0];
	$build = (string)$config->xpath("/webapi/information/build")[0];
	
	$return_codes = $config->xpath("/webapi/information/returncodes/return");
	
	$notes = str_replace(array("\n", "\t"), array("<br/>", "&nbsp;&nbsp;&nbsp;&nbsp;"), (string)$config->xpath("/webapi/information/notes")[0]);
	
	$groups = $config->xpath("/webapi/functions/group");
	
	function p($txt, $args = null) {
		if ($args == null)
			echo $txt;
		else
			printf($txt, $args);
	}
	function d($txt) {print_r($txt);}
	
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
	<head>
		<?PHP echo "<title>$title</title>"; ?>
		<link rel="stylesheet" type="text/css" href="help.webapi.css" />
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta/css/bootstrap.min.css" integrity="sha384-/Y6pD6FV/Vv2HJnA6t+vslU6fwYXjCFtcEpHbNJ0lyAFsXTsjBbfaDjzALeQsN6M" crossorigin="anonymous">
		
		<script src="https://code.jquery.com/jquery-3.2.1.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>		
		<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.11.0/umd/popper.min.js" integrity="sha384-b/U6ypiBEHpOf/4+1nzFpr53nxSS+GLCkfwBdFNTxtclqqenISfwAzpKaMNFNmj4" crossorigin="anonymous"></script>		
		<script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta/js/bootstrap.min.js" integrity="sha384-h0AbiXch4ZDo7tp9hKZ4TsHbi047NrKGLO3SEJAg45jXxnGIfYzk4Si90RDIqNm1" crossorigin="anonymous"></script>
		
	</head>
	<body>		
		
		<!-- Modal -->
		<div class="modal fade" id="APIInformationModal" tabindex="-1" role="dialog" aria-labelledby="APIInformationModalLabel" aria-hidden="true">
		  <div class="modal-dialog" role="document">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="APIInformationModalLabel">API Information</h5>
				<button type="button" class="close" data-dismiss="modal" aria-label="Close">
				  <span aria-hidden="true">&times;</span>
				</button>
			  </div>
			  <div class="modal-body">
				  <?PHP 
					echo "Title: $title<br/>";
					echo "Version: $version<br/>";
					echo "Build: $build<br/>";
				  ?>
			  </div>
			  <div class="modal-footer">
				<button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
			  </div>
			</div>
		  </div>
		</div>

		<!-- Modal -->
		<div class="modal fade" id="APIAdditionalNoteModal" tabindex="-1" role="dialog" aria-labelledby="APIAdditionalNoteModalLabel" aria-hidden="true">
		  <div class="modal-dialog" role="document">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="APIAdditionalNoteModalLabel">API Additional Notes</h5>
				<button type="button" class="close" data-dismiss="modal" aria-label="Close">
				  <span aria-hidden="true">&times;</span>
				</button>
			  </div>
			  <div class="modal-body">
				<?PHP p($notes); ?>
			  </div>
			  <div class="modal-footer">
				<button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
			  </div>
			</div>
		  </div>
		</div>

		<!-- Modal -->
		<div class="modal fade" id="APIAvailableReturnCodeModal" tabindex="-1" role="dialog" aria-labelledby="APIAvailableReturnCodeModalLabel" aria-hidden="true">
		  <div class="modal-dialog modal-lg" role="document">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="APIAvailableReturnCodeModalLabel">API Available Return Code</h5>
				<button type="button" class="close" data-dismiss="modal" aria-label="Close">
				  <span aria-hidden="true">&times;</span>
				</button>
			  </div>
			  <div class="modal-body">
				<table class='table'>
					<thead>
						<tr>
							<th>Code</th>
							<th>Description</th>					
						</tr>
					</thead>
					<tbody>
						<?PHP
						foreach ($return_codes as $rc) {
						echo "<tr>
						<td>".$rc['code']."</td>
						<td>".$rc."</td>
						</tr>";
						}	
						?>
					</tbody>
				</table>
			  </div>
			  <div class="modal-footer">
				<button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
			  </div>
			</div>
		  </div>
		</div>
		
		<nav class="navbar navbar-expand-lg navbar-light bg-light sticky-top">
  		  <a class="navbar-brand" href="#">
			<img id='logo' src='data:image/png;base64,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' width="30" height="30" class="d-inline-block align-top" alt="">
			<?PHP p($title); ?>
		  </a>
		  <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
			<span class="navbar-toggler-icon"></span>
		  </button>

		  <div class="collapse navbar-collapse" id="navbarSupportedContent">
			<ul class="navbar-nav mr-auto">
			  <li class="nav-item">
				<a class="nav-link" href="#" data-toggle="modal" data-target="#APIInformationModal">API Information</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="#" data-toggle="modal" data-target="#APIAdditionalNoteModal">API Additional Notes</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="#" data-toggle="modal" data-target="#APIAvailableReturnCodeModal">API Available Return Code</a>
			  </li>
			</ul>
			<div class="form-inline my-2 my-lg-0">
			  <input id='search_field' class="form-control mr-sm-2" type="text" placeholder="Search" aria-label="Search" onkeyup="if (event.keyCode == 13) {searchOnPage();}">
			  <button class="btn btn-outline-secondary my-2 my-sm-0" type="button" onclick="searchOnPage();">Search</button>
			</div>
			
			<button class="btn btn-primary ml-3 my-2 my-sm-0" type="button" data-toggle='collapse' data-target='#testBox'>Open Test Console</button>
		  </div>
		</nav>
		
		<div class="container mt-5">
			<div class='card-columns'>
			<?PHP
				$g_index = 0;
				foreach ($groups as $g) {
					//echo "<div class='p-2'><a href='#G_".$g_index++."'>".$g['name']."</a></div>";
					
					$ob = ($g['obsolete'] == "true") ? "text-white text-secondary" : "";
					$btn_color = ($g['obsolete'] == "true") ? "secondary" : "primary";
					$warning = ($g['obsolete'] == "true") ? "<div class='card-footer bg-transparent border-light'><span class='font-weight-bold'>OBSOLETE</span> - Try not to use</div>" : "";
					
					
					printf("
						<div class='card $ob' style='width: 20rem;'>	  
						  <div class='card-body'>
							<h4 class='card-title'>%s</h4>
							<p class='card-text'>%s</p>
							<a href='%s' class='btn btn-$btn_color'>See Functions</a>
						  </div>
						  $warning
						</div>", $g['name'], $g['description'], "#G_".$g_index++);
				}
			?>		
			</div>
		</div>
		
		<div class="container mt-5">
	
	<?PHP
	
		function wrtieMethodBtnGroup($function_action) {
			$output = "";
			$output .= "<div class='btn-group' role='group' aria-label='Basic example'>";
			foreach ($function_action as $a) {
				$G = (string)$a['method'];
					$output .= "<button type='button' class='btn $G'>$G</button>";
				}
			$output .= "</div>";
			return $output;
		}
	
		$g_index = 0;
		$f_index = 0;
		$a_index = 0;
		$function_path_uri = $_SERVER['REQUEST_SCHEME']."://".$_SERVER['HTTP_HOST'].rtrim($_SERVER['REDIRECT_URL'], "/");
		
		foreach ($groups as $g) {
		
			$group_name	= $g['name'];			
			$group_description	= $g['description'];			

			// echo "<div id='G_".$g_index++."' class='jumbotron jumbotron-fluid mt-1 pt-1 rounded-top'>
			  // <div class='container'>
				// <h1 class='display-3'>$group_name</h1>				
			  // </div>";
			
			echo "<div class='card ml-3 mr-3 mb-3'>
					<div id='G_".$g_index++."' class='card-header'>
						<h3 class='card-title mb-0 pb-0'>$group_name</h3>
					</div>
					<div class='card-body bg-light'>
						$group_description
					</div>
					<ul class='list-group list-group-flush'>"; // card-header close
			
				foreach ($g->xpath("./function") as $f) {
					$function_uri = $f['uri'];					
					$function_name = (string)$f->name[0];					
					$function_action = $f->xpath("./action");
					
					
					//echo "<div class='card ml-3 mr-3 mb-3'>";
					
					//echo "<div class='card-header'>
					echo "<li class='list-group-item'><h4 class='card-title pt-3 pb-3'>$function_name</h4>";
					// echo "</div>"; // card-header close
						
					//echo "<div class='card-body'>
					
					echo "<div class='input-group mb-3'>
						<span class='input-group-addon' id='basic-addon1'>$function_path_uri</span>					
						<input type='text' class='form-control' aria-label='URI' aria-describedby='basic-uri' value='$function_uri'>						
						<span class='input-group-btn'>
							<button class='btn btn-secondary' type='button' data-toggle='collapse' data-target='#testBox'>Test</button>
						</span>
					</div>
					
					";
					
					$f_parameter = "";
					foreach ($f->xpath("./parameter") as $p) {
						$parameter_name 		= 		$p['name'];
						$parameter_type 		= 		strtoupper($p['type']);
						$parameter_description	=		(string)$p;
						$f_parameter .= "<tr><td class='text-center'>$parameter_name</td><td class='text-center'>$parameter_type</td><td>$parameter_description</td></tr>";
					}
					
					if ($f_parameter != "")
						echo "<div class='mt-2 mb-2'>
							<table class='table table-striped table-bordered table-sm'>
								<thead><tr><th>Parameter Name</th><th>Type</th><th>Description</th></tr></thead>
								<tbody>$f_parameter</tbody>
							</table>
						</div>";
					
					
					echo "<table class='table table-bordered table-sm'>";
					
					foreach ($function_action as $a) {
						$a_index++;
						
						$action_class				=		(string)$a['class'];
						$action_function			=		(string)$a['func'];
						$action_name				=		((string)$a->name[0] == "") ? $action_function : (string)$a->name[0];
						$action_method				=		$a['method'];
						$action_enable				=		((string)$a['enable'] == "true") ? "enable" : "disable";
						$action_description 		=		(string)$a->description[0];
						$action_exemple				=		$a->exemple;
						
						
						// Show real function name and existence (FOR DEBUG ONLY)
						$debug = "";
						if (DEBUG) {
							$action_func_exist		=		((bool)checkIfClassAndFunctionExists(trim($action_class), trim($action_function))[0]) ? "silver" : "#F6B6B6";
							$debug = "<span style='background-color: $action_func_exist;'>DEBUG INFO: <span>$action_class/$action_function</span></span>";
						}
						
						$exemple = "";
						$a_parameter = "";
						for($i=0; $i<count($action_exemple); $i++) {
							$exemple .= "<span>Exemple #".($i+1)." : </span><code>".json_encode(json_decode($action_exemple[$i]), JSON_PRETTY_PRINT)."</code><br/>";
						}
						foreach ($a->xpath("./parameter") as $p) {
							$parameter_name 		= 		$p['name'];
							$parameter_type 		= 		$p['type'];
							$parameter_description	=		(string)$p;
							$a_parameter .= "<tr><td class='text-center'>$parameter_name</td><td class='text-center'>$parameter_type</td><td>$parameter_description</td></tr>";
						}
						
						
						echo "<tr>
								<td class='text-center method $action_method'>$action_method</td><td>
									<div class='function-title' data-toggle='collapse' data-target='#testcon_$a_index'>$action_name $debug</div>									
									<div class='collapse' id='testcon_$a_index'><br/>
										<div class='field'>											
											<div>$action_description</div>
											<div class='m-2'>
												$exemple
											</div>";
											
											if ($a_parameter != "")
												echo "<div class='mt-2'>
													<table class='table table-striped table-bordered table-sm'>
														<thead><tr><th>Parameter Name</th><th>Type</th><th>Description</th></tr></thead>
														<tbody>$a_parameter</tbody>
													</table>
												</div>";
											
										echo "</div>
									</div>
								</td></tr>";
						
						//if ($action_exemple != "") {

							//echo "<textarea>".json_encode(json_decode($action_exemple), JSON_PRETTY_PRINT)."</textarea>";
						
							
						
							
						//}


						foreach ($a->xpath("./return") as $r) {					
							$return_code 			=		$r['code'];
							$return_description		=		(string)$r;						
						}
						
					}					
					
					echo "</table>"; // table close
					
					echo "</li>"; // card close
					
				}
			echo "</ul></div><br/>"; // container mt-5 close
			
		}
	?>

		<div style='height: 300px; width: 100%;'>
		</div>
			
		<!-- TEST BOX -->
		<div class="collapse fixed-bottom" id="testBox">
			<div class="card card-body" id="testBoxCard">
				<div class='input-group mb-3'>
			  <div class="input-group-btn">
				<button type="button" class="btn GET dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
				  GET
				</button>
				<div class="dropdown-menu">
				  <a class="dropdown-item" href="#">GET</a>
				  <a class="dropdown-item" href="#">POST</a>
				  <a class="dropdown-item" href="#">PUT</a>
				  <a class="dropdown-item" href="#">DELETE</a>
				</div>
			  </div>
					<span class='input-group-addon' id='basic-addon1'><?PHP p($function_path_uri); ?></span>
					<input id='testbox_uri' type='text' class='form-control' aria-label='URI' aria-describedby='basic-uri' value=''>
					<span class='input-group-btn'>
						<button class='btn btn-secondary' type='button' data-toggle='collapse' data-target='#testBox'>GO</button>
					</span>
				</div>
			</div>
		</div>
			

		</div>
	
	</body>

	<script defer>
		function searchOnPage() {
			console.log($('#search_field').val());
			find($('#search_field').val());
		}
	</script>
	
</html>


