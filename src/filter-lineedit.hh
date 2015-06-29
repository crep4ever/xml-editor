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

/**
 * \file filter-lineedit.hh
 *
 * Filter lineedit with embedded clear button.
 *
 */
#ifndef __FILTER_LINEEDIT_HH__
#define __FILTER_LINEEDIT_HH__

#include <QToolButton>
#include "utils/lineedit.hh"

/*!
  \file filter-lineedit.hh
  \class CClearButton
  \brief Button that resets the content of a CFilterLineEdit
  \image html clear-button.png
 */
class CClearButton : public QToolButton
{
    Q_OBJECT

public:
    /// Constructor.
    CClearButton(QWidget *parent = 0);

protected slots:
void textChanged(const QString &text);

protected:
void paintEvent(QPaintEvent *event);

private:
QImage m_icon;
};

/*!
  \file filter-lineedit.hh
  \class CMagButton
  \brief Magnify button in a CFilterLineEdit widget
  \image html mag-button.png
 */
class CMagButton : public QToolButton
{
    Q_OBJECT

public:
    /// Constructor.
    CMagButton(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage m_icon;
};

class QAction;

/*!
  \file filter-lineedit.hh
  \class CFilterLineEdit
  \brief A QLineEdit widget that allows one to filter results in a model
 */
class CFilterLineEdit : public LineEdit
{
    Q_OBJECT

public:
    /// Constructor.
    CFilterLineEdit(QWidget *parent = 0);
    /// Destructor.
    ~CFilterLineEdit();

    /// Add an action to the menu that is displayed
    /// when clicking on the CMagButton on the left.
    void addAction(QAction *action);

public slots:
    void filterModifiedValues();

private:
    QMenu* m_menu;
};

#endif // __FILTER_LINEEDIT_HH__
