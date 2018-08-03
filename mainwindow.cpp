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

   /********************************************/
    // Загрузкка в трей
    m_trayIcon = new QSystemTrayIcon( QIcon( QDir::currentPath().append("/res/clock.ico") ), this );
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
    /********************************************/
    stat_win = new table_date();
    stat_win->setFixedSize(662,405);
    QTimer *tmr  = new QTimer();
    tmr->setInterval(2000);
    connect(tmr, SIGNAL(timeout()), &date_time_obj, SLOT(remind_demon()));
    connect(&date_time_obj, SIGNAL(stop_timer()), tmr, SLOT(stop()));
    connect(&date_time_obj, SIGNAL(start_timer(int)), tmr, SLOT(start(int)));
    connect(&date_time_obj, SIGNAL(show_form()), this, SLOT(show()));
    connect(this, SIGNAL(sendData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*)), stat_win, SLOT(recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*)));
    connect(stat_win, SIGNAL(add_date_time_sig(date_time&,std::vector <QString>&)), this, SLOT(add_date_time_slot(date_time&,std::vector <QString>&)));
    connect(stat_win, SIGNAL(add_new_remind_sig(QString)), this, SLOT(add_new_remind_slot(QString)));

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
    //settings.remove("reminder"); удаление из реестра
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


int My_date_time::qstring_to_date_time(QString line, date_time &new_date_time, std::vector <QString>& filename)
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
    //file_name = shortList[5];
    for(int i = 5; i < shortList.count();i++)
        filename.push_back(shortList[i]);
    return 0;
}

int My_date_time::update_arrays()
{
    count = 0;
    array_date_time.clear();
    array_messages.clear();
    array_file_names.clear();

    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    if (!QFile(path_date_time_rem).exists())
    {
        //qDebug() << "fuck in file_date_time_rem update_arrayswdsdsdsdsd";
        return 0;
    }
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line;
        date_time new_date_time;
        QTextStream in(&file_date_time_rem);
        while (!in.atEnd())
        {
            line = in.readLine();
            std::vector <QString> filename;
            qstring_to_date_time(line, new_date_time, filename);
            array_date_time.push_back(new_date_time);
            Files newfiles;
            newfiles.files_arr = filename;
            array_file_names.push_back(newfiles);
            filename.clear();
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
void MainWindow::add_new_remind_slot(QString remind_text)
{
    add_new_remind(remind_text);
}

void add_date_time(date_time &nem_date_time, std::vector <QString>& filename)
{
    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&file_date_time_rem);
        outstream << nem_date_time.year << "#$#" << nem_date_time.month << "#$#" << nem_date_time.day << "#$#";
        outstream << nem_date_time.hour << "#$#" << nem_date_time.minute;

        QString file_name_str;

        if (!filename.empty())
        {
            qDebug() << filename;
            for (int i = 0; i < filename.size(); i++)
            {
                outstream << "#$#" << filename[i] ;
            }
            outstream << "\n";
        }
        else
        {
            file_name_str = "NULL";
            filename.push_back(file_name_str);
            outstream << file_name_str << "\n";
        }


        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem";
    }
}
void MainWindow::add_date_time_slot(date_time &nem_date_time, std::vector <QString>& filename)
{
    add_date_time(nem_date_time, filename);
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
    for(int i = 0; i < this->array_date_time.size(); i++)
    {
        if (this->array_date_time[i].year <= dt.date().year() &&
                this->array_date_time[i].month <= dt.date().month() &&
                this->array_date_time[i].day <= dt.date().day() &&
                this->array_date_time[i].hour <= dt.time().hour() &&
                this->array_date_time[i].minute <= dt.time().minute())
        {
            /*QMessageBox msgBox;
            msgBox.setWindowTitle("Напоминание");
            msgBox.setText("Сработало напоминание!");
            msgBox.exec();*/
            emit stop_timer();

            QMessageBox msgBox;
            QString str = "Напоминание!\n";
            QString str_files = "";
            for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
            {
                str_files.append(array_file_names[i].files_arr[j]).append("\n");
            }
            msgBox.setText(str.append(array_messages[i]).append("\n").append("Файлы: \n").append(str_files));
            QAbstractButton* pButtonClose = msgBox.addButton(tr("Закрыть"), QMessageBox::YesRole);
            QAbstractButton* pButtonWait = msgBox.addButton(tr("Отложить"), QMessageBox::NoRole);
            QAbstractButton* pButtonOpen = msgBox.addButton(tr("Файл"), QMessageBox::YesRole);

            msgBox.exec();

            if (msgBox.clickedButton()==pButtonClose)
            {
                for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
                {
                    QFile(QDir::currentPath().append("/data/documents/").append(array_file_names[i].files_arr[j])).remove();
                }
                array_date_time.erase(array_date_time.begin()+i);
                array_messages.erase(array_messages.begin()+i);
                array_file_names.erase(array_file_names.begin()+i);
                QFile(QDir::currentPath().append("/data/reminders.txt")).remove();
                QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove();


                for(int i = 0; i < array_date_time.size(); i++)
                {
                    add_date_time(array_date_time[i],array_file_names[i].files_arr);
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
                    add_date_time(array_date_time[i],array_file_names[i].files_arr);
                    add_new_remind(array_messages[i]);
                }
                emit show_form();
            }
            else
            {
                for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
                {
                    //str_files.append(array_file_names[i].files_arr[j]).append("\n");
                    QString str = QDir::currentPath().append("/data/documents/").append(array_file_names[i].files_arr[j]);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(str));
                }
                //QString str = QDir::currentPath().append("/data/documents/").append(array_file_names[i]);
                //QDesktopServices::openUrl(QUrl::fromLocalFile(str));
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



void MainWindow::on_createButton_clicked()
{
    QDate choose_date = ui->calendarWidget->selectedDate();
    QTime choose_time = ui->timeEdit->time();
    date_time nem_date_time = create_date_time_object(choose_date, choose_time);
    QString remind_text = ui->plainTextEdit->toPlainText();
    //qDebug() << remind_text;


    add_new_remind(remind_text);
    date_time_obj.array_messages.push_back(remind_text);
    if (!date_time_obj.file_path.empty())
    {
        //qDebug() << filename;
        for (int i = 0; i < date_time_obj.file_path.size(); i++)
        {
            QStringList shortList = date_time_obj.file_path[i].split('/');
            QString file_name_str = shortList[shortList.count()-1];

            QFile::copy(date_time_obj.file_path[i],QDir::currentPath().append("/data/documents/").append(file_name_str));
            date_time_obj.file_path[i] = file_name_str;
        }
    }

    add_date_time(nem_date_time, date_time_obj.file_path);
    Files newfiles;
    newfiles.files_arr = date_time_obj.file_path;
    date_time_obj.array_file_names.push_back(newfiles);
    date_time_obj.array_date_time.push_back(nem_date_time);
    this->date_time_obj.file_path.clear();
    ui->label_file->setText("Файл не выбран");

}
/*
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
*/


void MainWindow::on_all_rem_Button_clicked()
{
    stat_win->show();
    emit sendData(&date_time_obj.array_date_time, &date_time_obj.array_messages, &date_time_obj.array_file_names);
}

void MainWindow::on_add_file_Button_clicked()
{
     QString str = QFileDialog::getOpenFileName(0,
                                QString::fromUtf8("Загрузить файл"),
                                QDir::currentPath(),
                                "All files (*.*)");
    if (!str.isEmpty())
    {
        date_time_obj.file_path.push_back(str);

        //qDebug() << fileName;
        if (ui->label_file->text() == "Файл не выбран")
            ui->label_file->setText("Добавленные файлы:");
        QString str_label = ui->label_file->text();
        QStringList shortList = str.split('/');
        //this->date_time_obj.file_path.push_back(shortList[shortList.count()-1]);
        ui->label_file->setText(str_label.append("\n").append( shortList[shortList.count()-1]));
    }
}



void MainWindow::on_clear_file_Button_clicked()
{
    this->date_time_obj.file_path.clear();
    ui->label_file->setText("Файл не выбран");
}
