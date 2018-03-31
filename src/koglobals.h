/*
  This file is part of KOrganizer.

  Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

#ifndef KORG_KOGLOBALS_H
#define KORG_KOGLOBALS_H

#include "korganizerprivate_export.h"

#include <QPixmap>
#include <QString>
#include <QDate>
#include <QMap>
#include <QList>

namespace KHolidays {
class HolidayRegion;
}

class KORGANIZERPRIVATE_EXPORT KOGlobals
{
    friend class KOGlobalsSingletonPrivate;
public:
    static KOGlobals *self();

    static bool reverseLayout();

    ~KOGlobals();

    QPixmap smallIcon(const QString &name) const;

    QMap<QDate, QStringList> holiday(const QDate &start, const QDate &end) const;

    int firstDayOfWeek() const;

    /**
       Returns a list containing work days between @p start and @end.
    */
    QList<QDate> workDays(const QDate &start, const QDate &end) const;

    int getWorkWeekMask();

    /**
       Set which holidays the user wants to use.
       @param h a HolidayRegion object initialized with the desired locale.
       We capture this object, so you must not delete it.
    */
    void setHolidays(KHolidays::HolidayRegion *h);

    /** return the HolidayRegion object or 0 if none has been defined
    */
    KHolidays::HolidayRegion *holidays() const;

protected:
    KOGlobals();

private:
    KHolidays::HolidayRegion *mHolidays = nullptr;
};

#endif
