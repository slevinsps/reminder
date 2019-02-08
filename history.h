#ifndef HISTORY_H
#define HISTORY_H

#include <QMainWindow>
#include <QMessageBox>
#include "ui_history.h"
#include "structers.h"
#include "logger.h"
namespace Ui {
class History;
}
void add_history(QString remind_text, date_time &nem_date_time, std::vector <QString>& filename);
class History : public QMainWindow
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = 0);
    void closeEvent( QCloseEvent* event );
    ~History();

private:
    Ui::History *ui;
    std::vector <date_time> array_date_time;
    std::vector <QString> array_messages;
    std::vector <Files> array_file_names;
    QMessageBox qm;
public slots:
    void update_history(void);
private slots:
    void on_clear_button_clicked();
    void on_recover_button_clicked();
    void on_delete_button_clicked();

signals:
    void add_new_remind_sig(QString);
    void add_date_time_sig(date_time&, std::vector <QString>&);
    void update_remembers_sig();
    void send_trey_not(QString, QString);
};

#endif // HISTORY_H
