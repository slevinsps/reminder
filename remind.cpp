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
#include "history.h"

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
    /*for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        QFile(QDir::currentPath().append("/data/documents/").append((*array_file_names)[i].files_arr[j])).remove();
    }*/
    add_history((*array_messages)[i], (*array_date_time)[i], (*array_file_names)[i].files_arr);
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        //str_files.append(array_file_names[i].files_arr[j]).append("\n");
        QString file_name_str = (*array_file_names)[i].files_arr[j];
        if (QFile::exists(QDir::currentPath().append("/data/history/documents/").append(file_name_str)))
        {
            QFile::remove(QDir::currentPath().append("/data/history/documents/").append(file_name_str));
        }
        bool ok = QFile::copy(QDir::currentPath().append("/data/documents/").append(file_name_str),
                              QDir::currentPath().append("/data/history/documents/").append(file_name_str));
        if (!ok)
        {
            //QMessageBox::information(this, "Ошибка", QString("Не удалось скопировать файл ").append(file_name_str));
            //m_trayIcon->showMessage(QString("Не удалось скопировать файл ").append(file_name_str),"");
            emit send_trey_not(QString("Не удалось скопировать файл ").append(file_name_str),"");
        }
        if (!QFile(QDir::currentPath().append("/data/documents/").append((*array_file_names)[i].files_arr[j])).remove())
            add_to_log(Q_FUNC_INFO,"error in remove one document");
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
    qDebug() << "gggggg " <<  dt.date().month();
    int choosing_time  = ui->comboBox->currentIndex();
    (*array_date_time)[i].day = dt.date().day();
    (*array_date_time)[i].month = dt.date().month();
    (*array_date_time)[i].year = dt.date().year();
    (*array_date_time)[i].hour = dt.time().hour();
    (*array_date_time)[i].minute = dt.time().minute();
    switch (choosing_time) {
    case 0:
        (*array_date_time)[i].minute += 30;
        break;
    case 1:
        (*array_date_time)[i].hour++;
        break;
    case 2:
        (*array_date_time)[i].day+=1;
        break;
    case 3:
        (*array_date_time)[i].day+=7;
        break;
    default:
        break;
    }

    if ((*array_date_time)[i].minute >= 60)
    {
        (*array_date_time)[i].minute -= 60;
        (*array_date_time)[i].hour++;
    }
    if ((*array_date_time)[i].hour >= 24)
    {
        (*array_date_time)[i].hour -= 24;
        (*array_date_time)[i].day++;
    }

    if ((*array_date_time)[i].day >= 30 && ((*array_date_time)[i].month == 4 ||
                                            (*array_date_time)[i].month == 6 ||
                                            (*array_date_time)[i].month == 9 ||
                                            (*array_date_time)[i].month == 11))
    {
        (*array_date_time)[i].day -= 30;
        (*array_date_time)[i].month++;
    }
    if ((*array_date_time)[i].day >= 28 && ((*array_date_time)[i].month == 2))
    {
        (*array_date_time)[i].day -= 28;
        (*array_date_time)[i].month++;
    }
    if ((*array_date_time)[i].day >= 31 && ((*array_date_time)[i].month == 1 ||
                                            (*array_date_time)[i].month == 3 ||
                                            (*array_date_time)[i].month == 5 ||
                                            (*array_date_time)[i].month == 7 ||
                                            (*array_date_time)[i].month == 8 ||
                                            (*array_date_time)[i].month == 10 ||
                                            (*array_date_time)[i].month == 12))
    {
        (*array_date_time)[i].day -= 31;
        (*array_date_time)[i].month++;
    }
    if ((*array_date_time)[i].month > 12)
    {
        (*array_date_time)[i].month -= 12;
        (*array_date_time)[i].year++;
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
