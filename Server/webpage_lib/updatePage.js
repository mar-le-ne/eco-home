/*
    CREDITS:
    Written by August Valentin, student ID: S194802
*/

window.onload = function () {
    // '$' in the code is an alias for the global jQuery object. it is used to access the jQuery members.
    // can (possibly) use 'jQuery' instead, if desired.
    
    // Set constants
    const port = "8081";
    const protocol = "http://"
    
    // Set aliases for the divNames, to avoid typos later on.
    // prefaced with '#' to indiciate a div to jQuery.
    const buttonDivName = "#buttonLeftHouse";
    const fridgeDivName = "#fridgeOpen";
    const lightOnDivName = "#lightOn";
    const waitTimeDivName = "#waitTimeText"
    const forgotLightDivName = "#forgotLight";
    const lightTimeDivName = "#lightTime"
    const faucetDivName = "#faucetOn"
    const showerDivName = "#showerRunning";
    const showerTimeDivName = "#showerTime"
    // set aliases for the tags, to avoid typos later on. The tags are strings used in the server's stored JSON.
    const homeTag = "HOME";
    const fridgeTag = "FRIDGE";
    const lightTag = "LIGHT";
    const waitTimeTag = "WAIT_TIME";
    const forgotLightTag = "FORGOT_LIGHT";
    const lightTimeTag = "LIGHT_TIME";
    const lightAutoTag = "LIGHT_AUTO";
    const faucetTag = "FAUCET";
    const showerTag = "SHOWER";
    const showerTimeTag = "SHOWER_TIME";

    // List of tags of the server. It's utilized to check if incoming data from GET is valid. 
    const dataTags = [homeTag, fridgeTag, 
        lightTag, waitTimeTag, forgotLightTag, lightTimeTag, lightAutoTag, 
        faucetTag, showerTag, showerTimeTag];
    const tagToDiv = {homeTag : buttonDivName, fridgeTag : fridgeDivName, 
        lightTag : lightOnDivName, waitTimeTag : waitTimeDivName, forgotLightTag : forgotLightDivName, lightTimeTag: lightTimeDivName, 
        faucetTag : faucetDivName, showerTag :  showerDivName, showerTimeTag : showerTimeDivName};
    // List of deprecated (but valid) tags.
    const removedTags = [lightTag, lightTimeTag, lightAutoTag]

    // support functions:
    // alias for "get element by id":
    function gebi(id) { 
        return document.getElementById(id);
    }

    // parse string as Boolean.
    function parseBoolean (string) { 
        // source: https://stackoverflow.com/a/1414175
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
        // function for retrieving all the data stored in the server.
        console.log("fetching data.");
        const apiExtension = "/API" + "/ALL";
        let urlAPI = url + apiExtension;
        let ajaxReq = $.get(urlAPI, function(data) {} ); // sends an async request to the server
        
        return ajaxReq; // return the ajax-request object.
    }


    // function for updating a div "divName" on the webpage with a value "value".
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
                case homeTag: 
                    atHome = parseBoolean(value);
                    updateButtonText();
                    break;
                case fridgeTag:
                    let isFridgeOpen = parseBoolean(value);
                    updateFridgeText(isFridgeOpen);
                    break;
                /* cases for deprecated features related to the Lighting:
                case "LIGHT": 
                    let isLightOn = parseBoolean(value);
                    updateLightOnText(isLightOn);   
                    break;
                case "LIGHT_TIME":
                    let lightTime = value;
                    updateLightTimeText(lightTime);
                    break;
                case "LIGHT_AUTO":
                    // the webpage doesn't really care about this value.
                    break; 
                */
                case waitTimeTag:
                    waitTime = parseInt(value);
                    updateWaitTimeText(waitTime);   
                    break;
                case forgotLightTag:
                    let hasForgottenLight = parseBoolean(value);
                    updateForgotLightText(hasForgottenLight );   
                    break;
                case faucetTag:
                    let isFaucetRunning = parseBoolean(value);
                    updateFaucetRunningText(isFaucetRunning);   
                    break;
                case showerTag:
                    let isShowerRunning = parseBoolean(value);
                    updateShowerRunningText(isShowerRunning);   
                    break;
                case showerTimeTag:
                    let showerTime = value;
                    updateShowerTimeText(showerTime);   
                    break;
                default:
                    if (removedTags.includes(key)) {
                        // do nothing, we don't care about these
                    } 
                    else if (!(tagToDiv[key] === 'undefined')) { // check that the key is an actual key of tagToDiv, and isn't removed.
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

    // Functions to wrap the POST requests user interactions.
    // POST of the Home status: is user home, or have they left?
    function postHomeStatus(isHome) { 
        const path = homeTag;
        const apiExtension = "/API/" + path;
        let urlAPI = url + apiExtension;
        let isLeaving = !isHome; // isLeaving should be a 1 if user is leaving the house, 0 if user is returning. This is boolean negation of isHome.
        let ajaxReq = $.post(urlAPI, path + "=" + isLeaving); // send a post request with the value
        return ajaxReq;
    }

    function postWaitTime(waitTimeArg) { // send the waiting time argument, an integer representing how long the arduino should wait, to the server.
        const path = waitTimeTag;
        const apiExtension = "/API/" + path;
        let urlAPI = url + apiExtension;
        waitTimeArg = Math.ceil(waitTimeArg) // make sure the value is an integer (not a decimal value or bool).
        waitTimeArg = String(waitTimeArg) // Then turn it into a string.
        let ajaxReq = $.post(urlAPI, path + "=" + waitTime); // send a post request with the logic
        return ajaxReq;
    }
    
    // Specific functions for handling each div.
    // Could probably be a lot cleaner, since they all function the same way with a few exceptions (lightTime, showerTime).
    // and we should also store the messages in a better format, so it's easier to manage.
    function updateButtonText() {
        let divName = buttonDivName;
        let buttonText = atHome ? ("<b> I've left the house</b>.") : ("<b> I've returned</b>.");
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
        let divText = isFaucetRunning ? ("What are you doing on your phone? Aren't you using the sink right now?") : ("The faucet is not running &#129312")
        updateDiv(divName, divText)
    }

    function updateShowerRunningText(isShowerRunning) { 
        let divName = showerDivName;
        let divText = isShowerRunning ? ("Looking at the phone.. in the shower? I swear to God.<br>Oh, never mind. The showerhead's just dripping I guess") : ("The shower is not running. ")
        updateDiv(divName, divText)   
    }

    function updateShowerTimeText(showerTime) {
        let divName = showerTimeDivName;
        let divText = "You've showered " + showerTime + " minutes. Do your best to be as quick as possible! &#9201"; 
        updateDiv(divName, divText)
    }

    // When we have loaded the ip file, call this function:
    function onIPtextLoaded(ip_text) {
        // get the IP from the file, assign it to the variables.
        ip = ip_text.trim();
        url = protocol + ip + ":" + port
        console.log("URL IS: " + url);

        // Update the page with current server data, and then poll again continuously by interval:
        fetchAndSetData();
        const pollInterval = 15 * 1000; // 15 * 1000milliseconds = 15 seconds.
        setInterval( fetchAndSetData, pollInterval);

        // event listener for the "Left home" & "returned" button: 
        gebi("buttonLeftHouse").addEventListener("click", function (event) {
            console.log("Home status is: " + atHome)
            let divName = buttonDivName;
            const stallMsg = "loading";
            if (atHome === stallMsg) { // If user clicks on the button before it has finished sending and retrieving data from the server.
                alert("Still posting data. Check your connection to the server if this continues.");
            }
            else { // if we are not waiting for the response from the server:
                // set new text on the button
                // (should only be visible for a fraction of a second before it updates)
                updateDiv(divName, "Initiating procedures...");  
                $.when(postHomeStatus(atHome)).done(function() { // send post request and, when its done:
                    fetchAndSetData(); // fetch data, and update the page (also assigning the atHome value to data from the server.)         
                });
            }
            atHome = stallMsg; // assign the atHome value to this, so next time we click on it before the website has updated, it will give us a warning
        });


        // event listener for the "waiting time" range:
        gebi("waitTimeRange").addEventListener("change", function (event) {
            waitTime = event.currentTarget.value;
            updateWaitTimeText(waitTime);
            $.when(postWaitTime(waitTime)).done(function() { // send POST request and, when its done:
                fetchAndSetData() // fetch data, and update the page.
            })            
        });
    }
    
    let ip = "IP not loaded yet - IP" // global IP variable. Follow the guide document to set it up.
    let url =  "IP not loaded yet - URL" ; 
    fetch('ip.txt') // fetch the IP file 
        .then(response => response.text())
            .then(text => onIPtextLoaded(text));


}
