// SCRIPT TO TEST API

function get() {
   var x = document.getElementById("form");
   console.log(x.elements[0].value);
   var landID = x.elements[0].value;
   var reading_type = x.elements[1].value;
   var link = "http://127.0.0.1:5000/getLandInfo/" + landID + "/" + reading_type;
  // fetch JSON data from the API
    return fetch(link, {
        headers: {
            Accept: "application/json",
        },
        method: 'GET', // defines the method
    }).then(resp => {
        return resp.json()
    }).then(r => {
    
        
        console.log(reading_type);
        if (reading_type != "all") {
            r = JSON.parse(r);
            // console.log(r[1]);       ACCESS NUMERICAL VALUE
        } else {
            r = JSON.parse(JSON.stringify(r));
        }

        console.log(r);

        const app = document.getElementById('root');

        // create a container as a div in the html
        const container = document.createElement('div');
        container.setAttribute('class', 'container');

        // place the container on the website
        app.appendChild(container);

        // create a card as a div (this is a container for the joke!)
        const card = document.createElement('div');
        card.setAttribute('class', 'card');

        // text placeholder
        const p1 = document.createElement('p');
        p1.textContent = "RETURNED RAW " + reading_type + " VALUES: " + JSON.stringify(r); 


        container.appendChild(card); 

        // places the text on the card
        card.appendChild(p1);

    })

}
