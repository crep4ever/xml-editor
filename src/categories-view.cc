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
#include "categories-view.hh"

#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QDebug>
#include <QToolButton>
#include <QScrollArea>

#include "conf-model.hh"

#define ITEMS_PER_ROW 4

CCategoriesView::CCategoriesView(QWidget *parent)
  : QWidget(parent)
  , m_categories()
  , m_subcategories()
  , m_categoriesLayout(new QGridLayout)
  , m_subcategoriesLayout(new QGridLayout)
{
  // Categories
  QLabel *catTitle = new QLabel(tr("Categories"));
  QFont titleFont("Arial", 16, QFont::Bold);
  catTitle->setFont(titleFont);

  QVBoxLayout *categoriesWrapperLayout = new QVBoxLayout;
  categoriesWrapperLayout->addWidget(catTitle);
  categoriesWrapperLayout->addLayout(m_categoriesLayout);
  categoriesWrapperLayout->addStretch();

  QWidget *categoriesWidget = new QWidget;
  categoriesWidget->setLayout(categoriesWrapperLayout);

  // SubCategories
  QLabel *subcatTitle = new QLabel(tr("SubCategories"));
  subcatTitle->setFont(titleFont);

  QVBoxLayout *subcategoriesWrapperLayout = new QVBoxLayout;
  subcategoriesWrapperLayout->addWidget(subcatTitle);
  subcategoriesWrapperLayout->addLayout(m_subcategoriesLayout);
  subcategoriesWrapperLayout->addStretch();

  QWidget *subcategoriesWidget = new QWidget;
  subcategoriesWidget->setLayout(subcategoriesWrapperLayout);

  // Main layout
  QSplitter *splitter = new QSplitter;
  splitter->setOrientation(Qt::Vertical);
  splitter->addWidget(categoriesWidget);
  splitter->addWidget(subcategoriesWidget);
  splitter->setStretchFactor(1, 10);

  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(splitter);

  QLayout *layout = new QHBoxLayout;
  layout->addWidget(scrollArea);

  setLayout(layout);
}

CCategoriesView::~CCategoriesView()
{
}

void CCategoriesView::reset()
{
  m_categories.clear();
  m_subcategories.clear();

  resetCategories();
  resetSubCategories();
}

void CCategoriesView::resetCategories()
{
  // remove items without destroying containers
  for (int i = 0; i < m_categoriesLayout->rowCount(); ++i)
    for (int j = 0; j < m_categoriesLayout->columnCount(); ++j)
      {
	QLayoutItem * item = m_categoriesLayout->itemAtPosition(i, j);
	if (item)
	  {
	    delete item->widget();
	  }
      }
}

void CCategoriesView::resetSubCategories()
{
  // remove items without destroying containers
  for (int i = 0; i < m_subcategoriesLayout->rowCount(); ++i)
    for (int j = 0; j < m_subcategoriesLayout->columnCount(); ++j)
      {
	QLayoutItem * item = m_subcategoriesLayout->itemAtPosition(i, j);
	if (item)
	  {
	    delete item->widget();
	  }
      }
}

QIcon CCategoriesView::iconFromCategory(const QString & category)
{
  if (category.contains("Process"))
    {
      return QIcon(":/icons/conf-editor/src/process.svg");
    }
  else if (category.contains("Display"))
    {
      return QIcon(":/icons/conf-editor/src/display.svg");
    }
  else if (category.contains("HardwareInterface"))
    {
      return QIcon(":/icons/conf-editor/src/hardware-interface.svg");
    }
  else if (category.contains("Machine"))
    {
      return QIcon(":/icons/conf-editor/src/machine.svg");
    }
  else if (category.contains("Vision"))
    {
      return QIcon(":/icons/conf-editor/src/vision.svg");
    }
  else if (category.contains("Algo"))
    {
      return QIcon(":/icons/conf-editor/src/algo.svg");
    }
  else if (category.contains("Log"))
    {
      return QIcon(":/icons/conf-editor/src/logs.svg");
    }
  else if (category.contains("Benchmark"))
    {
      return QIcon(":/icons/conf-editor/src/benchmarks.svg");
    }
  else if (category.contains("Cheating"))
    {
      return QIcon(":/icons/conf-editor/src/warning.svg");
    }
  else if (category.contains("Performance"))
    {
      return QIcon(":/icons/conf-editor/src/metrics.svg");
    }
  else if (category.contains("System"))
    {
      return QIcon(":/icons/conf-editor/src/system.svg");
    }

  return QIcon(":/icons/conf-editor/src/tools.svg");
}

void CCategoriesView::setModel(CConfModel *model)
{
  reset();

  for (int i = 0; i < model->rowCount(); ++i)
    {
      const QString cat = model->data(model->index(i, 0)).toString();
      const QString subcat = model->data(model->index(i, 1)).toString();

      m_categories.insert(cat);
      m_subcategories.insert(cat, subcat);
    }

  foreach (const QString & cat, m_categories)
    {
      QToolButton *catButton = new QToolButton;
      catButton->setMinimumSize(QSize(140, 120));
      catButton->setText(cat);
      catButton->setToolTip(cat);
      catButton->setIcon(iconFromCategory(cat));
      catButton->setIconSize(QSize(80, 80));
      catButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
      m_categoriesLayout->addWidget(catButton,
				    m_categoriesLayout->count() / ITEMS_PER_ROW,
				    m_categoriesLayout->count() % ITEMS_PER_ROW);
      connect(catButton, SIGNAL(clicked()), this, SLOT(listSubCategories()));
    }
}

void CCategoriesView::listSubCategories()
{
  QToolButton *categoryButton = qobject_cast< QToolButton* >(QObject::sender());
  if (!categoryButton)
    return;

  resetSubCategories();

  QString category = categoryButton->text();
  emit(categoryFilterChanged(category));

  QStringList subcategories(m_subcategories.values(category));
  subcategories.removeDuplicates();

  for (int i = 0; i < subcategories.size(); ++i)
    {
      const QString subcat = subcategories[i];

      QToolButton *subcatButton = new QToolButton;
      subcatButton->setMinimumSize(QSize(140, 120));
      subcatButton->setText(subcat);
      subcatButton->setToolTip(subcat);
      subcatButton->setIcon(categoryButton->icon());
      subcatButton->setIconSize(QSize(80, 80));
      subcatButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
      m_subcategoriesLayout->addWidget(subcatButton,
				       m_subcategoriesLayout->count() / ITEMS_PER_ROW,
				       m_subcategoriesLayout->count() % ITEMS_PER_ROW);
      connect(subcatButton, SIGNAL(clicked()), this, SLOT(filterSubCategories()));
    }
}

void CCategoriesView::filterSubCategories()
{
  QToolButton *button = qobject_cast< QToolButton* >(QObject::sender());
  if (!button)
    return;

  emit(subcategoryFilterChanged(button->text()));
}
