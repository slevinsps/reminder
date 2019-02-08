#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMessageBox>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <qDebug>

#include "table_date.h"
#include "remind.h"
#include "history.h"
#include "structers.h"

namespace Ui {
class MainWindow;
}





class My_date_time : public QObject
{
    Q_OBJECT
public:
    int count;
    std::vector <QString> file_path;
    std::vector <date_time> array_date_time;
    std::vector <QString> array_messages;
    std::vector <Files> array_file_names;
    int update_arrays(void);
    int qstring_to_date_time(QString line, date_time &new_date_time, std::vector <QString>& filename);

    My_date_time()
    {
        update_arrays();

    }
public slots:
    void remind_demon();
signals:
    close_table();
    stop_timer();
    start_timer(int);
    show_rem_win();
    void sendData_withi(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*, int);
};




class MessageBox : public QMessageBox
{
    Q_OBJECT
public:
    MessageBox()
    {
        QTimer *tmr  = new QTimer();
        tmr->setInterval(2000);
        tmr->start();
        connect(tmr, SIGNAL(timeout()), tmr, SLOT(stop()));
        connect(tmr, SIGNAL(timeout()), this, SLOT(close_all()));
    }
private slots:
    void close_all()
    {
        qDebug() << "mamammamama";
        //this.button(QMessageBox::Ok).animateClick
    }

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
    void on_createButton_clicked();

    void iconActivated( QSystemTrayIcon::ActivationReason reason );
    //void onShowMessageInTray();

    void on_all_rem_Button_clicked();
    void on_add_file_Button_clicked();

    void add_new_remind_slot(QString remind_text);
    void add_date_time_slot(date_time &nem_date_time, std::vector <QString>&);
    void add_trey_not(QString, QString);
    void on_clear_file_Button_clicked();
    void close_stat_win();
    void update_remembers_slot();




    void on_pushButton_clicked();

    void exit_app();

signals:
    void sendData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*);
    void send_trey_not(QString, QString);
    void update_history(void);

private:
    QTimer *tmr_qm;
    QMessageBox qm;
    Ui::MainWindow *ui;
    QSystemTrayIcon* m_trayIcon;
    table_date* stat_win;
    remind *rem_win;
    History *hist_win;
    QString find_dir;
};

#endif // MAINWINDOW_H
