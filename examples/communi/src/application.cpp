/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "application.h"
#include "commandxmlreader.h"
#include "settingswizard.h"
#include "sharedtimer.h"
#include "session.h"
#include <QMutableMapIterator>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <QDir>
#include <QSet>

Settings Application::ApplicationData::settings;
QList<Session*> Application::ApplicationData::sessions;
Commands Application::ApplicationData::commands;
QStringList Application::ApplicationData::categories;

Application::Application(int& argc, char* argv[]) : QApplication(argc, argv)
{
    setApplicationName("Communi");
    setApplicationVersion(VERSION);
    setOrganizationName("communi.bitbucket.org");
    setOrganizationDomain("http://communi.bitbucket.org");

    QIcon icon;
    icon.addFile(":/resources/icons/16x16/communi.png");
    icon.addFile(":/resources/icons/26x26/communi.png");
    icon.addFile(":/resources/icons/32x32/communi.png");
    icon.addFile(":/resources/icons/48x48/communi.png");
    icon.addFile(":/resources/icons/64x64/communi.png");
    icon.addFile(":/resources/icons/128x128/communi.png");
    setWindowIcon(icon);

    qRegisterMetaTypeStreamOperators<Settings>("Settings");
    qRegisterMetaTypeStreamOperators<Connection>("Connection");
    qRegisterMetaTypeStreamOperators<Connections>("Connections");

    QSettings settings;
    ApplicationData::settings = settings.value("settings").value<Settings>();

    //QDir dataDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    //if (!dataDir.exists())
    //    dataDir.mkpath(dataDir.path());
    //if (!dataDir.exists("commands.xml"))
    //    QFile::copy(":/resources/commands.xml", dataDir.filePath("commands.xml"));

    //QFile commandFile(dataDir.filePath("commands.xml"));
    QFile commandFile(":/resources/commands.xml");
    commandFile.open(QIODevice::ReadOnly);

    CommandXmlReader commandReader;
    setCommands(commandReader.read(&commandFile));

    SharedTimer::instance()->setInterval(500);
}

Application::~Application()
{
    QSettings settings;
    settings.setValue("settings", ApplicationData::settings);

    //CommandXmlWriter commandWriter;
    //...
}

QString Application::applicationSlogan()
{
    return tr("%1 is a simple and light-weight cross-platform IRC client.").arg(applicationName());
}

Settings Application::settings()
{
    return ApplicationData::settings;
}

void Application::setSettings(const Settings& settings)
{
    if (ApplicationData::settings != settings)
    {
        ApplicationData::settings = settings;
        QMetaObject::invokeMethod(qApp, "settingsChanged", Q_ARG(Settings, settings));
    }
}

QList<Session*> Application::sessions()
{
    return ApplicationData::sessions;
}

void Application::setSessions(const QList<Session*>& sessions)
{
    ApplicationData::sessions = sessions;
}

QStringList Application::commandCategories()
{
    return ApplicationData::categories;
}

Commands Application::commands(const QString& category)
{
    Commands result = ApplicationData::commands;
    if (!category.isNull())
    {
        QMutableMapIterator<QString, Command> it(result);
        while (it.hasNext())
        {
            if (!it.next().value().categories.contains(category))
                it.remove();
        }
    }
    return result;
}

void Application::setCommands(const Commands& commands)
{
    if (ApplicationData::commands != commands)
    {
        ApplicationData::commands = commands;

        QSet<QString> cats;
        foreach (const Command& command, commands)
        {
            foreach (const QString& category, command.categories)
                cats.insert(category);
        }
        ApplicationData::categories = QStringList(cats.toList());
        qSort(ApplicationData::categories);

        QMetaObject::invokeMethod(qApp, "commandsChanged", Q_ARG(Commands, commands));
    }
}

void Application::aboutApplication()
{
    QMessageBox::about(
#ifdef Q_WS_MAC
        0,
#else
        activeWindow(),
#endif // Q_WS_MAC
        applicationName(), QString(
        "<h3>%1</h3>"
        "<p>%2</p>"
        "<p><a href='%3'>%3</a></p>"
        "<p><small>Copyright (C) 2008-2010 J-P Nurmi &lt;"
        "<a href='mailto:jpnurmi@gmail.com'>jpnurmi@gmail.com</a>&gt;</small></p>")
            .arg(tr("About %1").arg(applicationName()))
            .arg(applicationSlogan())
            .arg(organizationDomain())
        );
}

void Application::aboutOxygen()
{
    QMessageBox::about(
#ifdef Q_WS_MAC
        0,
#else
        activeWindow(),
#endif // Q_WS_MAC
        "Oxygen", QString(
        "<h3>%1</h3>"
        "<p><a href='http://www.oxygen-icons.org/'>http://www.oxygen-icons.org/</a></p>"
        ).arg(tr("About %1").arg("Oxygen")));
}

void Application::showSettings()
{
    SettingsWizard wizard(activeWindow());
    wizard.setSettings(settings());
    if (wizard.exec())
        setSettings(wizard.settings());
}