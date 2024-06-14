#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QHeaderView>

#include<QStorageInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置工具栏 宽高
    ui->toolBar->setFixedSize(1800,70);
    //设置工具栏 使文字在图片下面
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //设置工具栏 不可移动
    ui->toolBar->setMovable(false);
    //设置列表列 自适应窗口大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    qDebug() << QCoreApplication::applicationDirPath();

    QString folder_name("temporary files");    //要创建的文件夹名称
    QDir dir(QCoreApplication::applicationDirPath());    //初始化dir为exe文件所在的目录
    if(!dir.exists(folder_name))    //如果Images文件夹不存在
    {
        dir.mkdir(folder_name);    //创建文件夹（名为Images）
        qDebug()<<QString("文件夹%1创建成功！").arg(folder_name);
    }
    else
    {
        qDebug()<<QString("文件夹%1已存在！").arg(folder_name);
    }
}

void MainWindow::immediately_delete(){
    delete task;
    task = nullptr;
    qDebug() << "immediately_delete";
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::get_url(){
    // 创建输入对话框
    QInputDialog *in = new QInputDialog();
    in->resize(450, in->width());//设置长度和宽度
    in->setWindowTitle("输入新任务的地址");
    in->setLabelText("地址：");

    // 获取文本输入
    if (in->exec() != QInputDialog::Accepted) {
        delete in;
        return "";
    }
    QString u = in->textValue();
    delete in;//工作完成 释放内存
    //正则表达式
    static QRegularExpression urlRegex("^https?://(?:www\\.)?\\S+\\.\\S+$");
    //使用正则表达式验证输入的url是否正确
    if (urlRegex.match(u).hasMatch() == false){
        QMessageBox::warning(this, "警告", "输入的地址错误");
        on_actionNew_triggered();
        return "";
    }
    return u;
}

//创建新任务
void MainWindow::on_actionNew_triggered()
{
    //只允许运行一个未开始下载的窗口
    // if (task != nullptr){
    //     QMessageBox::warning(this, "警告", "请开始下载或关闭窗口");
    //     return;
    // }
    QString u = get_url();
    // QString u = "https://vscode.download.prss.microsoft.com/dbazure/download/stable/dc96b837cf6bb4af9cd736aa3af08cf8279f7685/VSCodeSetup-x64-1.89.1.exe";
    if (u.isEmpty())
        return;
    QUrl url(u);
    task = new TaskWindow(url);
    task->show();

    //连接信号与曹 实现Download通知窗口进行相应操作
    connect(task, SIGNAL(maintain_signal()), this, SLOT(maintain()));
    connect(task, SIGNAL(delete_signal()), this, SLOT(immediately_delete()));
    // connect(dt.d, SIGNAL(finished_signal()), dt.task, SLOT(onFinished()));
    // connect(dt.d, SIGNAL(AddTable_signal()), this, SLOT(add_table()));
    // connect(dt.d, SIGNAL(refresh_TaskWindow_signal()), dt.task, SLOT(refresh_the_page()));
}

void MainWindow::maintain(){
    qDebug() << "maintain";
    maintain_queue.push_back(task);
    task = nullptr;
}

void MainWindow::add_table(){
    qDebug() << "add_table";
}

//删除任务
void MainWindow::on_actionD_triggered()
{
    qDebug() << "ads";
}

