#include "history.h"
#include "ui_history.h"
#include "structers.h"
#include "logger.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
History::History(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

History::~History()
{
    delete ui;
}


int qstring_to_date_time(QString line, date_time &new_date_time, std::vector <QString>& filename)
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

void History::update_history()
{
    std::vector <date_time> array_date_time;
    std::vector <QString> array_messages;
    std::vector <Files> array_file_names;
    {
        int count = 0;


        QString path_date_time_rem = QDir::currentPath().append("/data/history/date_time_rem_h.txt");
        if (!QFile(path_date_time_rem).exists())
        {
            //qDebug() << "error in file_date_time_rem update_history";
            return;
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
            qDebug() << "error in file_date_time_rem update_history";
            return;
        }

        QString path_new_rem = QDir::currentPath().append("/data/history/reminders_h.txt");
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
            return;
        }
    }
    /******/
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);
    /*for (int i = 0; i < (*array_date_time).size();i++)
    {
        qDebug() << (*array_date_time)[i].day << " " << (*array_date_time)[i].month << " " << (*array_date_time)[i].year << " "
                 << (*array_date_time)[i].minute << " " << (*array_date_time)[i].hour << " ";
    }
    qDebug() << "\n";
    for (int i = 0; i < (*array_messages).size();i++)
    {
        qDebug() << i << " " << (*array_messages)[i] << " " << (*array_messages).size();
    }*/
    for (int i = 0; i < array_date_time.size(); i++)
    {
        QTableWidgetItem *item1, *item2, *item3, *item4;
        item1 = new QTableWidgetItem(QString::number(array_date_time[i].day).append(".").append(QString::number(array_date_time[i].month)).append(".").append(QString::number(array_date_time[i].year)));
        item2 = new QTableWidgetItem(QString::number(array_date_time[i].hour).append(":").append(QString::number(array_date_time[i].minute)));
        item3 = new QTableWidgetItem(array_messages[i]);
        QString str = "";
        for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
        {
            str.append(array_file_names[i].files_arr[j]).append("\n");
        }
        item4 = new QTableWidgetItem(str);

        ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 0, item1);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 1, item2);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 2, item3);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 3, item4);
        ui->tableWidget->setColumnWidth(0, 80);
        ui->tableWidget->setColumnWidth(1, 50);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tableWidget->setColumnWidth(3, 140);
    }
    array_date_time.clear();
    array_messages.clear();
    array_file_names.clear();
}

void History::on_clear_button_clicked()
{

        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);
        QStringList name_table;
        name_table << "Дата" << "Время" << "Напоминание" << "Файл";
        ui->tableWidget->setHorizontalHeaderLabels(name_table);
        QDir dir(QDir::currentPath().append("/data/history/"));
        if (!dir.removeRecursively())
        {
            add_to_log(Q_FUNC_INFO,"error in removeRecursively History");
        }
        if (!QDir().mkpath(QDir::currentPath().append("/data/history")))
        {
            add_to_log(Q_FUNC_INFO,"error in mkpath History");
        }
}
