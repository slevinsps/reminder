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
    stop_timer();
    start_timer(int);
    show_form();
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

    void onTrayIconActivated( QSystemTrayIcon::ActivationReason reason );
    void onShowMessageInTray();

    void on_all_rem_Button_clicked();
    void on_add_file_Button_clicked();

    void add_new_remind_slot(QString remind_text);
    void add_date_time_slot(date_time &nem_date_time, std::vector <QString>&);

    void on_clear_file_Button_clicked();

signals:
    void sendData(std::vector <date_time>*, std::vector <QString>*, std::vector <Files>*);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* m_trayIcon;
    table_date* stat_win;
};

#endif // MAINWINDOW_H
