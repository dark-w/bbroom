#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostInfo>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_exitButton_clicked();

    void on_loginButton_clicked();

    void on_sendMsgButton_clicked();

    void onSocketReadyRead();

private:
    void switch_page();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpClient;
    QString addr;
    quint16 port;
};

#endif // MAINWINDOW_H
