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
#include "table-view.hh"

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QKeyEvent>
#include <QShortcut>
#include <QAction>
#include <QDebug>

#include "conf-model.hh"


CTableView::CTableView(QWidget *parent) : QTableView(parent)
{
    setShowGrid(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::SelectedClicked |
                    QAbstractItemView::EditKeyPressed |
                    QAbstractItemView::DoubleClicked);
    setSortingEnabled(true);
    verticalHeader()->setVisible(false);

    // Context menu
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *action = new QAction(tr("&Adjust columns"), this);
    connect(action, SIGNAL(triggered()),
            this, SLOT(resizeColumnsToContents()));
    addAction(action);

    action = new QAction(tr("Revert to &initial value"), this);
    connect(action, SIGNAL(triggered()),
            this, SLOT(revertToOriginalValue()));
    addAction(action);

    action = new QAction(tr("Revert to &default value"), this);
    connect(action, SIGNAL(triggered()),
            this, SLOT(revertToDefaultValue()));
    addAction(action);

    // Additional shortcuts
    QShortcut *s = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(s, SIGNAL(activated()), SLOT(enterKeyPressed()), Qt::QueuedConnection);

    s = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(s, SIGNAL(activated()), SLOT(enterKeyPressed()), Qt::QueuedConnection);

    s = new QShortcut(QKeySequence(Qt::Key_I), this);
    connect(s, SIGNAL(activated()), SLOT(revertToOriginalValue()));

    s = new QShortcut(QKeySequence(Qt::Key_D), this);
    connect(s, SIGNAL(activated()), SLOT(revertToDefaultValue()));

}

CTableView::~CTableView()
{
}

void CTableView::setProxyModel(QSortFilterProxyModel *model)
{
    setModel(model);

    setColumnHidden(0, true);  // hide category
    setColumnHidden(1, true);  // hide subcategory
    setColumnHidden(2, false); // show parameter name
    setColumnHidden(3, false); // show parameter value
    setColumnHidden(4, true);  // hide parameter initial value
    setColumnHidden(5, false); // show parameter default value
}

QSortFilterProxyModel* CTableView::proxyModel()
{
    QSortFilterProxyModel *proxy = qobject_cast<QSortFilterProxyModel*>(model());
    Q_ASSERT(proxy);
    return proxy;
}

CConfModel* CTableView::sourceModel()
{
    Q_ASSERT(proxyModel());
    CConfModel* source = qobject_cast<CConfModel*>(proxyModel()->sourceModel());
    Q_ASSERT(source);
    return source;
}

void CTableView::revertToOriginalValue()
{
    const QModelIndex & current = selectionModel()->currentIndex();
    sourceModel()->revertToOriginalValue(proxyModel()->mapToSource(current));
}

void CTableView::revertToDefaultValue()
{
    const QModelIndex & current = selectionModel()->currentIndex();
    sourceModel()->revertToDefaultValue(proxyModel()->mapToSource(current));
}


void CTableView::resizeColumns()
{
    setColumnWidth(2, 450);
    setColumnWidth(3, 150);
    setColumnWidth(4, 150);
    horizontalHeader()->setStretchLastSection(true);
}

bool CTableView::focusNextPrevChild(bool p_next)
{
    if (p_next)
    {
        const QModelIndex next = proxyModel()->index(currentIndex().row() + 1, 3);
        setCurrentIndex(next);
    }
    else
    {
        const QModelIndex prev = proxyModel()->index(currentIndex().row() - 1, 3);
        setCurrentIndex(prev);
    }

    setState(QAbstractItemView::NoState);
    setFocus(Qt::ShortcutFocusReason);

    return true;
}

void CTableView::enterKeyPressed()
{
    if (state() != QAbstractItemView::EditingState)
    {
        edit(currentIndex());
        setState(QAbstractItemView::EditingState);
    }
    else
    {
        const QModelIndex next = proxyModel()->index(currentIndex().row() + 1, 3);
        setCurrentIndex(next);
        setFocus(Qt::ShortcutFocusReason);
    }
}

