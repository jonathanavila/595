/*
 * Implement all your JavaScript in this file!
 */

var display = null; 
var temps = [];

var y0 = 0
var x0 = -1


var height = 320;
var width = 520;

var svg = d3.select("#chart").append("svg").attr("width", width + 30).attr("height", height + 30);


var interData = setInterval(function() {
        updateData();
}, 1000);


var interChart = setInterval(function() {
		updateChart();
	}, 1000);


function updateData() {
    x0++;
    var req;

    if (onOff == "On") {
        req = current_url + "temp?"+lastStatType+convertTempAppended()
    } else {
        req = current_url + "temp?"+"o"+convertTempAppended()
    }
    $.ajax({     
        type: "POST",
        url: req,
        success: function (data) {
            console.log(y0);   
            y0 = data;
        },
    });
    if (y0 == -1) {
        if (recieving) {
            recieving = false;
            document.getElementById("recieving").value = "Not recieving";

        }
    } else {
        if (recieving = false) {
            recieving = true;
            document.getElementById("recieving").value = "Recieving";

        }
        y0 = 30 + 10*Math.random();
        temps.push({"x": x0, "y": y0});
    }
}


function updateChart() {     
    minimum = 0 
    if (temps.length - 1 - Number(lastTimeFrame) > 0) {
        minimum = temps.length - Number(lastTimeFrame) - 1 ;
    }
    maximum = temps.length - 1; 
    if (temps.length <= Number(lastTimeFrame)){
        maximum = Number(lastTimeFrame);
    }


    var x = d3.scaleLinear().domain([minimum, maximum]).range([0, width]);
	var y = d3.scaleLinear().domain([0, convertTemperature(50)]).range([height, 0]);


	var xAxis = d3.axisBottom().scale(x).ticks(5);
    var yAxis = d3.axisRight().scale(y).ticks(10);




	var line = d3.line()
   	 .x(function(d) { return x(d.x); })
   	 .y(function(d) { return y(convertTemperature(d.y)); });

    d3.selectAll("svg > *").remove();


	var graph = svg.append("path").attr("d", line(getSubArray())).attr("stroke", "blue")
                            .attr("stroke-width", 1)
                            .attr("fill", "none");

    svg.append("g")
        .attr("class", "y axis")
        .attr("transform", "translate(0, 0 )")
        .call(yAxis)

    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);   

    getStat();            
}