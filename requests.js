var lastStatType = "C"
var lastTimeFrame = "10"
var onOff = "On"
var lastTemperatureType = "Celsius"
var lastTemp = 23;
var recieving = true;


$(document).ready(function() {

	document.getElementById("recieving").value = "Recieving";

	$('button').on('click', function() {
		var buttonPressed = $(this);
		if (buttonPressed.attr("class") == "Temperature") {
			if  (lastTemperatureType == "Celsius") {
				lastTemperatureType = "Fahrenheit"
			} else {
				lastTemperatureType = "Celsius"

			}
			document.getElementById("Temp").value = lastTemperatureType;
			getStat();
		} else if (buttonPressed.attr("class") == "onoff") {
			if (onOff == "On") {
				onOff = "Off"
				clearInterval(interChart);
				document.getElementById("display").value = "";
				$.ajax({
					type: "GET",
					url: "http://localhost:3001/off"
				});
			} else {
				onOff = "On"
				getStat();
				interChart = setInterval(function() {
					updateChart();}, 1000);
				}
				$.ajax({
					type: "GET",
					url: "http://localhost:3001/on"
				});
		}
		updateData();
	})

	$('select').on('change', function() {
		var optionSelected = $(this);
		if (optionSelected.attr("class")== "stat") {
			if (optionSelected.val() != lastStatType) {
				lastStatType = optionSelected.val()
				getStat();
			}
		}
		else if (optionSelected.attr("class") == "timeFrame") {
			if (optionSelected.val() != lastTimeFrame) {
				lastTimeFrame = optionSelected.val() 
				getStat();
			} 
		}
		updateData();

	});

});

getStat = function() {
	if (lastStatType == "H") {
		getHigh();
	}
	else if (lastStatType == "L") {
		getLow();
	}
	else if (lastStatType == "A") {
		getAverage();
	}
	else {
		getActual();
	}
}

getAverage = function() {
	var num = Number(lastTimeFrame)
	var sum = 0
	var avg = 0;
	if (temps.length < num) {
		for (var i = 0; i < temps.length; i++) {
			sum += Number(temps[i].y)
		}
		console.log(avg)
	} else {
		for (var i = temps.length - num; i < temps.length; i++) {
			sum += Number(temps[i].y)
		}
	}
	avg = sum / num
	document.getElementById("display").value = convertTemperature(avg);
	
}

//get the actual temperature for the given time length
getActual = function() {
	document.getElementById("display").value = 
		convertTemperature(temps[temps.length-1].y)

}

////get the high temperature for the given time length
getHigh = function() {
	var num = Number(lastTimeFrame)
	var max = 0
	if (temps.length < num) {
		for (var i = 0; i < temps.length; i++) {
			if (max < Number(temps[i].y)) {
				max = Number(temps[i].y)
			}
		}
	} else {
		for (var i = temps.length - num; i < temps.length; i++) {
			if (max < Number(temps[i].y)) {
				max = Number(temps[i].y)
			}
		}
	}
	document.getElementById("display").value = convertTemperature(max);

}
//get the low temperature for the given time length
getLow = function() {
	num = Number(lastTimeFrame)
	var min = Number.POSITIVE_INFINITY
	if (temps.length < num) {
		for (var i = 0; i < temps.length; i++) {
			if (min > Number(temps[i].y)) {
				min = Number(temps[i].y)
			}
		}
	} else {
		for (var i = temps.length - num; i < temps.length; i++) {
			if (min > Number(temps[i].y)) {
				min = Number(temps[i].y)
			}
		}
	}
	document.getElementById("display").value = convertTemperature(min);
}

//converts the temperature 
convertTemperature = function(val) {
	if (lastTemperatureType == "Celsius") {
		lastTemp = val;
		return val;
	}
	else {
		lastTemp = 32 + (9*val)/5;
		return lastTemp;
	}
}

//gives you temperature value 
convertTempAppended = function() {
	if (lastTemperatureType == "Celsius") {
		return lastTemp.toFixed(1)+"C";
	}
	else {
		return lastTemp.toFixed(1)+"F";
	} 
}

//gives you a sub-array of a given time length
//each element in the array is a reading for a second
getSubArray = function() {
	if (temps.length <= Number(lastTimeFrame)) {
		return temps;
	}
	else {
		startInterval = temps.length - Number(lastTimeFrame) -1;
		return temps.slice(startInterval, temps.length - 1)
	}
}