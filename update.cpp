#include "update.h"
#include "ui_update.h"
#include <qDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "logger.h"

Update::Update(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);

}

Update::~Update()
{
    delete ui;
}


void Update::recieveData(date_time *date, QString *comment, Files *files, int curRow, int rowInArray)
{
    clear = false;
    date1 = date;
    comment1 = comment;
    files1 = files;
    curRow1 = curRow;
    rowInArray1 = rowInArray;
    QTime time(date->hour,date->minute);
    ui->timeEdit->setTime(time);
    QDate cur_date(date->year,date->month, date->day);
    ui->calendarWidget->setSelectedDate(cur_date);
    ui->textEdit->setText(*comment);
    QString str = "";
    if ((*files).files_arr[0] != "NULL")
    {
        for (int j = 0; j < (*files).files_arr.size(); j++)
        {
            str.append((*files).files_arr[j]).append("\n");
        }
    }

    ui->label_file->setText(str);
    //qDebug() << date->day << " " << *comment << " " << files->files_arr;
}

void Update::on_add_file_Button_clicked()
{
    QString str = QFileDialog::getOpenFileName(0,
                               QString::fromUtf8("Загрузить файл"),
                               QDir::currentPath(),
                               "All files (*.*)");
   if (!str.isEmpty())
   {

       files_path.push_back(str);
       //qDebug() << fileName;
       if (ui->label_file->text() == "Файл не выбран")
           ui->label_file->setText("Добавленные файлы:");
       QString str_label = ui->label_file->text();
       QStringList shortList = str.split('/');
       //this->date_time_obj.file_path.push_back(shortList[shortList.count()-1]);

       ui->label_file->setText(str_label.append("\n").append( shortList[shortList.count()-1]));
   }
}


void Update::on_clear_file_Button_clicked()
{
    clear = true;
    files_path.clear();
    ui->label_file->setText("Файл не выбран");
}

void Update::on_createButton_clicked()
{
    QDate choose_date = ui->calendarWidget->selectedDate();
    QTime choose_time = ui->timeEdit->time();
    date1->day = choose_date.day();
    date1->month = choose_date.month();
    date1->year = choose_date.year();
    date1->minute = choose_time.minute();
    date1->hour = choose_time.hour();
    *comment1 = ui->textEdit->toPlainText();

    if (files1->files_arr[0] == "NULL")
    {
        files1->files_arr.clear();
    }
    if (clear)
    {
        for (int j = 0; j < files1->files_arr.size(); j++)
        {
            //str_files.append(array_file_names[i].files_arr[j]).append("\n");
            if (!QFile(QDir::currentPath().append("/data/documents/").append(files1->files_arr[j])).remove())
                add_to_log(Q_FUNC_INFO,"error in remove one document");
        }
        files1->files_arr.clear();
    }

    for (int i = 0; i < files_path.size(); i++)
    {
        QStringList shortList = files_path[i].split('/');
        QString file_name_str = shortList[shortList.count()-1];
        qDebug() << files_path[i];
        if (file_name_str == "NULL") { // if file don`t attached
            continue;
        }
        if (QFile::exists(QDir::currentPath().append("/data/documents/").append(file_name_str)))
        {
            QFile::remove(QDir::currentPath().append("/data/documents/").append(file_name_str));
        }

        bool ok = QFile::copy(files_path[i],QDir::currentPath().append("/data/documents/").append(file_name_str));
        if (!ok)
        {
            //QMessageBox::information(this, "Ошибка", QString("Не удалось скопировать файл ").append(file_name_str));
            //m_trayIcon->showMessage(QString("Не удалось скопировать файл ").append(file_name_str),"");
            QMessageBox qm;
            qm.setText(QString("Не удалось скопировать файл ").append(file_name_str));
            qm.setIcon(QMessageBox::Information);
            qm.setStandardButtons(QMessageBox::Ok);
            qm.button(QMessageBox::Ok)->animateClick(1500);
            qm.exec();
            files_path.erase(files_path.begin()+i);
            i--;
        }
    }
    for (int i = 0; i < files_path.size(); i++)
    {
        QStringList shortList = files_path[i].split('/');
        QString file_name_str = shortList[shortList.count()-1];
        files1->files_arr.push_back(file_name_str);
    }



    if (files1->files_arr.empty())
        files1->files_arr.push_back("NULL");
    files_path.clear();
    emit update_table_sig(curRow1, rowInArray1);
    this->close();
}
