#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"taskwindow.h"

#include <QMainWindow>

#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QInputDialog>
#include<QMessageBox>
#include<QNetworkRequest>
#include<QMessageBox>


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

private:

private:
    Ui::MainWindow *ui;

    static QRegularExpression urlRegex;
};
#endif // MAINWINDOW_H
