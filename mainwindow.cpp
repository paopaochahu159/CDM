#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //设置工具栏宽高
    ui->toolBar->setFixedSize(1800,70);
    //设置工具栏 使文字在图片下面
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //设置工具栏不可移动
    ui->toolBar->setMovable(false);

    //设置列表第一个列的宽度
    ui->tableWidget->setColumnWidth(0,220);
    //设置列表列为自适应窗口大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QRegularExpression MainWindow::urlRegex("^https?://(?:www\\.)?\\S+\\.\\S+$");
//新任务
void MainWindow::on_actionNew_triggered()
{
    // // 创建输入对话框
    // QInputDialog *in = new QInputDialog();
    // in->resize(450, in->width());//设置长度和宽度
    // in->setWindowTitle("输入新任务的地址");
    // in->setLabelText("地址：");

    // // 获取文本输入
    // if (in->exec() != QInputDialog::Accepted) {
    //     delete in;
    //     return ;
    // }
    // QString u = in->textValue();
    // delete in;//工作完成 释放内存

    // //使用正则表达式验证输入的url是否正确
    // if (urlRegex.match(u).hasMatch() == false){
    //     QMessageBox::warning(this, "警告", "输入的地址错误");
    //     on_actionNew_triggered();
    //     return;
    // }

    QString u = "https://vscode.download.prss.microsoft.com/dbazure/download/stable/dc96b837cf6bb4af9cd736aa3af08cf8279f7685/VSCodeSetup-x64-1.89.1.exe";
    // QString u = "https://blog.csdn.net/qq_28351609/article/details/88934471";
    QUrl url(u);
    TaskWindow *task = new TaskWindow();
    task->show();
}

//删除任务
void MainWindow::on_actionD_triggered()
{

}

