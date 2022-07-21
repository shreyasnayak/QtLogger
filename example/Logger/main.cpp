#include <QCoreApplication>
#include <logger.h>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger *logger = Logger::GetInstance("App-Prefix");
    logger->setLogLevel(Logger::Info);
    logger->deleteLogFiles(15); // Delete log file older than 15 days
    logger->logMessage(Logger::Info,__FILE__,"Application is started sucessfully");
    std::cout << "File Path : " << logger->getLogFilePath().toStdString() << std::endl;
    return a.exec();
}
