/*
    This file is part of KOrganizer.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/
#ifndef KORG_RESOURCEVIEW_H
#define KORG_RESOURCEVIEW_H

#include "calendarview.h"

#include <libkcal/resourcecalendar.h>
#include <qlistview.h>

namespace KCal {
class CalendarResources;
}
using namespace KCal;
class KListView;
class ResourceView;
class QPushButton;

class ResourceViewFactory : public CalendarViewExtension::Factory
{
  public:
    ResourceViewFactory( KCal::CalendarResources *calendar,
                         CalendarView *view );

    CalendarViewExtension *create( QWidget * );

  private:
    KCal::CalendarResources *mCalendar;
    CalendarView *mView;
};


class ResourceItem : public QCheckListItem
{
  public:
    ResourceItem( KCal::ResourceCalendar *resource, ResourceView *view,
                  KListView *parent );
    ResourceItem( KCal::ResourceCalendar *resource, const QString& sub,
                  ResourceView *view, ResourceItem* parent );

    KCal::ResourceCalendar *resource() { return mResource; }

    void update();

  protected:
    void stateChange( bool active );

    void setGuiState();

  private:
    KCal::ResourceCalendar *mResource;
    ResourceView *mView;
    bool mBlockStateChange;
    bool mIsSubresource;
};

/**
  This class provides a view of calendar resources.
*/
class ResourceView : public CalendarViewExtension
{
    Q_OBJECT
  public:
    ResourceView( KCal::CalendarResourceManager *manager, QWidget *parent = 0,
                  const char *name = 0 );
    ~ResourceView();

    void updateView();

    void emitResourcesChanged();
    void emitErrorMessage( const QString & );

  public slots:
    void addResourceItem( ResourceCalendar * );
    void updateResourceItem( ResourceCalendar * );

  signals:
    void resourcesChanged();
    void signalErrorMessage( const QString & );

  protected:
    ResourceItem *findItem( ResourceCalendar * );

  private slots:
    void addResource();
    void removeResource();
    void editResource();
    void currentChanged( QListViewItem* );
    void slotSubresourceAdded( ResourceCalendar *, const QString &,
                               const QString &resource );
    void slotSubresourceRemoved( ResourceCalendar *, const QString &,
                                 const QString &resource );

  private:
    KListView *mListView;
    KCal::CalendarResourceManager *mManager;
    QPushButton *mAddButton;
    QPushButton *mDeleteButton;
    QPushButton *mEditButton;
};

#endif
