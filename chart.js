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
var numOfErrors = 0;

function updateData() {
    var req;
    if (onOff == "On") {
        req = current_url + "temp?"+lastStatType+convertTempAppended()
    } else {
        req = current_url + "temp?"+"o"+convertTempAppended()
    }
    try {
        $.ajax({     
            type: "POST",
            url: req,
            success: function (data) {
                y0 = data;
                if (y0 == 9999.9) {
                    numOfErrors++;
                    if (recieving && numErrors > 5) {
                        recieving = false;
                        document.getElementById("recieving").value = "Arduino disconnected";
                    }
                }
                else {
                    if (recieving = false) {
                        recieving = true;
                        document.getElementById("recieving").value = "Arduino connected";
                        numOfErrors = 0;
                    }
                    x0++;
                    console.log(y0);
                    temps.push({"x": x0, "y": y0});
                }
            },

        });
    } catch (e) {
        console.log('catch', e);
        document.getElementById("recieving").value = "Console Off!";
        clearInterval(interData);
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