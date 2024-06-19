#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QDialog>
#include<QCloseEvent>
#include<QProgressBar>
#include<QTimer>
#include<QMessageBox>

#include"download.h"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TaskWindow(const QUrl&, const int, QWidget *parent = nullptr);
    ~TaskWindow();
    void refresh_location(){
        --location;
    }

signals:
    //发出维护信号 通知维护这个下载任务
    void start_signal(const int, const QString&, const double&);
    void close_signal();
    void schedule_updat_signal(const int, const double, const double);
    void state_signal(const int);

private slots:
    void on_pushButton_2_clicked();
    void add_progressBar();
    void refresh_signal(const int, const qint64, const qint64);
    void schedule_update();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_clicked();

private:
    virtual void closeEvent(QCloseEvent* event) override{
        emit close_signal();
        event->accept();
    }

private:
    Ui::TaskWindow *ui;
    QWidget *whiteWidget;
    QVector<QProgressBar *> bars;

    //用于确定本窗口在维护队列的位置
    int location = 0;
    qint64 Existing_bytes_size = 0;
    QTimer *timer;
    QTime startTime;

    QUrl url;
    DownloadManager *downloadManager;
};

#endif // TASKWINDOW_H
