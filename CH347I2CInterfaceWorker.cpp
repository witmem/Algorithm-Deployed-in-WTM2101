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

#include "CH347I2CInterfaceWorker.h"
#include <QDebug>
#include <QFileInfo>
#include <QtEndian>

CH347I2CInterfaceWorker::CH347I2CInterfaceWorker(QObject *parent)  : QObject(parent)
{
    m_lSpiI2cGpioDevIndex = -1;
}

CH347I2CInterfaceWorker::~CH347I2CInterfaceWorker()
{

}

BOOL CH347I2CInterfaceWorker::openDevice()
{    
    if(m_lSpiI2cGpioDevIndex == (ULONG)CB_ERR){
        goto Exit;
    }
    if(m_bDevIsOpened == true){
        closeDevice();
    }
    m_bDevIsOpened = (CH347OpenDevice(m_lSpiI2cGpioDevIndex) != INVALID_HANDLE_VALUE);
    CH347SetTimeout(m_lSpiI2cGpioDevIndex,500,500);
Exit:
    return m_bDevIsOpened;
}

BOOL CH347I2CInterfaceWorker::closeDevice()
{
    CH347CloseDevice(m_lSpiI2cGpioDevIndex);
    m_bDevIsOpened = false;
    return true;
}

WINBOOL CH347I2CInterfaceWorker::ch347InitI2c(I2CDATACFG sCfgData)
{
    bool status =  CH347I2C_Set(m_lSpiI2cGpioDevIndex,sCfgData.iClock);
    return status;
}

WINBOOL CH347I2CInterfaceWorker::i2cWorkerWriteByte(UCHAR *pRegisterAddress, UCHAR *pFileData, ULONG nFileSize)
{
    BOOL RetVal = FALSE;
    ULONG iLength = 0;
    UCHAR buffer[nFileSize*2];
    UCHAR oReadBuffer[nFileSize*2];
    UCHAR devAddress = (0x67 << 1)|0;
    buffer[iLength++] = devAddress;

    memcpy(buffer + iLength, pRegisterAddress, 4);
    iLength += 4;
    memcpy(buffer + iLength, pFileData,nFileSize);
    iLength += nFileSize;
    RetVal = CH347StreamI2C(m_lSpiI2cGpioDevIndex,iLength,buffer,iLength,oReadBuffer);
    if(m_bIsWriteFile == true){
        emit signalI2cShowBar(m_nFileDataTailSize,nFileSize);
    }
    return RetVal;
}

void CH347I2CInterfaceWorker::slotI2cCfgData(I2CDATACFG sCfgData)
{
    m_lSpiI2cGpioDevIndex = sCfgData.nSelectDevIndex;
    bool status = true;
    status = openDevice();
    if(status == false){
        emit signalI2cStatus(DEV_OPEN, status);
        return;
    }

    status = ch347InitI2c(sCfgData);
    if(status == false){
        emit signalI2cStatus(DEV_INITSET, status);
        return;
    }
    emit signalI2cStatus(DEV_NORMAL, status);
}

void CH347I2CInterfaceWorker::slotFilePath(const QString &strBinFilePath, const QString &resAddress)
{
    m_nFileDataTailSize = 0;
    m_nSendCount = 0;

    FileDealManger * pFileOperation = new  FileDealManger(strBinFilePath);
    m_nFileDataTailSize = pFileOperation->getFileSizeTotal();
    m_nSendCount = m_nFileDataTailSize / WRITE_I2C_DATA_SIZE;
    int nFinalDataSize = m_nFileDataTailSize % WRITE_I2C_DATA_SIZE;
    if(nFinalDataSize > 0){
        m_nSendCount += 1;
    }

    int nFullSize = 0;
    int transmissionsCount;
    UCHAR * pStarttAddress = new UCHAR[4];
    bool ok;
    int dec = resAddress.toInt(&ok,16);
    m_bIsWriteFile = true;
    uchar *pArrySectionalData256= new uchar[1024];
    for(transmissionsCount = 0; transmissionsCount < m_nSendCount;){
        pFileOperation->getI2cFilePiecewiseArry(nFinalDataSize, transmissionsCount,m_nSendCount,&pArrySectionalData256,nFullSize);
        int nSpiAdress = dec + transmissionsCount*WRITE_I2C_DATA_SIZE;
        pFileOperation->int32To8Byte(nSpiAdress, pStarttAddress);
        BOOL status = i2cWorkerWriteByte(pStarttAddress , pArrySectionalData256 ,nFullSize);
        if(status == true){
            transmissionsCount++;
        }else{
            return;
        }
    }

    if(transmissionsCount == m_nSendCount){
        m_bIsWriteFile = false;
        if(pStarttAddress){
            delete pStarttAddress;
            pStarttAddress = nullptr;
        }

        if(pArrySectionalData256){
            delete pArrySectionalData256;
            pArrySectionalData256 = nullptr;
        }

        if(pFileOperation){
            delete pFileOperation;
            pFileOperation = nullptr;
        }
    }
}

void CH347I2CInterfaceWorker::slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry)
{
    (void)nPortType;
    (void)WriteCount;

    BOOL status = false;
    status= i2cWorkerWriteByte(pAddressArryHead , dataArry ,4);
    emit signalInitMpuIsSucess(status);
    if(pAddressArryHead != nullptr){
        delete pAddressArryHead;
        pAddressArryHead = nullptr;
    }
    if(dataArry != nullptr){
        delete dataArry;
        dataArry = nullptr;
    }
}

void CH347I2CInterfaceWorker::slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail)
{
    (void)nPortType;

    i2cWorkerWriteByte(pAddressArryTail , pDataArryTail, 4);
    if(WriteCount == WRITE_CMD_COUNT -1){
        closeDevice();
        emit signalDevClose();
    }

    if(pAddressArryTail != nullptr){
        delete pAddressArryTail;
        pAddressArryTail = nullptr;
    }

    if(pDataArryTail != nullptr){
        delete pDataArryTail;
        pDataArryTail = nullptr;
    }
}

