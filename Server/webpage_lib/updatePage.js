window.onload = function () {
    // '$' in the code is an alias for the global jQuery object. it is used to access the jQuery members.
    // can (possibly) use 'jQuery' instead, if desired.
    const port = "8081";
    const protocol = "http://"
    // BEFORE IP
    const ip = "192.168.1.13" // IP, insert using method from documentation.

    const url =  protocol + ip + ":" + port; 
    // "/API/BUTTON" //"http://localhost:8081/API/BUTTON"; // "25.57.47.113:8081"; //'http://localhost:8081/API/BUTTON'; // example: 'http://echo.jsontest.com/Hello/world'
    const buttonDivName = "#buttonLeftHouse";
    const fridgeDivName = "#fridgeOpen";
    const dataTags = ["HOME", "FRIDGE", "LIGHT", "LIGHT_TIME",  "LIGHT_AUTO", "FAUCET", "SHOWER", "SHOWER_TIME"]; // names used in the server's stored JSON.
    const tagToDiv = {"HOME" : buttonDivName, "FRIDGE" : fridgeDivName, 
        "LIGHT" : "#lightOn", "FAUCET" : "#faucetOn", 
        "SHOWER" : "#showerTime" } // preface with '#' to indiciate a div to jQuery.

    // support functions:
    function gebi(id) { // alias for "get element by id":
        return document.getElementById(id);
    }

    function stringToBoolean (string) { // source: https://stackoverflow.com/a/1414175
        switch(string.toLowerCase().trim()){
            case "true": 
            case "yes": 
            case "1": 
                return true;
    
            case "false": 
            case "no": 
            case "0": 
            case null: 
              return false;
    
            default: 
              return JSON.parse(string.toLowerCase())
        }
    }


    function fetchData() {
        console.log("fetching data.");
        const apiExtension = "/API/ALL";
        let urlAPI = url + apiExtension;
        //console.log(urlAPI);
        let ajaxReq = $.get(urlAPI, function(data) {
        });
        return ajaxReq; // return the ajax-request object.
    }

    function updateDiv(divName, value) {
        $(divName).html(value); 
    }
    
    let atHome = true; // bool relating to whether user is home or not. updated on fetch.
    function setPageData(data) { // data is a JSON object 
        console.log("setting data.");
        for (const [key, value] of Object.entries(data)) { // iterate through the data-object's key/value pairs.
            if (dataTags.includes(key)) {
                if (!(tagToDiv[key] === 'undefined')) { // check that the key is an actual key of tagToDiv.
                    // not really used anymore.
                    let divName = tagToDiv[key]; // find the name of the div from the key. i.e. #fridge = tagToDiv["FRIDGE"] 
                    //updateDiv(divName, value); // Assign the divs on the html page to the retrieved data
                }
                if (key == "HOME") {
                    atHome = stringToBoolean(value);
                    updateButtonText();
                }
                else if (key == "FRIDGE") {
                    let isFridgeOpen = stringToBoolean(value);
                    updateFridgeText(isFridgeOpen);
                }
            }
            else {
                console.log("tag " + key + " is incorrect.");
            }
    
        }
    }

     // Fetch the latest data from the server, then update the webpage.
    function fetchAndSetData() {
        // when the data has been loaded, call the setPageData function. This is necessary because requests are done asynchronously.
        let fetch_promise = fetchData()
        $.when(fetch_promise).done(function(fetch_result) {
            response_text = fetch_result;
            response_text = JSON.parse(response_text); 
            //console.log(response_text);
            setPageData(response_text);
        })
        return fetch_promise; // if we want to fetchAndSet, and also do something depending on the fetch being completed.
    }


    function postHomeStatus(isHome) { // isLeaving should be a 1 if user is leaving the house (isHome), 0 if user is returning. 
        const path = "HOME"
        const apiExtension = "/API/" + path;
        let urlAPI = url + apiExtension;
        let isLeaving = !isHome;
        let ajaxReq = $.post(urlAPI, path + "=" + isLeaving); // send a post request with the logic
        return ajaxReq;
    }

    function updateFridgeText(isFridgeOpen) {
        let divName = fridgeDivName;
        let divText = isFridgeOpen ? ("Your fridge is open! You should close it.") : ("Your fridge is closed.")
        updateDiv(divName, divText)
    }

    function updateButtonText() {
        let divName = buttonDivName;
        let buttonText = atHome ? ("I've left the house.") : ("I've returned");
        updateDiv(divName, buttonText);
    }

    // Button for "Left home" & "returned":
    gebi("buttonLeftHouse").addEventListener("click", function (event) {
        console.log("Home status is: " + atHome)
        let divName = buttonDivName;
        const stallMsg = "loading";
        if (! (atHome === stallMsg) ) {
            updateDiv(divName, "Initiating procedures...")
            $.when(postHomeStatus(atHome)).done(function() { // send post request and, when its done:
                $.when(fetchAndSetData()).done( function() { // fetch data. 
                    // and when fetch is done, update the button's text.
                    updateButtonText();
                })            
            })
        }
        else {
            alert("still posting");
        }
        atHome = stallMsg;
    });


    // Call the function on page-load, and then poll again continuously by interval:
    fetchAndSetData();
    const pollInterval = 15 * 1000; // 15 seconds.
    setInterval( fetchAndSetData, pollInterval);
}
