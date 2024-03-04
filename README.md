# cloudstrike
This is a custom c2 framework with beacons for both linux and windows

<pre>
  ______  __        ______    __    __   _______       _______.___________..______       __   __  ___  _______ 
 /      ||  |      /  __  \  |  |  |  | |       \     /       |           ||   _  \     |  | |  |/  / |   ____|
|  ,----'|  |     |  |  |  | |  |  |  | |  .--.  |   |   (----`---|  |----`|  |_)  |    |  | |  '  /  |  |__   
|  |     |  |     |  |  |  | |  |  |  | |  |  |  |    \   \       |  |     |      /     |  | |    <   |   __|  
|  `----.|  `----.|  `--'  | |  `--'  | |  '--'  |.----)   |      |  |     |  |\  \----.|  | |  .  \  |  |____ 
 \______||_______| \______/   \______/  |_______/ |_______/       |__|     | _| `._____||__| |__|\__\ |_______|

</pre>

I created this framework to learn a bit about malware developments and command and control servers. As this is (at the time of release) not detected by some antivirus software like Windows Defender, you can also see that it is not too hard to write malware that goes undetected even though I didn't even obfuscate too much. Obviously, the beacons will probably still be detected by an antivirus with good dynamic detection, so this malware is not something harmful. BUT you can use this as a base for lateral movement in CTFs to manage your machines.

## Features
### C2

To install the requirements:
<pre>pip install -r requirements.txt</pre>

The C2 also has a fallback to the default http.server module instead of flask if you can't install flask and you can also disable it by setting `FLASK`=False in `cloudstrike.py` but this will make the beacon fail to download files from the webserver so you would need to change some lines in main.c. Feel free to advance the fallback so the beacon automatically chooses from where to download and contribute the changes!  

You also need to configure the following constants in `cloudstrike.py`  
`C2_ADDR`="0.0.0.0"  
`C2_PORT`=1337  
`C2_WEB_PORT`=1337  
`HOME`="/home/user/cloudstrike/" #needs to end with a '/' !!!

You can leave the C2_WEB_PORT and C2_PORT the same as the c2 distinguishes between traffic with the web server and clients.

You can access the web server at `C2_ADDR:C2_WEB_PORT/{magic}/some file`. You can get the current magic string from the cloudstrike command `magic`.

#### Available Functions
- `use <id>` : if you have connected clients you can use this to drop into the shell
- `list/ps` : list current connected clients
- `delete/kill <id>` : kill connection to client
- `connect <ip> <port>` : connnect to bind shell
- `listen <port>` : add an additional C2 PORT to listen for clients
- `forward local_port:dest:dest_port` : forward local port to remote service
- `routes` : keeps track of your forwards and displays the current port forwardings you are doing
- `restart web` : restart the web server (may not work as this was intended only if you want to start the webserver after the webserver didn't start initially)
- `read_weblog` : read flask web log
- `magic` : display current magic string
- `clear` : clear screen
- `broadcast <command>` : send command to all clients at the same time
- `exit`

### Beacon
The Beacon supports an encoded channel with xor (NO this is not secure but it can obfuscate the traffic) and a plain channel. You can configure this in `es2.h`.

#### Available Functions
- `implant`: execute persistence mechanism specific to the operating system
- `upload`: upload file (binary files currently only partially supported and only works in combination to cloudstrike)
- `download`: download file(binary files currently only partially supported and only works in combination to cloudstrike)
- `destroy`: destroys the machines files
- `panic`: removes itself quietly from the system(only for windows as it is easy for linux to do it manually)
- `pyinstall`: installs python on the target(windows only because for linux python is most of the time preinstalled)
- `passdump`: dumps creds (see the tip in the code to see how to use mimikatz to get the creds from the dump and this is really noisy and will propably be detected by an av)
- `crash`: crashes system with a fork bomb
- `exit`
- `help`

## Building

On Linux, you can build the beacon with: `bash make.sh`

On Windows, it would be: `make.bat` (I used MinGW but I am sure you can also use other compilers if you reconfigure the command. If you port this over, it would be cool if you contribute these changes!)  

After installing you can use patch.sh to change a previous `C2_HOST` to a new one without rebuilding the whole binary

For some features, you need to put the following files into the static folder:

- For PwnKit: [CVE-2021-4034.py](https://raw.githubusercontent.com/joeammond/CVE-2021-4034/main/CVE-2021-4034.py) and rename it to `PwnKite.py`.
- You can download Procdump from [here](https://learn.microsoft.com/de-de/sysinternals/downloads/procdump) and rename it to `procdump.exe`.
- Go ahead and download Nyancat for Linux from [here](https://github.com/klange/nyancat/tree/master) and name it `nyancat`.
- Just grab a windows python installer executable for windows from here: [here](https://www.python.org/downloads/release/python-385/) you can also use another version but then you need to change the name in `main.c` from `python-3.8.5.exe` to your executable filename

## Contribution

Feel free to contribute!

If you look at the top of the files, there are some of the features I would like to implement someday. As I developed this in my free time, this software also has some other trolling features, but I removed them as they are not too helpful in CTFs and I just played around a little. So if you want some weird trolling features, feel free to ask, and I can add them into a separate version.

## Disclaimer

This software is only intended for learning purposes, and I do not take any responsibility for misuse.

This repository is for academic purposes, and the use of this software is your responsibility.

## Ascii Art from:

[https://patorjk.com/software/taag/](https://patorjk.com/software/taag/)
