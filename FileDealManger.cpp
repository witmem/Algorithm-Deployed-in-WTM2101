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

#include "FileDealManger.h"
#include <QFileInfo>
#include <QtEndian>
#include <QDebug>

FileDealManger::FileDealManger(const QString &strBinFilePath)
{
    m_strFilePath = strBinFilePath;
    getFileSourceData(m_strFilePath,&m_pSourceArry, m_nFileSizeTotal);
}

FileDealManger::~FileDealManger()
{
    if(m_pSourceArry){
        delete m_pSourceArry;
        m_pSourceArry = nullptr;
    }
}

int FileDealManger::getFileSizeTotal()
{
    return m_nFileSizeTotal;
}

void FileDealManger::getFileSourceData(const QString &strBinFilePath, UCHAR **pData, int &nSize)
{
    if(strBinFilePath.isEmpty())
        return;
    QFile objectFile(strBinFilePath);
    if(!objectFile.exists()){
        return;
    }
    if(!objectFile.open(QIODevice::ReadOnly)){
        return;
    }
    QFileInfo fileInfo;
    fileInfo.setFile(strBinFilePath);

    UCHAR* pFileData = new UCHAR[fileInfo.size()];
    *pData = pFileData;
    objectFile.read((char *)pFileData, fileInfo.size());

    int *pSourceArry = (int*)pFileData;
    QByteArray tailArry;
    for(int i = 0; i< fileInfo.size()/4 ;i++){
        int a = *(pSourceArry + i);
        int *pSourc  = &a;
        qToBigEndian(a, (uchar*)pSourc);
        QByteArray data  =  QByteArray::fromRawData(reinterpret_cast<const char *>(pSourc),4);
        tailArry.append(data);
    }
    memcpy(pFileData, tailArry,  tailArry.size());

    nSize =  fileInfo.size();
    objectFile.close();
}

void FileDealManger::getFilePiecewiseArry(int nFinalDataSize, int count, int tailCount, uchar **arry, int &nFullSize)
{
    if(nFinalDataSize > 0 && (count + 1)  == tailCount){
        memcpy(*arry,m_pSourceArry + count *WRITE_DATA_SIZE,nFinalDataSize);
        nFullSize = nFinalDataSize;
    }else{
        memcpy(*arry,m_pSourceArry + count *WRITE_DATA_SIZE,WRITE_DATA_SIZE);
        nFullSize = WRITE_DATA_SIZE;
    }
}

void FileDealManger::int32To8Byte(int nStartAddress, UCHAR *pStartAddressArry)
{
    pStartAddressArry[0] = (UCHAR)(nStartAddress >> 24);
    pStartAddressArry[1] = (UCHAR)(nStartAddress >> 16);
    pStartAddressArry[2] = (UCHAR)(nStartAddress >> 8);
    pStartAddressArry[3] = (UCHAR)(nStartAddress);
}

QByteArray FileDealManger::intToArry32(int nData)
{
    int *p  = &nData;
    qToBigEndian(nData, (uchar*)p);
    QByteArray Arry = QByteArray::fromRawData(reinterpret_cast<const char *>(p),4);
    QByteArray RawDataArry;
    RawDataArry.append(Arry);
    return RawDataArry;
}

QByteArray FileDealManger::intToArry16(int nData)
{
    int16_t dataSize = (nData -1) & 0xFFFF;
    int16_t *p = &dataSize;
    qToBigEndian(dataSize, (uchar*)p);
    QByteArray sendFileSize = QByteArray::fromRawData(reinterpret_cast<const char *>(p),2);
    QByteArray RawDataArry;
    RawDataArry.append(sendFileSize);
    return RawDataArry;
}

void FileDealManger::getI2cFilePiecewiseArry(int nFinalDataSize, int count, int tailCount, uchar **arry, int &nFullSize)
{
    if(nFinalDataSize > 0 && (count + 1)  == tailCount){
        memcpy(*arry,m_pSourceArry + count *WRITE_I2C_DATA_SIZE,nFinalDataSize);
        nFullSize = nFinalDataSize;
    }else{
        memcpy(*arry,m_pSourceArry + count *WRITE_I2C_DATA_SIZE,WRITE_I2C_DATA_SIZE);
        nFullSize = WRITE_I2C_DATA_SIZE;
    }
}
