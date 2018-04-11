$(document).ready(function() {

	var lastStatType = "Actual"
	var lastTimeFrame = "10 seconds"
	var onOff = "On"
	var lastTemperatureType = "Celsius"

	$('button').on('click', function() {
		var buttonPressed = $(this);
		if (buttonPressed.attr("class") == "Temperature") {
			console.log("Sending GET localhost:3001/ color")
			if  (lastTemperatureType == "Celsius") {
				lastTemperatureType = "Fahrenheit"
			} else {
				lastTemperatureType = "Celsius"

			}
			$.ajax({
				type: "GET",
				url: "localhost:3001/color"
			});
			document.getElementById("Temp").value = lastTemperatureType;

		} else if (buttonPressed.attr("class") == "OnOff") {

		}
	})

	$('select').on('change', function() {
		var optionSelected = $(this);
		if (optionSelected.attr("class")== "stat") {

			if (optionSelected.val() == 'High') {
				//get request for high

				//once request is gotten, display value
				document.getElementById("display").value = optionSelected.val();


			} else if (optionSelected.val() == 'Low') {
				document.getElementById("display").value = optionSelected.val();
				//get request for Low

				//once request is gotten, display value

			}
			else if (optionSelected.val() == 'Average') {

				document.getElementById("display").value = optionSelected.val();
				//get request for Hi

				//once request is gotten, display value
			}
			else {
				document.getElementById("display").value = optionSelected.val();

			}
		}


	});

});