#include "NeroProducer/Nero/interface/NeroLogger.hpp"

void Logger::LogLevel(int level)
{
    if (level == 0) std::cout<<"[ERROR]";
    else if (level == 1) std::cout<<"[WARNING]";
    else if (level == 2) std::cout<<"[INFO]";
    else std::cout<<"[DEBUG]";
}

//void Logger::Log(const string& mex , int level)
//{
//    LogLevel(level);
//    std::cout <<":"<<mex<<endl;
//}

void Logger::Log(const std::string& mex , int level)
{
    if (level>verbosity_) return;
    LogLevel(level);
    std::cout <<":"<<mex<<std::endl;
    return;
}
