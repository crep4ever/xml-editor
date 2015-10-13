// Copyright (C) 2014, Romain Goffe <romain.goffe@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//******************************************************************************

/*!
 * \file main.cc
 * \mainpage conf-editor Documentation
 *
 * The conf-editor application provides a simple way
 * to access and modify values from a configuration file.
 * It is specifically designed for ViT xml format so it's not really
 * generic and portable to other projects.
 *
 * The application features one main screen:
 * \li edit page: complementary views of file contents
 *
 * \image html edit.png Edit screen
 */

#include <QApplication>

#include <QTranslator>
#include <QDate>
#include <QLocale>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "config.hh"
#include "main-window.hh"

namespace // anonymous namespace
{
bool isFilenameSupported(const QString & filename)
{
    return (filename.endsWith(".xml"));
}
}

/// Main routine of the application
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QApplication::setOrganizationName("ViTechnology");
    QApplication::setOrganizationDomain("vitechnology.com");
    QApplication::setApplicationName(CONF_EDITOR_APPLICATION_NAME);
    QApplication::setApplicationVersion(CONF_EDITOR_VERSION);

    // Load the application ressources (icons, ...)
    Q_INIT_RESOURCE(xml);

    // Check for a standard theme icon. If it does not exist, for
    // instance on MacOSX or Windows, fallback to one of the theme
    // provided in the ressource file.
    if (!QIcon::hasThemeIcon("document-open"))
    {
        QIcon::setThemeName("tango");
    }

    // Parse command line arguments
    QStringList arguments = QApplication::arguments();
    bool helpFlag = false;;
    bool versionFlag = false;
    if (arguments.contains("-h") || arguments.contains("--help"))
        helpFlag = true;
    else if (arguments.contains("-v") || arguments.contains("--version"))
        versionFlag = true;

    // Localization
    QDir translationDirectory;
    QString translationFilename = QString("conf-editor_%1.qm").arg(QLocale::system().name().split('_').first());
    QString directory;

    translationDirectory = QDir(CONF_EDITOR_DATA_PATH);
    if (translationDirectory.exists())
        directory = translationDirectory.absoluteFilePath("lang");
    else
        directory = QDir::current().absoluteFilePath("lang");

    QTranslator translator;
    translator.load(translationFilename, directory);
    application.installTranslator(&translator);

    if (helpFlag)
    {
        QTextStream out(stdout);
        out << "Usage: " << QApplication::applicationName() << " [OPTIONS] [FILE]" << endl;
        out << endl;

        out << "Options:" << endl;
        out << "    " << "-h, --help" << "\t\t" << QObject::tr("Print this help and exit") << endl;
        out << "    " << "-v, --version" << "\t" << QObject::tr("Print the version of this application") << endl;
        out << endl;

        out << "File:" << endl;
        out << "    " << QObject::tr("XML configuration file") << endl;

        out << " " << QApplication::applicationVersion() << endl;
        return 0;
    }
    else if (versionFlag)
    {
        QTextStream out(stdout);
        out << QApplication::applicationName();
        out << " " << QApplication::applicationVersion() << endl;
        return 0;
    }

    CMainWindow mainWindow;
    mainWindow.show();

    foreach (const QString & arg, arguments)
    {
        if (QFile(arg).exists() && isFilenameSupported(arg))
        {
            mainWindow.open(arg);
        }
    }

    return application.exec();
}
