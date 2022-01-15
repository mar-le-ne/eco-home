# This is a sample Python script.

from http.server import HTTPServer, BaseHTTPRequestHandler
from json import dumps
from socket import gethostname, gethostbyname
from time import time

# Data sent and accessed with the API:
stored_data = {"HOME": "true",  # POST from webpage, GET from arduino
               "FRIDGE": "false",  # POST from arduino
               "LIGHT": "false",  # POST from arduino
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

def secondsToMinutes(sec):
    minute = 60
    return round(sec / minute, 2)

document_path = "webpage_lib/"


def is_element_stored(element):
    return element in stored_data.keys()


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
            if is_element_stored(element):
                # Special POST cases:
                if element == "SHOWER":  # handle when status of SHOWER changes:
                    if result.lower() == "true" and stored_data[element].lower() == "false":  # if SHOWER turns on
                        eventDates[element] = time.time()
                    elif result.lower() == "false" and stored_data[element].lower() == "true":  # if SHOWER turns off
                        current_shower_time = float(stored_data["SHOWER_TIME"])
                        stored_data["SHOWER_TIME"] = str(current_shower_time +
                                                         secondsToMinutes(eventDates[element] - time.time()))
                        eventDates[element] = 0
                if element == "LIGHT_AUTO":  # Case of arduino automatically turning off the lights.
                    if result.lower() == "true" and stored_data["LIGHT"].lower() == "true":  # if SHOWER turns on
                        eventDates[element] = time.time()
                        stored_data[element] = result  # Store that the light was turned off automatically.
                if element == "LIGHT":
                    stored_data[element] = result  # Save the current status of light
                    # and if it's turned on after being automatically turned off,
                    # store the amount of time the light was turned off.
                    if result.lower() == "true" and stored_data["LIGHT_AUTO"].lower() == "true":
                        current_light_time = float(stored_data["LIGHT_TIME"])
                        stored_data["SHOWER_TIME"] = str(current_light_time +
                                                         secondsToMinutes(eventDates["LIGHT_AUTO"] - time.time()))
                        eventDates[element] = 0
                        stored_data["LIGHT_AUTO"] = "false"  # Store that the light was turned on manually.
                else:  # General POST case.
                    stored_data[element] = result
                # Succesful POST request.
                # Response:
                # print(f"Server will store {result} in {element}")
                self.send_response(200)
                # Response:
                content = "200, server handshake"
                self.send_header("Content-Length", len(content))
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(bytes(content, 'utf-8'))
            else:
                print(f"{element} is invalid and {result} will not be stored")
                self.send_response(403)
                # Response:
                content = "403, server handshake"
                self.send_header("Content-Length", len(content))
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
                elif is_element_stored(element):
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
            self.send_response(404)
        print(f"path is: {self.path}")


def load_binary(filename):
    with open(filename, 'rb') as file_handle:
        return file_handle.read()


if __name__ == '__main__':
    print("SERVER BEGINS")
    ip = '192.168.152.233'  # IP, insert using method from documentation.
    port = 8081  # use this port. Shouldn't be changed.
    httpd = HTTPServer((ip, port), Serv)  # '192.168.152.233' #  '192.168.101.1' # 'localhost'

    httpd.serve_forever()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/


""" Old code:
    host_name = gethostname()
    local_ip = gethostbyname("localhost") # host_name
    print(f"hostname is {host_name}")
    print(f"PC's Local IP Is: {local_ip}")
    # local_ip = "localhost
"""