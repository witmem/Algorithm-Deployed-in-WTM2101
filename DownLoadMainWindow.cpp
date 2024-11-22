/******************************************************************************************
**  Copyright 2023 Hangzhou Zhicun (Witmem) Technology Co., Ltd.  All rights reserved.   **
**                                                                                       **
**  This program is free software: you can redistribute it and/or modify                 **
**  it under the terms of the GNU General Public License as published by                 **
**  the Free Software Foundation, either version 3 of the License, or                    **
**  (at your option) any later version.                                                  **
**                                                                                       **
**  This program is distributed in the hope that it will be useful,                      **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of                       **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        **
**  GNU General Public License for more details.                                         **
**                                                                                       **
**  You should have received a copy of the GNU General Public License                    **
**  along with this program.  If not, see http://www.gnu.org/licenses/.                  **
*******************************************************************************************/

#include "DownLoadMainWindow.h"
#include "ui_DownLoadMainWindow.h"
#include "PassObjectFileDialog.h"
#include <QDebug>
DownLoadMainWindow::DownLoadMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownLoadMainWindow)
{
    ui->setupUi(this);
    initUi();
    initPort();
    initI2CUi();
    initSPIUi();
    ui->pushButton->hide();
}

DownLoadMainWindow::~DownLoadMainWindow()
{
    if(m_pSerialPortWork != nullptr){
        m_pSerialPortWork->deleteLater();
    }
    if(m_pUartThread != nullptr){
        m_pUartThread->quit();
        m_pUartThread->wait();
        m_pUartThread->deleteLater();
    }


    if(m_pSpiWorker != nullptr){
        m_pSpiWorker->deleteLater();
    }

    if(m_pSpiThread != nullptr){
        m_pSpiThread->quit();
        m_pSpiThread->wait();
        m_pSpiThread->deleteLater();
    }

    if(m_pI2cWorker != nullptr){
        m_pI2cWorker->deleteLater();
    }
    if(m_pI2cThread != nullptr){
        m_pI2cThread->quit();
        m_pI2cThread->wait();
        m_pI2cThread->deleteLater();
    }

    delete ui;
}

void DownLoadMainWindow::initUi()
{
    m_pSerialPortWork = nullptr;
    m_pSpiWorker = nullptr;
    m_pI2cWorker = nullptr;

    ui->tabWidget->setTabText(0, "I2C 协议");
    ui->tabWidget->setTabText(1, "SPI 协议");
    ui->tabWidget->setTabText(2, "UART 协议");
    ui->tabWidget->setCurrentIndex(0);

    ui->I2CTabWidget->setObjectName("tabwidget");
    ui->SPITabWidget->setObjectName("tabwidget");
    ui->UartTabWidget->setObjectName("tabwidget");

    ui->UARTupteSerialButton->setObjectName("grayBtn");
    ui->I2cResDevBtn->setObjectName("grayBtn");
    ui->spiResDevBtn->setObjectName("grayBtn");
    ui->UARTNextStepButton->setObjectName("importBtn");
    ui->I2CNextStepButton->setObjectName("importBtn");
    ui->SPINextStepButton->setObjectName("importBtn");
    ui->spiSetGpioBtn->setObjectName("importBtn");
    ui->uartSetGpioSetBtn->setObjectName("importBtn");

    this->setFixedSize(700,500);
    setWindowIcon(QIcon(":/Resource/icon/logo16.png"));

    qRegisterMetaType<SETSERIALPORT>("SETSERIALPORT");
    qRegisterMetaType<SPIDATACFG>("SPIDATACFG");
    qRegisterMetaType<I2CDATACFG>("I2CDATACFG");
    qRegisterMetaType<_SPI_CONFIG>("_SPI_CONFIG");
}

void DownLoadMainWindow::initPort()
{
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos){
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            ui->choseSerialPortBox->addItem(info.portName());
            serial.close();
        }
    }

    QStringList baudList;
    QStringList parityList;
    QStringList dataBitsList;
    QStringList stopBitsList;

    baudList<<"1200"<<"2400"<<"4800"<<"9600"
           <<"14400"<<"19200"<<"38400"<<"56000"
          <<"57600"<<"115200";
    ui->choseBitRateBox->addItems(baudList);
    ui->choseBitRateBox->setCurrentIndex(9);

    parityList<<"无"<<"奇"<<"偶";
    ui->choseDigitBox->addItems(parityList);
    ui->choseDigitBox->setCurrentIndex(2);

    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->choseDataBitBox->addItems(dataBitsList);
    ui->choseDataBitBox->setCurrentIndex(3);

    stopBitsList<<"1"<<"2";
    ui->stopBigitBox->addItems(stopBitsList);
    ui->stopBigitBox->setCurrentIndex(0);
}

void DownLoadMainWindow::initI2CUi()
{
    QStringList frequencyList;
    frequencyList<<"20KHz"<<"100KHz"<<"400KHz" << "750KHz";
    ui->I2CClockBox->addItems(frequencyList);
    ui->I2CClockBox->setCurrentIndex(1);
}

void DownLoadMainWindow::initSPIUi()
{
    QStringList InitList;
    InitList  << "POLA_LOW"  << "POLA_HIGH";
    ui->SPIsalveHighOrLowBox->addItems(InitList);
    ui->SPIsalveHighOrLowBox->setCurrentIndex(0);

    InitList.clear();
    InitList  << "Mode 0" <<"Mode 1" << "Mode 2" <<"Mode 3" ;
    ui->SPIModeSelectBox->addItems(InitList);
    ui->SPIModeSelectBox->setCurrentIndex(1);
    InitList.clear();

    InitList <<"LSB" <<"MSB";
    ui-> SPIBitOrderaBox->addItems(InitList);
    ui-> SPIBitOrderaBox->setCurrentIndex(1);
    InitList.clear();

    InitList <<"CS1" <<"CS2";
    ui->SPIChipSelectBox->addItems(InitList);
    InitList.clear();

    InitList << "60MHz" <<"30MHz" <<"15MHz" <<"7.5MHz"<<"3.75MHz" << "1.875MHz"  <<"937.5KHz "<<"468.75KHz";
    ui->SPIClockBox->addItems(InitList);
    ui->SPIClockBox->setCurrentIndex(7);
    InitList.clear();

    QStringList listDev;
    listDev.clear();
    enumDevice(listDev);
    ui->SPIselectDevBox->addItems(listDev);
    ui->I2CSeleceDevBox->addItems(listDev);
}

void DownLoadMainWindow::on_UARTNextStepButton_clicked()
{
    if(ui->choseSerialPortBox->currentText().isEmpty()){
        QMessageBox::critical(this, tr("提示"), tr("可用串口数量为0!"));
        return;
    }

    if (nullptr == m_pSerialPortWork){
        m_pSerialPortWork = new  UARTserialportWorker;
        connect(this, &DownLoadMainWindow::signalSerialCfg, m_pSerialPortWork, &UARTserialportWorker::setComCfg);
        connect(m_pSerialPortWork, &UARTserialportWorker::signalOpenSerialStatus, this, [=](bool isOpen){
            if(isOpen == false){
                QMessageBox::critical(this, tr("提示"), tr("串口没有打开!"));
                return;
            }else{
                ui->uartSetGpioSetBtn->setStyleSheet("");
                PassObjectFileDialog *pDialog = new  PassObjectFileDialog(ui->tabWidget->currentIndex(), this);
                connect(pDialog, &PassObjectFileDialog::signalFilePath, m_pSerialPortWork, &UARTserialportWorker::slotFilePath);
                connect(pDialog, &PassObjectFileDialog::signalStartCmd, m_pSerialPortWork, &UARTserialportWorker::slotStartWriteCmd);
                connect(pDialog, &PassObjectFileDialog::signalEndCmd, m_pSerialPortWork, &UARTserialportWorker::slotEndWriteCmd);
                connect(m_pSerialPortWork, &UARTserialportWorker::signalInitMpuIsSucess, pDialog, &PassObjectFileDialog::slotInitMpuIsSucess);
                connect(m_pSerialPortWork, &UARTserialportWorker::signalShowBar, pDialog, &PassObjectFileDialog::slotProgressBarStart);
                pDialog->exec();
            }
        });
        m_pUartThread = new QThread();
        m_pSerialPortWork->moveToThread(m_pUartThread);
        m_pUartThread->start();
    }

    SETSERIALPORT data;
    data. strName = ui->choseSerialPortBox->currentText().trimmed();
    data. nBaud = ui->choseBitRateBox->currentText().toInt();
    data. nData = ui->choseDataBitBox->currentText().toInt();
    data. nStop = ui->stopBigitBox->currentText().toInt();
    data. nParty = 0;
    data.nSelectDevIndex = ui->I2CSeleceDevBox->currentIndex();

    switch (ui->choseDigitBox->currentIndex()) {
    case 0:
        data. nParty = QSerialPort::NoParity;
        break;
    case 1:
        data. nParty = QSerialPort::OddParity;
        break;
    case 2:
        data. nParty = QSerialPort::EvenParity;
        break;
    default:
        break;
    }

    emit signalSerialCfg(data);
}

void DownLoadMainWindow::on_SPINextStepButton_clicked()
{
    if(ui->SPIselectDevBox->currentText().isEmpty()){
        QMessageBox::critical(this, tr("错误"), tr("请插入一个CH347设备"));
        return;
    }

    if(m_pSpiWorker == nullptr){
        m_pSpiWorker = new CH347SpiInterfaceWorker;
        connect(this, &DownLoadMainWindow::signalSPIDataCfg, m_pSpiWorker, &CH347SpiInterfaceWorker::slotSpiCfgData);
        connect(m_pSpiWorker, &CH347SpiInterfaceWorker::signalSpiStatus, this, [=](int type,int status){
            if(status != true){
                if(type == DEV_INITSET){
                    QMessageBox::critical(this, tr("错误"), QString("设备 %1 初始化失败 ！").arg(ui->SPIselectDevBox->currentIndex()));
                    return;
                }
                if(type == DEV_CHIP_SET){
                    QMessageBox::critical(this, tr("错误"), QString("%1 片选设置失败 ！").arg(ui->SPIChipSelectBox->currentText()));
                    return;
                }
            }else{
                ui->spiSetGpioBtn->setStyleSheet("");
                PassObjectFileDialog *pDialog = new  PassObjectFileDialog(ui->tabWidget->currentIndex(), this);
                connect(pDialog, &PassObjectFileDialog::signalFilePath, m_pSpiWorker, &CH347SpiInterfaceWorker::slotFilePath);
                connect(pDialog, &PassObjectFileDialog::signalStartCmd, m_pSpiWorker, &CH347SpiInterfaceWorker::slotStartWriteCmd);
                connect(pDialog, &PassObjectFileDialog::signalEndCmd, m_pSpiWorker, &CH347SpiInterfaceWorker::slotEndWriteCmd);
                connect(m_pSpiWorker, &CH347SpiInterfaceWorker::signalInitMpuIsSucess, pDialog, &PassObjectFileDialog::slotInitMpuIsSucess);
                connect(m_pSpiWorker, &CH347SpiInterfaceWorker::signalSpiShowBar, pDialog, &PassObjectFileDialog::slotProgressBarStart);
                pDialog->exec();
            }
        });

        m_pSpiThread = new QThread();
        m_pSpiWorker->moveToThread(m_pSpiThread);
        m_pSpiThread->start();
    }

    SPIDATACFG spiCfgData;
    spiCfgData.nSelectDevIndex = ui->SPIselectDevBox->currentIndex();
    spiCfgData.cSPiMode = (UCHAR)ui->SPIModeSelectBox->currentIndex();
    spiCfgData.cBitOrder = (UCHAR)ui->SPIBitOrderaBox->currentIndex();
    spiCfgData.iSpiOutDefaultData = 0XFF;
    spiCfgData.cChipSelect = ui->SPIChipSelectBox->currentIndex();
    spiCfgData.iClock = ui->SPIClockBox->currentIndex();
    spiCfgData.cCS1Polarity = (UCHAR)ui->SPIsalveHighOrLowBox->currentIndex();
    spiCfgData.cCS2Polarity = (UCHAR)ui->SPIsalveHighOrLowBox->currentIndex();

    emit signalSPIDataCfg(spiCfgData);
}

void DownLoadMainWindow::on_I2CNextStepButton_clicked()
{
    if(ui->I2CSeleceDevBox->currentText().isEmpty()){
        QMessageBox::critical(this, tr("错误"), tr("请插入一个CH347设备"));
        return;
    }

    if(m_pI2cWorker == nullptr){
        m_pI2cWorker = new CH347I2CInterfaceWorker;
        connect(this, &DownLoadMainWindow::signalI2CDataCfg, m_pI2cWorker, &CH347I2CInterfaceWorker::slotI2cCfgData);
        connect(m_pI2cWorker, &CH347I2CInterfaceWorker::signalI2cStatus, this, [=](int type,int status){
            if(status != true){
                if(type == DEV_OPEN){
                    QMessageBox::critical(this, tr("错误"), QString("设备 %1 打开失败 ！").arg(ui->SPIselectDevBox->currentText()));
                    return;
                }
                if(type == DEV_INITSET){
                    QMessageBox::critical(this, tr("错误"), QString("设备 %1 初始化失败 ！").arg(ui->SPIselectDevBox->currentText()));
                    return;
                }
            }else{
                PassObjectFileDialog *pDialog = new  PassObjectFileDialog(ui->tabWidget->currentIndex(), this);
                connect(pDialog, &PassObjectFileDialog::signalFilePath, m_pI2cWorker, &CH347I2CInterfaceWorker::slotFilePath);
                connect(pDialog, &PassObjectFileDialog::signalStartCmd, m_pI2cWorker, &CH347I2CInterfaceWorker::slotStartWriteCmd);
                connect(pDialog, &PassObjectFileDialog::signalEndCmd, m_pI2cWorker, &CH347I2CInterfaceWorker::slotEndWriteCmd);
                connect(m_pI2cWorker, &CH347I2CInterfaceWorker::signalInitMpuIsSucess, pDialog, &PassObjectFileDialog::slotInitMpuIsSucess);
                connect(m_pI2cWorker, &CH347I2CInterfaceWorker::signalI2cShowBar, pDialog, &PassObjectFileDialog::slotProgressBarStart);
                pDialog->exec();
            }
        });

        m_pI2cThread = new QThread();
        m_pI2cWorker->moveToThread(m_pI2cThread);
        m_pI2cThread->start();
    }

    I2CDATACFG i2cData;
    i2cData.nSelectDevIndex = ui->I2CSeleceDevBox->currentIndex();
    i2cData.iClock = ui->I2CClockBox->currentIndex();

    emit signalI2CDataCfg(i2cData);
}

void DownLoadMainWindow::on_UARTupteSerialButton_clicked()
{
    ui->choseSerialPortBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos){
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            ui->choseSerialPortBox->addItem(info.portName());
            serial.close();
        }
    }
}

void DownLoadMainWindow::on_spiResDevBtn_clicked()
{
    QStringList listDev;
    enumDevice(listDev);
    ui->SPIselectDevBox->clear();
    ui->SPIselectDevBox->addItems(listDev);
    ui->SPIselectDevBox->setCurrentIndex(0);

    ui->I2CSeleceDevBox->clear();
    ui->I2CSeleceDevBox->addItems(listDev);
    ui->I2CSeleceDevBox->setCurrentIndex(0);
}

void DownLoadMainWindow::enumDevice(QStringList &devList)
{
    devList.clear();
    ULONG i,DevCnt = 0;
    CHAR tem[256] = "";
    mDeviceInforS DevInfor = {0};

    for(i=0;i<16;i++){
        if(CH347OpenDevice(i) != INVALID_HANDLE_VALUE){
            CH347GetDeviceInfor(i,&DevInfor);
            if(DevInfor.ChipMode == 3)
                continue;
            sprintf(tem,"%d# %s",(int)i,DevInfor.FuncDescStr);
            devList.append(tem);
            memcpy(&m_sSpiI2cDevInfor[DevCnt],&DevInfor,sizeof(DevInfor));
            DevCnt++;
        }
        CH347CloseDevice(i);
    }
}

void DownLoadMainWindow::on_I2cResDevBtn_clicked()
{
    QStringList listDev;
    enumDevice(listDev);
    ui->I2CSeleceDevBox->clear();
    ui->I2CSeleceDevBox->addItems(listDev);
    ui->I2CSeleceDevBox->setCurrentIndex(0);

    ui->SPIselectDevBox->clear();
    ui->SPIselectDevBox->addItems(listDev);
    ui->SPIselectDevBox->setCurrentIndex(0);
}
UCHAR DownLoadMainWindow::getGpio()
{
    ULONG SpiI2cGpioDevIndex = ui->SPIselectDevBox->currentIndex();
    BOOL RetVal;
    UCHAR iDir = 0,iData = 0;
    RetVal = CH347GPIO_Get(SpiI2cGpioDevIndex,&iDir,&iData);
    return iData;
}

BOOL DownLoadMainWindow::setGpio(UCHAR gpio, UCHAR oSetDirOut, UCHAR oSetDataOut)
{
    ULONG SpiI2cGpioDevIndex = ui->SPIselectDevBox->currentIndex();
    BOOL  RetVal;
    UCHAR oEnable = 1;
    oEnable |= (1<< gpio);
    if(oSetDirOut == 1){
        oSetDirOut = (1 << gpio) | oSetDirOut;
    }else{
        oSetDirOut = ~(1 << gpio) & oSetDirOut;
    }

    if(oSetDataOut == 1){
        oSetDataOut = (1 << gpio) |getGpio();
    }else{
        oSetDataOut = ~(1 << gpio) &getGpio();
    }

    RetVal = CH347GPIO_Set(SpiI2cGpioDevIndex,oEnable,oSetDirOut,oSetDataOut);
    return RetVal;
}

BOOL DownLoadMainWindow::spiSetGpio()
{
    BOOL status = true ;
    status = setGpio(0,1,0);
    status = setGpio(1,1,1);
    status = setGpio(2,1,0);
    Sleep(100);

    status = setGpio(0,1,1);
    status = setGpio(2,1,1);
    Sleep(100);

    status = setGpio(0,1,0);
    status = setGpio(2,1,1);
    Sleep(100);

    status = setGpio(0,1,1);
    status = setGpio(2,1,0);
    Sleep(100);

    status = setGpio(0,1,0);
    status = setGpio(2,1,0);

    Sleep(100);
    status = setGpio(0,1,1);
    status = setGpio(2,1,1);

    return status;
}

WINBOOL DownLoadMainWindow::uartSetGpio()
{
    BOOL status = true ;

    status = setGpio(0,1,0);
    status = setGpio(1,1,0);
    status = setGpio(2,1,0);
    Sleep(100);

    status = setGpio(1,1,1);
    Sleep(100);

    status = setGpio(1,1,0);
    Sleep(100);

    status = setGpio(1,1,1);
    Sleep(100);

    status = setGpio(1,1,0);
    Sleep(100);

    status = setGpio(1,1,1);
    return status;
}


void DownLoadMainWindow::on_spiSetGpioBtn_clicked()
{
    if(ui->SPIselectDevBox->currentText().isEmpty()){
        QMessageBox::critical(this, tr("错误"), tr("请插入一个CH347设备"));
        return;
    }

    ULONG SpiI2cGpioDevIndex = ui->SPIselectDevBox->currentIndex();
    (CH347OpenDevice(SpiI2cGpioDevIndex) != INVALID_HANDLE_VALUE);
    CH347SetTimeout(SpiI2cGpioDevIndex,500,500);
    bool status = spiSetGpio();
    if(status == true){
        ui->spiSetGpioBtn->setStyleSheet("background:#06ad24;color:white;");
    }
}


void DownLoadMainWindow::on_uartSetGpioSetBtn_clicked()
{
    on_I2cResDevBtn_clicked();

    if(ui->I2CSeleceDevBox->currentText().isEmpty()){
        QMessageBox::critical(this, tr("错误"), tr("请插入一个CH347设备"));
        return;
    }

    ULONG SpiI2cGpioDevIndex = ui->SPIselectDevBox->currentIndex();
    (CH347OpenDevice(SpiI2cGpioDevIndex) != INVALID_HANDLE_VALUE);
    CH347SetTimeout(SpiI2cGpioDevIndex,500,500);


    bool status= uartSetGpio();
    if(status == true){
        ui->uartSetGpioSetBtn->setStyleSheet("background:#06ad24;color:white;");
    }

    CH347CloseDevice(SpiI2cGpioDevIndex);
}

void DownLoadMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1) {
        QString fileName =qApp->applicationDirPath() + "/2302固件下载工具说明书.pdf";
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    QWidget::keyPressEvent(event);
}
