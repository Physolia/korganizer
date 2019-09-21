/*
  This file is part of KOrganizer.

  Copyright (c) 2002,2003 Cornelius Schumacher <schumacher@kde.org>

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

#include "koglobals.h"
#include "prefs/koprefs.h"

#include <KHolidays/HolidayRegion>

#include <QApplication>

class KOGlobalsSingletonPrivate
{
public:
    KOGlobals instance;
};

Q_GLOBAL_STATIC(KOGlobalsSingletonPrivate, sKOGlobalsSingletonPrivate)

KOGlobals *KOGlobals::self()
{
    return &sKOGlobalsSingletonPrivate->instance;
}

KOGlobals::KOGlobals()
{
}

KOGlobals::~KOGlobals()
{
    qDeleteAll(mHolidayRegions);
}

bool KOGlobals::reverseLayout()
{
    return QApplication::isRightToLeft();
}

QMap<QDate, QStringList> KOGlobals::holiday(const QDate &start, const QDate &end) const
{
    QMap<QDate, QStringList> holidaysByDate;

    if (mHolidayRegions.isEmpty()) {
        return holidaysByDate;
    }

    for (const KHolidays::HolidayRegion *region : qAsConst(mHolidayRegions)) {
        if (region && region->isValid()) {
            const KHolidays::Holiday::List list = region->holidays(start, end);
            const int listCount(list.count());
            for (int i = 0; i < listCount; ++i) {
                const KHolidays::Holiday &h = list.at(i);
                // dedupe, since we support multiple holiday regions which may have similar holidays
                if (!holidaysByDate[h.observedStartDate()].contains(h.name())) {
                    holidaysByDate[h.observedStartDate()].append(h.name());
                }
            }
        }
    }

    return holidaysByDate;
}

int KOGlobals::firstDayOfWeek() const
{
    return KOPrefs::instance()->mWeekStartDay + 1;
}

QList<QDate> KOGlobals::workDays(const QDate &startDate, const QDate &endDate) const
{
    QList<QDate> result;

    const int mask(~(KOPrefs::instance()->mWorkWeekMask));
    const qint64 numDays = startDate.daysTo(endDate) + 1;

    for (int i = 0; i < numDays; ++i) {
        const QDate date = startDate.addDays(i);
        if (!(mask & (1 << (date.dayOfWeek() - 1)))) {
            result.append(date);
        }
    }

    if (KOPrefs::instance()->mExcludeHolidays) {
        for (const KHolidays::HolidayRegion *region : qAsConst(mHolidayRegions)) {
            const KHolidays::Holiday::List list = region->holidays(startDate, endDate);
            for (int i = 0; i < list.count(); ++i) {
                const KHolidays::Holiday &h = list.at(i);
                if (h.dayType() == KHolidays::Holiday::NonWorkday) {
                    result.removeAll(h.observedStartDate());
                }
            }
        }
    }

    return result;
}

int KOGlobals::getWorkWeekMask()
{
    return KOPrefs::instance()->mWorkWeekMask;
}

void KOGlobals::setHolidays(const QStringList &regions)
{
    qDeleteAll(mHolidayRegions);
    mHolidayRegions.clear();
    for (const QString &regionStr : regions) {
        KHolidays::HolidayRegion *region = new KHolidays::HolidayRegion(regionStr);
        if (region->isValid()) {
            mHolidayRegions.append(region);
        } else {
            delete region;
        }
    }
}

QList<KHolidays::HolidayRegion *> KOGlobals::holidays() const
{
    return mHolidayRegions;
}
