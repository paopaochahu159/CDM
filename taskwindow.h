#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();

private:
    Ui::TaskWindow *ui;
};

#endif // TASKWINDOW_H
