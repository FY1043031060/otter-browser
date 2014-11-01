/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2014 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "SettingsManager.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

namespace Otter
{

SettingsManager* SettingsManager::m_instance = NULL;
QString SettingsManager::m_globalPath;
QString SettingsManager::m_overridePath;
QHash<QString, QVariant> SettingsManager::m_defaults;

SettingsManager::SettingsManager(const QString &path, QObject *parent) : QObject(parent)
{
	m_globalPath = path + QLatin1String("/otter.conf");
	m_overridePath = path + QLatin1String("/override.ini");
}

void SettingsManager::createInstance(const QString &path, QObject *parent)
{
	m_instance = new SettingsManager(path, parent);
}

void SettingsManager::registerOption(const QString &key)
{
	QSettings(m_globalPath, QSettings::IniFormat).remove(key);

	emit m_instance->valueChanged(key, getValue(key));
}

void SettingsManager::removeOverride(const QUrl &url)
{
	QSettings(m_overridePath, QSettings::IniFormat).remove(url.isLocalFile() ? QLatin1String("localhost") : url.host());
}

void SettingsManager::setDefaultValue(const QString &key, const QVariant &value)
{
	m_defaults[key] = value;

	emit m_instance->valueChanged(key, getValue(key));
}

void SettingsManager::setValue(const QString &key, const QVariant &value, const QUrl &url)
{
	if (!url.isEmpty())
	{
		if (value.isNull())
		{
			QSettings(m_overridePath, QSettings::IniFormat).remove((url.isLocalFile() ? QLatin1String("localhost") : url.host()) + QLatin1Char('/') + key);
		}
		else
		{
			QSettings(m_overridePath, QSettings::IniFormat).setValue((url.isLocalFile() ? QLatin1String("localhost") : url.host()) + QLatin1Char('/') + key, value);
		}

		return;
	}

	if (getValue(key) != value)
	{
		QSettings(m_globalPath, QSettings::IniFormat).setValue(key, value);

		emit m_instance->valueChanged(key, value);
	}
}

SettingsManager* SettingsManager::getInstance()
{
	return m_instance;
}

QVariant SettingsManager::getDefaultValue(const QString &key)
{
	return m_defaults[key];
}

QVariant SettingsManager::getValue(const QString &key, const QUrl &url)
{
	if (!url.isEmpty())
	{
		return QSettings(m_overridePath, QSettings::IniFormat).value((url.isLocalFile() ? QLatin1String("localhost") : url.host()) + QLatin1Char('/') + key, getValue(key));
	}

	return QSettings(m_globalPath, QSettings::IniFormat).value(key, getDefaultValue(key));
}

}
