#include "table_date.h"
#include "ui_table_date.h"
#include <qDebug>

#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

#include "logger.h"

table_date::table_date(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::table_date)
{
    ui->setupUi(this);
    //qDebug() << MainWindow::date_time_obj.array_messages[0];
    //ui->centralwidget
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    update_win = new Update();
    update_win->setFixedSize(563,591);
    connect(this, SIGNAL(sendData(date_time*,QString*, Files*, int)), update_win, SLOT(recieveData(date_time*,QString*, Files*, int)));
    connect(update_win, SIGNAL(update_table_sig(int)), this, SLOT(update_table(int)));
}

table_date::~table_date()
{
    delete ui;
}

void table_date::recieveData(std::vector<date_time>* one, std::vector<QString>* two, std::vector <Files>* three)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);

    this->array_date_time = one;
    this->array_messages = two;
    this->array_file_names = three;
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
    for (int i = 0; i < (*array_date_time).size(); i++)
    {
        QTableWidgetItem *item1, *item2, *item3, *item4;
        item1 = new QTableWidgetItem(QString::number((*array_date_time)[i].day).append(".").append(QString::number((*array_date_time)[i].month)).append(".").append(QString::number((*array_date_time)[i].year)));
        item2 = new QTableWidgetItem(QString::number((*array_date_time)[i].hour).append(":").append(QString::number((*array_date_time)[i].minute)));
        item3 = new QTableWidgetItem((*array_messages)[i]);
        QString str = "";
        for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
        {
            str.append((*array_file_names)[i].files_arr[j]).append("\n");
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
        ui->tableWidget->setColumnWidth(3, 130);
    }
    ui->tableWidget->selectRow(0);
}

void table_date::update_table(int i)
{
    QTableWidgetItem *item1, *item2, *item3, *item4;
    item1 = new QTableWidgetItem(QString::number((*array_date_time)[i].day).append(".").append(QString::number((*array_date_time)[i].month)).append(".").append(QString::number((*array_date_time)[i].year)));
    item2 = new QTableWidgetItem(QString::number((*array_date_time)[i].hour).append(":").append(QString::number((*array_date_time)[i].minute)));
    item3 = new QTableWidgetItem((*array_messages)[i]);
    QString str = "";
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        str.append((*array_file_names)[i].files_arr[j]).append("\n");
    }
    item4 = new QTableWidgetItem(str);

    //ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
    ui->tableWidget->setItem   ( i, 0, item1);
    ui->tableWidget->setItem   ( i, 1, item2);
    ui->tableWidget->setItem   ( i, 2, item3);
    ui->tableWidget->setItem   ( i, 3, item4);
}

void table_date::closeEvent( QCloseEvent* event )
{
    if (!QFile(QDir::currentPath().append("/data/reminders.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove reminders.txt");
    if (!QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove date_time_rem.txt");
    for(int i = 0; i < (*array_date_time).size(); i++)
    {
        emit add_date_time_sig((*array_date_time)[i], (*array_file_names)[i].files_arr);
        emit add_new_remind_sig((*array_messages)[i]);
    }
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

void table_date::on_delete_button_clicked()
{


    if ((*array_date_time).empty())
    {
        /*QMessageBox msgBox;
        msgBox.setWindowTitle("Информация");
        msgBox.setText("Список напоминаний пуст!");
        msgBox.exec();*/
        emit send_trey_not("Список напоминаний пуст!","");
    }
    else
    {
        qm.setText("Вы уверены что хотите удалить напоминание?");
        qm.setIcon(QMessageBox::Information);
        qm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        qm.setButtonText(QMessageBox::Yes, tr("Да"));
        qm.setButtonText(QMessageBox::No, tr("Нет"));
        qm.button(QMessageBox::No)->animateClick(1500);
        int button = qm.exec();
         if (button == QMessageBox::Yes) {
             int i = ui->tableWidget->currentRow();
             if (i < 0)
             {
                 return;
             }
             add_history((*array_messages)[i], (*array_date_time)[i], (*array_file_names)[i].files_arr);
             ui->tableWidget->removeRow(i);
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
         }

    }
}

void table_date::on_clear_button_clicked()
{
    if ((*array_date_time).empty())
    {
        /*QMessageBox msgBox;
        msgBox.setWindowTitle("Информация");
        msgBox.setText("Список напоминаний пуст!");
        msgBox.exec();*/
        emit send_trey_not("Список напоминаний пуст!","");
    }
    else
    {
        qm.setText("Вы уверены что хотите удалить напоминания?");
        qm.setIcon(QMessageBox::Information);
        qm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        qm.setButtonText(QMessageBox::Yes, tr("Да"));
        qm.setButtonText(QMessageBox::No, tr("Нет"));
        qm.button(QMessageBox::No)->animateClick(1500);
        int button = qm.exec();
        if (button == QMessageBox::Yes) {
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            QStringList name_table;
            name_table << "Дата" << "Время" << "Напоминание" << "Файл";
            ui->tableWidget->setHorizontalHeaderLabels(name_table);
            QDir dir(QDir::currentPath().append("/data/documents/"));
            if (!dir.removeRecursively())
            {
                add_to_log(Q_FUNC_INFO,"error in removeRecursively");
            }
            if (!QDir().mkpath(QDir::currentPath().append("/data/documents")))
            {
                add_to_log(Q_FUNC_INFO,"error in mkpath");
            }

            (*array_date_time).clear();
            (*array_messages).clear();
            (*array_file_names).clear();
         }
    }
}

void table_date::on_updateButton_clicked()
{
    int i = ui->tableWidget->currentRow();
    if (i < 0)
    {
        return;
    }
    update_win->show();

    emit sendData(&(*array_date_time)[i],&(*array_messages)[i],&(*array_file_names)[i], i);

}

void table_date::on_openButton_clicked()
{
    int i = ui->tableWidget->currentRow();
    if (i < 0)
    {
        return;
    }
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
}
