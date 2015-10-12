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
#include "keys-view.hh"

#include <QLabel>
#include <QBoxLayout>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QPushButton>

#include "table-view.hh"
#include "conf-model.hh"
#include "filter-lineedit.hh"

CKeysView::CKeysView(QWidget *parent)
: QWidget(parent)
, m_view(new CTableView)
, m_filterLineEdit(new CFilterLineEdit)
, m_revertChangesButton(0)
{
    connect(m_filterLineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(parameterFilterChanged(const QString &)));

    m_revertChangesButton = new QPushButton(tr("Revert changes"));
    m_revertChangesButton->setIcon(QIcon::fromTheme("document-revert", QIcon(":/icons/tango/src/document-revert.svg")));
    m_revertChangesButton->setEnabled(false);

    QLayout *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(m_filterLineEdit);
    headerLayout->addWidget(m_revertChangesButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(headerLayout);
    layout->addWidget(m_view);
    setLayout(layout);
}

CKeysView::~CKeysView()
{
}

void CKeysView::reset()
{
}

void CKeysView::setModel(QSortFilterProxyModel *model)
{
    m_view->setProxyModel(model);

    connect(m_revertChangesButton, SIGNAL(clicked()),
            model->sourceModel(), SLOT(revert()));

    connect(model->sourceModel(), SIGNAL(editedValueCountChanged(int)),
            this, SLOT(updateRevertChangesLabel(int)));
}

void CKeysView::updateRevertChangesLabel(int count)
{
    if (count == 0)
    {
        m_revertChangesButton->setText(tr("Revert changes"));
        m_revertChangesButton->setEnabled(false);
    }
    else
    {
        m_revertChangesButton->setText(tr("Revert changes (%1)").arg(count));
        m_revertChangesButton->setEnabled(true);
    }
}

void CKeysView::setFocus()
{
    m_filterLineEdit->setFocus();
}

void CKeysView::resizeColumns()
{
    m_view->resizeColumns();
}

