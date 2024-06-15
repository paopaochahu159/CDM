#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QDialog>
#include<QCloseEvent>

namespace Ui {
class SetupWindow;
}

class SetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SetupWindow(QWidget *parent = nullptr);
    ~SetupWindow();

signals:
    void kill();

private slots:
    void on_comboBox_activated(int index);

private:
    virtual void closeEvent(QCloseEvent* event) override{
        event->accept();
        emit kill();
    }

private:
    Ui::SetupWindow *ui;
};

#endif // SETUPWINDOW_H
