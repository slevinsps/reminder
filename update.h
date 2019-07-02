#ifndef UPDATE_H
#define UPDATE_H

#include <QMainWindow>
#include "structers.h"
#include <vector>

namespace Ui {
class Update;
}

class Update : public QMainWindow
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent = 0);
    ~Update();
public slots:
    void recieveData(date_time*, QString*, Files*, int, int);


private slots:
    void on_add_file_Button_clicked();

    void on_clear_file_Button_clicked();

    void on_createButton_clicked();

private:
    std::vector <QString> files_path;
    date_time *date1;
    QString *comment1;
    Files *files1;
    bool clear;
    int curRow1;
    int rowInArray1;
    Ui::Update *ui;

signals:
    void update_table_sig(int, int);
};

#endif // UPDATE_H
