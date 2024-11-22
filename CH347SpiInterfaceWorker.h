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

#ifndef CH347SPIINTERFACEWORKER_H
#define CH347SPIINTERFACEWORKER_H
#include <QObject>
#include "DataDefine.h"
#include "FileDealManger.h"

#define WM_CH347DevArrive WM_USER+10
#define WM_CH347DevRemove WM_USER+11

#define CH347DevID "VID_1A86&PID_55D\0"

class CH347SpiInterfaceWorker : public QObject
{
    Q_OBJECT

public:
    explicit CH347SpiInterfaceWorker(QObject *parent = nullptr);
    ~ CH347SpiInterfaceWorker();

private:
    bool openDevice();
    bool closeDevice();
    BOOL ch347InitSpi(SPIDATACFG sCfgData);
    BOOL ch347SpiCsCtrl(USHORT ChipSec = 0);
    BOOL spiWorkerWriteByte(UCHAR* pRegisterAddress, UCHAR *pFileData, ULONG nFileSize);

public:
    void CALLBACK UsbDevPnpNotify (ULONG iEventStatus);

public slots:
    void slotSpiCfgData(SPIDATACFG sCfgData);
    void slotFilePath(const QString &strBinFilePath,const QString& resAddress);
    void slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry);
    void slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail);

signals:
    void signalSpiStatus(int nStatusType ,int nStatus);
    void signalSpiShowBar(int maxValue, int Value);
    void signalInitMpuIsSucess(bool status);

private:
    bool m_bDevIsOpened;
    bool m_bEnablePnPAutoOpen;
    bool m_bIsWriteFile = false;
    int m_nFileDataTailSize = 0;
    int m_nSendCount = 0;
    ULONG m_lChipSelect;
    ULONG m_lPolaLowOrHigh;
    ULONG m_lSpiI2cGpioDevIndex;
    mDeviceInforS m_sSpiI2cDevInfor[16] = {0};
};

#endif // CH347SPIINTERFACEWORKER_H
