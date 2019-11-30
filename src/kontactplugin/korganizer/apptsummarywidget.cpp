/*
  This file is part of Kontact.

  Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>
  Copyright (c) 2005-2006,2008-2009 Allen Winter <winter@kde.org>

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

#include "apptsummarywidget.h"
#include "korganizerplugin.h"
#include "summaryeventinfo.h"
#include <kwidgetsaddons_version.h>
#include "korganizerinterface.h"

#include <CalendarSupport/Utils>
#include <CalendarSupport/CalendarSingleton>

#include <AkonadiCore/Collection>
#include <Akonadi/Calendar/IncidenceChanger>

#include <KCalendarCore/Calendar>
#include <KCalendarCore/Event>

#include <KontactInterface/Core>

#include <KConfigGroup>
#include <KColorScheme>
#include <KLocalizedString>
#include <QMenu>
#include <KUrlLabel>
#include <KConfig>

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>

ApptSummaryWidget::ApptSummaryWidget(KOrganizerPlugin *plugin, QWidget *parent)
    : KontactInterface::Summary(parent)
    , mPlugin(plugin)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3, 3, 3, 3);

    QWidget *header = createHeader(
        this, QStringLiteral("view-calendar-upcoming-events"), i18n("Upcoming Events"));
    mainLayout->addWidget(header);

    mLayout = new QGridLayout();
    mainLayout->addItem(mLayout);
    mLayout->setSpacing(3);
    mLayout->setRowStretch(6, 1);

    QStringList mimeTypes;
    mimeTypes << KCalendarCore::Event::eventMimeType();
    mCalendar = CalendarSupport::calendarSingleton();

    mChanger = new Akonadi::IncidenceChanger(parent);

    connect(
        mCalendar.data(), &Akonadi::ETMCalendar::calendarChanged, this,
        &ApptSummaryWidget::updateView);
    connect(
        mPlugin->core(), &KontactInterface::Core::dayChanged, this, &ApptSummaryWidget::updateView);

    // Update Configuration
    configUpdated();
}

ApptSummaryWidget::~ApptSummaryWidget()
{
}

void ApptSummaryWidget::configUpdated()
{
    KConfig config(QStringLiteral("kcmapptsummaryrc"));

    KConfigGroup group = config.group("Days");
    mDaysAhead = group.readEntry("DaysToShow", 7);

    group = config.group("Show");
    mShowBirthdaysFromCal = group.readEntry("BirthdaysFromCalendar", true);
    mShowAnniversariesFromCal = group.readEntry("AnniversariesFromCalendar", true);

    group = config.group("Groupware");
    mShowMineOnly = group.readEntry("ShowMineOnly", false);

    updateView();
}

void ApptSummaryWidget::updateView()
{
    qDeleteAll(mLabels);
    mLabels.clear();

    // The event print consists of the following fields:
    //  icon:start date:days-to-go:summary:time range
    // where,
    //   the icon is the typical event icon
    //   the start date is the event start date
    //   the days-to-go is the #days until the event starts
    //   the summary is the event summary
    //   the time range is the start-end time (only for non-floating events)

    QLabel *label = nullptr;
    int counter = 0;

    QPixmap pm = QIcon::fromTheme(QStringLiteral("view-calendar-day")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize));
    QPixmap pmb = QIcon::fromTheme(QStringLiteral("view-calendar-birthday")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize));
    QPixmap pma = QIcon::fromTheme(QStringLiteral("view-calendar-wedding-anniversary")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize));

    QStringList uidList;
    SummaryEventInfo::setShowSpecialEvents(mShowBirthdaysFromCal,
                                           mShowAnniversariesFromCal);
    QDate currentDate = QDate::currentDate();

    const SummaryEventInfo::List events = SummaryEventInfo::eventsForRange(currentDate, currentDate.addDays(
                                                                               mDaysAhead - 1),
                                                                           mCalendar);

    QPalette todayPalette = palette();
    KColorScheme::adjustBackground(todayPalette, KColorScheme::ActiveBackground, QPalette::Window);
    QPalette urgentPalette = palette();
    KColorScheme::adjustBackground(urgentPalette, KColorScheme::NegativeBackground,
                                   QPalette::Window);

    for (SummaryEventInfo *event : events) {
        // Optionally, show only my Events
        /*      if ( mShowMineOnly &&
                  !KCalendarCore::CalHelper::isMyCalendarIncidence( mCalendarAdaptor, event->ev ) ) {
              continue;
            }
            TODO: CalHelper is deprecated, remove this?
        */

        KCalendarCore::Event::Ptr ev = event->ev;
        // print the first of the recurring event series only
        if (ev->recurs()) {
            if (uidList.contains(ev->instanceIdentifier())) {
                continue;
            }
            uidList.append(ev->instanceIdentifier());
        }

        // Icon label
        label = new QLabel(this);
        if (ev->categories().contains(QLatin1String("BIRTHDAY"), Qt::CaseInsensitive)) {
            label->setPixmap(pmb);
        } else if (ev->categories().contains(QLatin1String("ANNIVERSARY"), Qt::CaseInsensitive)) {
            label->setPixmap(pma);
        } else {
            label->setPixmap(pm);
        }
        label->setMaximumWidth(label->minimumSizeHint().width());
        mLayout->addWidget(label, counter, 0);
        mLabels.append(label);

        // Start date or date span label
        QString dateToDisplay = event->startDate;
        if (!event->dateSpan.isEmpty()) {
            dateToDisplay = event->dateSpan;
        }
        label = new QLabel(dateToDisplay, this);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        mLayout->addWidget(label, counter, 1);
        mLabels.append(label);
        if (event->makeBold) {
            QFont font = label->font();
            font.setBold(true);
            label->setFont(font);
            if (!event->makeUrgent) {
                label->setPalette(todayPalette);
            } else {
                label->setPalette(urgentPalette);
            }
            label->setAutoFillBackground(true);
        }

        // Days to go label
        label = new QLabel(event->daysToGo, this);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        mLayout->addWidget(label, counter, 2);
        mLabels.append(label);

        // Summary label
        KUrlLabel *urlLabel = new KUrlLabel(this);
        urlLabel->setText(event->summaryText);
        urlLabel->setUrl(event->summaryUrl);
        urlLabel->installEventFilter(this);
        urlLabel->setTextFormat(Qt::RichText);
        urlLabel->setWordWrap(true);
        mLayout->addWidget(urlLabel, counter, 3);
        mLabels.append(urlLabel);
#if KWIDGETSADDONS_VERSION < QT_VERSION_CHECK(5, 65, 0)
        connect(urlLabel, QOverload<const QString &>::of(
                    &KUrlLabel::leftClickedUrl), this, &ApptSummaryWidget::viewEvent);
        connect(urlLabel, QOverload<const QString &>::of(
                    &KUrlLabel::rightClickedUrl), this, &ApptSummaryWidget::popupMenu);
#else
        connect(urlLabel, &KUrlLabel::leftClickedUrl, this, [this, urlLabel] {
            viewEvent(urlLabel->url());
        });
        connect(urlLabel, &KUrlLabel::rightClickedUrl, this, [this, urlLabel] {
            popupMenu(urlLabel->url());
        });
#endif
        if (!event->summaryTooltip.isEmpty()) {
            urlLabel->setToolTip(event->summaryTooltip);
        }

        // Time range label (only for non-floating events)
        if (!event->timeRange.isEmpty()) {
            label = new QLabel(event->timeRange, this);
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            mLayout->addWidget(label, counter, 4);
            mLabels.append(label);
        }

        counter++;
    }

    qDeleteAll(events);

    if (!counter) {
        QLabel *noEvents = new QLabel(
            i18np("No upcoming events starting within the next day",
                  "No upcoming events starting within the next %1 days",
                  mDaysAhead), this);
        noEvents->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        mLayout->addWidget(noEvents, 0, 0);
        mLabels.append(noEvents);
    }

    for (QLabel *label : qAsConst(mLabels)) {
        label->show();
    }
}

void ApptSummaryWidget::viewEvent(const QString &uid)
{
    Akonadi::Item::Id id = mCalendar->item(uid).id();

    if (id != -1) {
        mPlugin->core()->selectPlugin(QStringLiteral("kontact_korganizerplugin"));   //ensure loaded
        OrgKdeKorganizerKorganizerInterface korganizer(
            QStringLiteral("org.kde.korganizer"), QStringLiteral(
                "/Korganizer"), QDBusConnection::sessionBus());
        korganizer.editIncidence(QString::number(id));
    }
}

void ApptSummaryWidget::removeEvent(const Akonadi::Item &item)
{
    mChanger->deleteIncidence(item);
}

void ApptSummaryWidget::popupMenu(const QString &uid)
{
    QMenu popup(this);

    // FIXME: Should say "Show Appointment" if we don't have rights to edit
    // Doesn't make sense to edit events from birthday resource for example
    QAction *editIt = popup.addAction(i18n("&Edit Appointment..."));

    QAction *delIt = popup.addAction(i18n("&Delete Appointment"));
    delIt->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));

    Akonadi::Item item = mCalendar->item(uid);
    delIt->setEnabled(mCalendar->hasRight(item, Akonadi::Collection::CanDeleteItem));

    const QAction *selectedAction = popup.exec(QCursor::pos());
    if (selectedAction == editIt) {
        viewEvent(uid);
    } else if (selectedAction == delIt) {
        removeEvent(item);
    }
}

bool ApptSummaryWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (obj->inherits("KUrlLabel")) {
        KUrlLabel *label = static_cast<KUrlLabel *>(obj);
        if (e->type() == QEvent::Enter) {
            Q_EMIT message(i18n("Edit Event: \"%1\"", label->text()));
        }
        if (e->type() == QEvent::Leave) {
            Q_EMIT message(QString());
        }
    }

    return KontactInterface::Summary::eventFilter(obj, e);
}

QStringList ApptSummaryWidget::configModules() const
{
    return QStringList() << QStringLiteral("kcmapptsummary.desktop");
}
