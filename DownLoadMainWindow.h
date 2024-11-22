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

#ifndef DOWNLOADMAINWINDOW_H
#define DOWNLOADMAINWINDOW_H
#include <QMainWindow>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QTextCodec>

#include "UARTserialportWorker.h"
#include "CH347I2CInterfaceWorker.h"
#include "CH347SpiInterfaceWorker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DownLoadMainWindow;
}
QT_END_NAMESPACE

class DownLoadMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    DownLoadMainWindow(QWidget *parent = nullptr);

    ~DownLoadMainWindow();

private slots:
    void on_UARTNextStepButton_clicked();
    void on_SPINextStepButton_clicked();
    void on_I2CNextStepButton_clicked();
    void on_UARTupteSerialButton_clicked();
    void on_spiResDevBtn_clicked();
    void on_I2cResDevBtn_clicked();
    void on_spiSetGpioBtn_clicked();
    void on_uartSetGpioSetBtn_clicked();

private:
    void initUi();
    void initPort();
    void initI2CUi();
    void initSPIUi();
    void enumDevice(QStringList &devList);
    BOOL setGpio(UCHAR gpio, UCHAR oSetDirOut, UCHAR oSetDataOut);
    UCHAR getGpio();
    BOOL spiSetGpio();
    BOOL uartSetGpio();
    void keyPressEvent(QKeyEvent *event);

signals:
    void signalSerialCfg(SETSERIALPORT setPortData);
    void signalSPIDataCfg(SPIDATACFG SPICfgData);
    void signalI2CDataCfg(I2CDATACFG I2cCfgData);
    void signalCH347OpenDev();

private:
    Ui::DownLoadMainWindow *ui;

private:
    mDeviceInforS m_sSpiI2cDevInfor[16] = {0};

private:
    UARTserialportWorker  *m_pSerialPortWork;
    QThread *m_pUartThread = nullptr;

    CH347SpiInterfaceWorker *m_pSpiWorker;
    QThread *m_pSpiThread = nullptr;

    CH347I2CInterfaceWorker *m_pI2cWorker;
    QThread *m_pI2cThread = nullptr;
};
#endif // DOWNLOADMAINWINDOW_H
