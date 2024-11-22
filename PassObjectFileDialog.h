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

#ifndef PASSOBJECTFILEDIALOG_H
#define PASSOBJECTFILEDIALOG_H
#include <QApplication>
#include <QProcess>
#include "DataDefine.h"
#include <QRegExpValidator>

#define URAT_SEND_MAX_SIZE 4096

namespace Ui {
class PassObjectFileDialog;
}

class PassObjectFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PassObjectFileDialog(int nProType, QWidget *parent = nullptr);

    ~PassObjectFileDialog();

private slots:
    void on_choseFilePathBtn_clicked();

    void on_DownLoadBtn_clicked();

    void on_choseFilePattEdit_textChanged(const QString &arg1);

    void on_startInitMPUBtn_clicked();

    void on_EndMPUBtn_clicked();

    void on_resAddressEdit_textEdited(const QString &arg1);

public slots:
    void slotProgressBarStart(int maxValue, int value);
    void slotInitMpuIsSucess(bool isSucess);

private:
    void initUi();
    QString getElfToBinFile(QString elfPath);

private:
    Ui::PassObjectFileDialog *ui;

signals:
    void signalFilePath(const QString& path, const QString& resAddress);
    void signalStartCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryHead,  UCHAR *dataArry);
    void signalEndCmd(int nPortType, int WriteCount,  UCHAR *pAddressArryTail,  UCHAR *pDataArryTail);
    void signalSendData(QByteArray data);

private:
    int  m_nProType;
};

#endif // PASSOBJECTFILEDIALOG_H
