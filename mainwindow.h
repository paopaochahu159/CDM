#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<iostream>
using namespace std;

#include"taskwindow.h"
#include"download.h"

#include <QMainWindow>

#include<QInputDialog>
#include<QMessageBox>

#include<QDir>

struct Download_TaskWindow{
    TaskWindow *task = nullptr;
    Download *d = nullptr;
    bool b = false;
};

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
    void maintain();
    void immediately_delete();

private:
    QString get_url();

private:
    Ui::MainWindow *ui;
    //正则表达式
    static QRegularExpression urlRegex;

    Download_TaskWindow dt;
    vector<Download_TaskWindow> maintain_queue;
};
#endif // MAINWINDOW_H
