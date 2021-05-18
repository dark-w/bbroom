#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tcpClient(new QTcpSocket(this)),
    addr("120.27.245.242"),
    port(55559)
{
    ui->setupUi(this);

    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(ui->idInputBox, SIGNAL(returnPressed()), ui->loginButton,
        SIGNAL(clicked()), Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switch_page() {
    qint8 page_index = ui->stackedWidget->currentIndex();
    qint8 page_count = ui->stackedWidget->count();

    page_index++;
    if (page_index == page_count) {
        page_index = 0;
    }

    ui->stackedWidget->setCurrentIndex(page_index);
}

void MainWindow::on_exitButton_clicked()
{
    if (tcpClient->state() == QAbstractSocket::ConnectedState) {
        tcpClient->disconnectFromHost();
        ui->outputBox->clear();

        ui->sendMsgButton->disconnect(ui->inputBox);

        switch_page();

        ui->idInputBox->setFocus();
    } else {
        qDebug() << "disconnect error";
    }
}

void MainWindow::on_loginButton_clicked()
{
    if (tcpClient->state() == QAbstractSocket::UnconnectedState) {
        tcpClient->connectToHost(addr, port);

        // login
        QString id = ui->idInputBox->text();
        QByteArray str = id.toUtf8();
        str.append('\n');
        tcpClient->write(str);

        switch_page();

        connect(ui->inputBox, SIGNAL(returnPressed()), ui->sendMsgButton,
            SIGNAL(clicked()), Qt::UniqueConnection);
        ui->inputBox->setFocus();
    } else {
        qDebug() << "connect error";
    }
}

void MainWindow::on_sendMsgButton_clicked()
{
    QString msg = ui->inputBox->text();
    QByteArray str = msg.toUtf8();
    str.append('\n');

    tcpClient->write(str);

    ui->inputBox->clear();
    ui->inputBox->setFocus();
}

void MainWindow::onSocketReadyRead()
{
    while (tcpClient->canReadLine())
        ui->outputBox->appendPlainText(tcpClient->readLine());
}
