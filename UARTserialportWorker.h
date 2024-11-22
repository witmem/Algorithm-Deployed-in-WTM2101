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

#ifndef UARTSERIALPORTWORKER_H
#define UARTSERIALPORTWORKER_H
#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QtEndian>
#include <QApplication>
#include <QProcess>

#include "DataDefine.h"
#include "FileDealManger.h"

class UARTserialportWorker: public QObject
{
    Q_OBJECT

public:
    explicit UARTserialportWorker(QObject *parent = nullptr);
    ~ UARTserialportWorker();
public:
    int getFileTailNumber();
signals:
    void signalShowProcessDialog();
    void signalOpenSerialStatus(bool isOpen = true);
    void signalSerial(QByteArray arry);
    void signalShowBar(int maxValue, int Value);
    void signalShowErrorMsg(QString errMsg);
    void signalInitMpuIsSucess(bool status);

public slots:
    void setComCfg(SETSERIALPORT serPortData);
    void slotFilePath(const QString& path, const QString& Suffix);
    void soltReadSerialData();
    void slotTestData();
    void slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry);
    void slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail);

private:
    void uartFileDealEscape(uchar* arry, int nSoucreSize ,uchar** data, int& nEscapeSize);
    QByteArray intToArry16(int nData);
    void uartWriteFileBytes(int transmissionsCount);
    void uartWriteCMD(WRITE_CMD_TYPE cmdDataType);
    QByteArray uartCharDealEscape(uchar ch);
    uchar calCrcTable(uchar *ptr, int len);
    BOOL spiSetGpio();
    BOOL setGpio(UCHAR gpio, UCHAR oSetDirOut, UCHAR oSetDataOut);
    UCHAR getGpio();

private:
    QSerialPort *m_pUartSerialPort;
    QString m_strUartRegadress;
    bool m_bFileDataWriteStatus = false;
    bool m_bStartCmdIsWriteFinshed = false;
    int m_nStartCmdCount = 1;

    int m_nSendCount;
    int m_nTransmissionsCount;
    int m_nFileDataTailSize;
    int m_nLastPackDataSize = 0;
    int m_nEndWriteCount = 1;
    QByteArray m_arryStartCmd;
    QByteArray m_arryEndCmd;
    QString m_strFilePath;
    FileDealManger * m_pFileOperation = nullptr;
    uchar *m_pArrySectionalData256;
};

#endif // UARTSERIALPORTWORKER_H
