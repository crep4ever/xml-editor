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
#include "main-window.hh"

#include <QApplication>
#include <QAction>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>
#include <QStringList>
#include <QSplitter>
#include <QLabel>
#include <QStackedWidget>
#include <QDebug>

#include "preferences.hh"
#include "home.hh"
#include "conf-model.hh"
#include "categories-view.hh"
#include "keys-view.hh"
#include "conf-proxy-model.hh"
#include "config.hh"

CMainWindow::CMainWindow(QWidget *parent)
: QMainWindow(parent)
  , m_mainWidget(new QStackedWidget(this))
  , m_mainToolBar(0)
  , m_model(0)
  , m_proxy(0)
, m_categoriesView(0)
  , m_keysView(0)
, m_isToolBarDisplayed(true)
  , m_isStatusBarDisplayed(true)
, m_openPath(QDir::homePath())
  , m_savePath(QDir::homePath())
{
  setWindowTitle(QApplication::applicationName());
  setWindowIcon(QIcon(":/icons/xml-editor/src/xml-editor.svg"));

  createActions();
  createMenus();
  createToolBar();

  // place elements into the main window

  QSplitter *splitter = new QSplitter;
  m_categoriesView = new CCategoriesView;
  m_keysView = new CKeysView;
  splitter->addWidget(m_categoriesView);
  splitter->addWidget(m_keysView);
  splitter->setStretchFactor(0, 10);
  splitter->setStretchFactor(1, 6);

  CHome *home = new CHome(this);
  connect(home, SIGNAL(requestEditFile(const QString &)),
	  this, SLOT(open(const QString &)));

  m_mainWidget->addWidget(home);
  m_mainWidget->addWidget(splitter);

  setCentralWidget(m_mainWidget);

  readSettings(true);
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::readSettings(bool firstLaunch)
{
  QSettings settings;
  settings.beginGroup("general");
  if (firstLaunch)
    {
      resize(settings.value("size", QSize(800,600)).toSize());
      move(settings.value("pos", QPoint(200, 200)).toPoint());
      if (settings.value("maximized", isMaximized()).toBool())
	showMaximized();
    }
  m_openPath = settings.value("openPath", QDir::homePath()).toString();
  m_savePath = settings.value("savePath", QDir::homePath()).toString();
  settings.endGroup();

  settings.beginGroup("display");
  setStatusBarDisplayed(settings.value("statusBar", true).toBool());
  setToolBarDisplayed(settings.value("toolBar", true).toBool());
  settings.endGroup();

  settings.beginGroup("home");
  m_recentPaths = settings.value("recent-paths", QStringList()).toStringList();
  settings.endGroup();
}

void CMainWindow::writeSettings()
{
  QSettings settings;
  settings.beginGroup("general");
  settings.setValue("maximized", isMaximized());
  if (!isMaximized())
    {
      settings.setValue("pos", pos());
      settings.setValue("size", size());
    }
  settings.setValue("openPath", m_openPath);
  settings.setValue("savePath", m_savePath);
  settings.endGroup();

  settings.beginGroup("home");
  QStringList recentFiles;
  const int max = settings.value("max-recent-files", 5).toInt();
  for (int i = 0; i < max; ++i)
    {
      if (i >= m_recentPaths.count())
	break;

      recentFiles << m_recentPaths[i];
    }

  settings.setValue("recent-paths", recentFiles);
  settings.endGroup();
}

void CMainWindow::createActions()
{
  m_openAct = new QAction(tr("&Open..."), this);
  m_openAct->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/tango/src/document-open.svg")));
  m_openAct->setShortcut(QKeySequence::Open);
  m_openAct->setStatusTip(tr("Open a data file"));
  connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

  m_saveAct = new QAction(tr("&Save"), this);
  m_saveAct->setShortcut(QKeySequence::Save);
  m_saveAct->setEnabled(false);
  m_saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/tango/src/document-save.svg")));
  m_saveAct->setStatusTip(tr("Save the current data file"));
  connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  m_saveAsAct = new QAction(tr("Save &As..."), this);
  m_saveAsAct->setShortcut(QKeySequence::SaveAs);
  m_saveAsAct->setEnabled(false);
  m_saveAsAct->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/src/document-save-as.svg")));
  m_saveAsAct->setStatusTip(tr("Save the current data file with a different name"));
  connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  m_homeAct = new QAction(tr("&Home"), this);
  m_homeAct->setShortcut(QKeySequence::SaveAs);
  m_homeAct->setIcon(QIcon::fromTheme("go-home", QIcon(":/icons/tango/src/go-home.svg")));
  m_homeAct->setStatusTip(tr("Return to welcome screen"));
  connect(m_homeAct, SIGNAL(triggered()), this, SLOT(home()));

  m_aboutAct = new QAction(tr("&About"), this);
  m_aboutAct->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/tango/src/help-about.svg")));
  m_aboutAct->setStatusTip(tr("About this application"));
  m_aboutAct->setMenuRole(QAction::AboutRole);
  connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  m_exitAct = new QAction(tr("&Quit"), this);
  m_exitAct->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/tango/src/application-exit.svg")));
  m_exitAct->setShortcut(QKeySequence::Quit);
  m_exitAct->setStatusTip(tr("Quit the program"));
  m_exitAct->setMenuRole(QAction::QuitRole);
  connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  m_preferencesAct = new QAction(tr("&Preferences"), this);
  m_preferencesAct->setIcon(QIcon::fromTheme("document-properties", QIcon(":/icons/tango/src/document-properties.svg")));
  m_preferencesAct->setStatusTip(tr("Configure the application"));
  m_exitAct->setShortcut(QKeySequence::Preferences);
  m_preferencesAct->setMenuRole(QAction::PreferencesRole);
  connect(m_preferencesAct, SIGNAL(triggered()), SLOT(preferences()));
}

void CMainWindow::setToolBarDisplayed(bool value)
{
  m_mainToolBar->setVisible(value);
}

bool CMainWindow::isToolBarDisplayed()
{
  return m_isToolBarDisplayed;
}

void CMainWindow::setStatusBarDisplayed(bool value)
{
  m_isStatusBarDisplayed = value;
  statusBar()->setVisible(value);
}

bool CMainWindow::isStatusBarDisplayed()
{
  return m_isStatusBarDisplayed;
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
  if (isWindowModified())
    {
      if (QMessageBox::question(this, windowTitle(), tr("All your changes to %1 will be lost. "
							"Do you want to save your changes?").arg(m_model->filename()),
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton) == QMessageBox::Yes)
	save();

    }

  writeSettings();
  event->accept();
}

void CMainWindow::createMenus()
{
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);

  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(m_openAct);
  fileMenu->addAction(m_saveAct);
  fileMenu->addAction(m_saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_preferencesAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_exitAct);

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(m_aboutAct);
}

void CMainWindow::createToolBar()
{
  m_mainToolBar = new QToolBar(tr("Main"), this);
  m_mainToolBar->setMovable(false);
  m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_mainToolBar->addAction(m_homeAct);
  m_mainToolBar->addSeparator();
  m_mainToolBar->addAction(m_openAct);
  m_mainToolBar->addAction(m_saveAct);
  m_mainToolBar->addAction(m_saveAsAct);

  addToolBar(m_mainToolBar);

  setUnifiedTitleAndToolBarOnMac(true);
}

void CMainWindow::home()
{
  if (isWindowModified())
    {
      if (QMessageBox::question(this, windowTitle(), tr("All your changes to %1 will be lost. "
							"Do you want to save your changes?").arg(m_model->filename()),
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton) == QMessageBox::Yes)
	save();

    }

  m_mainWidget->setCurrentIndex(0);
  setModified(0);
  m_saveAct->setEnabled(false);
  m_saveAsAct->setEnabled(false);
}

void CMainWindow::preferences()
{
  CConfigDialog dialog(this);
  dialog.exec();
  readSettings();
}

void CMainWindow::about()
{
  QString title(tr("About XML Editor"));
  QString version = QCoreApplication::applicationVersion();

  QString description(tr("This program allows to edit xml configuration files"));

  QStringList authorsList = QStringList() << "R. Goffe";
  QString authors = authorsList.join(", ");

  QMessageBox::about(this, title, tr("<p>%1</p>"
				     "<p><b>Version:</b> %2</p>"
				     "<p><b>Authors:</b> %3</p>")
		     .arg(description).arg(version).arg(authors));
}


void CMainWindow::open(const QString & filename)
{
  QFileInfo fi(filename);
  m_openPath = fi.absolutePath();

  if (!m_recentPaths.contains(filename))
    m_recentPaths.insert(m_recentPaths.begin(), filename);

  setWindowTitle(QString("%1 (%2) [*] - %3")
		 .arg(fi.fileName())
		 .arg(fi.canonicalPath())
		 .arg(QApplication::applicationName()));

  if (m_model)
    {
      delete m_model;
      delete m_proxy;
    }

  m_model = new CConfModel(filename);
  m_proxy = new CConfProxyModel(this);
  m_proxy->setSourceModel(m_model);
  m_proxy->setSortLocaleAware(true);
  m_proxy->setDynamicSortFilter(true);
  m_proxy->setFilterKeyColumn(-1);
  m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

  m_categoriesView->setModel(m_model);
  m_keysView->setModel(m_proxy);
  m_keysView->resizeColumns();

  connect(m_model, SIGNAL(editedValueCountChanged(int)),
	  this, SLOT(setModified(int)));

  connect(m_categoriesView, SIGNAL(categoryFilterChanged(const QString &)),
	  m_proxy, SLOT(setFilterWildcard(const QString &)));

  connect(m_categoriesView, SIGNAL(subcategoryFilterChanged(const QString &)),
	  m_proxy, SLOT(setFilterWildcard(const QString &)));

  connect(m_keysView, SIGNAL(parameterFilterChanged(const QString &)),
	  m_proxy, SLOT(setFilterWildcard(const QString &)));

  m_mainWidget->setCurrentIndex(1);

  m_saveAsAct->setEnabled(true);


  statusBar()->showMessage(filename);
  writeSettings(); // updates openPath setting

  m_keysView->setFocus();
}

void CMainWindow::open()
{
  QString filename = QFileDialog::getOpenFileName(this,
						  tr("Open xml file"),
						  m_openPath,
						  tr("Data files (*.xml)"));
  if (!filename.isEmpty())
    open(filename);
}

void CMainWindow::save()
{
  statusBar()->showMessage(tr("Save: %1").arg(m_model->filename()));
  m_model->save();
}

void CMainWindow::saveAs()
{
  QString filename = QFileDialog::getSaveFileName(this,
                                                  tr("Save file"),
                                                  m_savePath,
                                                  tr("XML files (*.xml)"));
  if (!filename.isEmpty())
    {
      QFileInfo fi(filename);
      m_savePath = fi.absolutePath();
      writeSettings(); //update savePath setting
      m_model->setFilename(filename);

      setWindowTitle(QString("%1 (%2) [*] - %3")
		     .arg(fi.fileName())
		     .arg(fi.absolutePath())
		     .arg(QApplication::applicationName()));
      save();
    }
}

void CMainWindow::setModified(int changesCount)
{
  setWindowModified(changesCount > 0);
  m_saveAct->setEnabled(changesCount > 0);
}
