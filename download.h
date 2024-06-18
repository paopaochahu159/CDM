#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QDialog>

#include<QMutexLocker>

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
    explicit Download(const QUrl&, const int, const QVector<qint64>&, QObject *parent = nullptr);
    ~Download();
    void start();
    void stop();
    void go_on();


private slots:
    void onFinished();
    void onDownloadProgress(const qint64&, const qint64&);

signals:
    void refresh_signal(const int, const qint64, const qint64);
    void thread_finished_signal();

private:
    QNetworkAccessManager *manager = nullptr; // 网络管理器
    QNetworkReply *reply = nullptr;           // 网络应答
    QString m_fileName;
    QMutex mutex;

    int fileOrder = 0;
    QVector<qint64> a;
    qint64 bytes = 0;
    qint64 m_bytesReceived = 0;
    QUrl url;
};

#include <QThread>
#include<QCoreApplication>
#include<QStandardPaths>
#include<QRegularExpression>

// 下载管理类，负责管理多个下载任务
class DownloadManager : public QObject
{
    Q_OBJECT

public:
    // 构造函数
    DownloadManager(const QUrl&, QObject *parent = nullptr);
    ~DownloadManager();
    // 开始下载任务
    void startDownload();
    void stop();
    void go_on();

signals:
    //头部信息解析完成 可用解开按钮并更新界面
    void header_OK(const QString&, const double&);
    void add_progress();
    void refresh_signal(const int, const qint64, const qint64);
    void completes_signals();



private slots:
    // 处理下载完成
    void merging_data();
    void get_SizeAndName(QNetworkReply*);

private:
    void divide_equally(const qint64& fileSize);


private:
    QList<QThread*> threads; // 线程列表
    QList<Download*> tasks; // 任务列表
    QVector<QVector<qint64>> vecSize;


    QUrl url;
    QString fileName;
    QAtomicInt num = 0;
};

#endif // DOWNLOAD_H
