const Plotly = window.Plotly;

$.ajax({
  url: "data.csv",
  timeout: 1000,
  dataType: "text"
}).done(displayPlots);

var plot_names = [];
var plot_numbers = [];
var plot_pHs = [];
var plot_moistures = [];
var plot_COs = [];
var plot_temperatures = [];
var plot_humidities = [];


function displayPlots(data) {
  var allRows = data.split(/\r?\n|\r/);
  //console.log(allRows);

  var table = "<table>";
  for (var singleRow = 0; singleRow < allRows.length; singleRow++) {
    if (singleRow === 0) {
      table += "<thead>";
      table += "<tr>";
    } else {
      table += "<tr>";
    }
    var rowCells = allRows[singleRow].split(",");
    var count = 0;
    for (var rowCell = 0; rowCell < rowCells.length; rowCell++) {
      if (singleRow === 0) {
        table += "<th>";
        table += rowCells[rowCell];
        //console.log(rowCells[rowCell]); // table headers
        table += "</th>";
      } else {
        table += "<td>";

        table += rowCells[rowCell];

        // PLOT NAME
        if (count % 7 == 0) {
          var plot_name = rowCells[rowCell];
          plot_names.push(plot_name);
          count += 1;
          
          
        // PLOT NUMBER
        } else if (count % 7 == 1) {
          var plot_number = rowCells[rowCell];
          plot_numbers.push(plot_number);
          count += 1;
        
        // PLOT PH 
        } else if (count % 7 == 2) {
          var plot_pH = rowCells[rowCell];
          plot_pHs.push(plot_pH);
          count += 1;
        }
        
        // PLOT MOISTURE
        else if (count % 7 == 3) {
          var plot_moisture = rowCells[rowCell];
          plot_moistures.push(plot_moisture);
          count += 1;
        }
        
        // PLOT CO
        else if (count % 7 == 4) {
          var plot_CO = rowCells[rowCell];
          plot_COs.push(plot_CO);
          count += 1;
        }
        
        // PLOT TEMPERATURE
        else if (count % 7 == 5) {
          var plot_temperature = rowCells[rowCell];
          plot_temperatures.push(plot_temperature);
          count += 1;
        }
        
        // PLOT HUMIDITY
        else if (count % 7 == 6) {
          var plot_humidity = rowCells[rowCell];
          plot_humidities.push(plot_humidity);
          count += 1;
        }


        table += "</td>";
      }
    }
    if (singleRow === 0) {
      table += "</tr>";
      table += "</thead>";
      table += "<tbody>";
    } else {
      table += "</tr>";
    }
  }
  table += "</tbody>";
  table += "</table>";
  $("#pHtable").append(table);
  
  console.log(plot_COs);
  console.log(plot_names);
  console.log(plot_numbers);

  plot_pHs = plot_pHs.map(Number);
  console.log(plot_pHs);
  
  var row1 = plot_pHs.slice(0,4);
  //console.log(row1);
  var row2 = plot_pHs.slice(4,8);
  //console.log(row2);
  
  plot_moistures = plot_moistures.map(Number);
  console.log(plot_moistures);
  
  plot_temperatures = plot_temperatures.map(Number);
  console.log(plot_temperatures);
  
  plot_humidities = plot_humidities.map(Number);
  console.log(plot_humidities);
  
  
  
  //////////////////////////////////////////////////////////////

  window.PLOTLYENV = { BASE_URL: "https://plot.ly" };

  var COheatmap = document.getElementById("co-heatmap");
  var resizeDebounce = null;

  function resizePlot() {
    var bb = plot_COs.getBoundingClientRect();
    Plotly.relayout(plot_COs, {
      width: bb.width,
      height: bb.height
    });
  }

  window.addEventListener("resize", function() {
    if (resizeDebounce) {
      window.clearTimeout(resizeDebounce);
    }
    resizeDebounce = window.setTimeout(resizePlot, 100);
  });
  

  
  
  var CO_figure = {

    data: [
      {
        uid: "1",
        type: "heatmap",
        zmax: 10,
        zmin: 5,
        // the readings
        z: [plot_COs.slice(0,4),plot_COs.slice(4,8)],
        // the labels
        text: [
          plot_names.slice(0,4),
          plot_names.slice(4,8)
        ],
        zsmooth: "best", // best / worst
        colorscale: [
          [0, "#ffffff"],
          [1, "#2c3e50"]
          //[0, "rgb(0,0,0)"],
          //[0.3, "rgb(230,0,0)"],
          //[0.6, "rgb(255,210,0)"],
          //[1, "rgb(255,255,255)"]
        ]
      }
    ],
    layout: {
      font: {
        size: 18,
        family: "Arial, sans serif"
      },
      title: "Carbon Monoxide Levels",
      width: 1080,
      xaxis: {
       type: "linear",
       range: [0, 4],
       autorange: true
     },
     yaxis: {
       type: "linear",
        //the coordinates
       range: [0, 2],
       autorange: true
      },
      height: 598,
      autosize: true,
      showlegend: true,
    },
    frames: []
  };

  Plotly.plot(COheatmap, {
    data: CO_figure.data,
    layout: CO_figure.layout,
    frames: CO_figure.frames,
    config: {
      showLink: false,
      linkText: "Export to plot.ly",
      mapboxAccessToken:
        "pk.eyJ1IjoiY2hyaWRkeXAiLCJhIjoiY2lxMnVvdm5iMDA4dnhsbTQ5aHJzcGs0MyJ9.X9o_rzNLNesDxdra4neC_A"
    }
  });

///////////////////////////////////////////////////////////////////////////
  
  
  //////////////////////////////////////////////////////////////

  window.PLOTLYENV = { BASE_URL: "https://plot.ly" };

  var pHheatmap = document.getElementById("pH-heatmap");
  var resizeDebounce = null;

  function resizePlot() {
    var bb = pHheatmap.getBoundingClientRect();
    Plotly.relayout(pHheatmap, {
      width: bb.width,
      height: bb.height
    });
  }

  window.addEventListener("resize", function() {
    if (resizeDebounce) {
      window.clearTimeout(resizeDebounce);
    }
    resizeDebounce = window.setTimeout(resizePlot, 100);
  });
  

  
  
  var pH_figure = {

    data: [
      {
        uid: "1",
        type: "heatmap",
        zmax: 14,
        zmin: 0,
        // the readings
        z: [row1,row2],
        // the labels
        text: [
          plot_names.slice(0,4),
          plot_names.slice(4,8)
        ],
        zsmooth: "best", // best / worst
        colorscale: [
          [0, "#ff9e8f"],
          [1, "#968fff"],
          //[0, "rgb(0,0,0)"],
          //[0.3, "rgb(230,0,0)"],
          //[0.6, "rgb(255,210,0)"],
          //[1, "rgb(255,255,255)"]
        ]
      }
    ],
    layout: {
      font: {
        size: 18,
        family: "Arial, sans serif"
      },
      title: "Soil pH",
      width: 1080,
      xaxis: {
       type: "linear",
       range: [0, 4],
       autorange: true
     },
     yaxis: {
       type: "linear",
        //the coordinates
       range: [0, 2],
       autorange: true
      },
      height: 598,
      autosize: true,
      showlegend: true,
    },
    frames: []
  };

  Plotly.plot(pHheatmap, {
    data: pH_figure.data,
    layout: pH_figure.layout,
    frames: pH_figure.frames,
    config: {
      showLink: false,
      linkText: "Export to plot.ly",
      mapboxAccessToken:
        "pk.eyJ1IjoiY2hyaWRkeXAiLCJhIjoiY2lxMnVvdm5iMDA4dnhsbTQ5aHJzcGs0MyJ9.X9o_rzNLNesDxdra4neC_A"
    }
  });

///////////////////////////////////////////////////////////////////////////
  
  var moistureheatmap = document.getElementById("moisture-heatmap");
  var resizeDebounce = null;

  function resizePlot() {
    var bb = moistureheatmap.getBoundingClientRect();
    Plotly.relayout(moistureheatmap, {
      width: bb.width,
      height: bb.height
    });
  }

  window.addEventListener("resize", function() {
    if (resizeDebounce) {
      window.clearTimeout(resizeDebounce);
    }
    resizeDebounce = window.setTimeout(resizePlot, 100);
  });
  

  
  
  var moisture_figure = {

    data: [
      {
        uid: "2",
        type: "heatmap",
        zmax: 14,
        zmin: 0,
        // the readings
        z: [plot_moistures.slice(0,4),plot_moistures.slice(4,8)],
        // the labels
        text: [
          plot_names.slice(0,4),
          plot_names.slice(4,8)
        ],
        zsmooth: "best", // best / worst
        colorscale: [
          [0, "#ffffff"],
          [1, "#076585"]
        ]
      }
    ],
    layout: {
      font: {
        size: 18,
        family: "Arial, sans serif"
      },
      title: "Soil Moisture",
      width: 1080,
      xaxis: {
       type: "linear",
       range: [0, 4],
       autorange: true
     },
     yaxis: {
       type: "linear",
        //the coordinates
       range: [0, 2],
       autorange: true
      },
      height: 598,
      autosize: true,
      showlegend: true,
    },
    frames: []
  };

  Plotly.plot(moistureheatmap, {
    data: moisture_figure.data,
    layout: moisture_figure.layout,
    frames: moisture_figure.frames,
    config: {
      showLink: false,
      linkText: "Export to plot.ly",
      mapboxAccessToken:
        "pk.eyJ1IjoiY2hyaWRkeXAiLCJhIjoiY2lxMnVvdm5iMDA4dnhsbTQ5aHJzcGs0MyJ9.X9o_rzNLNesDxdra4neC_A"
    }
  });

  
}
