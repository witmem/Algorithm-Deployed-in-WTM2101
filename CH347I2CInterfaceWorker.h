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

#ifndef CH347I2CINTERFACEWORKER_H
#define CH347I2CINTERFACEWORKER_H

#include <QObject>
#include "lib/CH347DLL.H"
#include "DataDefine.h"
#include "FileDealManger.h"

#define WM_CH347DevArrive WM_USER+10
#define WM_CH347DevRemove WM_USER+11
#define CH347DevID "VID_1A86&PID_55D\0"

class CH347I2CInterfaceWorker : public QObject
{
    Q_OBJECT

public:
    explicit CH347I2CInterfaceWorker(QObject *parent = nullptr);
    ~ CH347I2CInterfaceWorker();

private:
    BOOL closeDevice();
    BOOL openDevice();
    BOOL i2cWorkerWriteByte(UCHAR* pRegisterAddress, UCHAR *pFileData, ULONG nFileSize);
    BOOL ch347InitI2c(I2CDATACFG sCfgData);
    void getFileData(const QString &strBinFilePath, UCHAR** pData, int& nSize);
    void int32To8Byte(int nStartAddress, UCHAR* pStartAddressArry);

public:
    void CALLBACK usbDevPnpNotify (ULONG iEventStatus);

public slots:
    void slotI2cCfgData(I2CDATACFG sCfgData);
    void slotFilePath(const QString &strBinFilePath,const QString& resAddress);
    void slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry);
    void slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail);
signals:
    void signalI2cStatus(int nStatusType ,int nStatus);
    void signalI2cShowBar(int maxValue, int Value);
    void signalReadData(QByteArray ReadArry);
    void signalDevClose();
    void signalInitMpuIsSucess(bool status);
private:
    bool m_bDevIsOpened;
    bool m_bEnablePnPAutoOpen;
    bool m_bIsWriteFile = false;
    ULONG m_lSpiI2cGpioDevIndex;
    mDeviceInforS m_sSpiI2cDevInfor[16] = {0};
    int m_nFileDataTailSize;
    int m_nSendCount;
};

#endif // CH347I2CInterfaceWorker_H
