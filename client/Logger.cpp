//Created by Karandeep Ubhi

//Header to include Logger.h File
#include "Logger.h"

//Header file for sockets
#include <sys/socket.h>

//Header file for a number of things used for sockaddr_in
#include <netinet/in.h>

//Header file for Input and output
#include <iostream>

//Header file for inet services like in_port
#include <arpa/inet.h> 

//Header file for threads
#include <thread>

//Header files for pthreads (similar to threads)
#include <pthread.h>

//Header file for certain miscellaneous constants and types
#include <unistd.h>

//Header file for strings
#include <string.h>

//Header for using mutexes in program
#include <mutex>

//Header for signals
#include <signal.h>

using namespace std;

int socket_fd;
struct sockaddr_in serverAddress;
string ipAddress = "127.0.0.1";
const int port = 3091;
socklen_t socketLength;
LOG_LEVEL globalLevel = ERROR;
bool is_running = true;
const int MAX_BUF_LEN = 256;
char buf[MAX_BUF_LEN];
pthread_mutex_t logMutex;
pthread_t tid;

//Initializing the log

int InitializeLog(){

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    serverAddress.sin_port = htons(port);
    socketLength = sizeof(serverAddress);

    pthread_mutex_init(&logMutex, NULL);

    thread recvThread(threadReciever, socket_fd);
    recvThread.detach(); 

}

//Log Exit function

void ExitLog(){

    is_running = false;
    close(socket_fd);
}


//Setting the Log Level

void SetLogLevel(LOG_LEVEL level){

    globalLevel = level;
}

//Log function 

void Log(LOG_LEVEL level, const char *prog, const char *func, int line, const char *message){
    

    if (level >= globalLevel) 
    {
        //tried adding mutexing here but it seems stop the program completely
        time_t now = time(0);
        char *dt = ctime(&now);
        
        memset(buf, 0, MAX_BUF_LEN);
        char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"};
        int dataLength = sprintf(buf, "%s %s %s:%s:%d %s\n\n", dt, levelStr[level], prog, func, line, message)+1;
        buf[dataLength-1]='\0';

        sendto(socket_fd, buf, dataLength, 0, (struct sockaddr*)&serverAddress, socketLength);

    }
    
}

//Spawned Thread function.

void threadReciever(int fd){

    struct timeval timeout;
    timeout.tv_usec = 0;
    timeout.tv_sec = 1;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (is_running)
    {
        memset(buf, 0, MAX_BUF_LEN);
        pthread_mutex_lock(&logMutex);
        int size = recvfrom(fd, buf, MAX_BUF_LEN, 0, (struct sockaddr*)&serverAddress, &socketLength);
        string msg = buf;

        if (size > 0)
        {
            string log;

            log = msg.substr((msg.find("=") + 1));

            if (log == "DEBUG")
            {
                globalLevel = DEBUG;

            }else if(log == "WARNING"){

                globalLevel = WARNING;

            }else if(log == "ERROR"){

                globalLevel = ERROR;
            
            }else if(log == "CRITICAL"){

                globalLevel = CRITICAL;

            }else{
                continue;
            }
            

        }else{
            sleep(1);
        }
        msg.clear();
        pthread_mutex_unlock(&logMutex);
    }
    
}