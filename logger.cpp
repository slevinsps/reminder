#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
void add_to_log(const char* name_func, QString str)
{
    QDateTime dt;
    dt = QDateTime::currentDateTime();
    QString path = QDir::currentPath().append("/log/logs.txt");
    QFile log_file(path);
    if (log_file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&log_file);
        outstream << QString(name_func) << " ; ";
        outstream << str << " ; ";
        outstream << dt.date().day() << "." << dt.date().month() << "." << dt.date().year() << " ; " <<dt.time().hour() << ":" << dt.time().minute();
        outstream << "\n";
        log_file.close();
    }
}
