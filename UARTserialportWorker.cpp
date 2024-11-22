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
//test
#include "UARTserialportWorker.h"

uchar head = 0x7F;
uchar tail = 0xF0;
uchar ChipId = 0X01;
uchar WriteCmd = 0x5A;
uchar var = 0XFF;

static const uchar crc_table[256] ={
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3,
};

UARTserialportWorker::UARTserialportWorker(QObject *parent) : QObject(parent)
{
    m_pUartSerialPort = nullptr;
    m_pFileOperation =  nullptr;

    m_bFileDataWriteStatus = false;
    m_nTransmissionsCount = 0;
    m_nFileDataTailSize = 0;
    m_nSendCount = 0;
}

UARTserialportWorker::~UARTserialportWorker()
{
    if (m_pUartSerialPort != nullptr){
        delete m_pUartSerialPort;
        m_pUartSerialPort = nullptr;
    }
}

int UARTserialportWorker::getFileTailNumber()
{
    return m_nFileDataTailSize;
}

void UARTserialportWorker::setComCfg(SETSERIALPORT serPortData)
{
    if (m_pUartSerialPort == nullptr){
        m_pUartSerialPort = new QSerialPort;
        connect(m_pUartSerialPort,&QIODevice::readyRead,this,&UARTserialportWorker::soltReadSerialData);

    }

    if(m_pUartSerialPort->isOpen()){
        m_pUartSerialPort->close();
    }
    m_pUartSerialPort->setPortName(serPortData.strName);
    bool isOpen = m_pUartSerialPort->open(QIODevice::ReadWrite);
    emit signalOpenSerialStatus(isOpen);
    if(isOpen == false){return;}
    m_pUartSerialPort->setBaudRate(serPortData.nBaud);
    m_pUartSerialPort->setParity((QSerialPort::Parity)serPortData.nParty);
    m_pUartSerialPort->setDataBits((QSerialPort::DataBits)serPortData.nData);
    m_pUartSerialPort->setStopBits((QSerialPort::StopBits)serPortData.nStop);
}

void UARTserialportWorker::slotFilePath(const QString &path, const QString& resAddress)
{
    m_strFilePath = path;
    m_strUartRegadress = resAddress;

    m_nTransmissionsCount = 0;
    m_nFileDataTailSize = 0;
    m_nEndWriteCount = 0;
    m_bFileDataWriteStatus =false;
    QFileInfo fileInfo;
    fileInfo.setFile(path);

    m_nFileDataTailSize = fileInfo.size();
    m_nSendCount = m_nFileDataTailSize / WRITE_DATA_SIZE;
    m_nLastPackDataSize = m_nFileDataTailSize % WRITE_DATA_SIZE;
    if(m_nLastPackDataSize > 0){
        m_nSendCount += 1;
    }

    m_pArrySectionalData256= new uchar[1024];

    uartWriteFileBytes(m_nTransmissionsCount);
}

void UARTserialportWorker::uartFileDealEscape(uchar* arry, int nSoucreSize ,uchar** data, int& nEscapeSize)
{
    *data  = arry;

    int n =0;
    for(int i =0 ;i < n + nSoucreSize; i++){
        if(arry[i] == head){
            arry[i] = 0XFF;
            memmove(arry + i +1, arry+i, n + nSoucreSize - i);
            n++;
            arry[i+1] = 0X5F;
            i++;
        }else if(arry[i] == tail){
            arry[i] = 0XFF;
            memmove(arry +i +1, arry+i, n + nSoucreSize - i);
            n++;
            arry[i+1] = 0XD0;
            i++;
        }else if(arry[i] == var){
            arry[i] = 0XFF;
            memmove(arry +i +1, arry+i, n + nSoucreSize - i);
            n++;
            arry[i+1] = 0XDF;
            i++;
        }
    }

    nEscapeSize = n + nSoucreSize;
}

void UARTserialportWorker::uartWriteFileBytes(int transmissionsCount)
{
    FileDealManger *m_pFileOperation = new  FileDealManger(m_strFilePath);
    int nFullSize = 0;
    uchar* EscapeData = nullptr;
    int nEscapeSize = 0;
    QByteArray sendData;
    QByteArray crcArry;
    sendData.clear();
    crcArry.clear();
    bool ok;
    int dec = m_strUartRegadress.toInt(&ok,16);
    int address =dec + transmissionsCount*WRITE_DATA_SIZE;
    QByteArray startAddress =  m_pFileOperation->intToArry32(address);

    m_pFileOperation->getFilePiecewiseArry(m_nLastPackDataSize, transmissionsCount,m_nSendCount,&m_pArrySectionalData256,nFullSize);
    QByteArray sendFileSizeArry = m_pFileOperation->intToArry16(nFullSize);
    QByteArray fileDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(m_pArrySectionalData256), nFullSize);

    crcArry.reserve(nFullSize*3);
    crcArry.append(ChipId);
    crcArry.append(WriteCmd);
    crcArry.append(startAddress);
    crcArry.append(sendFileSizeArry);
    crcArry.append(fileDataArry);

    uartFileDealEscape((uchar*)crcArry.data(),crcArry.size(),&EscapeData, nEscapeSize);
    QByteArray EscapeDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(EscapeData), nEscapeSize);

    uchar crc8  = calCrcTable((uchar*)EscapeDataArry.data(), EscapeDataArry.size());
    QByteArray  Crc8Arry = uartCharDealEscape(crc8);
    Crc8Arry.append(crc8);

    sendData.append(head);
    sendData.append(EscapeDataArry);
    sendData.append(Crc8Arry);
    sendData.append(tail);

    m_pUartSerialPort->write(sendData);
    emit signalShowBar(m_nFileDataTailSize ,nFullSize);
}

void UARTserialportWorker::uartWriteCMD(WRITE_CMD_TYPE cmdDataType)
{
    QByteArray sendDataArry;
    sendDataArry.clear();

    QByteArray dataArry;
    dataArry.append(ChipId);
    dataArry.append(WriteCmd);

    QByteArray addressAndData;
    QByteArray statrDataArry;
    if(cmdDataType == START_CMD){
        addressAndData = m_arryStartCmd.mid(0,4);
        dataArry.append(addressAndData);

        statrDataArry = m_arryStartCmd.mid(4,4);
        m_arryStartCmd = m_arryStartCmd.remove(0,8);
    }else{
        addressAndData = m_arryEndCmd.mid(0,4);
        dataArry.append(addressAndData);

        statrDataArry = m_arryEndCmd.mid(4,4);
        m_arryEndCmd = m_arryEndCmd.remove(0,8);
    }

    dataArry.append(intToArry16(statrDataArry.size()));
    dataArry.append(statrDataArry);

    uchar* pEscapeData;
    int nEscapeSize;
    uartFileDealEscape((uchar*)dataArry.data(),dataArry.size(),&pEscapeData,nEscapeSize);
    QByteArray EscapeDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(pEscapeData), nEscapeSize);

    uchar crc8 = calCrcTable((uchar*)EscapeDataArry.data(), EscapeDataArry.size());
    QByteArray  Crc8Arry = uartCharDealEscape(crc8);


    sendDataArry.append(head);
    sendDataArry.append(EscapeDataArry);
    sendDataArry.append(Crc8Arry);
    sendDataArry.append(tail);

    m_pUartSerialPort->write(sendDataArry);
}

QByteArray UARTserialportWorker::uartCharDealEscape(uchar ch)
{
    QByteArray EscapeArry;
    int Escapesize;
    uchar *data;
    EscapeArry.append(ch);
    uartFileDealEscape((uchar*)EscapeArry.data(),EscapeArry.size(),&data,Escapesize);
    QByteArray EscapeDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(data), Escapesize);
    EscapeArry.clear();
    EscapeArry.append( EscapeDataArry);
    return EscapeArry;
}

uchar UARTserialportWorker::calCrcTable(uchar *ptr, int len)
{
    uchar crc = 0xff;
    while (len--){
        crc = crc_table[crc ^ *ptr++];
    }
    return (crc);
}


void UARTserialportWorker::slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry)
{
    (void)nPortType;
    m_nStartCmdCount = 1;

    QByteArray Address = QByteArray::fromRawData(reinterpret_cast<const char *>(pAddressArryHead), 4);
    QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(dataArry), 4);

    m_arryStartCmd.append(Address);
    m_arryStartCmd.append(data);
    if(WriteCount == WRITE_CMD_COUNT -1 ){
        uartWriteCMD(START_CMD);
    }
}

void UARTserialportWorker::slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail)
{
    (void)nPortType;
    m_nEndWriteCount = 1;

    QByteArray Address = QByteArray::fromRawData(reinterpret_cast<const char *>(pAddressArryTail), 4);
    QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(pDataArryTail), 4);

    m_arryEndCmd.append(Address);
    m_arryEndCmd.append(data);
    if(WriteCount == WRITE_CMD_COUNT -1 ){
        uartWriteCMD(END_CMD);
    }
}

void UARTserialportWorker::soltReadSerialData()
{
    QByteArray buffer;
    buffer.clear();
    buffer.resize(5);
    buffer = m_pUartSerialPort->readAll();

    if(!buffer.isEmpty()){
        m_nStartCmdCount++;
        int fianlIndex = buffer.size() -1;
        if((buffer[0] == (char)0x7F && buffer[fianlIndex] == (char)0xF0) && m_nStartCmdCount <= WRITE_CMD_COUNT){
            uartWriteCMD(START_CMD);
        }

        if(m_nStartCmdCount -1 == WRITE_CMD_COUNT){
            m_bStartCmdIsWriteFinshed = true;
            emit signalInitMpuIsSucess(m_bStartCmdIsWriteFinshed);
        }

        if(m_bStartCmdIsWriteFinshed == true){
            if(m_nSendCount != 0){
                m_nTransmissionsCount++;
            }

            if((buffer[0] == (char)0x7f && buffer[fianlIndex] == (char)0xf0) && m_nTransmissionsCount <= m_nSendCount -1){
                uartWriteFileBytes(m_nTransmissionsCount);
            }
        }
        if(m_nTransmissionsCount -1 == m_nSendCount && m_nSendCount != 0){
            m_bFileDataWriteStatus = true;
        }

        if(m_bFileDataWriteStatus == true){
            m_nEndWriteCount++;
            if((buffer[0] == (char)0x7f && buffer[buffer.size() -1] == (char)0xf0) && m_nEndWriteCount <= WRITE_CMD_COUNT){
                uartWriteCMD(END_CMD);
            }
            if(m_nEndWriteCount -1 == WRITE_CMD_COUNT){
                m_pUartSerialPort->close();
            }
        }
    }
}

void UARTserialportWorker::slotTestData()
{
    QByteArray buffer;
    buffer.clear();
    buffer.resize(5);
    buffer[0]= 0x7F;
    buffer[1]= 0x00;
    buffer[2]= 0x00;
    buffer[3]= 0x00;
    buffer[4]= 0xF0;
    emit signalSerial(buffer);

    if(!buffer.isEmpty()){
        m_nStartCmdCount++;
        int fianlIndex = buffer.size() -1;
        if((buffer[0] == (char)0x7F && buffer[fianlIndex] == (char)0xF0) && m_nStartCmdCount <= WRITE_CMD_COUNT){
            uartWriteCMD(START_CMD);
        }

        if(m_nStartCmdCount -1 == WRITE_CMD_COUNT){
            m_bStartCmdIsWriteFinshed = true;
            emit signalInitMpuIsSucess(m_bStartCmdIsWriteFinshed);
        }

        if(m_bStartCmdIsWriteFinshed == true){
            if(m_nSendCount != 0){
                m_nTransmissionsCount++;
            }

            if((buffer[0] == (char)0x7f && buffer[fianlIndex] == (char)0xf0) && m_nTransmissionsCount <= m_nSendCount -1){
                uartWriteFileBytes(m_nTransmissionsCount);
            }
        }
        if(m_nTransmissionsCount -1 == m_nSendCount && m_nSendCount != 0){
            m_bFileDataWriteStatus = true;
        }

        if(m_bFileDataWriteStatus == true){
            m_nEndWriteCount++;
            if((buffer[0] == (char)0x7f && buffer[buffer.size() -1] == (char)0xf0) && m_nEndWriteCount <= WRITE_CMD_COUNT){
                uartWriteCMD(END_CMD);
            }
            if(m_nEndWriteCount -1 == WRITE_CMD_COUNT){
                m_pUartSerialPort->close();
            }
        }
    }
}

QByteArray UARTserialportWorker::intToArry16(int nData)
{
    int16_t dataSize = (nData -1) & 0xFFFF;
    int16_t *p = &dataSize;
    qToBigEndian(dataSize, (uchar*)p);
    QByteArray sendFileSize = QByteArray::fromRawData(reinterpret_cast<const char *>(p),2);
    QByteArray RawDataArry;
    RawDataArry.append(sendFileSize);
    return RawDataArry;
}

UCHAR UARTserialportWorker::getGpio()
{
    BOOL RetVal;
    UCHAR iDir = 0,iData = 0;
    qDebug("CH347GPIO_Get %s,iDir:%02X,iData:%02X",RetVal?"Succ":"Fail",iDir,iData);
    return iData;
}

BOOL UARTserialportWorker::setGpio(UCHAR gpio, UCHAR oSetDirOut, UCHAR oSetDataOut)
{
    BOOL  RetVal;
    UCHAR oEnable = 1;
    oEnable |= (1<< gpio);


    if(oSetDirOut == 1){
        oSetDirOut = (1 << gpio) | oSetDirOut;
    }else{
        oSetDirOut = ~(1 << gpio) & oSetDirOut;
    }

    if(oSetDataOut == 1){
        oSetDataOut = (1 << gpio) | getGpio();
    }else{
        oSetDataOut = ~(1 << gpio) & getGpio();
    }

    qDebug("CH347GPIO_SetOutput %s,oEnable:%02X,oSetDirOut:%02X,oSetDataOut:%02X",RetVal?"Succ":"Fail",oEnable,oSetDirOut,oSetDataOut);
    return RetVal;
}

BOOL UARTserialportWorker::spiSetGpio()
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
    Sleep(100);

    return status;
}
