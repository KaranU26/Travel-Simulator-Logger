//Created by Karandeep Ubhi

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
pthread_mutex_t logMutex;
struct sockaddr_in clientAddress, serverAddress;
string ipAddress = "127.0.0.1";
bool is_running = true;
bool thread_is_running = true;
const int MAX_BUF_LEN = 256;
char buf[MAX_BUF_LEN];
const int port = 3091;
const char* path = "./logfile.txt";


//Handle Ctrl-C and SIGINT signal, gracefully close application

static void signalHandler(int sig){

    switch (sig)
    {
    case SIGINT:
        cout << "SIGINT: Signal has been handled!" << endl;
        is_running = false;
        thread_is_running = false;
        break;
    
    default:
        cout << "Unhandled signal detected!" << endl;
    }
}

//Thread Function

void threadReciever(int fd){

    struct timeval timeout;
    timeout.tv_usec = 0;
    timeout.tv_sec = 1;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (thread_is_running)
    {
        FILE* file = fopen(path, "a");
        memset(buf, 0, MAX_BUF_LEN);
        pthread_mutex_lock(&logMutex);
        socklen_t clientAddressLength = sizeof(clientAddress);

        int size = recvfrom(fd, buf, MAX_BUF_LEN, 0, (struct sockaddr*)&clientAddress, &clientAddressLength);
        string msg = buf;

        if (size > 0)
        {
            fprintf(file, "%s", msg.c_str()); 

        }else{
            sleep(1);
        }
        fclose(file);
        pthread_mutex_unlock(&logMutex);
        
    }
    
}

//Set log level interface for user to filter which logs they want to see

void setLogLevel(){

    int input;

    cout << "Choose a level" << endl;
    cout << "--------------" << endl;
    cout << "1 - DEBUG" << endl;
    cout << "2 - WARNING" << endl;
    cout << "3 - ERROR" << endl;
    cout << "4 - CRITICAL" << endl;
    cout << "0 - EXIT" << endl;
    cout << "Your Choice: ";
    cin >> input;
    
    switch (input)
    {
    case (1):
    {
        memset(buf, 0, MAX_BUF_LEN);
        int len = sprintf(buf, "Set Log Level=%s", "DEBUG")+1;
        sendto(socket_fd, buf, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        
    }break;
    
    case (2):
    {
        memset(buf, 0, MAX_BUF_LEN);
        int len = sprintf(buf, "Set Log Level=%s", "WARNING")+1;
        sendto(socket_fd, buf, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
    }break;
    
    case (3):
    {
        memset(buf, 0, MAX_BUF_LEN);
        int len = sprintf(buf, "Set Log Level=%s", "ERROR")+1;
        sendto(socket_fd, buf, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
    }break;
    
    case (4):
    {
        memset(buf, 0, MAX_BUF_LEN);
        int len = sprintf(buf, "Set Log Level=%s", "CRITICAL")+1;
        sendto(socket_fd, buf, len, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
    }break;
    
    case (0):
        break;
    default:
        cout << "Invalid Choice!" << endl;
    }
}

int main(){

    int input;

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(socket_fd < 0){
        cout << "ERROR - Socket not created" << endl;
        exit(1);
    }

    struct sigaction action;
    action.sa_handler = signalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    int sigact = sigaction(SIGINT, &action, NULL);

    pthread_mutex_init(&logMutex, NULL);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    serverAddress.sin_port = htons(port);
    bind(socket_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    thread recvThread(threadReciever, socket_fd); 
    recvThread.detach();
    
    while(is_running){

        cout << "Pick and option:" << endl;
        cout << "----------------" << endl;
        cout << "1 - Set Log Level" << endl;
        cout << "2 - Dump the log file" << endl;
        cout << "0 - Shut down" << endl;
        cout << "Please enter option: ";
        cin >> input;

        switch (input)
        {
        case (1):
            setLogLevel();
            break;
        case (2):
        {
            FILE* file = fopen(path, "r");
            size_t lineLength = 0;
            char* logLine = NULL;

            while ((getline(&logLine, &lineLength, file)) != -1)
            {
                cout << logLine << endl;
            }
            fclose(file);
            cout << "Press Enter to Continue"; //doesnt want to work for some reason
            cin.ignore();
        }break;
        case (0):
            is_running = false;
            thread_is_running = false;
            recvThread.join();
            break;
        default:
            break;
        }

    }
}