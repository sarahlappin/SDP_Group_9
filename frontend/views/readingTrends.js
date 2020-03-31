
window.onload = function () {
var chart = new CanvasJS.Chart("chartContainer1",
{
title:{
text: "Earthquakes - per month"
},
data: [
{
type: "line",
dataPoints: [
{ x: new Date(2012, 00, 1), y: 450 },
{ x: new Date(2012, 01, 1), y: 414 },
{ x: new Date(2012, 02, 1), y: 520 },
{ x: new Date(2012, 03, 1), y: 460 },
{ x: new Date(2012, 04, 1), y: 450 },
{ x: new Date(2012, 05, 1), y: 500 },
{ x: new Date(2012, 06, 1), y: 480 },
{ x: new Date(2012, 07, 1), y: 480 },
{ x: new Date(2012, 08, 1), y: 410 },
{ x: new Date(2012, 09, 1), y: 500 },
{ x: new Date(2012, 10, 1), y: 480 },
{ x: new Date(2012, 11, 1), y: 510 }
]
}
]
});
chart.render();
}






// window.onload = function () {

// var dps = []; // dataPoints
// var chart = new CanvasJS.Chart("chartContainer", {
// 	title :{
// 		text: "Carbon Monoxide"
// 	},
// 	axisY: {
// 		includeZero: true
// 	},      
// 	data: [{
// 		type: "line",
// 		dataPoints: dps
// 	}]
// });

// var xVal = 0;
// var yVal = 100; 
// var updateInterval = 10;
// var dataLength = 20; // number of dataPoints visible at any point

// var updateChart = function (count) {

// 	count = count || 1;

// 	for (var j = 0; j < count; j++) {
// 		yVal = yVal +  Math.round(5 + Math.random() *(-5-5));
// 		dps.push({
// 			x: xVal,
// 			y: yVal
// 		});
// 		xVal++;
// 	}

// 	if (dps.length > dataLength) {
// 		dps.shift();
// 	}

// 	chart.render();
// };

// updateChart(dataLength);
// setInterval(function(){updateChart()}, updateInterval);
// }