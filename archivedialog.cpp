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

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

// ArchiveDialog -- archive/delete past events.

#include "archivedialog.h"
#include "koprefs.h"
#include "eventarchiver.h"

#include <libkdepim/kdateedit.h>

#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <klineedit.h>
#include <kvbox.h>
#include <KComboBox>

#include <QLabel>
#include <QLayout>
#include <QDateTime>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>

#include "archivedialog.moc"

ArchiveDialog::ArchiveDialog(Calendar *cal,QWidget *parent)
  : KDialog (parent)
{
  setCaption( i18n("Archive/Delete Past Events and To-dos") );
  setButtons( User1|Cancel );
  setDefaultButton( User1 );
  setModal( false );
  showButtonSeparator( true );
  setButtonText( User1, i18n("&Archive") );
  mCalendar = cal;

  QFrame *topFrame = new QFrame( this );
  setMainWidget( topFrame );
  QVBoxLayout *topLayout = new QVBoxLayout(topFrame);
  topLayout->setSpacing(spacingHint());

  QLabel *descLabel = new QLabel(
    i18n("Archiving saves old items into the given file and "
         "then deletes them in the current calendar. If the archive file "
         "already exists they will be added. "
         "(<a href=\"whatsthis:In order to add an archive "
         "to your calendar, use the &quot;Merge Calendar&quot; function. "
         "You can view an archive by opening it in KOrganizer like any "
         "other calendar. It is not saved in a special format, but as "
         "vCalendar.\">How to restore</a>)"),
    topFrame);
  descLabel->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard );
  descLabel->setWordWrap( true );
  topLayout->addWidget(descLabel);

  QButtonGroup* radioBG = new QButtonGroup( this );
  connect( radioBG, SIGNAL( buttonClicked( int ) ), SLOT( slotActionChanged() ) );

  QHBoxLayout *dateLayout = new QHBoxLayout();
  dateLayout->setMargin( 0 );
  mArchiveOnceRB = new QRadioButton(i18n("Archive now items older than:"),topFrame);
  dateLayout->addWidget(mArchiveOnceRB);
  radioBG->addButton(mArchiveOnceRB);
  mDateEdit = new KPIM::KDateEdit(topFrame);
  mDateEdit->setWhatsThis(
    i18n("The date before which items should be archived. All older events and to-dos will "
         "be saved and deleted, the newer (and events exactly on that date) will be kept."));
  dateLayout->addWidget(mDateEdit);
  topLayout->addLayout(dateLayout);

  // Checkbox, numinput and combo for auto-archiving
  // (similar to kmail's mExpireFolderCheckBox/mReadExpiryTimeNumInput in kmfolderdia.cpp)
  KHBox* autoArchiveHBox = new KHBox(topFrame);
  topLayout->addWidget(autoArchiveHBox);
  mAutoArchiveRB = new QRadioButton(i18n("Automaticall&y archive items older than:"), autoArchiveHBox);
  radioBG->addButton(mAutoArchiveRB);
  mAutoArchiveRB->setWhatsThis(
    i18n("If this feature is enabled, KOrganizer will regularly check if events and to-dos have to be archived; "
         "this means you will not need to use this dialog box again, except to change the settings."));

  mExpiryTimeNumInput = new KIntNumInput(autoArchiveHBox);
  mExpiryTimeNumInput->setRange(1, 500, 1, false);
  mExpiryTimeNumInput->setEnabled(false);
  mExpiryTimeNumInput->setValue(7);
  mExpiryTimeNumInput->setWhatsThis(
    i18n("The age of the events and to-dos to archive. All older items "
         "will be saved and deleted, the newer will be kept."));

  mExpiryUnitsComboBox = new KComboBox(autoArchiveHBox);
  // Those items must match the "Expiry Unit" enum in the kcfg file!
  mExpiryUnitsComboBox->addItem( i18n("Day(s)") );
  mExpiryUnitsComboBox->addItem( i18n("Week(s)") );
  mExpiryUnitsComboBox->addItem( i18n("Month(s)") );
  mExpiryUnitsComboBox->setEnabled( false );

  QHBoxLayout *fileLayout = new QHBoxLayout();
  fileLayout->setMargin( 0 );
  fileLayout->setSpacing(spacingHint());
  QLabel *l = new QLabel(i18n("Archive &file:"),topFrame);
  fileLayout->addWidget(l);
  mArchiveFile = new KUrlRequester(KOPrefs::instance()->mArchiveFile,topFrame);
  mArchiveFile->setMode(KFile::File);
  mArchiveFile->setFilter(i18n("*.ics|iCalendar Files"));
  mArchiveFile->setWhatsThis(
    i18n("The path of the archive. The events and to-dos will be added to the "
         "archive file, so any events that are already in the file "
         "will not be modified or deleted. You can later load or merge the "
         "file like any other calendar. It is not saved in a special "
         "format, it uses the iCalendar format. "));
  l->setBuddy(mArchiveFile->lineEdit());
  fileLayout->addWidget(mArchiveFile);
  topLayout->addLayout(fileLayout);

  QGroupBox *typeBox = new QGroupBox( i18n("Type of Items to Archive") );
  topLayout->addWidget( typeBox );

  QBoxLayout *typeLayout = new QVBoxLayout( typeBox );

  mEvents = new QCheckBox( i18n("&Events") );
  typeLayout->addWidget( mEvents );
  mTodos = new QCheckBox( i18n("&To-dos") );
  typeLayout->addWidget( mTodos );
  typeBox->setWhatsThis( i18n("Here you can select which items "
                   "should be archived. Events are archived if they "
                   "ended before the date given above; to-dos are archived if "
                   "they were finished before the date.") );

  mDeleteCb = new QCheckBox(i18n("&Delete only, do not save"),
                            topFrame);
  mDeleteCb->setWhatsThis(
    i18n("Select this option to delete old events and to-dos without saving them. "
         "It is not possible to recover the events later."));
  topLayout->addWidget(mDeleteCb);
  connect(mDeleteCb, SIGNAL(toggled(bool)), mArchiveFile, SLOT(setDisabled(bool)));
  connect(mDeleteCb, SIGNAL(toggled(bool)), this, SLOT(slotEnableUser1()));
  connect(mArchiveFile->lineEdit(),SIGNAL(textChanged ( const QString & )),
          this,SLOT(slotEnableUser1()));

  // Load settings from KOPrefs
  mExpiryTimeNumInput->setValue( KOPrefs::instance()->mExpiryTime );
  mExpiryUnitsComboBox->setCurrentIndex( KOPrefs::instance()->mExpiryUnit );
  mDeleteCb->setChecked( KOPrefs::instance()->mArchiveAction == KOPrefs::actionDelete );
  mEvents->setChecked( KOPrefs::instance()->mArchiveEvents );
  mTodos->setChecked( KOPrefs::instance()->mArchiveTodos );

  slotEnableUser1();

  // The focus should go to a useful field by default, not to the top richtext-label
  if ( KOPrefs::instance()->mAutoArchive ) {
    mAutoArchiveRB->setChecked( true );
    mAutoArchiveRB->setFocus();
  } else {
    mArchiveOnceRB->setChecked( true );
    mArchiveOnceRB->setFocus();
  }
  slotActionChanged();
  connect(this,SIGNAL(user1Clicked()),this,SLOT(slotUser1()));
}

ArchiveDialog::~ArchiveDialog()
{
}

void ArchiveDialog::slotEnableUser1()
{
  bool state = ( mDeleteCb->isChecked() ||
                 !mArchiveFile->lineEdit()->text().isEmpty() );
  enableButton(KDialog::User1,state);
}

void ArchiveDialog::slotActionChanged()
{
  mDateEdit->setEnabled( mArchiveOnceRB->isChecked() );
  mExpiryTimeNumInput->setEnabled( mAutoArchiveRB->isChecked() );
  mExpiryUnitsComboBox->setEnabled( mAutoArchiveRB->isChecked() );
}

// Archive old events
void ArchiveDialog::slotUser1()
{
  EventArchiver archiver;
  connect( &archiver, SIGNAL( eventsDeleted() ), this, SLOT( slotEventsDeleted() ) );

  KOPrefs::instance()->mAutoArchive = mAutoArchiveRB->isChecked();
  KOPrefs::instance()->mExpiryTime = mExpiryTimeNumInput->value();
  KOPrefs::instance()->mExpiryUnit = mExpiryUnitsComboBox->currentIndex();

  if (mDeleteCb->isChecked()) {
    KOPrefs::instance()->mArchiveAction = KOPrefs::actionDelete;
  } else {
    KOPrefs::instance()->mArchiveAction = KOPrefs::actionArchive;

    // Get destination URL
    KUrl destUrl( mArchiveFile->url() );
    if ( !destUrl.isValid() ) {
      KMessageBox::sorry(this,i18n("The archive file name is not valid.\n"));
      return;
    }
    // Force filename to be ending with vCalendar extension
    QString filename = destUrl.fileName();
    if (!filename.endsWith(".vcs") && !filename.endsWith(".ics")) {
      filename.append(".ics");
      destUrl.setFileName(filename);
    }

    KOPrefs::instance()->mArchiveFile = destUrl.url();
  }
  if ( KOPrefs::instance()->mAutoArchive ) {
    archiver.runAuto( mCalendar, this, true /*with gui*/ );
    emit autoArchivingSettingsModified();
    accept();
  }
  else
    archiver.runOnce( mCalendar, mDateEdit->date(), this );
}

void ArchiveDialog::slotEventsDeleted()
{
  emit eventsDeleted();
  if ( !KOPrefs::instance()->mAutoArchive )
    accept();
}
