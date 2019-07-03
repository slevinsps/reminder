#include "table_date.h"
#include "ui_table_date.h"
#include <qDebug>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>
#include <syslimits.h>

#include "logger.h"
#include "history.h"
#include "mainwindow.h"

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
    key = new keyEnterReceiver();
    ui->findEdit->installEventFilter(key);
    connect(this, SIGNAL(sendData(date_time*,QString*, Files*, int, int)), update_win, SLOT(recieveData(date_time*,QString*, Files*, int, int)));
    connect(update_win, SIGNAL(update_table_sig(int, int)), this, SLOT(update_table(int, int)));
    connect(key, SIGNAL(findPressSignal()), this, SLOT(on_find_button_clicked()));
    ui->tableWidget->setColumnHidden(4, true);
}

table_date::~table_date()
{
    delete ui;
}


void table_date::insertRow(std::vector<date_time>* array_date_time, std::vector<QString>* array_messages, std::vector <Files>* array_file_names, int i) {
    QTableWidgetItem *item1, *item2, *item3, *item4, *item5;
    QString dayRow = QString::number((*array_date_time)[i].day);
    if ((*array_date_time)[i].day < 10) {
        dayRow = "0" + dayRow;
    }
    QString monthRow = QString::number((*array_date_time)[i].month);
    if ((*array_date_time)[i].month < 10) {
        monthRow = "0" + monthRow;
    }
    QString hourRow = QString::number((*array_date_time)[i].hour);
    if ((*array_date_time)[i].hour < 10) {
        hourRow = "0" + hourRow;
    }
    QString minuteRow = QString::number((*array_date_time)[i].minute);
    if ((*array_date_time)[i].minute < 10) {
        minuteRow = "0" + minuteRow;
    }

    QDateTime dt;
    dt = QDateTime::currentDateTime();




    item1 = new QTableWidgetItem(dayRow.append(".").append(monthRow).append(".").append(QString::number((*array_date_time)[i].year)));
    item2 = new QTableWidgetItem(hourRow.append(":").append(minuteRow));
    item3 = new QTableWidgetItem((*array_messages)[i]);
    QString str = "";
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        str.append((*array_file_names)[i].files_arr[j]).append("\n");
    }
    item4 = new QTableWidgetItem(str);
    item5 = new QTableWidgetItem(QString::number(i));

    if (
        (*array_date_time)[i].day <= dt.date().day() + 2 &&
        (*array_date_time)[i].month == dt.date().month() &&
        (*array_date_time)[i].year == dt.date().year()
        )
    {
        item1->setBackground( Qt::green);
        item2->setBackground( Qt::green);
        item3->setBackground( Qt::green);
        item4->setBackground( Qt::green);

        item3->setText(item3->text() + " [СРОЧНО]");
    }


    ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
    ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 0, item1);
    ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 1, item2);
    ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 2, item3);
    ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 3, item4);
    ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 4, item5);
    ui->tableWidget->setColumnWidth(0, 80);
    ui->tableWidget->setColumnWidth(1, 50);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget->setColumnWidth(3, 130);
    ui->tableWidget->setColumnHidden(4, true);

}

void table_date::recieveData(std::vector<date_time>* one, std::vector<QString>* two, std::vector <Files>* three)
{
    ui->findEdit->setText("");
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);

    this->array_date_time = one;
    this->array_messages = two;
    this->array_file_names = three;

    if (checkDateArrayOnSort(*array_date_time) == -1) {
        quickSortDateArray(*array_file_names, *array_date_time, *array_messages, 0, (*array_date_time).size() - 1);
    }
    for (int i = 0; i < (*array_date_time).size(); i++)
    {
        insertRow(array_date_time, array_messages, array_file_names, i);
    }

}

void table_date::update_table(int curRow, int i)
{
    QTableWidgetItem *item1, *item2, *item3, *item4;
    QString dayRow = QString::number((*array_date_time)[i].day);
    if ((*array_date_time)[i].day < 10) {
        dayRow = "0" + dayRow;
    }
    QString monthRow = QString::number((*array_date_time)[i].month);
    if ((*array_date_time)[i].month < 10) {
        monthRow = "0" + monthRow;
    }
    QString hourRow = QString::number((*array_date_time)[i].hour);
    if ((*array_date_time)[i].hour < 10) {
        hourRow = "0" + hourRow;
    }
    QString minuteRow = QString::number((*array_date_time)[i].minute);
    if ((*array_date_time)[i].minute < 10) {
        minuteRow = "0" + minuteRow;
    }
    item1 = new QTableWidgetItem(dayRow.append(".").append(monthRow).append(".").append(QString::number((*array_date_time)[i].year)));

    item2 = new QTableWidgetItem(hourRow.append(":").append(minuteRow));
    item3 = new QTableWidgetItem((*array_messages)[i]);
    QString str = "";
    for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
    {
        str.append((*array_file_names)[i].files_arr[j]).append("\n");
    }
    item4 = new QTableWidgetItem(str);

    ui->tableWidget->setItem   ( curRow, 0, item1);
    ui->tableWidget->setItem   ( curRow, 1, item2);
    ui->tableWidget->setItem   ( curRow, 2, item3);
    ui->tableWidget->setItem   ( curRow, 3, item4);
    ui->tableWidget->selectRow(0);
    writeOnDisk();
}

void table_date::writeOnDisk(void)
{
    if (!array_date_time || !array_file_names || !array_messages) {
        qDebug() << "!array_date_time || !array_file_names || !array_messages";
        return;
    }
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


void table_date::on_delete_button_clicked()
{

    if ((*array_date_time).empty())
    {
        /*QMessageBox msgBox;
        msgBox.setWindowTitle("Информация");
        msgBox.setText("Список напоминаний пуст!");
        msgBox.exec();*/
        emit showMessageSignal("Список напоминаний пуст!","");
    }
    else
    {
        if (!QDir(QDir::currentPath().append("/data/history/documents")).exists()) {
            QDir().mkdir(QDir::currentPath().append("/data/history/documents"));
        }
        if (!QDir(QDir::currentPath().append("/data/documents")).exists()) {
            QDir().mkdir(QDir::currentPath().append("/data/documents"));
        }
        qm.setText("Вы уверены что хотите удалить напоминание?");
        qm.setIcon(QMessageBox::Information);
        qm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        qm.setButtonText(QMessageBox::Yes, tr("Да"));
        qm.setButtonText(QMessageBox::No, tr("Нет"));
        qm.button(QMessageBox::No)->animateClick(1500);
        int button = qm.exec();
         if (button == QMessageBox::Yes) {
             int i = -1;
             int currRow = ui->tableWidget->currentRow();
             QTableWidgetItem *item = ui->tableWidget->item(currRow,4);
             if (NULL != item) {
                 i = item->text().toInt();
             } else {
                 return;
             }
             qDebug() << i;

             if (i < 0)
             {
                 return;
             }
             add_history((*array_messages)[i], (*array_date_time)[i], (*array_file_names)[i].files_arr);
             ui->tableWidget->removeRow(currRow);
             for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
             {
                 //str_files.append(array_file_names[i].files_arr[j]).append("\n");
                 QString file_name_str = (*array_file_names)[i].files_arr[j];
                 if (file_name_str == "NULL") { // if file don`t attached
                     continue;
                 }
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
                     emit showMessageSignal(QString("Не удалось скопировать файл ").append(file_name_str),"");
                 }
                 if (!QFile(QDir::currentPath().append("/data/documents/").append((*array_file_names)[i].files_arr[j])).remove())
                     add_to_log(Q_FUNC_INFO,"error in remove one document");
             }
             (*array_date_time).erase((*array_date_time).begin()+i);
             (*array_messages).erase((*array_messages).begin()+i);
             (*array_file_names).erase((*array_file_names).begin()+i);
         }

         writeOnDisk();

    }
}

void table_date::on_clear_button_clicked() // очистка из напоминаний
{
    /*if ((*array_date_time).empty())
    {
        emit showMessageSignal("Список напоминаний пуст!","");
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
    }*/
}

void table_date::on_updateButton_clicked()
{

    int i = -1;
    int currRow = ui->tableWidget->currentRow();
    QTableWidgetItem *item = ui->tableWidget->item(currRow,4);
    if (NULL != item) {
        i = item->text().toInt();
    } else {
        return;
    }

    if (i < 0)
    {
        return;
    }

    update_win->show();

    emit sendData(&(*array_date_time)[i],&(*array_messages)[i],&(*array_file_names)[i], currRow, i);

}

void table_date::on_openButton_clicked()
{
    int i = -1;
    int currRow = ui->tableWidget->currentRow();
    QTableWidgetItem *item = ui->tableWidget->item(currRow,4);
    if (NULL != item) {
        i = item->text().toInt();
    } else {
        return;
    }

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
            emit showMessageSignal(QString("Невозможно открыть файл ").append((*array_file_names)[i].files_arr[j]),"");
            //add_to_log(Q_FUNC_INFO,QString("error in openUrl ").append((*array_file_names)[i].files_arr[j]));
        }
    }
}


bool keyEnterReceiver::eventFilter(QObject* obj, QEvent* event) {
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (key->key()==Qt::Key_Enter || key->key()==Qt::Key_Return)  {
           emit findPressSignal();
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void table_date::on_find_button_clicked()
{
    QString find_text = ui->findEdit->text();
    if (find_text.isEmpty()) {
        this->recieveData(array_date_time, array_messages, array_file_names);
        return;
    }
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);

    for (int i = 0; i < (*array_date_time).size(); i++)
    {
        QString dayRow = QString::number((*array_date_time)[i].day);
        if ((*array_date_time)[i].day < 10) {
            dayRow = "0" + dayRow;
        }
        QString monthRow = QString::number((*array_date_time)[i].month);
        if ((*array_date_time)[i].month < 10) {
            monthRow = "0" + monthRow;
        }

        QString date = dayRow.append(".").append(monthRow).append(".").append(QString::number((*array_date_time)[i].year));

        if ((*array_messages)[i].indexOf(find_text,0,Qt::CaseInsensitive) < 0) {
            if (date.indexOf(find_text, 0, Qt::CaseInsensitive) < 0) {
                bool findFlag = false;
                for (int j = 0; j < (*array_file_names)[i].files_arr.size(); j++)
                {
                    if ((*array_file_names)[i].files_arr[j].indexOf(find_text,0,Qt::CaseInsensitive) > 0) {
                        findFlag = true;
                        break;
                    }
                }
                if (!findFlag) {
                    continue;
                }
            }
        }

        insertRow(array_date_time, array_messages, array_file_names, i);

    }
    ui->tableWidget->selectRow(0);
}

void table_date::on_showAll_button_clicked()
{
    ui->findEdit->setText("");
    this->recieveData(array_date_time, array_messages, array_file_names);
}
