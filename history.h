#ifndef HISTORY_H
#define HISTORY_H

#include <QMainWindow>

namespace Ui {
class History;
}

class History : public QMainWindow
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = 0);
    ~History();

private:
    Ui::History *ui;
public slots:
    void update_history(void);
private slots:
    void on_clear_button_clicked();
};

#endif // HISTORY_H
