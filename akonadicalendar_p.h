/*
    This file is part of Akonadi.

    Copyright (c) 2009 Sebastian Sauer <sebsauer@kdab.net>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
    USA.
*/

#ifndef AKONADICALENDAR_P_H
#define AKONADICALENDAR_P_H

#include "akonadicalendar.h"

#include <QObject>
#include <QCoreApplication>

#include <akonadi/entity.h>
#include <akonadi/collection.h>
#include <akonadi/collectionview.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/monitor.h>
#include <akonadi/session.h>

#include <KCal/Incidence>

using namespace KCal;

class AkonadiCalendarCollection : public QObject
{
    Q_OBJECT
  public:
    AkonadiCalendar *m_calendar;
    Akonadi::Collection m_collection;

    AkonadiCalendarCollection(AkonadiCalendar *calendar, const Akonadi::Collection &collection)
      : QObject()
      , m_calendar(calendar)
      , m_collection(collection)
    {
    }

    ~AkonadiCalendarCollection()
    {
    }
};

class AkonadiCalendarItem : public QObject
{
    Q_OBJECT
  public:
    AkonadiCalendar *m_calendar;
    Akonadi::Item m_item;

    AkonadiCalendarItem(AkonadiCalendar *calendar, const Akonadi::Item &item)
      : QObject()
      , m_calendar(calendar)
      , m_item(item)
    {
    }

    ~AkonadiCalendarItem()
    {
    }

    KCal::Incidence::Ptr incidence() const
    {
      Q_ASSERT( m_item.hasPayload() );
      return m_item.payload<KCal::Incidence::Ptr>();
    }

};

class KCal::AkonadiCalendar::Private : public QObject
{
    Q_OBJECT
  public:
    explicit Private(AkonadiCalendar *q)
      : q(q)
      , m_monitor( new Akonadi::Monitor() )
      , m_session( new Akonadi::Session( QCoreApplication::instance()->applicationName().toUtf8() + QByteArray("-AkonadiCal-") + QByteArray::number(qrand()) ) )
    {
      m_monitor->itemFetchScope().fetchFullPayload();
      m_monitor->ignoreSession( m_session );

      connect( m_monitor, SIGNAL(itemChanged( const Akonadi::Item&, const QSet<QByteArray>& )),
               this, SLOT(itemChanged( const Akonadi::Item&, const QSet<QByteArray>& )) );
      connect( m_monitor, SIGNAL(itemMoved( const Akonadi::Item&, const Akonadi::Collection&, const Akonadi::Collection& )),
               this, SLOT(itemMoved( const Akonadi::Item&, const Akonadi::Collection&, const Akonadi::Collection& ) ) );
      connect( m_monitor, SIGNAL(itemAdded( const Akonadi::Item&, const Akonadi::Collection& )),
               this, SLOT(itemAdded( const Akonadi::Item&, const Akonadi::Collection& )) );
      connect( m_monitor, SIGNAL(itemRemoved( const Akonadi::Item&, const Akonadi::Collection& )),
               this, SLOT(itemRemoved( const Akonadi::Item&, const Akonadi::Collection& )) );
      /*
      connect( m_monitor, SIGNAL(itemLinked(const Akonadi::Item&, const Akonadi::Collection&)),
               this, SLOT(itemAdded(const Akonadi::Item&, const Akonadi::Collection&)) );
      connect( m_monitor, SIGNAL(itemUnlinked( const Akonadi::Item&, const Akonadi::Collection& )),
               this, SLOT(itemRemoved( const Akonadi::Item&, const Akonadi::Collection& )) );
      */
    }

    ~Private()
    {
      delete m_monitor;
      delete m_session;
    }

    void clear()
    {
/*
      mEvents.clear();
      mTodos.clear();
      mJournals.clear();
      m_map.clear();
*/
      qDeleteAll(m_itemMap);
      qDeleteAll(m_collectionMap);
    }

/*
#if 0
    CalFormat *mFormat;                    // calendar format
#endif
    QHash<QString, Event *>mEvents;        // hash on uids of all Events
#if 0
    QMultiHash<QString, Event *>mEventsForDate;// on start dates of non-recurring, single-day Events
#endif
    QHash<QString, Todo *>mTodos;          // hash on uids of all Todos
#if 0
    QMultiHash<QString, Todo*>mTodosForDate;// on due dates for all Todos
#endif
    QHash<QString, Journal *>mJournals;    // hash on uids of all Journals
#if 0
    QMultiHash<QString, Journal *>mJournalsForDate; // on dates of all Journals
    Incidence::List mDeletedIncidences;    // list of all deleted Incidences
#endif
*/
    AkonadiCalendar *q;
    Akonadi::Monitor *m_monitor;
    Akonadi::Session *m_session;

    //keep instance to increment shared_ptr ref-counter
    //Akonadi::Item::List m_items;
    //QMap<Incidence*,Akonadi::Item> m_map;
    //QList<AkonadiCalendarCollection*> m_collectionList;
    QHash<Akonadi::Entity::Id, AkonadiCalendarCollection*> m_collectionMap;
    QHash<QString, AkonadiCalendarItem*> m_itemMap; //TODO replace Incidence::uid-QString with Akonadi::Entity::Id-int

  public Q_SLOTS:
  
    void listingDone( KJob *job )
    {
        kDebug();
        Akonadi::ItemFetchJob *fetchjob = static_cast<Akonadi::ItemFetchJob*>( job );
        if ( job->error() ) {
            kWarning( 5250 ) << "Item query failed:" << job->errorString();
            return;
        }
        itemsAdded( fetchjob->items(), fetchjob->collection() );
    }

    void createDone( KJob *job )
    {
        kDebug();
        if ( job->error() ) {
            kWarning( 5250 ) << "Item create failed:" << job->errorString();
            return;
        }
        //Akonadi::ItemCreateJob *createjob = static_cast<Akonadi::ItemCreateJob*>( job );
        //itemAdded( createjob->item(), createjob->collection() ); //done by the monitor
    }

    void deleteDone( KJob *job )
    {
        kDebug();
        if ( job->error() ) {
            kWarning( 5250 ) << "Item delete failed:" << job->errorString();
            return;
        }
        //Akonadi::ItemDeleteJob *deletejob = static_cast<Akonadi::ItemDeleteJob*>( job );
        //itemsRemoved( deletejob->items(), deletejob->collection() ); //done by the monitor
    }

    void modifyDone( KJob *job )
    {
        kDebug();
        if ( job->error() ) {
            kWarning( 5250 ) << "Item modify failed:" << job->errorString();
            return;
        }
        Akonadi::ItemModifyJob *modifyjob = static_cast<Akonadi::ItemModifyJob*>( job );
        //TODO
        emit q->calendarChanged();
    }

    void itemChanged( const Akonadi::Item&, const QSet<QByteArray>& )
    {
        kDebug()<<"TODO";
#if 0
        int row = rowForItem( item );
        if ( row < 0 ) return;
        items[ row ]->item = item;
        itemHash.remove( item );
        itemHash[ item ] = items[ row ];
        QModelIndex start = mParent->index( row, 0, QModelIndex() );
        QModelIndex end = mParent->index( row, mParent->columnCount( QModelIndex() ) - 1 , QModelIndex() );
        mParent->dataChanged( start, end );
#endif
    }

    void itemMoved( const Akonadi::Item &item, const Akonadi::Collection& colSrc, const Akonadi::Collection& colDst )
    {
        kDebug();
        if( m_collectionMap.contains(colSrc.id()) && ! m_collectionMap.contains(colDst.id()) )
            itemRemoved( item, colSrc );
        else if( m_collectionMap.contains(colDst.id()) && ! m_collectionMap.contains(colSrc.id()) )
            itemAdded( item, colDst );
    }

    void itemsAdded( const Akonadi::Item::List &items, const Akonadi::Collection &collection )
    {
        kDebug();
        Q_ASSERT( collection.isValid() );
        foreach( const Akonadi::Item &item, items ) {
            Q_ASSERT( item.isValid() );
            Q_ASSERT( item.hasPayload() );
            const KCal::Incidence::Ptr incidence = item.payload<KCal::Incidence::Ptr>();
            kDebug() << "Add uid=" << incidence->uid() << "summary=" << incidence->summary() << "type=" << incidence->type();
#if 0
            //TODO use visitor
            if( Event *event = dynamic_cast<Event*>( incidence.get() ) )
                mEvents.insert( incidence->uid(), event );
            else if( Todo *todo = dynamic_cast<Todo*>( incidence.get() ) )
                mTodos.insert( incidence->uid(), todo );
            else if( Journal *journal = dynamic_cast<Journal*>( incidence.get() ) )
                mJournals.insert( incidence->uid(), journal );
            else
                Q_ASSERT(false);
            incidence->registerObserver(q);
            m_map[ incidence ] = item;
#else
            incidence->registerObserver( q );

            Q_ASSERT( ! m_itemMap.contains( incidence->uid() ) );
            m_itemMap[ incidence->uid() ] = new AkonadiCalendarItem(q, item);
#endif
        }

        emit q->calendarChanged();
    }

    void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection )
    {
        kDebug();
        Q_ASSERT( item.isValid() );
        itemsAdded( Akonadi::Item::List() << item, collection );
    }

    void itemsRemoved( const Akonadi::Item::List &items, const Akonadi::Collection &collection ) {
        kDebug()<<items.count();
        foreach(const Akonadi::Item& item, items) {
            Q_ASSERT( item.isValid() );
            Q_ASSERT( item.hasPayload() );
            const KCal::Incidence::Ptr incidence = item.payload<KCal::Incidence::Ptr>();
#if 0
            mEvents.remove( incidence->uid() );
            mTodos.remove( incidence->uid() );
            mJournals.remove( incidence->uid() );
            m_map.remove( incidence.get() );
#else
            AkonadiCalendarItem *ci = m_itemMap.take( incidence->uid() );
            kDebug() << "Remove uid=" << incidence->uid() << "summary=" << incidence->summary() << "type=" << incidence->type();
            delete ci;
#endif
        }
        emit q->calendarChanged();
    }

    void itemRemoved( const Akonadi::Item &item, const Akonadi::Collection &collection )
    {
        kDebug();
        itemsRemoved( Akonadi::Item::List() << item, collection );
    }
  
};

#endif
