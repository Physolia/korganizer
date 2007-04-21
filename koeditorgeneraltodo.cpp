/*
    This file is part of KOrganizer.
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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <QToolTip>
#include <QLayout>
#include <QDateTime>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>

#include <QGridLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QVBoxLayout>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <ktextedit.h>

#include <kcal/todo.h>

#include <libkdepim/kdateedit.h>

#include "koprefs.h"
#include "koglobals.h"
#include "ktimeedit.h"

#include "koeditorgeneraltodo.h"
#include "koeditorgeneraltodo.moc"

KOEditorGeneralTodo::KOEditorGeneralTodo(QObject* parent,
                                         const char* name)
  : KOEditorGeneral( parent, name)
{
}

KOEditorGeneralTodo::~KOEditorGeneralTodo()
{
}

void KOEditorGeneralTodo::finishSetup()
{
  QWidget::setTabOrder( mSummaryEdit, mLocationEdit );
  QWidget::setTabOrder( mLocationEdit, mStartCheck );
  QWidget::setTabOrder( mStartCheck, mStartDateEdit );
  QWidget::setTabOrder( mStartDateEdit, mStartTimeEdit );
  QWidget::setTabOrder( mStartTimeEdit, mDueCheck );
  QWidget::setTabOrder( mDueCheck, mDueDateEdit );
  QWidget::setTabOrder( mDueDateEdit, mDueTimeEdit );
  QWidget::setTabOrder( mDueTimeEdit, mTimeButton );
  QWidget::setTabOrder( mTimeButton, mCompletedCombo );
  QWidget::setTabOrder( mCompletedCombo, mPriorityCombo );
  QWidget::setTabOrder( mPriorityCombo, mAlarmButton );
  QWidget::setTabOrder( mAlarmButton, mAlarmTimeEdit );
  QWidget::setTabOrder( mAlarmTimeEdit, mAlarmIncrCombo );
//   QWidget::setTabOrder( mAlarmIncrCombo, mAlarmSoundButton );
  QWidget::setTabOrder( mAlarmIncrCombo, mAlarmEditButton );
//   QWidget::setTabOrder( mAlarmSoundButton, mAlarmProgramButton );
//   QWidget::setTabOrder( mAlarmProgramButton, mDescriptionEdit );
  QWidget::setTabOrder( mAlarmEditButton, mDescriptionEdit );
  QWidget::setTabOrder( mDescriptionEdit, mCategoriesButton );
  QWidget::setTabOrder( mCategoriesButton, mSecrecyCombo );
//  QWidget::setTabOrder( mSecrecyCombo, mDescriptionEdit );

  mSummaryEdit->setFocus();
}

void KOEditorGeneralTodo::initTime(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *timeLayout = new QVBoxLayout();
  topLayout->addItem(timeLayout);

  QGroupBox *timeGroupBox = new QGroupBox( i18n("Date && Time"), parent );
  timeGroupBox->setWhatsThis(
                   i18n("Sets options for due and start dates and times "
                        "for this to-do.") );
  timeLayout->addWidget( timeGroupBox );

  QGridLayout *layoutTimeBox = new QGridLayout( timeGroupBox );
  layoutTimeBox->setSpacing( KDialog::spacingHint() );

  QString whatsThis = i18n("Sets the start date for this to-do");
  mStartCheck = new QCheckBox(i18n("Sta&rt:"),timeGroupBox);
  mStartCheck->setWhatsThis( whatsThis );
  layoutTimeBox->addWidget(mStartCheck,0,0);
  connect(mStartCheck,SIGNAL(toggled(bool)),SLOT(enableStartEdit(bool)));
  connect(mStartCheck,SIGNAL(toggled(bool)),SLOT(startDateModified()));

  mStartDateEdit = new KDateEdit(timeGroupBox);
  mStartDateEdit->setWhatsThis( whatsThis );
  layoutTimeBox->addWidget(mStartDateEdit,0,1);
  connect(mStartDateEdit,SIGNAL(dateChanged(const QDate&)),SLOT(startDateModified()));

  mStartTimeEdit = new KTimeEdit(timeGroupBox);
  mStartTimeEdit->setWhatsThis(
                   i18n("Sets the start time for this to-do.") );
  layoutTimeBox->addWidget(mStartTimeEdit,0,2);
  connect(mStartTimeEdit,SIGNAL(timeChanged(QTime)),SLOT(startDateModified()));

  whatsThis = i18n("Sets the due date for this to-do.");
  mDueCheck = new QCheckBox(i18n("&Due:"),timeGroupBox);
  mDueCheck->setWhatsThis( whatsThis );
  layoutTimeBox->addWidget(mDueCheck,1,0);
  connect(mDueCheck,SIGNAL(toggled(bool)),SLOT(enableDueEdit(bool)));
  connect(mDueCheck,SIGNAL(toggled(bool)),SLOT(showAlarm()));
  connect(mDueCheck,SIGNAL(toggled(bool)),SIGNAL(dueDateEditToggle(bool)));
  connect(mDueCheck,SIGNAL(toggled(bool)),SLOT(dateChanged()));

  mDueDateEdit = new KDateEdit(timeGroupBox);
  mDueDateEdit->setWhatsThis( whatsThis );
  layoutTimeBox->addWidget(mDueDateEdit,1,1);
  connect(mDueDateEdit,SIGNAL(dateChanged(const QDate&)),SLOT(dateChanged()));

  mDueTimeEdit = new KTimeEdit(timeGroupBox);
  mDueTimeEdit->setWhatsThis(
                   i18n("Sets the due time for this to-do.") );
  layoutTimeBox->addWidget(mDueTimeEdit,1,2);
  connect(mDueTimeEdit,SIGNAL(timeChanged( QTime )),SLOT(dateChanged()));

  mTimeButton = new QCheckBox(i18n("Ti&me associated"),timeGroupBox);
  mTimeButton->setWhatsThis(
                   i18n("Sets whether or not this to-do's start and due dates "
                        "have times associated with them.") );
  layoutTimeBox->addWidget( mTimeButton, 2, 0, 1, 3 );

  connect(mTimeButton,SIGNAL(toggled(bool)),SLOT(enableTimeEdits(bool)));
  connect(mTimeButton,SIGNAL(toggled(bool)),SLOT(dateChanged()));

  // some more layouting
  layoutTimeBox->setColumnStretch(3,1);
}


void KOEditorGeneralTodo::initCompletion(QWidget *parent, QBoxLayout *topLayout)
{
  QString whatsThis = i18n("Sets the current completion status of this to-do "
                           "as a percentage.");
  mCompletedCombo = new QComboBox(parent);
  mCompletedCombo->setWhatsThis( whatsThis );
  for (int i = 0; i <= 100; i+=10) {
    // xgettext:no-c-format
    QString label = i18nc("Percent complete", "%1 %", i);
    mCompletedCombo->addItem( label );
  }
  connect(mCompletedCombo,SIGNAL(activated(int)),SLOT(completedChanged(int)));
  topLayout->addWidget(mCompletedCombo);

  mCompletedLabel = new QLabel(i18n("co&mpleted"),parent);
  topLayout->addWidget(mCompletedLabel);
  mCompletedLabel->setBuddy( mCompletedCombo );
  mCompletionDateEdit = new KDateEdit( parent );
  topLayout->addWidget( mCompletionDateEdit );
  mCompletionTimeEdit = new KTimeEdit( parent, QTime() );
  topLayout->addWidget( mCompletionTimeEdit );
}

void KOEditorGeneralTodo::initPriority(QWidget *parent, QBoxLayout *topLayout)
{
  QString whatsThis = i18n("Sets the priority of this to-do on a scale "
                           "from one to nine, with one being the highest "
                           "priority, five being a medium priority, and "
                           "nine being the lowest. In programs that have a "
                           "different scale, the numbers will be adjusted "
                           "to match the appropriate scale.");
  QLabel *priorityLabel = new QLabel(i18n("&Priority:"),parent);
  topLayout->addWidget(priorityLabel);

  mPriorityCombo = new QComboBox(parent);
  mPriorityCombo->addItem( i18n("unspecified") );
  mPriorityCombo->addItem( i18n("1 (highest)") );
  mPriorityCombo->addItem( i18n("2") );
  mPriorityCombo->addItem( i18n("3") );
  mPriorityCombo->addItem( i18n("4") );
  mPriorityCombo->addItem( i18n("5 (medium)") );
  mPriorityCombo->addItem( i18n("6") );
  mPriorityCombo->addItem( i18n("7") );
  mPriorityCombo->addItem( i18n("8") );
  mPriorityCombo->addItem( i18n("9 (lowest)") );
  topLayout->addWidget(mPriorityCombo);
  priorityLabel->setBuddy( mPriorityCombo );
}

void KOEditorGeneralTodo::initStatus(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *statusLayout = new QHBoxLayout();
  topLayout->addItem(statusLayout);

  initCompletion( parent, statusLayout );

  statusLayout->addStretch( 1 );

  initPriority( parent, statusLayout );
}

void KOEditorGeneralTodo::setDefaults( const QDateTime &due, bool allDay )
{
kdDebug()<<"KOEditorGeneralTodo::setDefaults: " << due <<endl;
  KOEditorGeneral::setDefaults(allDay);

  mTimeButton->setChecked( !allDay );
  if(mTimeButton->isChecked()) {
    mTimeButton->setEnabled(true);
  }
  else {
    mTimeButton->setEnabled(false);
  }

  enableTimeEdits( !allDay );

  mDueCheck->setChecked( due.isValid() );
  enableDueEdit( due.isValid() );

  mStartCheck->setChecked(false);
  enableStartEdit(false);

  if ( due.isValid() ) {
    mDueDateEdit->setDate( due.date() );
    mDueTimeEdit->setTime( due.time() );
  }

  if ( QDateTime::currentDateTime() < due ) {
    mStartDateEdit->setDate( QDate::currentDate() );
    mStartTimeEdit->setTime( QTime::currentTime() );
  } else {
    mStartDateEdit->setDate( due.date().addDays( -1 ) );
    mStartTimeEdit->setTime( due.time() );
  }
  mStartDateModified = false;

  mPriorityCombo->setCurrentIndex(5);

  mCompletedCombo->setCurrentIndex(0);
}

void KOEditorGeneralTodo::readTodo(Todo *todo)
{
  KOEditorGeneral::readIncidence(todo);

  QDateTime dueDT;

  KDateTime::Spec timeSpec = KOPrefs::instance()->timeSpec();
  if (todo->hasDueDate()) {
    enableAlarm( true );
    dueDT = todo->dtDue().toTimeSpec(timeSpec).dateTime();
    mDueDateEdit->setDate(dueDT.date());
    mDueTimeEdit->setTime(dueDT.time());
    mDueCheck->setChecked(true);
  } else {
    enableAlarm( false );
    mDueDateEdit->setEnabled(false);
    mDueTimeEdit->setEnabled(false);
    mDueDateEdit->setDate(QDate::currentDate());
    mDueTimeEdit->setTime(QTime::currentTime());
    mDueCheck->setChecked(false);
  }

  if (todo->hasStartDate()) {
    QDateTime start = todo->dtStart().toTimeSpec(timeSpec).dateTime();
    mStartDateEdit->setDate(start.date());
    mStartTimeEdit->setTime(start.time());
    mStartCheck->setChecked(true);
  } else {
    mStartDateEdit->setEnabled(false);
    mStartTimeEdit->setEnabled(false);
    mStartDateEdit->setDate(QDate::currentDate());
    mStartTimeEdit->setTime(QTime::currentTime());
    mStartCheck->setChecked(false);
  }

  mTimeButton->setChecked( !todo->floats() );

  mAlreadyComplete = false;
  mCompletedCombo->setCurrentIndex(todo->percentComplete() / 10);
  if (todo->isCompleted() && todo->hasCompletedDate()) {
    mCompleted = todo->completed().toTimeSpec(timeSpec).dateTime();
    mAlreadyComplete = true;
  }
  setCompletedDate();

  mPriorityCombo->setCurrentIndex( todo->priority() );
  mStartDateModified = false;
}

void KOEditorGeneralTodo::writeTodo(Todo *todo)
{
  KOEditorGeneral::writeIncidence(todo);

  // temp. until something better happens.
  QString tmpStr;

  todo->setHasDueDate(mDueCheck->isChecked());
  todo->setHasStartDate(mStartCheck->isChecked());

  QDate tmpSDate, tmpDDate;
  QTime tmpSTime, tmpDTime;
  QDateTime tmpStartDT, tmpDueDT;
  if ( mTimeButton->isChecked() ) {
    todo->setFloats(false);

    // set due date/time
    tmpDDate = mDueDateEdit->date();
    tmpDTime = mDueTimeEdit->getTime();
    tmpDueDT.setDate(tmpDDate);
    tmpDueDT.setTime(tmpDTime);

    // set start date/time
    if ( mStartCheck->isChecked() ) {
      tmpSDate = mStartDateEdit->date();
      tmpSTime = mStartTimeEdit->getTime();
      tmpStartDT.setDate(tmpSDate);
      tmpStartDT.setTime(tmpSTime);
    } else {
      tmpStartDT = tmpDueDT;
    }
  } else {
    todo->setFloats(true);

    // need to change this.
    tmpDDate = mDueDateEdit->date();
    tmpDTime.setHMS(0,0,0);
    tmpDueDT.setDate(tmpDDate);
    tmpDueDT.setTime(tmpDTime);

    if ( mStartCheck->isChecked() ) {
      tmpSDate = mStartDateEdit->date();
      tmpSTime.setHMS(0,0,0);
      tmpStartDT.setDate(tmpSDate);
      tmpStartDT.setTime(tmpSTime);
    } else {
      tmpStartDT = tmpDueDT;
    }
  }

  // TODO: Don't use the due date for the recurrence, but the start date (cf. rfc 2445)
  KDateTime::Spec timeSpec = KOPrefs::instance()->timeSpec();
  if ( todo->doesRecur() && !mStartDateModified ) {
    todo->setDtDue( KDateTime(tmpDueDT, timeSpec) );
  } else {
      todo->setDtDue( KDateTime(tmpDueDT, timeSpec), true );
      todo->setDtStart( KDateTime(tmpStartDT, timeSpec) );
      todo->setDtRecurrence( KDateTime(tmpDueDT, timeSpec) );
  }

  todo->setPriority( mPriorityCombo->currentIndex() );

  // set completion state
  todo->setPercentComplete(mCompletedCombo->currentIndex() * 10);

  if (mCompletedCombo->currentIndex() == 10 && mCompleted.isValid()) {
    QDateTime completed( mCompletionDateEdit->date(),
                         mCompletionTimeEdit->getTime() );
    int difference = mCompleted.secsTo( completed );
    if ( (difference < 60) && (difference > -60) &&
         (completed.time().minute() == mCompleted.time().minute() ) ) {
      // completion time wasn't changed substantially (only the seconds
      // truncated, but that's an effect done by KTimeEdit automatically).
      completed = mCompleted;
    }
    todo->setCompleted( KDateTime(completed, timeSpec) );
  }
}

void KOEditorGeneralTodo::enableDueEdit(bool enable)
{
  mDueDateEdit->setEnabled( enable );

  if(mDueCheck->isChecked() || mStartCheck->isChecked()) {
    mTimeButton->setEnabled(true);
  } else {
    mTimeButton->setEnabled(false);
  }

  if (enable) {
    mDueTimeEdit->setEnabled( mTimeButton->isChecked() );
  } else {
    mDueTimeEdit->setEnabled( false );
  }
}

void KOEditorGeneralTodo::enableStartEdit( bool enable )
{
  mStartDateEdit->setEnabled( enable );

  if(mDueCheck->isChecked() || mStartCheck->isChecked()) {
    mTimeButton->setEnabled(true);
  }
  else {
    mTimeButton->setEnabled(false);
    mTimeButton->setChecked(false);
  }

  if (enable) {
    mStartTimeEdit->setEnabled( mTimeButton->isChecked() );
  } else {
    mStartTimeEdit->setEnabled( false );
  }
}

void KOEditorGeneralTodo::enableTimeEdits(bool enable)
{
  if(mStartCheck->isChecked()) {
    mStartTimeEdit->setEnabled( enable );
  }
  if(mDueCheck->isChecked()) {
    mDueTimeEdit->setEnabled( enable );
  }
}

void KOEditorGeneralTodo::showAlarm()
{
  enableAlarm( mDueCheck->isChecked() );
}

bool KOEditorGeneralTodo::validateInput()
{
  if (mDueCheck->isChecked()) {
    if (!mDueDateEdit->date().isValid()) {
      KMessageBox::sorry(0,i18n("Please specify a valid due date."));
      return false;
    }
    if (mTimeButton->isChecked()) {
      if (!mDueTimeEdit->inputIsValid()) {
        KMessageBox::sorry(0,i18n("Please specify a valid due time."));
        return false;
      }
    }
  }

  if (mStartCheck->isChecked()) {
    if (!mStartDateEdit->date().isValid()) {
      KMessageBox::sorry(0,i18n("Please specify a valid start date."));
      return false;
    }
    if (mTimeButton->isChecked()) {
      if (!mStartTimeEdit->inputIsValid()) {
        KMessageBox::sorry(0,i18n("Please specify a valid start time."));
        return false;
      }
    }
  }

  if (mStartCheck->isChecked() && mDueCheck->isChecked()) {
    QDateTime startDate;
    QDateTime dueDate;
    startDate.setDate(mStartDateEdit->date());
    dueDate.setDate(mDueDateEdit->date());
    if (mTimeButton->isChecked()) {
      startDate.setTime(mStartTimeEdit->getTime());
      dueDate.setTime(mDueTimeEdit->getTime());
    }
    if (startDate > dueDate) {
      KMessageBox::sorry(0,
                         i18n("The start date cannot be after the due date."));
      return false;
    }
  }

  return KOEditorGeneral::validateInput();
}

void KOEditorGeneralTodo::completedChanged(int index)
{
  if (index == 10) {
    mCompleted = QDateTime::currentDateTime();
  }
  setCompletedDate();
}

void KOEditorGeneralTodo::dateChanged()
{
  KLocale *l = KGlobal::locale();
  QString dateTimeStr = "";

  if ( mStartCheck->isChecked() ) {
    dateTimeStr += i18n("Start: %1",
                                     l->formatDate( mStartDateEdit->date() ) );
    if ( mTimeButton->isChecked() )
      dateTimeStr += QString(" %1").arg(
                                   l->formatTime( mStartTimeEdit->getTime() ) );
  }

  if ( mDueCheck->isChecked() ) {
    dateTimeStr += i18n("   Due: %1",
                                      l->formatDate( mDueDateEdit->date() ) );
    if ( mTimeButton->isChecked() )
      dateTimeStr += QString(" %1").arg(
                                    l->formatTime( mDueTimeEdit->getTime() ) );
  }

  emit dateTimeStrChanged( dateTimeStr );
  QDateTime endDt( mDueDateEdit->date(), mDueTimeEdit->getTime() );
  emit signalDateTimeChanged( endDt, endDt );
}

void KOEditorGeneralTodo::startDateModified()
{
  mStartDateModified = true;
  dateChanged();
}

void KOEditorGeneralTodo::setCompletedDate()
{
  if (mCompletedCombo->currentIndex() == 10 && mCompleted.isValid()) {
    mCompletedLabel->setText(i18n("co&mpleted on"));
//        .arg(KGlobal::locale()->formatDateTime(mCompleted)));
    mCompletionDateEdit->show();
    mCompletionTimeEdit->show();
    mCompletionDateEdit->setDate( mCompleted.date() );
    mCompletionTimeEdit->setTime( mCompleted.time() );
  } else {
    mCompletedLabel->setText(i18n("co&mpleted"));
    mCompletionDateEdit->hide();
    mCompletionTimeEdit->hide();
  }
}

void KOEditorGeneralTodo::modified (Todo* todo, int modification)
{
  switch (modification) {
  case KOGlobals::PRIORITY_MODIFIED:
    mPriorityCombo->setCurrentIndex( todo->priority() );
    break;
  case KOGlobals::COMPLETION_MODIFIED:
    mCompletedCombo->setCurrentIndex(todo->percentComplete() / 10);
    if (todo->isCompleted() && todo->hasCompletedDate()) {
      mCompleted = todo->completed().toTimeSpec(KOPrefs::instance()->timeSpec()).dateTime();
    }
    setCompletedDate();
    break;
  case KOGlobals::CATEGORY_MODIFIED:
    setCategories( todo->categories() );
    break;
  case KOGlobals::UNKNOWN_MODIFIED: // fall through
  default:
    readTodo( todo );
    break;
  }
}
