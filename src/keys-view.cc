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

#include <QObject>
#include <QLabel>
#include <QBoxLayout>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QDebug>

#include "table-view.hh"
#include "conf-model.hh"
#include "filter-lineedit.hh"

CKeysView::CKeysView(QWidget *parent)
: QWidget(parent)
, m_view(new CTableView)
, m_filterLineEdit(new CFilterLineEdit)
, m_revertChangesButton(0)
, m_currentSelectionInfo(new QLabel)
{

    connect(m_filterLineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(parameterFilterChanged(const QString &)));

    m_revertChangesButton = new QPushButton(tr("Revert changes"));
    m_revertChangesButton->setIcon(QIcon::fromTheme("document-revert", QIcon(":/icons/tango/src/document-revert.svg")));
    m_revertChangesButton->setEnabled(false);

    QLayout *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(m_filterLineEdit);
    headerLayout->addWidget(m_revertChangesButton);

    // Current selection info

    m_currentSelectionInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);

    connect(m_view, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(updateSelectionInfo(const QModelIndex &)));

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(headerLayout);
    layout->addWidget(m_view);
    layout->addWidget(m_currentSelectionInfo);
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

CTableView* CKeysView::tableView() const
{
    return m_view;
}

CConfModel* CKeysView::sourceModel() const
{
    return m_view->sourceModel();
}

QSortFilterProxyModel* CKeysView::proxyModel() const
{
    return m_view->proxyModel();
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

void CKeysView::updateSelectionInfo(const QModelIndex & p_index)
{
    if (!p_index.isValid())
    {
        return;
    }

    const QModelIndex & idx = proxyModel()->mapToSource(p_index);

    const QString cat    = sourceModel()->data(idx, CConfModel::CategoryRole).toString();
    const QString subcat = sourceModel()->data(idx, CConfModel::SubCategoryRole).toString();
    const QString param  = sourceModel()->data(idx, CConfModel::ParameterRole).toString();
    const QString curVal = sourceModel()->data(idx, CConfModel::ValueRole).toString();
    const QString iniVal = sourceModel()->data(idx, CConfModel::InitialValueRole).toString();
    const QString defVal = sourceModel()->data(idx, CConfModel::DefaultValueRole).toString();

    const QString infoParameter = tr("<b>Parameter:</b> %1 / %2 / %3").arg(cat).arg(subcat).arg(param);
    const QString infoValues    = tr("<b>Values:</b> %1 (current) %2 (initial) %3 (default)").arg(curVal).arg(iniVal).arg(defVal);
    const QString info = QString("%1 <br/> %2").arg(infoParameter).arg(infoValues);

    m_currentSelectionInfo->setText(info);
}


