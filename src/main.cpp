/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     rekols <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QTranslator>
#include <QLocale>

#include "applicationmodel.h"
#include "mainwindow.h"

#include "volumemanager.h"
#include "battery.h"
#include "brightness.h"
#include "controlcenterdialog.h"
#include "systemtray/systemtraymodel.h"
#include "appearance.h"
#include "iconitem.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    qmlRegisterType<DockSettings>("Cutefish.Dock", 1, 0, "DockSettings");
    qmlRegisterType<VolumeManager>("Cutefish.Dock", 1, 0, "Volume");
    qmlRegisterType<Battery>("Cutefish.Dock", 1, 0, "Battery");
    qmlRegisterType<Brightness>("Cutefish.Dock", 1, 0, "Brightness");
    qmlRegisterType<ControlCenterDialog>("Cutefish.Dock", 1, 0, "ControlCenterDialog");
    qmlRegisterType<SystemTrayModel>("Cutefish.Dock", 1, 0, "SystemTrayModel");
    qmlRegisterType<Appearance>("Cutefish.Dock", 1, 0, "Appearance");
    qmlRegisterType<IconItem>("Cutefish.Dock", 1, 0, "IconItem");

    QString qmFilePath = QString("%1/%2.qm").arg("/usr/share/cutefish-dock/translations/").arg(QLocale::system().name());
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(QApplication::instance());
        if (translator->load(qmFilePath)) {
            QGuiApplication::installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    MainWindow w;

    return app.exec();
}
