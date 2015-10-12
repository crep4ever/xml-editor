// Copyright (C) 2015, Romain Goffe <romain.goffe@gmail.com>
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

#ifndef __TABLE_VIEW_HH__
#define __TABLE_VIEW_HH__

#include <QTableView>

class QKeyEvent;
class QSortFilterProxyModel;
class CConfModel;

class CTableView : public QTableView
{
    Q_OBJECT

public:
    /// Constructor.
    CTableView(QWidget *parent = 0);

    /// Destructor.
    ~CTableView();

    void setProxyModel(QSortFilterProxyModel *model);
    QSortFilterProxyModel* proxyModel();

    CConfModel* sourceModel();



public slots:
void resizeColumns();

private slots:
void revertToOriginalValue();
void revertToDefaultValue();
void editActivated();


protected:
virtual bool focusNextPrevChild (bool next);
//virtual void keyPressEvent(QKeyEvent * p_event);

};

#endif  // __TABLE_VIEW_HH__
