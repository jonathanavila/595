/*
 * Implement all your JavaScript in this file!
 */

 var display = null; 
 var temps = [ {"x": 1, "y": 250}, {"x": 2, "y": 100}, {"x": 3,"y": 200}, {"x": 4,"y": 240},{"x": 5,"y": 25}];

 var height = 320;
 var width = 520;

 var svg = d3.select("#chart").append("svg").attr("width", width + 30).attr("height", height + 30);


 var x = d3.scaleLinear().domain([0, 5]).range([0, width]);
 var y = d3.scaleLinear().domain([0, 300]).range([height, 0]);

var xAxis = d3.axisBottom().scale(x).ticks(5);


var line = d3.line()
    .x(function(d) { return x(d.x); })
    .y(function(d) { return y(d.y); });

var graph = svg.append("path").attr("d", line(temps)).attr("stroke", "blue")
                            .attr("stroke-width", 1)
                            .attr("fill", "none");

svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);