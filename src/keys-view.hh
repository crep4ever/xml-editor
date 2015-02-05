// Copyright (C) 2014, Romain Goffe <romain.goffe@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//******************************************************************************

#ifndef __KEYS_VIEW_HH__
#define __KEYS_VIEW_HH__

#include <QWidget>
#include <QString>

class QSortFilterProxyModel;
class QTableView;
class QPushButton;

/*!
  \file keys-view.hh
  \class CKeysView
  \brief Lists pairs of name/values of a CConfModel

  This view lists all parameters. Note that the category and
  subcategory are not displayed (see CCategoriesView).

  This view also contains a filtering search bar for quick search
  through the model.

  \image html keys.png
*/
class CKeysView : public QWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CKeysView(QWidget *parent = 0);

  /// Destructor.
  ~CKeysView();

  void setModel(QSortFilterProxyModel *model);

  void reset();

public slots:
  void resizeColumns();

signals:
  void parameterFilterChanged(const QString &);

private slots:
  void updateRevertChangesLabel(int count);

private:
  QTableView *m_view;
  QPushButton *m_revertChangesButton;
};

#endif  // __KEYS_VIEW_HH__
