#ifndef NERO_LOGGER_H
#define NERO_LOGGER_H
#include <iostream>
#include <sstream>
#include <string>

class Logger{
    private:
        Logger() {}; // Singleton
        Logger(Logger const &) = delete; // Singleton
        void operator=(Logger const&)=delete; // Singleton
        void LogLevel(int level);

    public:
         ~Logger(){}; // Singleton
        static Logger & getInstance(){  static Logger l ; return l;} // Singleton
       
        // ---- 
        enum Status { ERROR=0, WARNING=1, INFO=2, DEBUG=3 };
        int verbosity_{1};
        // ---- MEMBER FUNCTIONS
        void Log(const std::string& mex, int level);
};
#endif
