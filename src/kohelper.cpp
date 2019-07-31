/*
  This file is part of KOrganizer.

  Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofer.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#include "kohelper.h"
#include "prefs/koprefs.h"

#include <EventViews/Helper>

#include <CalendarSupport/KCalPrefs>
#include <KLocalizedString>
#include <KMessageBox>

QColor KOHelper::getTextColor(const QColor &c)
{
    double luminance = (c.red() * 0.299) + (c.green() * 0.587) + (c.blue() * 0.114);
    return (luminance > 128.0) ? QColor(0, 0, 0) : QColor(255, 255, 255);
}

QColor KOHelper::resourceColor(const Akonadi::Collection &coll)
{
    return EventViews::resourceColor(coll, KOPrefs::instance()->eventViewsPreferences());
}

QColor KOHelper::resourceColorKnown(const Akonadi::Collection &coll)
{
    return EventViews::resourceColor(coll, KOPrefs::instance()->eventViewsPreferences());
}

void KOHelper::setResourceColor(const Akonadi::Collection &collection, const QColor &color)
{
    EventViews::setResourceColor(collection, color, KOPrefs::instance()->eventViewsPreferences());
    KOPrefs::instance()->eventViewsPreferences()->writeConfig();
}

QColor KOHelper::resourceColor(const Akonadi::Item &item)
{
    return EventViews::resourceColor(item, KOPrefs::instance()->eventViewsPreferences());
}

int KOHelper::yearDiff(const QDate &start, const QDate &end)
{
    return end.year() - start.year();
}

bool KOHelper::isStandardCalendar(Akonadi::Collection::Id id)
{
    return id == CalendarSupport::KCalPrefs::instance()->defaultCalendarId();
}

void KOHelper::showSaveIncidenceErrorMsg(QWidget *parent, const KCalendarCore::Incidence::Ptr &incidence)
{
    KMessageBox::sorry(
        parent,
        i18n("Unable to save %1 \"%2\".",
             i18n(incidence->typeStr().constData()), incidence->summary()));
}
