#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSystemTrayIcon>
#include "table_date.h"
#include "structers.h"
namespace Ui {
class MainWindow;
}





class My_date_time : public QObject
{
    Q_OBJECT
public:
    int count;
    std::vector <date_time> array_date_time;
    std::vector <QString> array_messages;
    int update_arrays(void);
    int qstring_to_date_time(QString line, date_time &new_date_time);
    My_date_time()
    {
        update_arrays();
    }
public slots:
    void remind_demon();

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    My_date_time date_time_obj;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent( QCloseEvent* event );

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void onTrayIconActivated( QSystemTrayIcon::ActivationReason reason );
    void onShowMessageInTray();

    void on_pushButton_4_clicked();
signals:
    void sendData(std::vector <date_time>*, std::vector <QString>*);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* m_trayIcon;
    table_date* stat_win;
};

#endif // MAINWINDOW_H
