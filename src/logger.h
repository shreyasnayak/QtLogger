#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QTextStream>

#ifdef WIN
    #define LINEFEED "\r\n"
#else
    #define LINEFEED "\n"
#endif


class Logger : public QObject
{
    Q_OBJECT
public:
    enum LogLevel
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

private:
    static Logger* _pInstance;
    static bool _enSysLog;
    Logger::LogLevel _logLevel;
    QString _logFileName,_logDirPath;
    QFile* _plogFile;
    explicit Logger(QString fileNamePrefix=QString(),QObject *parent = nullptr);
    static void systemLog(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    QString logLevelToString(const LogLevel &level);

public:
    static Logger *GetInstance(const QString &fileNamePrefix = QString(),QObject* parent = nullptr);
    ~Logger();
    void setLogLevel(Logger::LogLevel logLevel);
    void setLogLevelInInt(int logLevel);
    void deleteLogFiles(unsigned int olderThanDays);
    void logMessage(const LogLevel &level,const QString &loggedBy,const QString &message);
    Logger::LogLevel getLogLevel();
    int getLogLevelInInt();
    QString getLogFilePath();
    QString getLogFolderPath();
};

#endif // LOGGER_H
