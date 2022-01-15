window.onload = function () {
    // '$' in the code is an alias for the global jQuery object. it is used to access the jQuery members.
    // can (possibly) use 'jQuery' instead, if desired.

    let url =  "http://192.168.101.1:8081"; // "/API/BUTTON" //"http://localhost:8081/API/BUTTON"; // "25.57.47.113:8081"; //'http://localhost:8081/API/BUTTON'; // example: 'http://echo.jsontest.com/Hello/world'
    const dataTags = ["HAS_LEFT", "FRIDGE"];
    const tagToDiv = {"HAS_LEFT" : "#has_left", "FRIDGE" : "#fridge" } // preface with '#'.

    const api_extension = "/API/all/" 

    function fetchData() {
        console.log("fetching data.");
        let urlAPI = url + api_extension;
        //console.log(urlAPI);
        ajaxreq = $.get(urlAPI, function(data) {
        });
        return ajaxreq; // return the ajax-request object.
    }

    function setPageData(data) { // data is a JSON object 
        console.log("setting data.");
        for (const [key, value] of Object.entries(data)) { // iterate through the data-object's key/value pairs.
            divName = tagToDiv[key] // find the name of the div from the key. i.e. #fridge = tagToDiv["FRIDGE"]  
            $(divName).html(value); // Assign the divs on the html page to the retrieved data
        }
    }

     // Fetch the latest data from the server, then update the webpage.
    function fetchAndSetData() {
        // when the data has been loaded, call the setPageData function. This is necessary because requests are done asynchronously.
        $.when(fetchData()).done(function(a1) {
            response_text = a1;
            response_text = JSON.parse(response_text); 
            //console.log(response_text);
            setPageData(response_text);
        })
    }

    // Call the function on page-load, and then poll again continuously by interval:
    fetchAndSetData();
    const pollInterval = 15 * 1000; // 15 seconds.
    setInterval( fetchAndSetData, pollInterval);
}
