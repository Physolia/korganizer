/*
  This file is part of the Groupware/KOrganizer integration.

  Requires the Qt and KDE widget libraries, available at no cost at
  http://www.trolltech.com and http://www.kde.org respectively

  Copyright (c) 2002-2004 Klarälvdalens Datakonsult AB
        <info@klaralvdalens-datakonsult.se>

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
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt.  If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
*/

#ifndef KOGROUPWARE_H
#define KOGROUPWARE_H

#include "korganizer_export.h"

#include <KCal/ICalFormat>

class FreeBusyManager;

namespace KOrg {
  class CalendarViewBase;
  class IncidenceChangerBase;
  class AkonadiCalendar;
  class AkonadiCalendar;
}

using namespace KOrg;

namespace KCal {
  class Event;
  class Incidence;
}
using namespace KCal;

class QString;

class KORGANIZER_CORE_EXPORT KOGroupware : public QObject
{
  Q_OBJECT
  public:
    static KOGroupware *create( KOrg::CalendarViewBase *, KOrg::AkonadiCalendar * );
    static KOGroupware *instance();

    FreeBusyManager *freeBusyManager();

    /** Send iCal messages after asking the user
         Returns false if the user cancels the dialog, and true if the
         user presses Yes or No.
    */
    bool sendICalMessage( QWidget *parent, KCal::iTIPMethod method,
                          KCal::Incidence *incidence, bool isDeleting = false,
                          bool statusChanged = false );

    /**
      Send counter proposal message.
      @param oldEvent The original event provided in the invitations.
      @param newEvent The new event as edited by the user.
    */
    void sendCounterProposal( KCal::Event *oldEvent, KCal::Event *newEvent ) const;

    // THIS IS THE ACTUAL KM/KO API
    enum EventState {
      Accepted,
      ConditionallyAccepted,
      Declined,
      Request
    };

    // DoNoNotify is a flag indicating that the user does not want
    // updates sent back to the organizer.
    void setDoNotNotify( bool notify ) { mDoNotNotify = notify; }
    bool doNotNotify() { return mDoNotNotify; }

  private slots:
    /** Handle iCals given by KMail. */
    void incomingDirChanged( const QString &path );

    /** Updates some slot connections when the view incidence changer changes */
    void slotViewNewIncidenceChanger( IncidenceChangerBase *changer );

    void initialCheckForChanges();

  protected:
    KOGroupware( KOrg::CalendarViewBase *, KOrg::AkonadiCalendar * );

  private:
    static KOGroupware *mInstance;
    KCal::ICalFormat mFormat;
    KOrg::CalendarViewBase *mView;
    KOrg::AkonadiCalendar *mCalendar;
    static FreeBusyManager *mFreeBusyManager;
    bool mDoNotNotify;
};

#endif
