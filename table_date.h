#ifndef TABLE_DATE_H
#define TABLE_DATE_H
//#include "mainwindow.h"
#include "structers.h"
#include <QMainWindow>
#include <QMessageBox>
#include "update.h"




namespace Ui {
class table_date;
}


class keyEnterReceiver : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* obj, QEvent* event);
signals:
    void findPressSignal();
};

class table_date : public QMainWindow
{
    Q_OBJECT

public:
    std::vector <date_time> *array_date_time;
    std::vector <QString> *array_messages;
    std::vector <Files> *array_file_names;
    explicit table_date(QWidget *parent = 0);
    void writeOnDisk(void);
    void insertRow(std::vector<date_time>* array_date_time, std::vector<QString>* array_messages, std::vector <Files>* array_file_names, int i);
    ~table_date();

private:
    Ui::table_date *ui;
    QMessageBox qm;
    Update *update_win;
    keyEnterReceiver* key;

public slots:
    void recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*);
    void update_table(int curRow, int);
private slots:
    void on_delete_button_clicked();
    void on_clear_button_clicked();
    void on_updateButton_clicked();

    void on_openButton_clicked();

    void on_find_button_clicked();

    void on_showAll_button_clicked();

signals:
    void add_new_remind_sig(QString);
    void add_date_time_sig(date_time&, std::vector <QString>&);
    void showMessageSignal(QString, QString);
    void sendData(date_time*, QString*, Files*, int, int);
};

#endif // TABLE_DATE_H
