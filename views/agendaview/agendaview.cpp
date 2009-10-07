/*
  Copyright (c) 2007 Volker Krause <vkrause@kde.org>

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
*/

#include "agendaview.h"
#include "akonadicalendar.h"

using namespace KOrg;

AgendaView::AgendaView( CalendarBase *cal, QWidget *parent )
  : KOEventView( cal, parent )
{
  AkonadiCalendar *calres = dynamic_cast<AkonadiCalendar *>( cal );
  if ( calres ) {
#if 0 //AKONADI_PORT_DISABLED: not needed any longer
    connect( calres, SIGNAL(signalResourceAdded(ResourceCalendar *)),
             SLOT(setUpdateNeeded()) );
    connect( calres, SIGNAL(signalResourceModified(ResourceCalendar *)),
             SLOT(setUpdateNeeded()) );
    connect( calres, SIGNAL(signalResourceDeleted(ResourceCalendar *)),
             SLOT(setUpdateNeeded()) );
#endif
  }
}

#include "agendaview.moc"
