# This is a Python script for running the Server of EcoHome.
"""
    To set this server up properly, please follow the guide.
    CREDITS:
    Written by August Valentin, student ID: S194802

"""

from http.server import HTTPServer, BaseHTTPRequestHandler  # to run the server
from json import dumps  # To format strings into json format.
from time import time  # To get the current time


def read_IP_from_file():
    """ Reads the ip from the ip.txt file in 'webpage_lib' folder. """
    ip_file = "webpage_lib/ip.txt"
    with open(ip_file, "r") as f:
        contents = f.read()
    return contents.strip()


# placeholder value for IP:
ip = ""

# AFTER IP
port = 8081  # use this port. Should not be changed.

# Strings for the data tags. Use these variables as aliases to avoid typos in strings:
HOME = "HOME"
FRIDGE = "FRIDGE"
LIGHT = "LIGHT"
WAIT_TIME = "WAIT_TIME"
FORGOT_LIGHT = "FORGOT_LIGHT"
LIGHT_TIME = "LIGHT_TIME"
FAUCET = "FAUCET"
SHOWER = "SHOWER"
SHOWER_TIME = "SHOWER_TIME"
LIGHT_AUTO = "LIGHT_AUTO"

# Dictionary for all data stored on the server.
# This data sent & accessed with GET & POST requests:
stored_data = {HOME: "true",  # POST from webpage, GET from arduino
               FRIDGE: "false",  # POST from arduino
               LIGHT: "false",  # POST from arduino
               WAIT_TIME: "1",  # POST from webpage, GET from arduino.
               FORGOT_LIGHT: "false",  # POST from arduino
               LIGHT_TIME: "0",  # GET from webpage
               FAUCET: "false",  # POST from arduino
               SHOWER: "false",  # POST from arduino
               SHOWER_TIME: "0",  # GET from webpage
               LIGHT_AUTO: "false"  # POST from arduino when light shuts down automatically.
               }

# This data is for internal server use. Stores "when" the event was triggered. For example when the shower begun.
eventDates = {
    "SHOWER": 0,
    "LIGHT_AUTO": 0  # Deprecated.
}

# relative path to the folder with all the webpage files
document_path = "webpage_lib/"


def secondsToMinutes(sec):
    """ Function for converting a number (int, float) representing number of seconds, to minutes (rounded down). """
    minute = 60.0
    return round(sec / minute, 2)


def is_element_valid(element):
    """ Function to check if the element (received 'tag' string) is a key in the server data. """
    return element in stored_data.keys()


def str2bool(input_str):
    """ Convert a string to matching boolean. source: https://stackoverflow.com/a/1414175"""
    input_str = input_str.lower().strip()
    switcher = {  # home-brewed Python switch-statement.
        # Source: https://jaxenter.com/implement-switch-case-statement-python-138315.html
        # true cases
        "true": True,
        "yes": True,
        "1": True,
        # false cases
        "false": False,
        "no": False,
        "0": False
    }

    def default_case(inp_str):
        print(f"Check what you're sending, because {inp_str} doesn't fit our patterns.")
        return False

    switch_result = switcher.get(input_str, default_case)
    # Have to check this way,
    # because the default case should *only* be called if key couldn't be found in the dict.
    if switch_result == default_case:
        return default_case(input_str)
    else:
        return switch_result


class Payload:
    """ Class for storing the 'element' and 'result' of a POST request.
        For example: "HOME=true" is stored as element="HOME",result="true"
    """

    def __init__(self, element, result):
        self.element = element
        self.result = result


# Functions to handle specific POST requests in a certain way.
def handleShowerPOST(result):
    """ Handles the shower POST request. Keeps track of how long the user showers. """
    # Deprecated functionality: Stored the cumulative amount of time spent showering.
    # Changed to reset between each shower.

    is_being_switched_on = str2bool(result)
    was_switched_on = str2bool(stored_data[SHOWER])
    stored_data[SHOWER] = result
    if is_being_switched_on and not was_switched_on:  # if SHOWER turns on and was previously turned off:
        eventDates["SHOWER"] = int(time())
    elif was_switched_on and not is_being_switched_on:  # if SHOWER turns off and was previously turned on.
        # store the amount of time showered in the most recent shower.
        stored_data[SHOWER_TIME] = str(secondsToMinutes(time() - eventDates[SHOWER]))
        eventDates[SHOWER] = 0


def handleLightAutoPOST(result):
    """ DEPRECATED.
        Similarly to shower POST requests, keeps track of the time.
    """
    is_light_auto_switched_off = str2bool(result)
    if is_light_auto_switched_off:  # if LIGHT is automatically turned off on
        eventDates["LIGHT_AUTO"] = int(time())
        stored_data["LIGHT_AUTO"] = result  # Store that the light was turned off automatically.


def handleLightPOST(result):
    """ DEPRECATED.
        Works in tandem with LightAuto POST requests, to keep track of time.
    """
    stored_data["LIGHT"] = result  # Save the current status of light
    # and if it's turned on after being automatically turned off,
    # store the amount of time the light was turned off.
    is_being_switched_on = str2bool(result)
    if is_being_switched_on:  # the user no longer forgot to turn off the light
        stored_data["FORGOT_LIGHT"] = str(False)
        was_auto_switched_off = str2bool(stored_data["LIGHT_AUTO"])
        if was_auto_switched_off:  # if the light was previously turned off automatically.
            current_light_time = float(stored_data["LIGHT_TIME"])
            stored_data["LIGHT_TIME"] = str(current_light_time +
                                            secondsToMinutes(eventDates["LIGHT_AUTO"] - time()))
            eventDates["LIGHT"] = 0
            stored_data["LIGHT_AUTO"] = str(False)  # Store that the light was turned on (manually).


def handleWaitTimePost(result):
    """ Handles the shower WAIT_TIME request.
        converts the string to int, and ensures it is within the imposed limits (between 1 and 15 minutes).
        Then converts back to string when storing it.
    """
    #  ensure the wait time posted is between some predefined limits
    clamp_values = [1, 15]
    min_wait_time = min(clamp_values)
    max_wait_time = max(clamp_values)
    int_result = int(result)
    int_result = min(max(int_result, min_wait_time), max_wait_time)  # clamp the result
    str_result = str(int_result)  # convert back into string.
    handleDefaultPOST(WAIT_TIME, str_result)  # use the default method for handling the rest of the request.


def handleDefaultPOST(element, result):
    """ Handle the POST requests that do not have special circumstances.
        Just assigns the payload as it is received.
    """
    stored_data[element] = result


def handleValidPOST(server_o, payload):
    """ Wrapping function to handle a valid POST requests,
        choosing the "correct" way to handle the POST request.
    """
    element = payload.element
    result = payload.result
    # Special POST cases:
    switcher = {
        "SHOWER": handleShowerPOST,
        "LIGHT_AUTO": handleLightAutoPOST,
        "LIGHT": handleLightPOST,
    }
    handle_function = switcher.get(element, handleDefaultPOST)
    if handle_function == handleDefaultPOST:  # check if the function is the default case (needs 2 parameters)
        handleDefaultPOST(element, result)
    else:  # The special cases only needs 1 parameter
        handle_function(result)
    # Successful POST request.
    # Response:
    server_o.send_response(200)  # Tell the client that the request was successful.
    # If I understand the HTTP specifications correctly, it is optional for the server
    # to respond with a payload to a POST request
    content = "200, server handshake"
    server_o.send_header("Content-Length", f"{len(content)}")
    server_o.send_header("Content-type", "text/html")
    server_o.end_headers()
    server_o.wfile.write(bytes(content, 'utf-8'))


def handleInvalidPOST(server_o):
    """ Handles POST requests which are deemed invalid.
        Send response code 403, "Forbidden element".
        Not completely sure if its 100% correct,
        but we use it as a catch-all for invalid POST requests.
    """
    # Response:
    server_o.send_response(403)  #
    content = "403, server handshake"
    server_o.send_header("Content-Length", f"{len(content)}")
    server_o.send_header("Content-type", "text/html")
    server_o.end_headers()
    server_o.wfile.write(bytes(content, 'utf-8'))


class Serv(BaseHTTPRequestHandler):
    """ Class for running the server.
        Using the python http.server library, it inherits from the BaseHTTPRequestHandler class."""
    # class variable:
    API_path = "API"

    def do_POST(self):
        """ Function called when the server receives a POST request. Handles it as defined by us.
            The name is important: the server functions by calling do_<METHOD_NAME> upon receiving a request,
            where METHOD_NAME is specified by the request.
        """
        print(f"POST: path is: {self.path}")
        path_list = self.path.split("/")
        if path_list[1].strip() == Serv.API_path:  # check if API was accessed
            # Attempt to read the length of the content, with the "content_length" header:
            try:
                content_length = int(self.headers['Content-Length'])
            except TypeError:  # If no "content_length" header was found
                content_length = 0

            # read the content of the POST request (containing the payload)
            file_content = self.rfile.read(content_length)
            file_content = file_content.decode("UTF-8")
            print(f"POST payload contains: {file_content}")
            if "=" in file_content:
                parsed_data = file_content.split("=")  # parse the message.
                element = parsed_data[0].upper()
                result = parsed_data[1]
                if is_element_valid(element):
                    print("POST is valid")
                    payload = Payload(element, result)
                    # Handle the valid POST request:
                    handleValidPOST(self, payload)
                else:  # if element was not considered valid:
                    print(f"{element} is invalid and {result} will not be stored")
                    handleInvalidPOST(self)

            else:  # invalid POST, missing an "=" so can't determine what to assign the element to.
                print(f"POST is invalid, no '=' found")
                # Response:
                handleInvalidPOST(self)
        else:
            print("POST path is invalid")
            handleInvalidPOST(self)

    def do_GET(self):
        """ Function called when the server receives a GET request. Handles it as defined by us.
                    As with do_POST, the naming of the function is important.
        """
        file_to_open = None  # assign a placeholder value for the variable, so we can check if its been modified later on.
        file_ext = self.path.split(".")  # file extension of the requested file. if no file-extension is found,
        # assume either main page is requested, or some text-values.
        if len(file_ext) > 1:
            file_ext = file_ext[-1]  # find file extension by the last token in the string separated by ".".
            # e.g. file-extension of "jquery.min.js" is "js".
        else:
            file_ext = "html"  # use None instead?
        path_list = self.path.split("/")
        if self.path == '/':  # webpage is retrieved when no path is specified.
            self.path = "webpage.html"
            self.send_response(200)
        elif path_list[1] == "API":  # path is "/API"...
            print(f"API accessed. Path list is: {path_list}")
            if len(path_list) > 1 and path_list[2] != "":  # there needs to be an extra path after "/API"
                element = path_list[-1].upper()  # last part of the path.
                if element == "ALL":  # when we want to retrieve all the data stored in the server.
                    self.send_response(200)
                    file_to_open = dumps(stored_data)
                elif is_element_valid(element):  # when we want to retrieve a specific value from the server.
                    self.send_response(200)
                    json_data = stored_data[element]
                    print(f"GET data is: {json_data}")
                    file_to_open = json_data
                    self.send_header("Content-type", "text/plain")
                else:  # if element was deemed invalid.
                    print(f"{element} is invalid and cannot be accessed")
                    self.send_response(404)
            else:  # If the path was == "/API/"
                print(f"GET request is invalid")
                self.send_response(404)
            print(f"loaded data is {file_to_open}")
        else:  # if GET request refers to text-documents such as updatePage.js, jQuery.js, ip.txt.
            # example: "http://<ip>:8081/updatePage.js"
            self.path = self.path.split("HTTP/1.1")[0]
            self.send_response(200)
        try:  # Try to open the file and send it.
            if not file_to_open:
                file_to_open = open(document_path + self.path).read()
            # self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes(file_to_open, 'utf-8'))
        except Exception as err: # case of failure, give response code 403.
            print(f"exception: {err}")
            self.send_response(403)
            # Response:
            content = "403, server handshake"
            self.send_header("Content-Length", f"{len(content)}")
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes(content, 'utf-8'))
        print(f"path is: {self.path}")


# Main script:
if __name__ == '__main__':
    try:
        # read the ip from the specified text file.
        IP = read_IP_from_file()
        print(f"IP loaded from file is: {IP}")
        # Create an instance of our server-class with our parameters.
        httpd = HTTPServer((IP, port), Serv)
        print("SERVER BEGINS")
        # Start the server "forever", or until you force-exit the program (ctrl+c in terminal).
        httpd.serve_forever()
    except Exception as e:
        # Handles raised exceptions.
        if e == KeyboardInterrupt:
            # If the exception was due to keyboard interrupt (ctrl + c).
            print("Server was turned off")
        else:
            print("There's probably an issue with the ip.txt file")
            print(f"error is: {e}")
            IP = "Check the ip.txt file"

    # Server can be shutdown using Ctrl + c,
    # Alternatively, some IDEs have a red square button for stopping functions, somewhere.
