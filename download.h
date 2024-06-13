#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QDialog>

#include<QUrl>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QFile>
#include<QFileDialog>

//处理单个下载
class Download : public QObject
{
    Q_OBJECT
public:
    explicit Download(const QUrl&, const int, QObject *parent = nullptr);
    void start();
    void get_vecSize(const QVector<qint64>& aa){
        a = aa;
    }

private slots:
    void onFinished();
    void onDownloadProgress();

signals:
    void AddTable_signal();
    void refresh_TaskWindow_signal();
    void thread_finished_signal();

private:
    QNetworkAccessManager *manager = nullptr; // 网络管理器
    QNetworkReply *reply = nullptr;           // 网络应答
    QFile *file = nullptr;                    // 输出文件
    QUrl url;
    int fileOrder = 0;
    QVector<qint64> a;
};

#include <QThread>
#include<QCoreApplication>

// 下载管理类，负责管理多个下载任务
class DownloadManager : public QObject
{
    Q_OBJECT

public:
    // 构造函数
    DownloadManager(const QUrl &url, QObject *parent = nullptr);
    ~DownloadManager();

    // 添加一个下载任务
    void addDownload();

private slots:
    // 处理下载完成
    void merging_data();

private:
    void divide_equally(const qint64& fileSize);


private:
    QList<QThread*> threads; // 线程列表
    QList<Download*> tasks; // 任务列表
    QVector<QVector<qint64>> vecSize;

    QAtomicInt done = 0;

    QUrl url;
    qint64 fileSize;
    QString fileName;
};

#endif // DOWNLOAD_H
