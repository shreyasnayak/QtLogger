/************************************************************************
 *
 *   Trace - Only when I would be "tracing" the code and trying to find one part of a function specifically.
 *   Debug - Information that is diagnostically helpful to people more than just developers (IT, sysadmins, etc.).
 *   Info - Generally useful information to log (service start/stop, configuration assumptions, etc). Info I want to always have available but usually don't care about under normal circumstances. This is my out-of-the-box config level.
 *   Warn - Anything that can potentially cause application oddities, but for which I am automatically recovering. (Such as switching from a primary to backup server, retrying an operation, missing secondary data, etc.)
 *   Error - Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.). These errors will force user (administrator, or direct user) intervention. These are usually reserved (in my apps) for incorrect connection strings, missing services, etc.
 *   Fatal - Any error that is forcing a shutdown of the service or application to prevent data loss (or further data loss). I reserve these only for the most heinous errors and situations where there is guaranteed to have been data corruption or loss.
****************************************************************************/

#include "logger.h"
#include <iostream>

Logger* Logger::_pInstance = nullptr;


Logger::Logger(QString fileNamePrefix, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Logger::LogLevel>("LogLevel");

    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    /* If there is No writableLocation put it in home folder */
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();

    /* append Log Directory Name */
    _logDirPath = QDir::toNativeSeparators(directory.append("/Logs"));

    /* Create Log Directory */
    QDir myPath;
    myPath.mkpath(_logDirPath);

    if(!fileNamePrefix.isEmpty()) fileNamePrefix.append("_");

    /* Create Log File Name */
    _logFileName = QDir::toNativeSeparators(_logDirPath +"/"+qApp->applicationName()+"_"+fileNamePrefix+QString("_%1.log").arg(QDateTime::currentDateTime().date().toString(Qt::ISODate)));

    /* Create Log File */
    if (!_logFileName.isEmpty())
    {
        _plogFile = new QFile(_logFileName,this);

        if (!_plogFile->open(QIODevice::WriteOnly | QIODevice::Append))
        {
            fprintf(stderr, "Failed to create Log File");
        }
    }
    #ifdef EN_SYSTEM_LOG
        qInstallMessageHandler(Logger::systemLog);
    #endif
}

void Logger::systemLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(_pInstance)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type)
        {
        case QtDebugMsg:
            Logger::_pInstance->logMessage(Logger::Debug,"System",msg);
            break;
        case QtInfoMsg:
            Logger::_pInstance->logMessage(Logger::Info,"System",msg);
            break;
        case QtWarningMsg:
            Logger::_pInstance->logMessage(Logger::Warn,"System",msg);
            break;
        case QtCriticalMsg:
            Logger::_pInstance->logMessage(Logger::Error,"System",msg);
            break;
        case QtFatalMsg:
            Logger::_pInstance->logMessage(Logger::Fatal,"System",msg);
        }
    }
    Q_UNUSED(context)
}

QString Logger::logLevelToString(const Logger::LogLevel &level)
{
    switch(level)
    {
    case LogLevel::Trace : return  " Trace";
    case LogLevel::Debug : return  " Debug";
    case LogLevel::Info  : return  " Info ";
    case LogLevel::Warn  : return  " Warn ";
    case LogLevel::Error : return  " Error";
    case LogLevel::Fatal : return  " Fatal";
    }
    return "Unknown";
}

Logger *Logger::GetInstance(const QString &fileNamePrefix, QObject *parent)
{
    if(Logger::_pInstance == nullptr)
    {
        Logger::_pInstance = new Logger(fileNamePrefix,parent);
    }
    return Logger::_pInstance;
}

Logger::~Logger()
{
    _plogFile->close();
    delete _plogFile;
    Logger::_pInstance = nullptr;
}

void Logger::setLogLevel(Logger::LogLevel logLevel)
{
    _logLevel = logLevel;
}

void Logger::setLogLevelInInt(int logLevel)
{
    if(logLevel>=0 && logLevel<6)
    {
        _logLevel = static_cast<LogLevel>(logLevel);
    }
    else
    {
         emit logMessage(Logger::Error,"Logger_Class","Invalid log level, Loglevel can have 0 to 5");
    }
}

void Logger::logMessage(const LogLevel &level, const QString &writtenBy, const QString &message)
{
    if(_plogFile->isOpen() && (level >= _logLevel))
    {
        QTextStream out(_plogFile);
        QByteArray log= (logLevelToString(level) +" | "+ QDateTime::currentDateTime().time().toString() +" | "+ writtenBy.leftJustified(25,' ') + " | " + message + " "+ LINEFEED ).toUtf8();
        out << log;

        #ifdef PRINT_LOG_ON_CONSOLE
            fprintf(stderr, "%s", log.constData());
        #endif
    }
    if(level==LogLevel::Fatal)
    {
        _plogFile->close();
        delete _plogFile;
        qFatal("Closing application duto fatal error,Please check log file");
    }
}

Logger::LogLevel Logger::getLogLevel()
{
    return _logLevel;
}

int Logger::getLogLevelInInt()
{
    return _logLevel;
}

QString Logger::getLogFilePath()
{
    return _logFileName;
}

QString Logger::getLogFolderPath()
{
    return  _logDirPath;
}

void Logger::deleteLogFiles(unsigned int olderThanDays)
{
    const QDate today = QDate::currentDate();
    Q_FOREACH (QFileInfo fileInfo, QDir(_logDirPath).entryInfoList(QDir::Files))
    {
        if (fileInfo.metadataChangeTime().date().daysTo(today) > olderThanDays)
        {
            QString filepath = fileInfo.absoluteFilePath();
            QDir deletefile;
            deletefile.setPath(filepath);
            deletefile.remove(filepath);
        }
    }
}
