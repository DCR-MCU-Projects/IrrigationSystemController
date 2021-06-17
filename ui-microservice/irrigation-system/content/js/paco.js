//const { remote } = require('electron');

// // In the Renderer process
// const { ipcRenderer } = require('electron')

// ipcRenderer.invoke('perform-action', "aaa")


$(document).ready(function() {
	$("#ad_username").focus();
});


$("#startPACO").on('click', () => {
	window.paco.start($("#ad_username").val(), $("#ad_password").val(), $("#rsa_username").val(), $("#rsa_password").val());
})

$("#stopPACO").on('click', () => {
	window.paco.stop();
})