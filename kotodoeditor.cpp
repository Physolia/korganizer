/*
    $Id$

     Requires the Qt and KDE widget libraries, available at no cost at
     http://www.troll.no and http://www.kde.org respectively

     Copyright (C) 1997, 1998 Preston Brown
     preston.brown@yale.edu

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

     -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

     This file implements a class for displaying a dialog box for
     adding or editing appointments/events.
*/

#include <stdio.h>

#include <qtooltip.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>

#include <kdatepik.h>
#include <kiconloader.h>
#include <kapp.h>
#include <klocale.h>
#include <ktoolbarbutton.h>
#include <kstddirs.h>

#include <catdlg.h>

#include "kotodoeditor.h"
#include "kotodoeditor.moc"

KOTodoEditor::KOTodoEditor(CalObject *calendar) :
  KDialogBase(Tabbed,i18n("Edit Event"),Ok|Apply|Cancel|Default|User1,Ok,0,0,
              false,false,"Delete")
{
  mCalendar = calendar;
  mTodo = 0;
  mRelatedTodo = 0;

  mCategoryDialog = new CategoryDialog();

  setupGeneralTab();
  setupDetailsTab();

  connect(mGeneral,SIGNAL(openCategoryDialog()),mCategoryDialog,SLOT(show()));
  connect(mDetails,SIGNAL(openCategoryDialog()),mCategoryDialog,SLOT(show()));
  connect(mCategoryDialog, SIGNAL(okClicked(QString)),
          mGeneral,SLOT(setCategories(QString)));
  connect(mCategoryDialog, SIGNAL(okClicked(QString)),
          mDetails,SLOT(setCategories(QString)));

  connect(this,SIGNAL(cancelClicked()),SLOT(reject()));
}

KOTodoEditor::~KOTodoEditor()
{
}

void KOTodoEditor::setupGeneralTab()
{
  QFrame *topFrame = addPage(i18n("General"));

  QBoxLayout *topLayout = new QVBoxLayout(topFrame);  
  topLayout->setMargin(marginHint());

  mGeneral = new KOEditorGeneralTodo(spacingHint(),topFrame);
  topLayout->addWidget(mGeneral);
}

void KOTodoEditor::setupDetailsTab()
{
  QFrame *topFrame = addPage(i18n("Details"));

  QBoxLayout *topLayout = new QVBoxLayout(topFrame);  
  topLayout->setMargin(marginHint());

  mDetails = new KOEditorDetails(spacingHint(),topFrame);
  topLayout->addWidget(mDetails);
}

void KOTodoEditor::editTodo( KOEvent *todo, QDate)
{
  mTodo = todo;
  readTodo(mTodo);
}

void KOTodoEditor::newTodo(QDateTime due,KOEvent *relatedTodo,bool allDay)
{
  mTodo = 0;
  setDefaults(due,relatedTodo,allDay);

  enableButton(User1,false);
}

void KOTodoEditor::slotDefault()
{
  setDefaults(QDateTime::currentDateTime().addDays(7),0,false);
}

void KOTodoEditor::slotApply()
{
  KOEvent *todo = 0;

  if (mTodo) todo = mTodo;
  else todo = new KOEvent;
  
  writeTodo(todo);
  
  if (mTodo) {
    todo->setRevisionNum(todo->getRevisionNum()+1);
    emit todoChanged(todo);
  } else {
    mCalendar->addTodo(todo);
    mTodo = todo;
    emit todoAdded(todo);
  }
}

void KOTodoEditor::slotOk()
{
  slotApply();
  accept();
}

void KOTodoEditor::slotUser1()
{
  if (mTodo) {
    emit todoToBeDeleted(mTodo);
    mCalendar->deleteTodo(mTodo);
    emit todoDeleted();
    reject();
  } else {
    reject();
  }
}

void KOTodoEditor::setDefaults(QDateTime due,KOEvent *relatedEvent,bool allDay)
{
  mRelatedTodo = relatedEvent;
  
  mGeneral->setDefaults(due,allDay);
  mDetails->setDefaults();
}

void KOTodoEditor::readTodo(KOEvent *event)
{
  mGeneral->readTodo(event);
  mDetails->readEvent(event);

  // We should handle read-only events here.
}

void KOTodoEditor::writeTodo(KOEvent *event)
{
  mGeneral->writeTodo(event);
  mDetails->writeEvent(event);

  // set related event, i.e. parent to-do in this case.
  if (mRelatedTodo) {
    event->setRelatedTo(mRelatedTodo);
  }
}
