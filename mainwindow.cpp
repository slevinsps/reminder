#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qDebug>
#include <QDir>
#include <stdio.h>
#include <QTimer>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QFileDialog>
#include <QDesktopServices>


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
    tmr->setInterval(2000);
    connect(tmr, SIGNAL(timeout()), &date_time_obj, SLOT(remind_demon()));
    connect(&date_time_obj, SIGNAL(stop_timer()), tmr, SLOT(stop()));
    connect(&date_time_obj, SIGNAL(start_timer(int)), tmr, SLOT(start(int)));
    connect(&date_time_obj, SIGNAL(show_form()), this, SLOT(show()));
    connect(this, SIGNAL(sendData(std::vector <date_time>*, std::vector <QString>*, std::vector <QString>*)), stat_win, SLOT(recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <QString>*)));

    tmr->start();
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

int My_date_time::qstring_to_date_time(QString line, date_time &new_date_time, QString &file_name)
{
    QStringList shortList = line.split("#$#");

    if (shortList.count() < 6)
    {
        qDebug() << shortList;
        qDebug() << shortList.count() << " fuck in qstring_to_date_time";
        return -1;
    }
    new_date_time.year = shortList[0].toInt();
    new_date_time.month = shortList[1].toInt();
    new_date_time.day = shortList[2].toInt();
    new_date_time.hour = shortList[3].toInt();
    new_date_time.minute = shortList[4].toInt();
    array_file_names.push_back(shortList[5]);
    file_name = shortList[5];
    /*for(int i = 5; i < shortList.count();i++)
        file_name += shortList[i] + " ";*/
    return 0;
}

int My_date_time::update_arrays()
{
    count = 0;
    array_date_time.clear();
    array_messages.clear();
    array_file_names.clear();

    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line;
        QString file_name;
        date_time new_date_time;
        QTextStream in(&file_date_time_rem);
        while (!in.atEnd())
        {
            line = in.readLine();
            qstring_to_date_time(line, new_date_time, file_name);
            array_date_time.push_back(new_date_time);
            //array_file_names.push_back(file_name);
            count++;
        }
        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem update_arrays";
        return -1;
    }

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

void add_new_remind(QString remind_text)
{
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
}

void add_date_time(date_time &nem_date_time, QString &filename)
{
    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&file_date_time_rem);
        outstream << nem_date_time.year << "#$#" << nem_date_time.month << "#$#" << nem_date_time.day << "#$#";
        outstream << nem_date_time.hour << "#$#" << nem_date_time.minute << "#$#" << filename << "\n";
        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem";
    }
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
            /*QMessageBox msgBox;
            msgBox.setWindowTitle("Напоминание");
            msgBox.setText("Сработало напоминание!");
            msgBox.exec();*/
            emit stop_timer();

            QMessageBox msgBox;
            QString str = "Напоминание!\n";
            msgBox.setText(str.append(array_messages[i]).append("\n").append("Файл: ").append(array_file_names[i]));
            QAbstractButton* pButtonClose = msgBox.addButton(tr("Закрыть"), QMessageBox::YesRole);
            QAbstractButton* pButtonWait = msgBox.addButton(tr("Отложить"), QMessageBox::NoRole);
            QAbstractButton* pButtonOpen = msgBox.addButton(tr("Файл"), QMessageBox::YesRole);

            msgBox.exec();

            if (msgBox.clickedButton()==pButtonClose)
            {
                array_date_time.erase(array_date_time.begin()+i);
                array_messages.erase(array_messages.begin()+i);
                QFile(QDir::currentPath().append("/data/reminders.txt")).remove();
                QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove();
                for(int i = 0; i < array_date_time.size(); i++)
                {
                    add_date_time(array_date_time[i],array_file_names[i]);
                    add_new_remind(array_messages[i]);
                }
                emit show_form();
            }
            else if(msgBox.clickedButton()==pButtonWait)
            {
                array_date_time[i].minute += 1;
                if (array_date_time[i].minute >= 60)
                {
                    array_date_time[i].minute -= 60;
                    array_date_time[i].hour++;
                }

                QFile(QDir::currentPath().append("/data/reminders.txt")).remove();
                QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove();
                for(int i = 0; i < array_date_time.size(); i++)
                {
                    add_date_time(array_date_time[i],array_file_names[i]);
                    add_new_remind(array_messages[i]);
                }
                emit show_form();
            }
            else
            {
                QString str = QDir::currentPath().append("/data/documents/").append(array_file_names[i]);
                QDesktopServices::openUrl(QUrl::fromLocalFile(str));
                qDebug() << "мазафака";
            }
            emit start_timer(2000);
        }
    }
    //qDebug() << "hello world!";
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
    QDate choose_date = ui->calendarWidget->selectedDate();
    QTime choose_time = ui->timeEdit->time();
    date_time nem_date_time = create_date_time_object(choose_date, choose_time);
    QString remind_text = ui->plainTextEdit->toPlainText();
    //qDebug() << remind_text;


    add_new_remind(remind_text);
    date_time_obj.array_messages.push_back(remind_text);
    QString file_name;
    if (this->date_time_obj.file_path != "")
    {
        QStringList shortList = this->date_time_obj.file_path.split('/');
        file_name = shortList[shortList.count()-1];
        QFile::copy(this->date_time_obj.file_path,QDir::currentPath().append("/data/documents/").append(file_name));
        //file_name = "'"+file_name+"'";
    }
    else
        file_name = "NULL";

    add_date_time(nem_date_time, file_name);
    date_time_obj.array_file_names.push_back(file_name);
    date_time_obj.array_date_time.push_back(nem_date_time);
    this->date_time_obj.file_path = "";
    ui->label_file->setText("Файл не выбран");

}

void MainWindow::on_pushButton_2_clicked()
{


    //date_time_obj.update_arrays();
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
    emit sendData(&date_time_obj.array_date_time, &date_time_obj.array_messages, &date_time_obj.array_file_names);
}

void MainWindow::on_pushButton_3_clicked()
{
    date_time_obj.file_path = QFileDialog::getOpenFileName(0,
                                QString::fromUtf8("Загрузить файл"),
                                QDir::currentPath(),
                                "All files (*.*)");
    //qDebug() << fileName;
    ui->label_file->setText("Файл загружен");
}
