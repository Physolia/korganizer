/*
  This file is part of KOrganizer.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>
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

#ifndef KORG_SEARCHDIALOG_H
#define KORG_SEARCHDIALOG_H

#include <AkonadiCore/Item>

#include <QDialog>

class QPushButton;
class CalendarView;
class KOEventPopupMenu;

namespace Ui {
class SearchDialog;
}

namespace EventViews {
class ListView;
}

namespace KCalendarCore {
class Incidence;
}

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(CalendarView *calendarview);
    ~SearchDialog() override;

    void updateView();

public Q_SLOTS:
    void popupMenu(const QPoint &);
    void changeIncidenceDisplay(KCalendarCore::Incidence *, int)
    {
        updateView();
    }

Q_SIGNALS:
    void showIncidenceSignal(const Akonadi::Item &);
    void editIncidenceSignal(const Akonadi::Item &);
    void deleteIncidenceSignal(const Akonadi::Item &);

protected:
    /*reimp*/
    void showEvent(QShowEvent *event) override;
private:
    void doSearch();
    void searchTextChanged(const QString &_text);
    void slotHelpRequested();
    void search(const QRegExp &);
    void readConfig();
    void writeConfig();

    Ui::SearchDialog *m_ui = nullptr;
    CalendarView *m_calendarview = nullptr; // parent
    KOEventPopupMenu *m_popupMenu = nullptr;
    Akonadi::Item::List mMatchedEvents;
    EventViews::ListView *listView = nullptr;
    QPushButton *mUser1Button = nullptr;
};

#endif
