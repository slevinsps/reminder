#ifndef REMIND_H
#define REMIND_H

#include <QMainWindow>
#include "structers.h"

namespace Ui {
class remind;
}

class remind : public QMainWindow
{
    Q_OBJECT

public:
    std::vector <date_time> *array_date_time;
    std::vector <QString> *array_messages;
    std::vector <Files> *array_file_names;
    int num_i;
    explicit remind(QWidget *parent = 0);
    void closeEvent( QCloseEvent* event );
    ~remind();

private slots:
    void on_deleteButton_clicked();
    void recieveData_withi(std::vector<date_time>* one, std::vector<QString>* two, std::vector <Files>* three, int number);

    void on_openButton_clicked();

    void on_waitButton_clicked();


    void show_close();

signals:
    void add_new_remind_sig(QString);
    void add_date_time_sig(date_time&, std::vector <QString>&);
    void show_form();
    start_timer_demon(int);
    void send_trey_not(QString, QString);

private:
    Ui::remind *ui;
    QTimer *tmr;
};

#endif // REMIND_H
