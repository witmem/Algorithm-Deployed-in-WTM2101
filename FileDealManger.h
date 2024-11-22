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

#ifndef FILEDEALMANGER_H
#define FILEDEALMANGER_H
#include <QObject>
#include "DataDefine.h"

class FileDealManger
{

public:
    FileDealManger(const QString &strBinFilePath);
    ~FileDealManger();
    int  getFileSizeTotal();
    void getFileSourceData(const QString &strBinFilePath, UCHAR** pData, int& nSize);
    void getFilePiecewiseArry(int nFinalDataSize,int count, int tailCount,uchar**arry,int& nFullSize);
    void int32To8Byte(int nStartAddress, UCHAR *pStartAddressArry);
    void getI2cFilePiecewiseArry(int nFinalDataSize,int count, int tailCount,uchar**arry,int& nFullSize);
    QByteArray intToArry32(int nData);
    QByteArray intToArry16(int nData);

private:
    QString m_strFilePath = "";
    int m_nFileSizeTotal = 0;
    UCHAR* m_pSourceArry = nullptr;
};

#endif // FILEDEALMANGER_H
