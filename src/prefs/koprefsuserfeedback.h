/*
  This file is part of KOrganizer.

  SPDX-FileCopyrightText: 2000, 2001, 2002, 2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once
#include "config-korganizer.h"
#include <KCModule>

#if KORGANIZER_WITH_KUSERFEEDBACK
namespace KUserFeedback
{
class FeedbackConfigWidget;
}
class KOPrefsUserFeedBack : public KCModule
{
public:
    explicit KOPrefsUserFeedBack(QObject *parent, const KPluginMetaData &data);

protected:
    void load() override;
    void save() override;

private:
    KUserFeedback::FeedbackConfigWidget *const mUserFeedbackWidget;
};
#endif
