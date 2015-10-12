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

#ifndef __CATEGORIES_VIEW_HH__
#define __CATEGORIES_VIEW_HH__

#include <QWidget>
#include <QString>
#include <QIcon>
#include <QMultiMap>
#include <QSet>

class CConfModel;
class QGridLayout;

/*!
  \file categories-view.hh
  \class CCategoriesView
  \brief Lists categories and subcategories of a CConfModel

  This view contains two sections:
  \li categories
  \li subcategories

  Clicking on a \a category item lists its subcategories.
  When items are clicked, the corresponding model is filtered
  to only display matching parameters.

  \image html categories.png
 */
class CCategoriesView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    CCategoriesView(QWidget *parent = 0);

    /// Destructor.
    ~CCategoriesView();

    void setModel(CConfModel *model);

    void reset();

private slots:
void listSubCategories();
void filterSubCategories();
void resetCategories();
void resetSubCategories();


signals:
void categoryFilterChanged(const QString &);
void subcategoryFilterChanged(const QString &);

private:
QIcon iconFromCategory(const QString & category);

QSet<QString> m_categories;
QMultiMap<QString, QString> m_subcategories;

QGridLayout *m_categoriesLayout;
QGridLayout *m_subcategoriesLayout;
};

#endif  // __CATEGORIES_VIEW_HH__
