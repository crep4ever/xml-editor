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
#include "home.hh"

#include <QDebug>
#include <QLabel>
#include <QBoxLayout>
#include <QDir>
#include <QPushButton>
#include <QToolButton>
#include <QSignalMapper>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QSettings>

CHome::CHome(QWidget *parent)
  : QWidget(parent)
{
  QStringList defaultFolderList;

  defaultFolderList <<  "/octopus/conf/";
  defaultFolderList <<  QString("%1/src/devMatrix/Delivery/linux/Debug/conf/").arg(QDir::homePath());
  defaultFolderList <<  QString("%1/src/devMatrix/Delivery/linux/RelWithDebInfo/conf/").arg(QDir::homePath());
  defaultFolderList <<  QString("%1/workspace/devMatrix/Delivery/linux/Debug/conf/").arg(QDir::homePath());
  defaultFolderList <<  QString("%1/workspace/devMatrix/Delivery/linux/RelWithDebInfo/conf/").arg(QDir::homePath());

  QSettings settings;
  settings.beginGroup("home");
  QStringList directories = settings.value("scan-paths", defaultFolderList).toStringList();
  QStringList recentFiles = settings.value("recent-paths", QStringList()).toStringList();
  settings.endGroup();

  QVBoxLayout *layout = new QVBoxLayout;

  QSplitter *splitter = new QSplitter;
  splitter->setOrientation(Qt::Vertical);

  QFont titleFont("Arial", 16, QFont::Bold);

  /*
   * Recent files
   */

  if (!recentFiles.isEmpty())
    {
      QWidget *recentFilesWidget = new QWidget;
      QLabel *recentFilesLabel = new QLabel(tr("Recent files"));
      recentFilesLabel->setFont(titleFont);

      QLabel *recentIcon = new QLabel;
      recentIcon->setToolTip(tr("List of recently opened files"));
      recentIcon->setPixmap(QIcon::fromTheme("document-open-recent", QIcon(":/icons/tango/src/document-open-recent.svg")).pixmap(QSize(48, 48)));

      QHBoxLayout *recentFilesTitleLayout = new QHBoxLayout;
      recentFilesTitleLayout->addWidget(recentIcon);
      recentFilesTitleLayout->addWidget(recentFilesLabel);
      recentFilesTitleLayout->addStretch();

      QHBoxLayout *recentFilesLayout = new QHBoxLayout;
      foreach (const QString & recentFile, recentFiles)
        {
          QFileInfo fi(recentFile);
          QToolButton *editButton = new QToolButton;
          editButton->setMinimumSize(QSize(100, 80));
          editButton->setToolTip(recentFile);
          editButton->setText(fi.fileName());
          editButton->setIcon(QIcon::fromTheme("accessories-text-editor", QIcon(":/icons/tango/src/accessories-text-editor.svg")));
          editButton->setIconSize(QSize(48, 48));
          editButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
          connect(editButton, SIGNAL(clicked()), this, SLOT(editFile()));

          recentFilesLayout->addWidget(editButton);
        }
      recentFilesLayout->addStretch();

      QVBoxLayout *recentMainLayout = new QVBoxLayout;
      recentMainLayout->addLayout(recentFilesTitleLayout);
      recentMainLayout->addLayout(recentFilesLayout);
      recentMainLayout->addStretch();
      recentFilesWidget->setLayout(recentMainLayout);

      splitter->addWidget(recentFilesWidget);
    }

  /*
   * Scan directories
   */

  QSignalMapper *collapseSignalMapper = new QSignalMapper(this);
  int count = 0;
  for (int i = 0; i < directories.count(); ++i)
    {
      const QString directory = directories[i];
      QStringList files = scanFiles(directory);

      if (files.isEmpty())
        continue;

      QWidget *splitterWidget = new QWidget(this);
      QVBoxLayout *splitterLayout = new QVBoxLayout;

      // collapse button
      QPushButton *collapseButton = new QPushButton;
      m_collapseButtons.append(collapseButton);
      collapseButton->setFlat(true);
      collapseButton->setToolTip(tr("Collapse"));
      collapseButton->setIcon(QIcon::fromTheme("go-up", QIcon(":/icons/tango/src/go-up.svg")));
      connect(collapseButton, SIGNAL(clicked()), collapseSignalMapper, SLOT(map()));
      collapseSignalMapper->setMapping(collapseButton, count);

      // title
      QLabel *title = new QLabel(directory);
      title->setFont(titleFont);

      // put header items in its layout
      QBoxLayout *headerLayout = new QHBoxLayout;
      headerLayout->addWidget(collapseButton);
      headerLayout->addWidget(title);
      headerLayout->addStretch();

      QGroupBox *groupBox = new QGroupBox;
      QHBoxLayout *filesLayout = new QHBoxLayout;
      foreach (QString file, files)
        {
          QToolButton *editButton = new QToolButton;
          editButton->setMinimumSize(QSize(100, 80));
          editButton->setToolTip(QString("%1/%2").arg(directory).arg(file));
          editButton->setText(file);
          editButton->setIcon(QIcon::fromTheme("accessories-text-editor", QIcon(":/icons/tango/src/accessories-text-editor.svg")));
          editButton->setIconSize(QSize(48, 48));
          editButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
          connect(editButton, SIGNAL(clicked()), this, SLOT(editFile()));

          filesLayout->addWidget(editButton);
        }
      filesLayout->addStretch();
      groupBox->setLayout(filesLayout);
      m_boxes.append(groupBox);

      splitterLayout->addLayout(headerLayout);
      splitterLayout->addWidget(groupBox);
      splitterLayout->addStretch();
      splitterWidget->setLayout(splitterLayout);
      splitter->addWidget(splitterWidget);

      ++count;
    }
  splitter->setStretchFactor(splitter->count() - 1, 10);


  // Main layout
  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(splitter);
  layout->addWidget(scrollArea);

  setLayout(layout);

  connect(collapseSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(collapseViewClicked(int)));
  connect(this, SIGNAL(collapseViewClicked(int)), this, SLOT(collapseView(int)));
}

CHome::~CHome()
{
}

QStringList CHome::scanFiles(const QString & path)
{
  QStringList xmlFiles;

  QDir directory(path);
  QStringList files = directory.entryList();
  foreach (const QString & filePath, files)
    {
      if (filePath.endsWith(".xml"))
        {
          xmlFiles << filePath;
        }
    }

  return xmlFiles;
}

void CHome::collapseView(int i)
{
  bool setVisible = !m_boxes[i]->isVisible();

  if (setVisible)
    {
      m_collapseButtons[i]->setToolTip(tr("Collapse"));
      m_collapseButtons[i]->setIcon(QIcon::fromTheme("go-up", QIcon(":/icons/tango/src/go-up.svg")));
    }
  else
    {
      m_collapseButtons[i]->setToolTip(tr("Expand"));
      m_collapseButtons[i]->setIcon(QIcon::fromTheme("go-down", QIcon(":/icons/tango/src/go-down.svg")));
    }

  m_boxes[i]->setVisible(setVisible);
}

void CHome::editFile()
{
  QToolButton *editButton = qobject_cast< QToolButton* >(QObject::sender());
  if (!editButton)
    return;

  emit(requestEditFile(editButton->toolTip()));
}
