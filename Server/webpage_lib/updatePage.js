window.onload = function () {
    // '$' in the code is an alias for the global jQuery object. it is used to access the jQuery members.
    // can (possibly) use 'jQuery' instead, if desired.
    const port = "8081";
    const protocol = "http://"
    
    const buttonDivName = "#buttonLeftHouse";
    const fridgeDivName = "#fridgeOpen";
    const lightOnDivName = "#lightOn";
    const waitTimeDivName = "#waitTimeText"
    const forgotLightDivName = "#forgotLight";
    const lightTimeDivName = "#lightTime"
    const faucetDivName = "#faucetOn"
    const showerDivName = "#showerRunning";
    const showerTimeDivName = "#showerTime"
    const dataTags = ["HOME", "FRIDGE", "LIGHT", "WAIT_TIME", "FORGOT_LIGHT", "LIGHT_TIME",  "LIGHT_AUTO", "FAUCET", "SHOWER", "SHOWER_TIME"]; // names used in the server's stored JSON.
    const tagToDiv = {"HOME" : buttonDivName, "FRIDGE" : fridgeDivName, 
        "LIGHT" : lightOnDivName, "WAIT_TIME" : waitTimeDivName, "FORGOT_LIGHT" : forgotLightDivName, "LIGHT_TIME": lightTimeDivName, 
        "FAUCET" : faucetDivName, "SHOWER" :  showerDivName, "SHOWER_TIME" : showerTimeDivName} // preface with '#' to indiciate a div to jQuery.

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
    let waitTime = 1; // integer for how long the arduino should wait before turning off the light.
    function setPageData(data) { // data is a JSON object 
        console.log("setting data.");
        for (const [key, value] of Object.entries(data)) { // iterate through the data-object's key/value pairs.
            wrapPageDataSpecifics(key,value);            
        }
    }

     // Fetch the latest data from the server, then update the webpage.
    function fetchAndSetData() {
        // when the data has been loaded, call the setPageData function. This is necessary because requests are done asynchronously.
        let fetch_promise = fetchData();
        $.when(fetch_promise).done(function(fetch_result) {
            response_text = fetch_result;
            response_text = JSON.parse(response_text); 
            //console.log(response_text);
            setPageData(response_text);
        })
        return fetch_promise; // if we want to fetchAndSet, and also do something depending on the fetch being completed.
    }

    function wrapPageDataSpecifics(key, value) {
        if (dataTags.includes(key)) {
            switch(key) {
                case "HOME": 
                    atHome = stringToBoolean(value);
                    updateButtonText();
                    break;
                case "FRIDGE":
                    let isFridgeOpen = stringToBoolean(value);
                    updateFridgeText(isFridgeOpen);
                    break;
                /*case "LIGHT": // Old cases for extra features related to the Lighting:
                    let isLightOn = stringToBoolean(value);
                    updateLightOnText(isLightOn);   
                    break;
                case "WAIT_TIME":
                    waitTime = parseInt(value);
                    updateWaitTimeText(waitTime);   
                    break;
                case "LIGHT_TIME":
                    let lightTime = value;
                    updateLightTimeText(lightTime);
                    break;
                case "LIGHT_AUTO":
                    // the webpage doesn't really care about this value.
                    break; */
                case "FORGOT_LIGHT":
                    let hasForgottenLight = stringToBoolean(value);
                    updateForgotLightText(hasForgottenLight );   
                    break;

                case "FAUCET":
                    let isFaucetRunning = stringToBoolean(value);
                    updateFaucetRunningText(isFaucetRunning);   
                    break;
                case "SHOWER":
                    let isShowerRunning = stringToBoolean(value);
                    updateShowerRunningText(isShowerRunning);   
                    break;
                case "SHOWER_TIME":
                    let showerTime = value;
                    updateShowerTimeText(showerTime);   
                    break;
                default: 
                    if (!(tagToDiv[key] === 'undefined')) { // check that the key is an actual key of tagToDiv.
                        // not really used anymore.
                        let divName = tagToDiv[key]; // find the name of the div from the key. i.e. #fridge = tagToDiv["FRIDGE"] 
                        // updateDiv(divName, value); // Assign the divs on the html page to the retrieved data
                        console.log(key + " doesn't have a proper place... sad :(");
                    }
                    else {
                        console.log("tag " + key + " is incorrect.");
                    } 
                }
        }
        else {
            console.log("tag " + key + " is very incorrect.");
        }
    } 

    function postHomeStatus(isHome) { 
        const path = "HOME"
        const apiExtension = "/API/" + path;
        let urlAPI = url + apiExtension;
        let isLeaving = !isHome; // isLeaving should be a 1 if user is leaving the house (isHome), 0 if user is returning. 
        let ajaxReq = $.post(urlAPI, path + "=" + isLeaving); // send a post request with the logic
        return ajaxReq;
    }

    function postWaitTime(waitTimeArg) { // send the waiting time argument, an integer representing how long the arduino should wait, to the server.
        const path = "WAIT_TIME"
        const apiExtension = "/API/" + path;
        let urlAPI = url + apiExtension;
        waitTimeArg = Math.ceil(waitTimeArg) // make sure the value is an integer (not a decimal value or bool).
        waitTimeArg = String(waitTimeArg) // Then turn it into a string.
        let ajaxReq = $.post(urlAPI, path + "=" + waitTime); // send a post request with the logic
        return ajaxReq;
    }
    

    function updateButtonText() {
        let divName = buttonDivName;
        let buttonText = atHome ? ("I've left the house.") : ("I've returned");
        updateDiv(divName, buttonText);
    }

    function updateFridgeText(isFridgeOpen) {
        let divName = fridgeDivName;
        let divText = isFridgeOpen ? ("Your fridge is open! You should close it.") : ("Your fridge is closed.")
        updateDiv(divName, divText)
    }

    function updateLightOnText(isLightOn) {
        let divName = lightOnDivName;
        let divText = isLightOn ? ("Your light is on. Are you sure you <it> really </it> need it?") : ("Your light is off.")
        updateDiv(divName, divText)
    }

    function updateWaitTimeText(waitTime) {
        let divName = waitTimeDivName;
        let divText = "Waiting time is " + waitTime + " minutes.";
        updateDiv(divName, divText)
    }

    function updateForgotLightText(hasForgottenLight) {
        let divName = forgotLightDivName;
        let divText = hasForgottenLight ? ("You forgot to turn off the light.") : (" ... ")
        updateDiv(divName, divText)
    }

    function updateLightTimeText(lightTime) {
        let divName = lightTimeDivName;
        let divText = "EcoHome has saved you " + lightTime + " minutes of lighting."; 
        updateDiv(divName, divText)
    }

    function updateFaucetRunningText(isFaucetRunning) {
        let divName = faucetDivName;
        let divText = isFaucetRunning ? ("What are you doing on your phone, you're using the sink right now. <b> Right? </b>") : ("Your faucet is turned off :D")
        updateDiv(divName, divText)
    }

    function updateShowerRunningText(isShowerRunning) { 
        let divName = showerDivName;
        let divText = isShowerRunning ? ("Looking at the phone.. in the shower? I swear to God.<br>Oh, never mind. The showerhead's just dripping I guess") : ("Your shower is turned off")
        updateDiv(divName, divText)   
    }

    function updateShowerTimeText(showerTime) {
        let divName = showerTimeDivName;
        let divText = "You've showered " + showerTime + " minutes. Try to be quicker, please."; 
        updateDiv(divName, divText)
    }

    function onIPtextLoaded(ip_text) {
        ip = ip_text.trim();
        url = protocol + ip + ":" + port
        console.log("URL IS: " + url);

        // Call the function on page-load, and then poll again continuously by interval:
        fetchAndSetData();
        const pollInterval = 15 * 1000; // 15 seconds.
        setInterval( fetchAndSetData, pollInterval);

        // event listener for the "Left home" & "returned" button: 
        gebi("buttonLeftHouse").addEventListener("click", function (event) {
            console.log("Home status is: " + atHome)
            let divName = buttonDivName;
            const stallMsg = "loading";
            if (atHome === stallMsg) { // If user clicks on the button before it has finished sending and retrieving data from the server.
                alert("Still posting data. Check your connection to the server if this continues.");
            }
            else {
                updateDiv(divName, "Initiating procedures...")
                $.when(postHomeStatus(atHome)).done(function() { // send post request and, when its done:
                    fetchAndSetData(); // fetch data, and update the page. 
                    
                    /*$.when(fetchAndSetData()).done( function() { 
                        // and when fetch is done, update the button's text.
                        // updateButtonText();
                    })*/            
                })
            }
            atHome = stallMsg;
        });


        // event listener for the "waiting time" range:
        gebi("waitTimeRange").addEventListener("change", function (event) {
            waitTime = event.currentTarget.value;
            updateWaitTimeText(waitTime);
            $.when(postWaitTime(waitTime)).done(function() { // send post request and, when its done:
                fetchAndSetData() // fetch data, and update the page.
            })            
        });
    }
    
    let ip = "IP not loaded yet" // IP, insert using method from documentation.
    let url =  "IP not loaded yet - URL" ; 
    fetch('ip.txt')
        .then(response => response.text())
            .then(text => onIPtextLoaded(text));


}
