#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>
#include<QCloseEvent>

#include"download.h"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(const QUrl&, QWidget *parent = nullptr);
    ~TaskWindow();

signals:
    //发出维护信号 通知维护这个下载任务
    void maintain_signal();
    void delete_signal();

private slots:
    //刷新界面
    void refresh_the_page();
    void on_pushButton_2_clicked();
    void onFinished(){
        qDebug() << 1;
    }

private:
    virtual void closeEvent(QCloseEvent* event) override{
        if (b){
            event->accept();
            return;
        }
        emit delete_signal();
        event->accept();
    }

private:
    Ui::TaskWindow *ui;
    QWidget *whiteWidget;
    QUrl url;

    DownloadManager *dow;

    bool b = false;
};

#endif // TASKWINDOW_H
