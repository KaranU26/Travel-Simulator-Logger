//Created by Karandeep Ubhi

using namespace std;

enum LOG_LEVEL {
    DEBUG = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL = 3
};

int InitializeLog();
void SetLogLevel(LOG_LEVEL level);
void Log(LOG_LEVEL level, const char *prog, const char *func, int line, const char *message);
void ExitLog();
void threadReciever(int fd);
