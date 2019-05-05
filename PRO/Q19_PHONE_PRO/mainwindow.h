#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>
#include "QByteArray"
#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothaddress.h>
#include <QtBluetooth/qbluetoothdevicediscoveryagent.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QGraphicsView>
#include <QTimer>
#include <qdatetime.h>

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

    void addBlueToothDevicesToList(const QBluetoothDeviceInfo&);

    void itemActivated(QListWidgetItem *item);

    void readBluetoothDataEvent();

    void bluetoothConnectedEvent();

    void bluetoothDisconnectedEvent();

    void on_pushButton_openBluetooth_clicked();

    void on_pushButton_scan_clicked();

    void on_pushButton_disconnect_clicked();

    void on_pushButton_closeDevice_clicked();

    void on_pushButton_clear_clicked();

    void timerUpdate(void);




    void on_pushButton_enter_clicked();

    void on_pushButton_passwd_clear_clicked();

    void on_pushButton_send_open_clicked();

    void on_pushButton_send_close_clicked();

private:
    Ui::MainWindow *ui;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QBluetoothSocket *socket;
    QByteArray rxDataBuffer;
    QString bluetooth_label;
    QTimer *timer;
    QBluetoothDeviceInfo *deviceInfo;

    bool    passwd_check;

};

#endif // MAINWINDOW_H
