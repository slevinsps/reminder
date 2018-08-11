#ifndef TABLE_DATE_H
#define TABLE_DATE_H
//#include "mainwindow.h"
#include "structers.h"
#include <QMainWindow>
#include "update.h"

namespace Ui {
class table_date;
}

class table_date : public QMainWindow
{
    Q_OBJECT

public:
    std::vector <date_time> *array_date_time;
    std::vector <QString> *array_messages;
    std::vector <Files> *array_file_names;
    explicit table_date(QWidget *parent = 0);
    void closeEvent( QCloseEvent* event );
    ~table_date();

private:
    Ui::table_date *ui;
    Update *update_win;
public slots:
    void recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*);
    void update_table(int);
private slots:
    void on_delete_button_clicked();
    void on_clear_button_clicked();
    void on_updateButton_clicked();

    void on_openButton_clicked();

signals:
    void add_new_remind_sig(QString);
    void add_date_time_sig(date_time&, std::vector <QString>&);
    void send_trey_not(QString, QString);
    void sendData(date_time*, QString*, Files*, int);
};

#endif // TABLE_DATE_H
