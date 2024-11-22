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

#include "PassObjectFileDialog.h"
#include "ui_PassObjectFileDialog.h"

#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QScopedArrayPointer>

PassObjectFileDialog::PassObjectFileDialog(int nProType, QWidget *parent):
    QDialog(parent),
    ui(new Ui::PassObjectFileDialog)
{
    ui->setupUi(this);
    m_nProType = nProType;

    initUi();
}

void PassObjectFileDialog::initUi()
{
    ui->writeFileProgressBar->setMinimum(0);
    ui->writeFileProgressBar->setVisible(false);
    ui->writeFileProgressBar->setValue(0);

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setAttribute(Qt::WA_DeleteOnClose);

    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
    setWindowTitle("文件下载");

    ui->choseFilePattEdit->setObjectName("customEdit");
    ui->DownLoadBtn->setObjectName("importBtn");
    ui->choseFilePathBtn->setObjectName("grayBtn");

    ui->startInitMPUBtn->setObjectName("importBtn");
    ui->EndMPUBtn->setObjectName("importBtn");

    ui->PathTipLabel->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->DownFilePAge);

    QFont font;
    font.setBold(true);
    ui->PathTipLabel->setFont(font);
    ui->stackedWidget->setObjectName("tabwidget");

    ui->choseFilePathBtn->setStyleSheet(  "QPushButton{"
                                          "background-color:#E6E6E6;"
                                          "border-radius: 6px;"
                                          "width:90px;"
                                          "height:30px;}"
                                          "QPushButton:hover{"
                                          "background-color:#DADADA;"
                                          "border-radius: 6px;}"
                                          "QPushButton:pressed{"
                                          "background-color:#B8B8B8;"
                                          "border-radius: 6px;}");

    ui->DownLoadBtn->setStyleSheet(  "QPushButton{"
                                     "background-color:#3790FA;"
                                     "border-radius: 6px;"
                                     "color:#FFFFFF;"
                                     "width:90px;"
                                     "height:30px;}"
                                     "QPushButton:hover{"
                                     "background-color:#3D6BE5;"
                                     "border-radius: 6px;"
                                     "color:#FFFFFF;}"
                                     "QPushButton:pressed{"
                                     "background-color:#0078d7;"
                                     "border-radius: 6px;"
                                     "color:#FFFFFF;}");
    if(m_nProType == SPI_PROTOCL){
        on_resAddressEdit_textEdited("31020000");
    }
    if(m_nProType == UART_PROTOCL){
        on_resAddressEdit_textEdited("31020000");
    }
    if(m_nProType == I2C_PROTOCL){
        on_resAddressEdit_textEdited("31020000");
    }

    ui->startCmdIsWriteSuccessLabel->setVisible(false);
    QRegExp regExp("^[0-9a-fA-F ]+$");
    ui->resAddressEdit->setValidator(new QRegExpValidator(regExp, ui->resAddressEdit));
    ui->resAddressEdit->setMaxLength(11);
}

QString PassObjectFileDialog::getElfToBinFile(QString elfPath)
{
    QString pathObjCopy = qApp->applicationDirPath() + "/" + BIN_EXE;
    QFileInfo elf(elfPath);
    QString elfFilePath = elf.path();
    QString elfFileName = elf.baseName();
    QString strBinFilePath = elfFilePath + "/" +QString("%1.bin").arg(elfFileName);
    QProcess* pBinProcess = new QProcess(this);
    pBinProcess->setProgram(pathObjCopy);

    QStringList cmdLdList;
    cmdLdList <<"-O" <<"binary"<< elfPath << strBinFilePath;
    pBinProcess->start(pBinProcess->program(),cmdLdList);
    if(pBinProcess->waitForFinished() == true){
        return strBinFilePath;
    }
    return nullptr;
}

PassObjectFileDialog::~PassObjectFileDialog()
{
    delete ui;
}

void PassObjectFileDialog::on_choseFilePathBtn_clicked()
{
    QString curPath = QDir::currentPath();
    QString aFileName = QFileDialog::getOpenFileName(this,QString("选择文件"),curPath,"bin(*.bin)");
    ui->choseFilePattEdit->setText(aFileName);
}

void PassObjectFileDialog::on_DownLoadBtn_clicked()
{
    QString path = ui->choseFilePattEdit->text();
    if(path.isEmpty()){
        ui->PathTipLabel->setStyleSheet("color:red;");
        ui->PathTipLabel->setVisible(true);
        ui->PathTipLabel->setText("请选择一个文件路径");
        return;
    }

    QFileInfo fileInfo(path);
    if(fileInfo.suffix() == "elf"){
        QString strBinFilePath = getElfToBinFile(path);
        fileInfo.setFile(strBinFilePath);
        path = strBinFilePath;
    }

    ui->writeFileProgressBar->setValue(0);
    double dProgress = (ui->writeFileProgressBar->value() - ui->writeFileProgressBar->minimum()) * 100.0
            / (ui->writeFileProgressBar->maximum() - ui->writeFileProgressBar->minimum());
    ui->writeFileProgressBar->setFormat(QString("当前进度为: %1%").arg(QString::number(dProgress, 'f', 2)));

    QString strResAddress = ui->resAddressEdit->text().trimmed().replace(" ","");
    emit signalFilePath(path,strResAddress);
}

void PassObjectFileDialog::on_choseFilePattEdit_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty()){
        ui->PathTipLabel->setVisible(false);
    }
}


void PassObjectFileDialog::on_startInitMPUBtn_clicked()
{
    const char* pAddressHead = "\x35\x00\x00\x04";
    const char* pHeadData = "\x08\xa5\x9b\x5f";

    for(int  CmdCount = 0; CmdCount < WRITE_CMD_COUNT / 2; CmdCount++){
        switch (CmdCount) {
        case 0:
            pAddressHead = "\x35\x00\x00\x04";
            pHeadData = "\x08\xa5\x9b\x5f";
            break;
        case 1:
            pAddressHead = "\x35\x00\x00\x00";
            pHeadData = "\x00\x00\x00\x01";
            break;
        default:
            break;
        }

        UCHAR *pAddressArry = new  UCHAR[4];
        UCHAR *pDataArry = new  UCHAR[4];

        memcpy(pAddressArry,pAddressHead,4);
        memcpy(pDataArry,pHeadData,4);
        emit signalStartCmd(m_nProType,CmdCount,pAddressArry, pDataArry);
    }
}


void PassObjectFileDialog::on_EndMPUBtn_clicked()
{
    const char* pTailAddress = "\x33\x00\x00\x40";
    const char* pTailData = "\x00\x02\x00\x00";

    for(int  CmdCount = 0; CmdCount < WRITE_CMD_COUNT; CmdCount ++){

        switch (CmdCount) {
        case 0:
            pTailAddress = "\x33\x00\x00\x40";
            pTailData = "\x00\x02\x00\x00";
            break;
        case 1:
            pTailAddress = "\x33\x00\x00\x48";
            pTailData = "\x00\x00\x00\x02";
            break;
        case 2:
            pTailAddress = "\x33\x00\x00\x04";
            pTailData = "\xFF\xFF\xFF\xFE";
            break;
        case 3:
            pTailAddress = "\x35\x00\x00\x28";
            pTailData = "\x00\x00\x00\x01";
            break;
        default:
            break;
        }

        UCHAR *pAddressArry = new  UCHAR[4];
        UCHAR *pDataArry = new  UCHAR[4];

        memcpy(pAddressArry,pTailAddress,4);
        memcpy(pDataArry,pTailData,4);
        emit signalEndCmd(m_nProType,CmdCount,pAddressArry, pDataArry);
    }

    this->close();
}

void PassObjectFileDialog::slotInitMpuIsSucess(bool isSucess)
{
    ui->startCmdIsWriteSuccessLabel->setVisible(true);
    if(isSucess == true){
        ui->startCmdIsWriteSuccessLabel->setText("初始化命令发送成功");
        ui->startCmdIsWriteSuccessLabel->setStyleSheet("color:#55ff7f");
    }else{
        ui->startCmdIsWriteSuccessLabel->setText("初始化命令发送失败");
        ui->startCmdIsWriteSuccessLabel->setStyleSheet("color:red");
    }
}

void PassObjectFileDialog::slotProgressBarStart(int maxValue, int value)
{
    ui->writeFileProgressBar->setVisible(true);
    ui->startCmdIsWriteSuccessLabel->setVisible(false);

    ui->writeFileProgressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->writeFileProgressBar->setMaximum(maxValue);
    ui->writeFileProgressBar->setValue(ui->writeFileProgressBar->value() + value);

    double dProgress = (ui->writeFileProgressBar->value() - ui->writeFileProgressBar->minimum()) * 100.0
            / (ui->writeFileProgressBar->maximum() - ui->writeFileProgressBar->minimum());
    ui->writeFileProgressBar->setFormat(QString("当前进度为: %1%").arg(QString::number(dProgress, 'f', 2)));
}


void PassObjectFileDialog::on_resAddressEdit_textEdited(const QString &arg1)
{
    int i,j;
    QString uStr,uStr2,uStr3;
    uStr = arg1;
    uStr = uStr.replace(" ","");
    j = uStr.count();
    if((j >= 3)&&(0 == (j%2))){
        uStr3 = "";
        for(i = 0;i< ((j/2)-1); i++){
            uStr2 = uStr.mid(i*2,2);
            uStr3 += uStr2 + " ";
        }
        uStr2 = uStr.mid(j - 2,2);
        uStr3 += uStr2;
        ui->resAddressEdit->setText(uStr3);
    }
}

