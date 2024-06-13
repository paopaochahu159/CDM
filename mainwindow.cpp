#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QHeaderView>

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
    delete dt.d;
    delete dt.task;
    dt.task = nullptr;
    dt.d = nullptr;
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

    //使用正则表达式验证输入的url是否正确
    if (urlRegex.match(u).hasMatch() == false){
        QMessageBox::warning(this, "警告", "输入的地址错误");
        on_actionNew_triggered();
        return "";
    }
    return u;
}

QRegularExpression MainWindow::urlRegex("^https?://(?:www\\.)?\\S+\\.\\S+$");//用来检车url的正则表达式
//创建新任务
void MainWindow::on_actionNew_triggered()
{
    //只允许运行一个未开始下载的窗口
    if (dt.task != nullptr){
        QMessageBox::warning(this, "警告", "请开始下载或关闭窗口");
        return;
    }
    // QString u = get_url();
    QString u = "https://vscode.download.prss.microsoft.com/dbazure/download/stable/dc96b837cf6bb4af9cd736aa3af08cf8279f7685/VSCodeSetup-x64-1.89.1.exe";
    // QString u = "data:image/png;base64,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";
    if (u.isEmpty())
        return;
    QUrl url(u);
    dt.task = new TaskWindow(url);
    dt.task->show();

    //连接信号与曹 实现Download通知窗口进行相应操作
    connect(dt.task, SIGNAL(maintain_signal()), this, SLOT(maintain()));
    connect(dt.task, SIGNAL(delete_signal()), this, SLOT(immediately_delete()));
    // connect(dt.d, SIGNAL(finished_signal()), dt.task, SLOT(onFinished()));
    // connect(dt.d, SIGNAL(AddTable_signal()), this, SLOT(add_table()));
    // connect(dt.d, SIGNAL(refresh_TaskWindow_signal()), dt.task, SLOT(refresh_the_page()));
}

void MainWindow::maintain(){
    qDebug() << "maintain";
    maintain_queue.push_back(dt);
    dt.task = nullptr;
    dt.d = nullptr;
}

void MainWindow::add_table(){
    qDebug() << "add_table";
}

//删除任务
void MainWindow::on_actionD_triggered()
{
    qDebug() << "ads";
}

