#include "remind.h"
#include "ui_remind.h"
#include <QDir>
#include <qDebug>
#include <QDesktopServices>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTime>
#include "logger.h"

remind::remind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::remind)
{
    ui->setupUi(this);

    tmr  = new QTimer();
    tmr->setInterval(5000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(show_close()));
    /*Qt::WindowFlags flags = 0;
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);*/
    setWindowFlags( Qt::WindowStaysOnTopHint | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint );

}

remind::~remind()
{
    tmr->stop();
    delete ui;
}

void remind::recieveData_withi(std::vector<date_time>* one, std::vector<QString>* two, std::vector <Files>* three, int number)
{
    this->array_date_time = one;
    this->array_messages = two;
    this->array_file_names = three;
    num_i = number;
    int i = num_i;

    QString str_files = "";
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        str_files.append((*array_file_names)[i].files_arr[j]).append("\n");
    }
    ui->textEdit->setText((*array_messages)[i]);
    ui->textEdit_2->setText(str_files);
    QString data = QString::number((*array_date_time)[i].day).append(".").append(QString::number((*array_date_time)[i].month)).append(".").append(QString::number((*array_date_time)[i].year));
    QString time = QString::number((*array_date_time)[i].hour).append(":").append(QString::number((*array_date_time)[i].minute));
    ui->label_data->setText(QString("Дата: ").append(data).append("  Время: ").append(time));
    tmr->start();
    this->activateWindow();
    this->showNormal();
    //qDebug() << "балабол";
}


void remind::on_deleteButton_clicked()
{
    int i = num_i;
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        QFile(QDir::currentPath().append("/data/documents/").append((*array_file_names)[i].files_arr[j])).remove();
    }
    (*array_date_time).erase((*array_date_time).begin()+i);
    (*array_messages).erase((*array_messages).begin()+i);
    (*array_file_names).erase((*array_file_names).begin()+i);
    if (!QFile(QDir::currentPath().append("/data/reminders.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove reminders.txt");
    if (!QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove date_time_rem.txt");


    for(int i = 0; i < (*array_date_time).size(); i++)
    {
        emit add_date_time_sig((*array_date_time)[i], (*array_file_names)[i].files_arr);
        emit add_new_remind_sig((*array_messages)[i]);
    }
    emit show_form();
    tmr->stop();
    emit start_timer_demon(2000);
    this->close();
}

void remind::on_openButton_clicked()
{
    int i = num_i;
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        QString str = QDir::currentPath().append("/data/documents/").append((*array_file_names)[i].files_arr[j]);
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(str)))
        {
            //QMessageBox::information(this, "Ошибка", QString("Невозможно открыть файл ").append((*array_file_names)[i].files_arr[j]));
            emit send_trey_not(QString("Невозможно открыть файл ").append((*array_file_names)[i].files_arr[j]),"");
            //add_to_log(Q_FUNC_INFO,QString("error in openUrl ").append((*array_file_names)[i].files_arr[j]));
        }
    }
    //qDebug() << "мазафака";
    tmr->stop();
    emit start_timer_demon(2000);
}

void remind::on_waitButton_clicked()
{
    int i = num_i;
    QDateTime dt;
    dt = QDateTime::currentDateTime();

    (*array_date_time)[i].day = dt.date().day();
    (*array_date_time)[i].month = dt.date().month();
    (*array_date_time)[i].year = dt.date().year();
    (*array_date_time)[i].hour = dt.time().hour();
    (*array_date_time)[i].minute = dt.time().minute();
    (*array_date_time)[i].minute += 1;
    if ((*array_date_time)[i].minute >= 60)
    {
        (*array_date_time)[i].minute -= 60;
        (*array_date_time)[i].hour++;
    }

    QFile(QDir::currentPath().append("/data/reminders.txt")).remove();
    QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove();
    for(int i = 0; i < (*array_date_time).size(); i++)
    {
        emit add_date_time_sig(((*array_date_time))[i], (*array_file_names)[i].files_arr);
        emit add_new_remind_sig((*array_messages)[i]);
    }
    emit show_form();
    tmr->stop();
    emit start_timer_demon(2000);
    this->close();
}

void remind::closeEvent( QCloseEvent* event )
{
    /*this->activateWindow();
    this->showNormal();
    event->ignore();*/
}


void remind::show_close()
{
    //qDebug() << "Привет";
    //tmr->stop();
    //emit start_timer_demon(2000);
    this->activateWindow();
    this->showNormal();
}