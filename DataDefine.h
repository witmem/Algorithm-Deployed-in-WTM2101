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

#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "lib/ftd2xx.h"
#include "lib/CH347DLL.H"

#include <QMessageBox>
#include <QMetaType>
#define  SPI_MAX_CLOCKRATE 30000000
#define CHANNEL_TO_OPEN 0
#define WRITE_DATA_SIZE 256
#define BIN_EXE "objcopy.exe"
#define WRITE_CMD_COUNT 4
#define WRITE_I2C_DATA_SIZE 16

enum PROTOCL_TYPE{
    I2C_PROTOCL =0,
    SPI_PROTOCL,
    UART_PROTOCL
};


enum UART_WRITE__TYPE{
    UART_HEAD =0,
    UART_TAIL
};

enum WRITE_CMD_TYPE{
    START_CMD =0,
    END_CMD
};

enum DEV_STATUS_TYPE{
    DEV_OPEN =0,
    DEV_CLOSE,
    DEV_INITSET,
    DEV_CHIP_SET,
    DEV_NORMAL
};

struct I2CDATACFG
{
    UCHAR iClock;
    QString strI2CSlaveAddress;
    ULONG nSelectDevIndex;

    I2CDATACFG() {
        strI2CSlaveAddress = "";
        iClock = 0;
        nSelectDevIndex = 0;
    }
};

struct SETSERIALPORT
{
    QString strName;
    int nBaud;
    int nData;
    int nStop;
    int nParty;
    ULONG nSelectDevIndex;
    QString strUartRegAdress;

    SETSERIALPORT() {
        nBaud = 0;
        nData = 0;
        nStop = 0;
        nParty = 0;
        nBaud = 0;
        strUartRegAdress = "";
    }
};
struct SPIDATACFG
{
    ULONG nSelectDevIndex;
    UCHAR cSPiMode;
    UCHAR cBitOrder;
    UCHAR cChipSelect;
    UCHAR cCS1Polarity ;
    UCHAR cCS2Polarity ;
    UCHAR iSpiOutDefaultData;
    UCHAR iClock;
    DWORD nDataBit;
    DWORD nFrequency;
    DWORD nStateLevel;

    QString strSpiRegAdress;

    SPIDATACFG() {
        nSelectDevIndex = 0;
        cSPiMode = 0;
        cBitOrder = 0;
        nDataBit = 0;
        nFrequency = 0;
        nStateLevel = 0;
        strSpiRegAdress = "";
    }
};


typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

Q_DECLARE_METATYPE(SETSERIALPORT)
Q_DECLARE_METATYPE(SPIDATACFG)
Q_DECLARE_METATYPE(I2CDATACFG)
Q_DECLARE_METATYPE(_SPI_CONFIG)

#endif // DATADEFINE_H
