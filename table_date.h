#ifndef TABLE_DATE_H
#define TABLE_DATE_H
//#include "mainwindow.h"
#include "structers.h"
#include <QMainWindow>

namespace Ui {
class table_date;
}

class table_date : public QMainWindow
{
    Q_OBJECT

public:
    std::vector <date_time> *array_date_time;
    std::vector <QString> *array_messages;
    std::vector <QString> *array_file_names;
    explicit table_date(QWidget *parent = 0);
    void closeEvent( QCloseEvent* event );
    ~table_date();

private:
    Ui::table_date *ui;
public slots:
    void recieveData(std::vector <date_time>*, std::vector <QString>*, std::vector <QString>*);
private slots:
    void on_delete_button_clicked();
    void on_clear_button_clicked();
};

#endif // TABLE_DATE_H
