	
var lastStatType = "C"
var lastTimeFrame = "10"
var onOff = "On"
var lastTemperatureType = "Celsius"
var lastTemp;

$(document).ready(function() {

	$('button').on('click', function() {
		var buttonPressed = $(this);
		if (buttonPressed.attr("class") == "Temperature") {
			console.log("Sending GET localhost:3001/color")
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
			getStat();

		} else if (buttonPressed.attr("class") == "onoff") {
			if (onOff == "On") {
				onOff = "Off"
				clearInterval(interChart);
				document.getElementById("display").value = "";
				$.ajax({
					type: "GET",
					url: "localhost:3001/off"
				});
			} else {
				onOff = "On"
				getStat();
				interChart = setInterval(function() {
					updateChart();}, 1000);
				}
				$.ajax({
					type: "GET",
					url: "localhost:3001/on"
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
getActual = function() {
	document.getElementById("display").value = 
		convertTemperature(temps[temps.length-1].y)

}

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

convertTempAppended = function() {
	if (lastTemperatureType == "Celsius") {
		return lastTemp.toFixed(1)+"C";
	}
	else {
		return lastTemp.toFixed(1)+"F";
	} 
}


getSubArray = function() {
	if (temps.length <= Number(lastTimeFrame)) {
		return temps;
	}
	else {
		startInterval = temps.length - Number(lastTimeFrame) -1;
		return temps.slice(startInterval, temps.length - 1)
	}
}