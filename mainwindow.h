#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"taskwindow.h"
#include"setupwindow.h"

#include <QMainWindow>
#include<QInputDialog>
#include<QMessageBox>
#include<QDir>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //新建任务
    void on_actionNew_triggered();
    //删除任务
    void on_actionD_triggered();
    void add_table();
    void start_download(const int, const QString&, const double&);
    void immediately_delete();

    void on_actionSet_triggered();

private:
    QString get_url();
    virtual void closeEvent(QCloseEvent* event) override{
        for (TaskWindow* t : maintain_queue){
            if (t != nullptr){
                t->accept();
                delete t;
            }
            maintain_queue.clear();
        }
        event->accept();
    }

private:
    Ui::MainWindow *ui;

    int location = 0;
    QVector<TaskWindow *> maintain_queue;
};
#endif // MAINWINDOW_H
