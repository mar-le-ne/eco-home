window.onload = function () {
    // '$' in the code is an alias for the global jQuery object. it is used to access the jQuery members.
    // can (possibly) use 'jQuery' instead, if desired.

    let url =  "http://192.168.101.1:8081"; // "/API/BUTTON" //"http://localhost:8081/API/BUTTON"; // "25.57.47.113:8081"; //'http://localhost:8081/API/BUTTON'; // example: 'http://echo.jsontest.com/Hello/world'
    const dataTags = ["HAS_LEFT", "FRIDGE", "HOME"];
    const tagToDiv = {"HAS_LEFT" : "#has_left", "FRIDGE" : "#fridge" } // preface with '#'.



    function gebi(id) { // alias for "get element by id":
        return document.getElementById(id);
    }

    function fetchData() {
        console.log("fetching data.");
        const api_extension = "/API/all/";
        let urlAPI = url + api_extension;
        //console.log(urlAPI);
        let ajaxreq = $.get(urlAPI, function(data) {
        });
        return ajaxreq; // return the ajax-request object.
    }

    
    let atHome = true; // bool relating to whether user is home or not. updated on fetch.
    function setPageData(data) { // data is a JSON object 
        console.log("setting data.");
        for (const [key, value] of Object.entries(data)) { // iterate through the data-object's key/value pairs.
            if (dataTags.includes(key)) {
                if (!(tagToDiv[key] === 'undefined')) { // check that the key is an actual key of tagToDiv.
                    divName = tagToDiv[key]; // find the name of the div from the key. i.e. #fridge = tagToDiv["FRIDGE"] 
                    $(divName).html(value); // Assign the divs on the html page to the retrieved data
                }
                if (key == "HOME") {
                    atHome = Boolean(parseInt(value));
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


    function postHomeStatus(isLeaving) { // isLeaving should be a 1 if user is leaving the house (isHome), 0 if user is returning. 
        const api_extension = "/API/HOME";
        let urlAPI = url + api_extension;
        let ajaxreq = $.post(urlAPI, {"HOME" : isLeaving}); // 
        return ajaxreq;
    }

    // Button for "Left home" & "returned":
    gebi("buttonLeftHouse").addEventListener("click", function (event) {
        let divName = "#buttonLeftHouse";
        $(divName).html("Initiating procedures...");
        $.when(postHomeStatus(atHome)).done(function() { // send post request and, when its done:
            $.when(fetchAndSetData()).done( function() { // fetch data. 
                // and when fetch is done, update the button's text.
                if (atHome) {
                    $(divName).html("I've left the house.");
                }
                else {
                    $(divName).html("I've returned");
                }
            })            
        })
    });


    // Call the function on page-load, and then poll again continuously by interval:
    fetchAndSetData();
    const pollInterval = 15 * 1000; // 15 seconds.
    setInterval( fetchAndSetData, pollInterval);
}
