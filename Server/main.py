# This is a sample Python script.

from http.server import HTTPServer, BaseHTTPRequestHandler
from json import dumps
from socket import gethostname, gethostbyname


stored_data = {"HAS_LEFT": "5000!",
        "FRIDGE": "false"}


class Serv(BaseHTTPRequestHandler):

    def do_POST(self):
        print("SOMETHING WAS POSTED!")
        self.send_response(200)
        print(f"path is: {self.path}")
        content_length = int(self.headers['Content-Length'])
        file_content = self.rfile.read(content_length)

        file_content = file_content.decode("UTF-8")
        print(f"File contains: {file_content}")
        if ("=" in file_content):
            parsed_data = file_content.split("=")  # do some JSON stuff here.
            element = parsed_data[0]
            result = parsed_data[1]
            stored_data[element] = result

    def do_GET(self):
        file_to_open = ""
        if self.path == '/':
            self.path = '/webpage.html'
            self.send_response(200)
            file_to_open = open(self.path[1:]).read()
        elif (self.path.split('/')[1] == "API"):
            self.send_response(200)
            file_to_open = dumps(stored_data)
            print("API accessed.")
        else:
            self.path = self.path.split("HTTP/1.1")[0]
            file_ext = self.path.split(".")[-1]  # get the last element in the string separated by '.'
            self.send_response(200)

        try:
            if not file_to_open:
                file_to_open = open(self.path[1:]).read()
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes(file_to_open, 'utf-8'))

        except:
            file_to_open = "File not found"
            self.send_response(404)
        print(f"path is: {self.path}")


def load_binary(filename):
    with open(filename, 'rb') as file_handle:
        return file_handle.read()


import argparse
import logging
import socket
from time import sleep

from zeroconf import IPVersion, ServiceInfo, Zeroconf
if __name__ == '__main__':
    print("TEST")
    host_name = gethostname()
    local_ip = gethostbyname(host_name)
    print(f"PC's Local IP Is: {local_ip}")
    #local_ip = "localhost"
    port = 8081
    print(f"local ip is: {local_ip}")
    httpd = HTTPServer(("192.168.101.1", port), Serv)  # '192.168.152.233' # 'localhost'

    httpd.serve_forever()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
