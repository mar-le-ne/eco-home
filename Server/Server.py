# This is a Python script for running the Server of EcoHome.


# from socket import gethostname, gethostbyname
from http.server import HTTPServer, BaseHTTPRequestHandler
from json import dumps
from time import time



def read_IP_from_file():
    ip_file = "webpage_lib/ip.txt"
    with open(ip_file, "r") as f:
        contents = f.read()
    return contents

# BEFORE IP
# ip = "192.168.152.233"  # IP, insert using method from documentation.
ip = "1"


# AFTER IP
port = 8081  # use this port. Shouldn't be changed.

# Data sent and accessed with the API:
stored_data = {"HOME": "true",  # POST from webpage, GET from arduino
               "FRIDGE": "false",  # POST from arduino
               "LIGHT": "false",  # POST from arduino
               "FORGOT_LIGHT": "false",  # POST from arduino
               "LIGHT_TIME": "0",  # GET from webpage
               "FAUCET": "false",  # POST from arduino
               "SHOWER": "false",  # POST from arduino
               "SHOWER_TIME": "0",  # GET from webpage
               "LIGHT_AUTO": "false"  # POST from arduino when light shuts down automatically.
               }
# Data the server handles.
eventDates = {
    "SHOWER": 0,
    "LIGHT_AUTO": 0
}

document_path = "webpage_lib/"


def secondsToMinutes(sec):
    minute = 60
    return round(sec / minute, 2)


def is_element_valid(element):
    return element in stored_data.keys()

def str2bool(input_str):  # Convert a string to matching boolean. source: https://stackoverflow.com/a/1414175
    input_str = input_str.lower().strip()
    switcher = { # home-brewed Python switch-statement.
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
    def default_case(input_str):
        print(f"Check what you're sending, because {input_str} doesn't fit our patterns.")
        return False
    return switcher.get(input(input_str), default_case(input_str))


class Serv(BaseHTTPRequestHandler):
    def do_POST(self):
        print(f"POST: path is: {self.path}")

        try:
            content_length = int(self.headers['Content-Length'])
        except TypeError:
            content_length = 0
        file_content = self.rfile.read(content_length)

        file_content = file_content.decode("UTF-8")
        # print(f"File contains: {file_content}")
        if "=" in file_content:
            parsed_data = file_content.split("=")  # do some JSON stuff here.
            element = parsed_data[0].upper()
            result = parsed_data[1]
            if is_element_valid(element):
                # Special POST cases:
                if element == "SHOWER":  # handle when status of SHOWER changes:
                    is_being_switched_on = str2bool(result)
                    was_switched_on = str2bool(stored_data[element])
                    if is_being_switched_on and not was_switched_on:  # if SHOWER turns on and was previously turned off:
                        eventDates[element] = int(time())
                    elif was_switched_on and not is_being_switched_on:  # if SHOWER turns off and was previously turned on.
                        current_shower_time = float(stored_data["SHOWER_TIME"])
                        stored_data["SHOWER_TIME"] = str(current_shower_time +
                                                         secondsToMinutes(eventDates[element] - time()))
                        eventDates[element] = 0
                if element == "LIGHT_AUTO":  # Case of arduino automatically turning off the lights.
                    is_light_auto_switched_off = str2bool(result)
                    if is_light_auto_switched_off:  # if LIGHT is automatically turned off on
                        eventDates[element] = int(time())
                        stored_data[element] = result  # Store that the light was turned off automatically.
                if element == "LIGHT":
                    stored_data[element] = result  # Save the current status of light
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
                            eventDates[element] = 0
                            stored_data["LIGHT_AUTO"] = str(False)  # Store that the light was turned on (manually).
                else:  # General POST case.
                    stored_data[element] = result
                # Successful POST request.
                # Response:
                # print(f"Server will store {result} in {element}")
                self.send_response(200)
                # Response:
                content = "200, server handshake"
                self.send_header("Content-Length", f"{len(content)}")
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(bytes(content, 'utf-8'))
            else:
                print(f"{element} is invalid and {result} will not be stored")
                self.send_response(403)
                # Response:
                content = "403, server handshake"
                self.send_header("Content-Length", f"{len(content)}")
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(bytes(content, 'utf-8'))

    def do_GET(self):
        file_to_open = None
        file_ext = self.path.split(".")  # file extension of the requested file. if no file-extension is found,
        # assume either main page is requested, or some text-values.
        if len(file_ext) > 1:
            file_ext = file_ext[-1]  # find file extension by the last token in the string separated by ".".
            # e.g. file-extension of "jquery.min.js" is "js".
        else:
            file_ext = "html"  # use None instead?

        if self.path == '/':
            self.path = "webpage.html"
            self.send_response(200)
        elif self.path.split('/')[1] == "API":  # "/API"...
            print("API accessed.")
            path_list = self.path.split("/")
            if len(path_list) > 1 and path_list[2] != "":
                element = path_list[-1].upper()  # last part of the path.
                if element == "ALL":
                    self.send_response(200)
                    file_to_open = dumps(stored_data)
                elif is_element_valid(element):
                    self.send_response(200)
                    file_to_open = dumps(stored_data[element])
                else:
                    print(f"{element} is invalid and cannot be accessed")
                    self.send_response(404)
            print(f"loaded data is  {file_to_open}")
        else:  # for GET requests to documents such as updatePage.js or jQuery.js
            self.path = self.path.split("HTTP/1.1")[0]
            self.send_response(200)
        try:
            if not file_to_open:
                file_to_open = open(document_path + self.path).read()
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes(file_to_open, 'utf-8'))
        except Exception as e:
            file_to_open = "File not found"
            print(f"exception: {e}")
            self.send_response(403)
            # Response:
            content = "403, server handshake"
            self.send_header("Content-Length", f"{len(content)}")
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes(content, 'utf-8'))
        print(f"path is: {self.path}")


def load_binary(filename):
    with open(filename, 'rb') as file_handle:
        return file_handle.read()


if __name__ == '__main__':
    try:
        IP = read_IP_from_file()
        httpd = HTTPServer((IP, port), Serv)
        print("SERVER BEGINS")
    except Exception as e:
        print("There's probably an issue with the ip.txt file")
        print(f"error is: {e}")
        IP = "Check the ip.txt file"
    httpd.serve_forever()
    # Server can be shutdown using Ctrl + C,
    # Alternatively, some IDEs have a red square for stopping functions.

""" Old code:
    host_name = gethostname()
    local_ip = gethostbyname("localhost") # host_name
    print(f"hostname is {host_name}")
    print(f"PC's Local IP Is: {local_ip}")
    # local_ip = "localhost
"""
