#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include "history.h"
#include <QMessageBox>
#include <QKeyEvent>
#include "mainwindow.h"
History::History(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    key = new keyEnterReceiverHistory();
    ui->findEdit->installEventFilter(key);
    connect(key, SIGNAL(findPressSignal()), this, SLOT(on_find_button_clicked()));
}

History::~History()
{
    delete ui;
}

bool keyEnterReceiverHistory::eventFilter(QObject* obj, QEvent* event) {
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


void History::insertRow(std::vector<date_time>* array_date_time, std::vector<QString>* array_messages, std::vector <Files>* array_file_names, int i) {
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

void History::showAllHistory() {
    ui->findEdit->setText("");
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);

    if (checkDateArrayOnSort(array_date_time) == -1) {
        quickSortDateArray(array_file_names, array_date_time, array_messages, 0, array_date_time.size() - 1);
    }
    for (int i = 0; i < array_date_time.size(); i++)
    {
        insertRow(&array_date_time, &array_messages, &array_file_names, i);
    }
    ui->tableWidget->selectRow(0);
}

void History::update_history()
{
    array_date_time.clear();
    array_messages.clear();
    array_file_names.clear();
    {
        int count = 0;


        QString path_date_time_rem = QDir::currentPath().append("/data/history/date_time_rem_h.txt");
        if (!QFile(path_date_time_rem).exists())
        {
            //qDebug() << "error in file_date_time_rem update_history";
            return;
        }
        QFile file_date_time_rem(path_date_time_rem);
        if (file_date_time_rem.open(QIODevice::ReadWrite | QIODevice::Text))
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
        if (file_new_rem.open(QIODevice::ReadWrite | QIODevice::Text))
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
    showAllHistory();
}



void History::on_clear_button_clicked()
{
    /*QMessageBox qm;
    qm.setText("Вы уверены что хотите очистить историю?");
    qm.setIcon(QMessageBox::Information);
    qm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qm.button(QMessageBox::No)->animateClick(1500);
    qm.setButtonText(QMessageBox::Yes, tr("Да"));
    qm.setButtonText(QMessageBox::No, tr("Нет"));
    int button = qm.exec();
    if (button == QMessageBox::Yes) {
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
        if (!QDir().mkpath(QDir::currentPath().append("/data/history/documents")))
        {
            add_to_log(Q_FUNC_INFO,"error in mkpath History");
        }
    }*/
}

void History::on_recover_button_clicked()
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

    /*for (int j = 0; j < array_file_names[i].files_arr.size(); j++) {
        qDebug() << array_file_names[i].files_arr[j] << " ";
    }
    qDebug() << "hello    ------ " << array_date_time[i].day;*/
    emit add_date_time_sig(array_date_time[i], array_file_names[i].files_arr);
    emit add_new_remind_sig(array_messages[i]);
    if (!QDir(QDir::currentPath().append("/data/history/documents")).exists()) {
        QDir().mkdir(QDir::currentPath().append("/data/history/documents"));
    }
    if (!QDir(QDir::currentPath().append("/data/documents")).exists()) {
        QDir().mkdir(QDir::currentPath().append("/data/documents"));
    }
    for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
    {
        //str_files.append(array_file_names[i].files_arr[j]).append("\n");
        QString file_name_str = array_file_names[i].files_arr[j];
        if (file_name_str == "NULL") { // if file don`t attached
            continue;
        }
        if (QFile::exists(QDir::currentPath().append("/data/documents/").append(file_name_str)))
        {
            QFile::remove(QDir::currentPath().append("/data/documents/").append(file_name_str));
        }
        bool ok = QFile::copy(QDir::currentPath().append("/data/history/documents/").append(file_name_str),
                              QDir::currentPath().append("/data/documents/").append(file_name_str));
        if (!ok)
        {
            //QMessageBox::information(this, "Ошибка", QString("Не удалось скопировать файл ").append(file_name_str));
            //m_trayIcon->showMessage(QString("Не удалось скопировать файл ").append(file_name_str),"");
            add_to_log(Q_FUNC_INFO,"Не удалось скопировать файл ");
        }
        //if (!QFile(QDir::currentPath().append("/data/history/documents/").append(array_file_names[i].files_arr[j])).remove())
        //    add_to_log(Q_FUNC_INFO,"error in remove one document");

    }

    emit update_remembers_sig();
    showMessageSignal("Запись восстановлена!", "d");

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
            //qDebug() << "error in open add_history remind_text";
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

void History::on_delete_button_clicked()
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

    qm.setText("Вы уверены что хотите удалить напоминание из истории?");
    qm.setIcon(QMessageBox::Information);
    qm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qm.setButtonText(QMessageBox::Yes, tr("Да"));
    qm.setButtonText(QMessageBox::No, tr("Нет"));
    qm.button(QMessageBox::No)->animateClick(1500);
    int button = qm.exec();
    if (button == QMessageBox::Yes) {
    ui->tableWidget->removeRow(currRow);
    for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
    {
        if (!QFile(QDir::currentPath().append("/data/history/documents/").append(array_file_names[i].files_arr[j])).remove())
            add_to_log(Q_FUNC_INFO,"error in remove one document");
    }
    array_date_time.erase(array_date_time.begin()+i);
    array_messages.erase(array_messages.begin()+i);
    array_file_names.erase(array_file_names.begin()+i);
    }

    writeOnDisk();



}




void History::writeOnDisk(void)
{
    if (!QFile(QDir::currentPath().append("/data/history/reminders_h.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove reminders_h.txt");
    if (!QFile(QDir::currentPath().append("/data/history/date_time_rem_h.txt")).remove())
        add_to_log(Q_FUNC_INFO,"error in remove date_time_rem_h.txt");
    int size = array_date_time.size();
    for(int i = 0; i < size; i++)
    {
        add_history(array_messages[i], array_date_time[i], array_file_names[i].files_arr);
    }
}

void History::on_find_button_clicked()
{
    QString find_text = ui->findEdit->text();
    if (find_text.isEmpty()) {
        this->showAllHistory();
        return;
    }
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList name_table;
    name_table << "Дата" << "Время" << "Напоминание" << "Файл";
    ui->tableWidget->setHorizontalHeaderLabels(name_table);

    for (int i = 0; i < array_date_time.size(); i++)
    {
        QString dayRow = QString::number(array_date_time[i].day);
        if (array_date_time[i].day < 10) {
            dayRow = "0" + dayRow;
        }
        QString monthRow = QString::number(array_date_time[i].month);
        if (array_date_time[i].month < 10) {
            monthRow = "0" + monthRow;
        }

        QString date = dayRow.append(".").append(monthRow).append(".").append(QString::number(array_date_time[i].year));

        if (array_messages[i].indexOf(find_text,0,Qt::CaseInsensitive) < 0) {
            if (date.indexOf(find_text, 0, Qt::CaseInsensitive) < 0) {
                bool findFlag = false;
                for (int j = 0; j < array_file_names[i].files_arr.size(); j++)
                {
                    if (array_file_names[i].files_arr[j].indexOf(find_text,0,Qt::CaseInsensitive) > 0) {
                        findFlag = true;
                        break;
                    }
                }
                if (!findFlag) {
                    continue;
                }
            }
        }

        insertRow(&array_date_time, &array_messages, &array_file_names, i);

    }
    ui->tableWidget->selectRow(0);
}

void History::on_showAll_button_clicked()
{
    ui->findEdit->setText("");
    this->showAllHistory();
}
