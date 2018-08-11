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
#include <logger.h>
#include <thread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


   /********************************************/
    // Загрузкка в трей
    m_trayIcon = new QSystemTrayIcon( QIcon( QDir::currentPath().append("/res/clock.ico") ), this );
    m_trayIcon->setToolTip("Tray Program" "\n"
                             "Работа со сворачиванием программы трей");
    /*connect(
        m_trayIcon,
        SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
        SLOT( onTrayIconActivated( QSystemTrayIcon::ActivationReason ) )
    );*/

    QMenu* menu = new QMenu;

    QAction* exitAction = menu->addAction( "Выход" );
    connect( exitAction, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );

    m_trayIcon->setContextMenu( menu );

    m_trayIcon->show();
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    /********************************************/
    stat_win = new table_date();
    stat_win->setFixedSize(729,425);
    rem_win = new remind();
    rem_win->setFixedSize(454,534);
    hist_win = new History();
    hist_win->setFixedSize(631,517);

    QTimer *tmr  = new QTimer();
    tmr->setInterval(2000);

    tmr_qm  = new QTimer();

    connect(tmr, SIGNAL(timeout()), &date_time_obj, SLOT(remind_demon()));
    connect(tmr_qm, SIGNAL(timeout()), tmr_qm, SLOT(stop()));
    connect(tmr_qm, SIGNAL(timeout()), &qm, SLOT(close()));
    connect(&date_time_obj, SIGNAL(stop_timer()), tmr, SLOT(stop()));
    connect(&date_time_obj, SIGNAL(start_timer(int)), tmr, SLOT(start(int)));
    connect(rem_win, SIGNAL(start_timer_demon(int)), tmr, SLOT(start(int)));
    connect(rem_win, SIGNAL(show_form()), this, SLOT(show()));
    connect(&date_time_obj, SIGNAL(show_rem_win()), rem_win, SLOT(show()));
    connect(this, SIGNAL(sendData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*)), stat_win, SLOT(recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*)));
    connect(stat_win, SIGNAL(add_date_time_sig(date_time&,std::vector <QString>&)), this, SLOT(add_date_time_slot(date_time&,std::vector <QString>&)));
    connect(stat_win, SIGNAL(add_new_remind_sig(QString)), this, SLOT(add_new_remind_slot(QString)));
    connect(rem_win, SIGNAL(add_date_time_sig(date_time&,std::vector <QString>&)), this, SLOT(add_date_time_slot(date_time&,std::vector <QString>&)));
    connect(rem_win, SIGNAL(add_new_remind_sig(QString)), this, SLOT(add_new_remind_slot(QString)));
    connect(&date_time_obj, SIGNAL(sendData_withi(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*, int)), rem_win, SLOT(recieveData_withi(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*, int)));
    connect(&date_time_obj, SIGNAL(close_table()), this, SLOT(close_stat_win()));
    connect(rem_win, SIGNAL(send_trey_not(QString,QString)), this, SLOT(add_trey_not(QString,QString)));
    connect(stat_win, SIGNAL(send_trey_not(QString,QString)), this, SLOT(add_trey_not(QString,QString)));
    connect(this, SIGNAL(send_trey_not(QString,QString)), this, SLOT(add_trey_not(QString,QString)));
    connect(this, SIGNAL(update_history()), hist_win, SLOT(update_history()));
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

void MainWindow::add_trey_not(QString one, QString two)
{
    //tmr_qm->start(3000);
    //MessageBox my_qm;
    qm.setText(one);
    qm.setIcon(QMessageBox::Information);
    qm.setStandardButtons(QMessageBox::Ok);
    qm.button(QMessageBox::Ok)->animateClick(1500);
    qm.exec();
    //qm.information(this, "Информация", one);
    //qDebug() << "dfdfdfdfdfdfdfdf";
    //m_trayIcon->showMessage(one, two)
    //QMessageBox::information(this, "Информация", one);
}

void MainWindow::closeEvent( QCloseEvent* event ) {   
    event->ignore();
    this->hide();
    /*QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    m_trayIcon->showMessage("Reminder",
                                  trUtf8("Приложение свернуто в трей. Для того чтобы, "
                                         "развернуть окно приложения, щелкните по иконке приложения в трее"),
                                  icon,
                                  1000);*/
}

/*void MainWindow::onTrayIconActivated( QSystemTrayIcon::ActivationReason reason ) {
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
*/
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }
        break;
    default:
        break;
    }
}

int My_date_time::qstring_to_date_time(QString line, date_time &new_date_time, std::vector <QString>& filename)
{
    QStringList shortList = line.split("#$#");

    if (shortList.count() < 6)
    {
        qDebug() << shortList;
        qDebug() << shortList.count() << " error in qstring_to_date_time";
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
        //qDebug() << "error in file_date_time_rem update_arrayswdsdsdsdsd";
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
        qDebug() << "error in file_date_time_rem update_arrays";
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
        qDebug() << "error in file_new_rem update_arrays";
        return -2;
    }
    return 0;
}


void add_history(QString remind_text, date_time &nem_date_time, std::vector <QString>& filename)
{
    {
        QString path_rem = QDir::currentPath().append("/data/history/reminders_h.txt");
        //qDebug() << path_rem;
        QFile file_rem(path_rem);
        if (file_rem.open(QIODevice::Append | QIODevice::Text))
        {
            //qDebug() << "dfdf";
            QTextStream outstream(&file_rem);
            outstream << remind_text;
            outstream << "\n@$@@$@@$@\n";
            file_rem.close();
        }
        else
        {
            add_to_log(Q_FUNC_INFO,"error in add_history remind_text");
            qDebug() << "error in open add_history remind_text";
        }
    }
    QString path_date_time_rem = QDir::currentPath().append("/data/history/date_time_rem_h.txt");
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
            outstream << "#$#" << file_name_str << "\n";
        }


        file_date_time_rem.close();
    }
    else
    {
        add_to_log(Q_FUNC_INFO,"error in add_history nem_date_time");
        qDebug() << "error in open add_history nem_date_time";
    }

}

void add_new_remind(QString remind_text)
{
    QString path_rem = QDir::currentPath().append("/data/reminders.txt");
    qDebug() << path_rem;
    QFile file_rem(path_rem);
    if (file_rem.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "dfdf";
        QTextStream outstream(&file_rem);
        outstream << remind_text;
        outstream << "\n@$@@$@@$@\n";
        file_rem.close();
    }
    else
    {
        add_to_log(Q_FUNC_INFO,"error in file_rem");
        qDebug() << "error in open file_rem";
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
            outstream << "#$#" << file_name_str << "\n";
        }


        file_date_time_rem.close();
    }
    else
    {
        add_to_log(Q_FUNC_INFO,"error in file_date_time_rem");
        qDebug() << "error in open date_time_rem";
    }
}
void MainWindow::add_date_time_slot(date_time &nem_date_time, std::vector <QString>& filename)
{
    add_date_time(nem_date_time, filename);
}

void MainWindow::close_stat_win()
{
    if (this->stat_win->isVisible())
        this->stat_win->close();
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
        if     (this->array_date_time[i].year < dt.date().year() ||
                this->array_date_time[i].year <= dt.date().year() && this->array_date_time[i].month < dt.date().month() ||
                this->array_date_time[i].year <= dt.date().year() && this->array_date_time[i].month <= dt.date().month() && this->array_date_time[i].day < dt.date().day() ||
                this->array_date_time[i].year <= dt.date().year() && this->array_date_time[i].month <= dt.date().month() && this->array_date_time[i].day <= dt.date().day() && this->array_date_time[i].hour < dt.time().hour() ||
                this->array_date_time[i].year <= dt.date().year() && this->array_date_time[i].month <= dt.date().month() && this->array_date_time[i].day <= dt.date().day() && this->array_date_time[i].hour <= dt.time().hour() && this->array_date_time[i].minute <= dt.time().minute())
        {
            emit stop_timer();
            emit show_rem_win();
            emit sendData_withi(&array_date_time, &array_messages, &array_file_names, i);
            emit close_table();
            //emit start_timer(2000);
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
            if (QFile::exists(QDir::currentPath().append("/data/documents/").append(file_name_str)))
            {
                QFile::remove(QDir::currentPath().append("/data/documents/").append(file_name_str));
            }
            bool ok = QFile::copy(date_time_obj.file_path[i],QDir::currentPath().append("/data/documents/").append(file_name_str));
            if (!ok)
            {
                //QMessageBox::information(this, "Ошибка", QString("Не удалось скопировать файл ").append(file_name_str));
                //m_trayIcon->showMessage(QString("Не удалось скопировать файл ").append(file_name_str),"");
                emit send_trey_not(QString("Не удалось скопировать файл ").append(file_name_str),"");
                date_time_obj.file_path.erase(date_time_obj.file_path.begin()+i);
                i--;
            }
        }
        for (int i = 0; i < date_time_obj.file_path.size(); i++)
        {
            QStringList shortList = date_time_obj.file_path[i].split('/');
            QString file_name_str = shortList[shortList.count()-1];
            date_time_obj.file_path[i] = file_name_str;
        }
    }
    qDebug() << date_time_obj.file_path;
    add_date_time(nem_date_time, date_time_obj.file_path);
    add_history(remind_text, nem_date_time, date_time_obj.file_path);
    Files newfiles;
    newfiles.files_arr = date_time_obj.file_path;
    date_time_obj.array_file_names.push_back(newfiles);
    date_time_obj.array_date_time.push_back(nem_date_time);
    this->date_time_obj.file_path.clear();
    ui->label_file->setText("Файл не выбран");
    ui->plainTextEdit->clear();
    emit send_trey_not(QString("Напоминание добавлено!"),"");
    //m_trayIcon->showMessage("Напоминание добавлено!","j");
}


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



void MainWindow::on_pushButton_clicked()
{
    hist_win->show();
    emit update_history();
}
