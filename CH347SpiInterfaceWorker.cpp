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

#include "CH347SpiInterfaceWorker.h"
#include <QDebug>
#include <QFileInfo>
#include <QtEndian>
#include <QSharedPointer>

CH347SpiInterfaceWorker::CH347SpiInterfaceWorker(QObject *parent)  : QObject(parent)
{
    m_lSpiI2cGpioDevIndex = -1;
}

CH347SpiInterfaceWorker::~CH347SpiInterfaceWorker()
{

}

void CH347SpiInterfaceWorker::UsbDevPnpNotify(ULONG iEventStatus)
{
    if(iEventStatus==CH347_DEVICE_ARRIVAL){
    }else if(iEventStatus==CH347_DEVICE_REMOVE){
    }
    return;
}

void CH347SpiInterfaceWorker::slotSpiCfgData(SPIDATACFG sCfgData)
{
    m_lSpiI2cGpioDevIndex = sCfgData.nSelectDevIndex;
    m_lChipSelect = sCfgData.cChipSelect;
    m_lPolaLowOrHigh = sCfgData.cCS1Polarity;

    bool status = true;
    status= openDevice();
    if(status == false){
        emit signalSpiStatus(DEV_OPEN, status);
        return;
    }

    status = ch347InitSpi(sCfgData);
    if(status == false){
        emit signalSpiStatus(DEV_INITSET, status);
        return;
    }

    status = ch347SpiCsCtrl();
    if(status == false){
        emit signalSpiStatus(DEV_CHIP_SET, status);
        return;
    }

    emit signalSpiStatus(DEV_NORMAL, status);
}

void CH347SpiInterfaceWorker::slotFilePath(const QString &strBinFilePath, const QString &resAddress)
{
    m_nFileDataTailSize = 0;
    m_nSendCount = 0;

    FileDealManger * pFileOperation = new  FileDealManger(strBinFilePath);
    m_nFileDataTailSize = pFileOperation->getFileSizeTotal();
    m_nSendCount = m_nFileDataTailSize / WRITE_DATA_SIZE;
    int nFinalDataSize = m_nFileDataTailSize % WRITE_DATA_SIZE;
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
        pFileOperation->getFilePiecewiseArry(nFinalDataSize, transmissionsCount,m_nSendCount,&pArrySectionalData256,nFullSize);

        int nSpiAdress = dec + transmissionsCount*WRITE_DATA_SIZE;
        pFileOperation->int32To8Byte(nSpiAdress, pStarttAddress);

        BOOL status = spiWorkerWriteByte(pStarttAddress , pArrySectionalData256 ,nFullSize);
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

void CH347SpiInterfaceWorker::slotStartWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry)
{
    (void)nPortType;
    (void)WriteCount;

    BOOL status = spiWorkerWriteByte(pAddressArryHead , dataArry ,4);
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

void CH347SpiInterfaceWorker::slotEndWriteCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail)
{
    (void)nPortType;
    spiWorkerWriteByte(pAddressArryTail , pDataArryTail, 4);
    if(WriteCount == WRITE_CMD_COUNT -1){
        closeDevice();
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

bool CH347SpiInterfaceWorker::openDevice()
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

bool CH347SpiInterfaceWorker::closeDevice()
{
    CH347CloseDevice(m_lSpiI2cGpioDevIndex);
    m_bDevIsOpened = false;
    return true;
}

BOOL CH347SpiInterfaceWorker::ch347InitSpi(SPIDATACFG sCfgData)
{
    BOOL RetVal = FALSE;
    mSpiCfgS SpiCfg = {0};

    SpiCfg.iMode = sCfgData.cSPiMode;
    SpiCfg.iClock = sCfgData.iClock;
    SpiCfg.iByteOrder = sCfgData.cBitOrder;
    SpiCfg.iSpiWriteReadInterval = 0;
    SpiCfg.iSpiOutDefaultData = sCfgData.iSpiOutDefaultData;
    SpiCfg.iChipSelect = sCfgData.cChipSelect;
    SpiCfg.CS1Polarity = (UCHAR)sCfgData.cCS1Polarity;
    SpiCfg.CS2Polarity = (UCHAR)sCfgData.cCS2Polarity;

    SpiCfg.iChipSelect = 0x80;
    SpiCfg.iIsAutoDeativeCS = 0;
    SpiCfg.iActiveDelay = 0;
    SpiCfg.iDelayDeactive = 0;
    RetVal = CH347SPI_Init(m_lSpiI2cGpioDevIndex, &SpiCfg);
    return RetVal;
}

BOOL CH347SpiInterfaceWorker::ch347SpiCsCtrl(USHORT ChipSec)
{
    USHORT          iEnableSelect = 1;
    USHORT          iChipSelect  ;
    USHORT          iIsAutoDeativeCS;
    ULONG           iActiveDelay ;
    ULONG           iDelayDeactive;
    UCHAR           CsSel;
    BOOL RetVal;

    CsSel = m_lChipSelect;
    iEnableSelect = 1;
    iChipSelect = ChipSec;
    iIsAutoDeativeCS = 1;
    iActiveDelay =FALSE & 0xFF;
    iDelayDeactive = FALSE & 0xFF;

    if(iEnableSelect){
        if(CsSel == 1){
            iEnableSelect = 0x0100;
            iChipSelect = (iChipSelect<<8) & 0xFF00;
            iIsAutoDeativeCS = (iIsAutoDeativeCS << 8) & 0xFF00;
            iActiveDelay = (iActiveDelay << 16) & 0xFFFF0000;
            iDelayDeactive = (iDelayDeactive << 16) & 0xFFFF0000;
        }else{
            iEnableSelect = 0x01;
        }
    }else{
        iEnableSelect = 0;
    }
    RetVal = CH347SPI_SetChipSelect(m_lSpiI2cGpioDevIndex,iEnableSelect,iChipSelect,iIsAutoDeativeCS,iActiveDelay,iDelayDeactive);
    return RetVal;
}

BOOL CH347SpiInterfaceWorker::spiWorkerWriteByte(UCHAR* pRegisterAddress, UCHAR *pFileData, ULONG nFileSize)
{
    UCHAR ChipSelect;
    BOOL RetVal = FALSE;
    ChipSelect = 0x00;
    ULONG iLength = 0;
    UCHAR buffer[nFileSize*2];
    UCHAR writeCmd = 0x00;
    buffer[iLength++] = writeCmd;
    memcpy(buffer + iLength, pRegisterAddress, 4);
    iLength += 4;
    UCHAR dummy = 0x00;
    buffer[iLength++] = dummy;

    memcpy(buffer + iLength, pFileData,nFileSize);
    iLength += nFileSize;
    RetVal = CH347StreamSPI4(m_lSpiI2cGpioDevIndex,ChipSelect,iLength,buffer);
    if(m_bIsWriteFile == true){
        emit signalSpiShowBar(m_nFileDataTailSize,nFileSize);
    }
    return RetVal;
}

