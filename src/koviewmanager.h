/*
  This file is part of KOrganizer.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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

#ifndef KORG_KOVIEWMANAGER_H
#define KORG_KOVIEWMANAGER_H

#include <EventViews/EventView>

#include <KCalendarCore/IncidenceBase> //for KCalendarCore::DateList typedef

#include <QDate>
#include <QObject>

class CalendarView;
class KOAgendaView;
class KOJournalView;
class KOListView;
class KOTimelineView;
class KOTodoView;
class KOWhatsNextView;
namespace KOrg {
class BaseView;
class MultiAgendaView;
class MonthView;
}

namespace Akonadi {
class Item;
}

class KConfig;
class QTabWidget;

/**
  This class manages the views of the calendar. It owns the objects and handles
  creation and selection.
*/
class KOViewManager : public QObject
{
    Q_OBJECT
public:

    enum RangeMode {
        NO_RANGE,
        DAY_RANGE,
        WORK_WEEK_RANGE,
        WEEK_RANGE,
        NEXTX_RANGE,
        OTHER_RANGE // for example, showing 8 days
    };

    explicit KOViewManager(CalendarView *);
    ~KOViewManager() override;

    /** changes the view to be the currently selected view */
    void showView(KOrg::BaseView *);

    void readSettings(KConfig *config);
    void writeSettings(KConfig *config);

    /** Read which view was shown last from config file */
    void readCurrentView(KConfig *config);
    /** Write which view is currently shown to config file */
    void writeCurrentView(KConfig *config);

    KOrg::BaseView *currentView();

    void setDocumentId(const QString &);

    void updateView();
    void updateView(QDate start, QDate end, QDate preferredMonth);

    void goMenu(bool enable);
    void raiseCurrentView();

    void connectView(KOrg::BaseView *);
    void addView(KOrg::BaseView *, bool isTab = false);

    Q_REQUIRED_RESULT Akonadi::Item currentSelection();

    /**
     * If there's a selected incidence, it's date is returned, otherwise
     * an invalid QDate is returned.
     */
    Q_REQUIRED_RESULT QDate currentSelectionDate();

    KOAgendaView *agendaView() const
    {
        return mAgendaView;
    }

    KOrg::MultiAgendaView *multiAgendaView() const
    {
        return mAgendaSideBySideView;
    }

    KOTodoView *todoView() const
    {
        return mTodoView;
    }

    KOrg::MonthView *monthView() const
    {
        return mMonthView;
    }

    void updateMultiCalendarDisplay();

    /**
     * Returns true if agenda is the current view.
     *
     * Never use the pointer returned by agendaView()
     * to know if agenda is selected, because agenda has other modes
     * (tabbed, side by side). Use this function instead.
     */
    Q_REQUIRED_RESULT bool agendaIsSelected() const;

    /**
      Return the current range mode:
      week, work week, day or nextX days, etc.
    */
    Q_REQUIRED_RESULT RangeMode rangeMode() const
    {
        return mRangeMode;
    }

Q_SIGNALS:
    void configChanged();
    void datesSelected(const KCalendarCore::DateList &);

public Q_SLOTS:
    void showWhatsNextView();
    void showListView();
    void showAgendaView();
    void showTodoView();
    void showTimeLineView();
    void showMonthView();
    void showJournalView();
    void showEventView();

    void selectDay();
    void selectWorkWeek();
    void selectWeek();
    void selectNextX();

    void connectTodoView(KOTodoView *todoView);

    void zoomInHorizontally();
    void zoomOutHorizontally();
    void zoomInVertically();
    void zoomOutVertically();

    /**
       Notifies all views that an update is needed. This means that the
       next time CalendarView::updateView() is called, views won't try to be smart
       and ignore the update for performance reasons.
    */
    void addChange(EventViews::EventView::Change change);

private Q_SLOTS:
    void currentAgendaViewTabChanged(int index);

private:
    QWidget *widgetForView(KOrg::BaseView *) const;
    QList<KOrg::BaseView *> mViews;
    CalendarView *mMainView = nullptr;

    KOAgendaView *mAgendaView = nullptr;
    KOrg::MultiAgendaView *mAgendaSideBySideView = nullptr;
    KOListView *mListView = nullptr;
    KOTodoView *mTodoView = nullptr;
    KOWhatsNextView *mWhatsNextView = nullptr;
    KOJournalView *mJournalView = nullptr;
    KOTimelineView *mTimelineView = nullptr;
    KOrg::MonthView *mMonthView = nullptr;
    KOrg::BaseView *mCurrentView = nullptr;

    KOrg::BaseView *mLastEventView = nullptr;
    QTabWidget *mAgendaViewTabs = nullptr;
    int mAgendaViewTabIndex;

    RangeMode mRangeMode;
};

#endif
