#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QTextEdit>
#include <QLineEdit>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTcpSocket *m_socket;
    QByteArray m_data;
    qint64 m_nextBlockSize;
    QTextEdit *m_txtInfo;
    QLineEdit *m_txtMat;
    QLineEdit *m_txtVec;
    QPushButton *button;
    QTime start;
    QTime end;

    MainWindow(const QString &host,int port,QWidget *parent = nullptr);
    ~MainWindow();


public slots:

    void sockReady();
    void sockDisc();
    void slotError(QAbstractSocket::SocketError err);
    void slotConnected();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
