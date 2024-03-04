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

## Building

On Linux, you can build the beacon with: `bash make.sh`

On Windows, it would be: `make.bat` (I used MinGW but I am sure you can also use other compilers if you reconfigure the command. If you port this over, it would be cool if you contribute these changes!)

For some features, you need to put the following files into the static folder:

- For PwnKit: [CVE-2021-4034.py](https://raw.githubusercontent.com/joeammond/CVE-2021-4034/main/CVE-2021-4034.py) and rename it to `PwnKite.py`.
- You can download Procdump from [here](https://learn.microsoft.com/de-de/sysinternals/downloads/procdump) and rename it to `procdump.exe`.
- Go ahead and download Nyancat for Linux from [here](https://github.com/klange/nyancat/tree/master) and name it `nyancat`.

## Contribution

Feel free to contribute!

If you look at the top of the files, these are some of the features I would like to implement someday. As I developed this in my free time, this software also has some other trolling features, but I removed them as they are not too helpful in CTFs and I just played around a little. So if you want some weird trolling features, feel free to ask, and I can add them into a separate version.

## Disclaimer

This software is only intended for learning purposes, and I do not take any responsibility for misuse.

This repository is for academic purposes, and the use of this software is your responsibility.

## Ascii Art from:

[https://patorjk.com/software/taag/](https://patorjk.com/software/taag/)
