#include "table_date.h"
#include "ui_table_date.h"
#include <qDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>

table_date::table_date(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::table_date)
{
    ui->setupUi(this);
    //qDebug() << MainWindow::date_time_obj.array_messages[0];
    //ui->centralwidget
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

table_date::~table_date()
{
    delete ui;
}

void table_date::recieveData(std::vector<date_time>* one, std::vector<QString>* two, std::vector <QString>* three)
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
        item4 = new QTableWidgetItem((*array_file_names)[i]);

        ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 0, item1);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 1, item2);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 2, item3);
        ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 3, item4);
        ui->tableWidget->setColumnWidth(0, 65);
        ui->tableWidget->setColumnWidth(1, 50);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tableWidget->setColumnWidth(3, 70);
    }
}


void add_new_remind_new(QString remind_text)
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

void add_date_time_new(date_time &nem_date_time, QString &file_name)
{
    QString path_date_time_rem = QDir::currentPath().append("/data/date_time_rem.txt");
    QFile file_date_time_rem(path_date_time_rem);
    if (file_date_time_rem.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outstream(&file_date_time_rem);
        outstream << nem_date_time.year << "#$#" << nem_date_time.month << "#$#" << nem_date_time.day << "#$#";
        outstream << nem_date_time.hour << "#$#" << nem_date_time.minute << "#$#" << file_name << "\n";
        file_date_time_rem.close();
    }
    else
    {
        qDebug() << "fuck in file_date_time_rem";
    }
}

void table_date::closeEvent( QCloseEvent* event )
{
    QFile(QDir::currentPath().append("/data/reminders.txt")).remove();
    QFile(QDir::currentPath().append("/data/date_time_rem.txt")).remove();
    for(int i = 0; i < (*array_date_time).size(); i++)
    {
        add_date_time_new((*array_date_time)[i], (*array_file_names)[i]);
        add_new_remind_new((*array_messages)[i]);
    }
}

void table_date::on_delete_button_clicked()
{
    if ((*array_date_time).empty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Информация");
        msgBox.setText("Список напоминаний пуст!");
        msgBox.exec();
    }
    else
    {
        int i = ui->tableWidget->currentRow();
        if (i < 0)
        {
            return;
        }
        ui->tableWidget->removeRow(i);
        (*array_date_time).erase((*array_date_time).begin()+i);
        (*array_messages).erase((*array_messages).begin()+i);
        (*array_file_names).erase((*array_file_names).begin()+i);
    }
}

void table_date::on_clear_button_clicked()
{
    if ((*array_date_time).empty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Информация");
        msgBox.setText("Список напоминаний пуст!");
        msgBox.exec();
    }
    else
    {
        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);
        QStringList name_table;
        name_table << "Дата" << "Время" << "Напоминание" << "Файл";
        ui->tableWidget->setHorizontalHeaderLabels(name_table);
        (*array_date_time).clear();
        (*array_messages).clear();
        (*array_file_names).clear();
    }
}
