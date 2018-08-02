#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qDebug>
#include <QDir>
#include <stdio.h>
#include <QTimer>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


        m_trayIcon = new QSystemTrayIcon( QIcon( QDir::currentPath().append("/res/smile.ico") ), this );
        connect(
            m_trayIcon,
            SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
            SLOT( onTrayIconActivated( QSystemTrayIcon::ActivationReason ) )
        );

        QMenu* menu = new QMenu;

        QAction* exitAction = menu->addAction( "Exit" );
        connect( exitAction, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );

        m_trayIcon->setContextMenu( menu );

        m_trayIcon->show();
    stat_win = new table_date();
    QTimer *tmr  = new QTimer();
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), &date_time_obj, SLOT(remind_demon()));
    tmr->start();
    connect(this, SIGNAL(sendData(std::vector <date_time>*, std::vector <QString>*)), stat_win, SLOT(recieveData(std::vector <date_time>*, std::vector <QString>*)));

    //Загрука в реестр
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    //qDebug() << settings.allKeys();
    //qDebug() << settings.allKeys().indexOf("reminder", 0);
    if(settings.allKeys().indexOf("reminder", 0) == -1 )
    {
        QString value = QCoreApplication::applicationFilePath(); //get absolute path of running exe
        value.replace("/","\\");
        settings.setValue("reminder", value);
    }
   //settings.remove("reminder");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent( QCloseEvent* event ) {
    hide();
    event->ignore();
}

void MainWindow::onTrayIconActivated( QSystemTrayIcon::ActivationReason reason ) {
    switch( reason ) {
    case QSystemTrayIcon::Trigger:
        setVisible( !isVisible() );
        break;

    default:
        break;
    }
}

void MainWindow::onShowMessageInTray() {
    m_trayIcon->showMessage( "Message title", "Message text", QSystemTrayIcon::Information );
}

int My_date_time::qstring_to_date_time(QString line, date_time &new_date_time)
{
    QStringList shortList = line.split(' ');
    if (shortList.count() != 5)
    {
        qDebug() << shortList.count() << " fuck in qstring_to_date_time";
        return -1;
    }
    new_date_time.year = shortList[0].toInt();
    new_date_time.month = shortList[1].toInt();
    new_date_time.day = shortList[2].toInt();
    new_date_time.hour = shortList[3].toInt();
    new_date_time.minute = shortList[4].toInt();
    return 0;
}

int My_date_time::update_arrays()
{
    count = 0;
    array_date_time.clear();
    array_messages.clear();

    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line;
        date_time new_date_time;
        while (!file_date_time_rem.atEnd())
        {
            line = file_date_time_rem.readLine();
            qstring_to_date_time(line, new_date_time);
            array_date_time.push_back(new_date_time);
            count++;
        }
        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem update_arrays";
        return -1;
    }

    /*for(int i = 1; i <= count; i++ )
    {
        QString path_new_rem = QDir::currentPath().append("/data/rem_").append(QString::number(i)).append(".txt");
        qDebug() << path_new_rem;
        QFile file_new_rem(path_new_rem);
        if (file_new_rem.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file_new_rem);
            array_messages.push_back(in.readAll());
            file_new_rem.close();
        }
        else
        {
            qDebug() << "fuck in file_new_rem update_arrays";
            return -2;
        }
    }*/
    QString path_new_rem = QDir::currentPath().append("/data/reminders.txt");
    QFile file_new_rem(path_new_rem);
    if (file_new_rem.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file_new_rem);
        QString line = in.readAll();
        QStringList firstList = line.split("\n@$@@$@@$@\n");
        for(int i = 0; i < count; i++ )
        {
            array_messages.push_back(firstList[i]);
        }
        //qDebug() << firstList;
    }
    else
    {
        qDebug() << "fuck in file_new_rem update_arrays";
        return -2;
    }
    return 0;
}

void My_date_time::remind_demon(void)
{
    QDateTime dt;
    dt = QDateTime::currentDateTime();
    /*qDebug() << dt.date().day();
    qDebug() << dt.date().month();
    qDebug() << dt.date().year();
    qDebug() << dt.time().hour();
    qDebug() << dt.time().minute();
    qDebug() << dt.time().second();*/
    dt.date().day();
    dt.date().month();
    dt.date().year();
    dt.time().hour();
    dt.time().minute();
    for(int i = 0; i < this->array_date_time.size(); i++)
    {
        if (this->array_date_time[i].year == dt.date().year() &&
                this->array_date_time[i].month == dt.date().month() &&
                this->array_date_time[i].day == dt.date().day() &&
                this->array_date_time[i].hour == dt.time().hour() &&
                this->array_date_time[i].minute == dt.time().minute())
        {
            //QMessageBox::information(this, "Напоминание", "Сработало напоминание!");
            QMessageBox msgBox;
            msgBox.setWindowTitle("Напоминание");
            msgBox.setText("Сработало напоминание!");
            msgBox.exec();
        }
    }
    //qDebug() << "hello world!";
}

void add_new_remind(QString file_name, QString remind_text, My_date_time &date_time_obj)
{
    /*QString path_new_rem = QDir::currentPath().append("/data/rem_").append(file_name).append(".txt");
    //qDebug() << path_new_rem;
    QFile file_new_rem(path_new_rem);
    if (file_new_rem.open(QIODevice::ReadWrite  | QIODevice::Text))
    {
        QTextStream outstream(&file_new_rem);
        outstream << remind_text;
        file_new_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_new_rem";
    }*/




    QString path_rem = QDir::currentPath().append("/data/reminders.txt");
    //qDebug() << path_new_rem;
    QFile file_rem(path_rem);
    if (file_rem.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&file_rem);
        outstream << remind_text;
        outstream << "\n@$@@$@@$@\n";
        file_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_rem";
    }





    date_time_obj.array_messages.push_back(remind_text);
}

void add_date_time(date_time nem_date_time, My_date_time &date_time_obj)
{
    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&file_date_time_rem);
        outstream << nem_date_time.year << " " << nem_date_time.month << " " << nem_date_time.day << " ";
        outstream << nem_date_time.hour << " " << nem_date_time.minute << "\n";
        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem";
    }
    date_time_obj.array_date_time.push_back(nem_date_time);
}

date_time create_date_time_object(QDate date, QTime time)
{
    date_time new_date_time;
    new_date_time.day = date.day();
    new_date_time.month = date.month();
    new_date_time.year = date.year();
    new_date_time.minute = time.minute();
    new_date_time.hour = time.hour();
    return new_date_time;
}

void MainWindow::on_pushButton_clicked()
{
    int number_remind;
    QDateTime dt = QDateTime::currentDateTime();
    /*qDebug() << dt.date().day();
    qDebug() << dt.date().month();
    qDebug() << dt.date().year();
    qDebug() << dt.time().hour();
    qDebug() << dt.time().minute();
    qDebug() << dt.time().second();*/
    QDate choose_date = ui->calendarWidget->selectedDate();
    QTime choose_time = ui->timeEdit->time();
    date_time nem_date_time = create_date_time_object(choose_date, choose_time);
    //qDebug() << QString(date[0]) << " " << QString(date[1]);
    /*qDebug() << "/n";
    qDebug() << choose_date.day();
    qDebug() << choose_date.month();
    qDebug() << choose_date.year();
    qDebug() << choose_time.minute();
    qDebug() << choose_time.hour();*/
    QString path_coun_rem = QDir::currentPath().append("/data/coun_rem.txt");
    //qDebug() << path_coun_rem;

    QString remind_text = ui->plainTextEdit->toPlainText();
    //qDebug() << remind_text;
    QFile file_coun_rem(path_coun_rem);
    if (file_coun_rem.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //qDebug() << file_coun_rem.size();
        if (file_coun_rem.size() <= 0.001)
        {
            number_remind = 1;
            QString line = QString::number(number_remind);
            QTextStream outstream(&file_coun_rem);
            outstream << line;
            add_date_time(nem_date_time, date_time_obj);
            //outstream << date[0] << " " << date[1] << " " << date[2] << " ";
            //outstream << time[0] << " " << time[1] << "\n";
            add_new_remind(line, remind_text, date_time_obj);
        }
        else
        {
            QString line = file_coun_rem.readLine();
            number_remind = line.toInt();
            file_coun_rem.seek(file_coun_rem.pos() - line.length());
            //qDebug() << line << "    " << file_coun_rem.pos() << "    " << line.length();
            number_remind++;
            QString s = QString::number(number_remind);
            add_date_time(nem_date_time, date_time_obj);
            add_new_remind(s, remind_text, date_time_obj);
            QTextStream outstream(&file_coun_rem);
            outstream << s;
        }
        /*while (!file_coun_rem.atEnd())
        {
            line = file_coun_rem.readLine();
            string = line;
            if (string.contains(rx))
            {
                std::cout << "  " << qPrintable(string);
                file.seek(file.pos() - line.length());

                line.clear();
                line.insert(0,string.toLower());
                file.write(line);
            }
        }*/
        file_coun_rem.close();
    }
    else
        qDebug() << "fuck in file_coun_rem";
    //uiTimeIzd->setText(dt.toString());
}

void MainWindow::on_pushButton_2_clicked()
{
    date_time_obj.update_arrays();
    for (int i = 0; i < date_time_obj.array_date_time.size();i++)
    {
        qDebug() << date_time_obj.array_date_time[i].day << " " << date_time_obj.array_date_time[i].month << " " << date_time_obj.array_date_time[i].year << " "
                 << date_time_obj.array_date_time[i].minute << " " << date_time_obj.array_date_time[i].hour << " ";
    }
    qDebug() << "\n";
    for (int i = 0; i < date_time_obj.array_messages.size();i++)
    {
        qDebug() << i << " " << date_time_obj.array_messages[i] << " " << date_time_obj.array_messages.size();
    }

}



void MainWindow::on_pushButton_4_clicked()
{
    stat_win->show();
    emit sendData(&date_time_obj.array_date_time, &date_time_obj.array_messages);
}
