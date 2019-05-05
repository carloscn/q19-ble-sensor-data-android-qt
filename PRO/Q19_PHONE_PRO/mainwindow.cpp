#include "mainwindow.h"
#include "ui_mainwindow.h"
static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");

QString hc_05_address = "98:D3:51:F5:B4:60";
static const QString id_num = "01233222";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    //timer->start(1000);
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    localDevice = new QBluetoothLocalDevice();
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    deviceInfo = new QBluetoothDeviceInfo();

    connect(discoveryAgent,
            SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this,
            SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo))
            );
    connect(ui->list,
            SIGNAL(itemActivated(QListWidgetItem*)),
            this,
            SLOT(itemActivated(QListWidgetItem*))
            );
    connect(socket,
            SIGNAL(readyRead()),
            this,
            SLOT(readBluetoothDataEvent())
            );
    connect(socket,
            SIGNAL(connected()),
            this,
            SLOT(bluetoothConnectedEvent())
            );
    connect(socket,
            SIGNAL(disconnected()),
            this,
            SLOT(bluetoothDisconnectedEvent())
            );

    if( localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff ) {
        ui->pushButton_openBluetooth->setEnabled(true);
        ui->pushButton_closeDevice->setEnabled(false);
        ui->pushButton_scan->setEnabled(false);
    }else {
        ui->pushButton_openBluetooth->setEnabled(false);
        ui->pushButton_closeDevice->setEnabled(true);
        ui->pushButton_scan->setEnabled(true);
    }
    ui->pushButton_disconnect->setEnabled(false);
    if( localDevice->hostMode() == QBluetoothLocalDevice::HostDiscoverable ) {
        ui->checkBox_discoverable->setChecked(true);
    }else {
        ui->checkBox_discoverable->setChecked(false);
    }
    on_pushButton_passwd_clear_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpdate(void)
{

}

void MainWindow::on_pushButton_openBluetooth_clicked()
{
    localDevice->powerOn();
    ui->pushButton_closeDevice->setEnabled(true);
    ui->pushButton_openBluetooth->setEnabled(false);
    ui->pushButton_scan->setEnabled(true);
}

void MainWindow::on_pushButton_scan_clicked()
{
    discoveryAgent->start();
    ui->pushButton_scan->setEnabled(false);
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    socket->disconnectFromService();
}

void MainWindow::on_pushButton_closeDevice_clicked()
{
    localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    ui->pushButton_closeDevice->setEnabled(true);
    ui->pushButton_openBluetooth->setEnabled(true);
    ui->pushButton_scan->setEnabled(true);


}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textBrowser->clear();

}

void MainWindow::addBlueToothDevicesToList(const QBluetoothDeviceInfo &info)
{
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());

    QList<QListWidgetItem *> items = ui->list->findItems(label, Qt::MatchExactly);
    qDebug() << "the add label : " + label;
    if (items.empty()) {

        ui->textBrowser->append("address : " + info.address().toString());
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(info.address());

        if( !QString::compare( info.address().toString(),hc_05_address ) ) {
            this->bluetooth_label   =   label;
        }
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
            item->setTextColor(QColor(Qt::green));
        else
            item->setTextColor(QColor(Qt::black));
        ui->list->addItem(item);
    }
}

void MainWindow::itemActivated(QListWidgetItem *item)
{
    QString text = item->text();

    int index = text.indexOf(' ');

    if (index == -1)
        return;

    QBluetoothAddress address(text.left(index));
    QString name(text.mid(index + 1));
    qDebug() << "You has choice the bluetooth address is " << address;
    qDebug() << "The device is connneting.... ";
    QMessageBox::information(this,tr("Info"),tr("The device is connecting..."));
    socket->connectToService(address, QBluetoothUuid(serviceUuid) ,QIODevice::ReadWrite);

}
// 12    3  4     5  6     7  8      9  10    11 12  1314
// @@  *  8H+8L  *  8H+8L  *  8H+8L  *   8H+8L  *  8H+8L *  ##
//head   temp     humi      co        ch4      dust  tail
void MainWindow::readBluetoothDataEvent()
{

    QByteArray rxArray;
    QString rxString;
    QByteArray array;
    QStringList cmd_array;
    rxArray = socket->readAll();
    rxDataBuffer.append(rxArray);
    rxString.clear();
    rxString.append(rxDataBuffer);
}

void MainWindow::bluetoothConnectedEvent()
{

    discoveryAgent->stop();
    qDebug() << "The android device has been connected successfully!";
    QMessageBox::information(this,tr("Info"),tr("Successful connection!"));
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_scan->setEnabled(false);
    socket->write("Welcome use bluetooth.\n\r");
}

void MainWindow::bluetoothDisconnectedEvent()
{
    qDebug() << "The android device has been disconnected successfully!";
    QMessageBox::information(this,tr("Info"),tr("Successful disconnection!"));
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_scan->setEnabled(true);

}



void MainWindow::on_pushButton_enter_clicked()
{
    QString passwd = "2018";
    QString line_str = ui->lineEdit_password->text();
    if( !QString::compare( line_str, passwd ) ) {
        QMessageBox::information(this,tr("Info"),tr("Password is correct!!"));
        ui->pushButton_send_open->setEnabled(true);
        ui->pushButton_send_close->setEnabled(true);
        ui->lineEdit_password->clear();
        ui->pushButton_enter->setEnabled(false);
    }else {
        QMessageBox::information(this,tr("Info"),tr("Password is fail!!"));
    }
}

void MainWindow::on_pushButton_passwd_clear_clicked()
{
    ui->lineEdit_password->clear();
    ui->pushButton_enter->setEnabled(true);
    ui->pushButton_send_close->setEnabled(false);
    ui->pushButton_send_open->setEnabled(false);
}

void MainWindow::on_pushButton_send_open_clicked()
{
    QByteArray  cmd;
    cmd.clear();

    cmd.append(0xAA);
    cmd.append(0x11);
    cmd.append(0xBB);
    socket->write( cmd );

}

void MainWindow::on_pushButton_send_close_clicked()
{
    QByteArray  cmd;
    cmd.clear();

    cmd.append(0xAA);
    cmd.append(0x22);
    cmd.append(0xBB);
    socket->write( cmd );
}
