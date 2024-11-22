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

#include "DownLoadMainWindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <QApplication>
#include <QFile>
#include<QTranslator>
#include<QDateTime>
#include<QMutex>
#include<QDir>
#include<QTextStream>
#include <QTextCodec>
#define LOG_FILE     qApp->applicationDirPath()+"/logs"

void Setstyle(const QString &styleName)
{
    QFile file(QString(":/Resource/style/%1").arg(styleName));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug:");
        break;
    case QtWarningMsg:
        text = QString("Warning:");
        break;
    case QtInfoMsg:
        text = QString("Info:");
        break;
    case QtCriticalMsg:
        text = QString("Critical:");
        break;
    case QtFatalMsg:
        text = QString("Fatal:");
    }
    QString context_info = QString("File:[%1]\tLine:[%2]").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString current_Info = QString("Info:[%1]").arg(msg);
    QString message = QString("%1\t%2 %3 \t%4").arg(current_date).arg(text).arg(context_info).arg(current_Info);

    QDir logDir(LOG_FILE);
    if (!logDir.exists()) {
        logDir.mkpath(LOG_FILE);
    }
    QString logPath = LOG_FILE +"/"+QString(QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt"));
    QFile file(logPath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);
    DownLoadMainWindow w;
    w.setWindowTitle(QString("2302固件下载工具_v%1").arg(APP_VERSION));
    Setstyle("CustomStyle.qss");
    w.show();
    return a.exec();
}
