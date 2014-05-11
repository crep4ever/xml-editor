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

#ifndef __MAIN_WINDOW_HH__
#define __MAIN_WINDOW_HH__

#include <QMainWindow>

class QToolBar;
class QStackedWidget;
class QSortFilterProxyModel;
class CConfModel;
class CCategoriesView;
class CKeysView;

/*!
  \file main-window.hh
  \class CMainWindow
  \brief Base class of the application.

  Class for the main window of the application.
*/
class CMainWindow : public QMainWindow
{
  Q_OBJECT

public slots:
  void open(const QString &filename);

public:
  /// Constructor.
  CMainWindow(QWidget *parent=0);

  /// Destructor.
  ~CMainWindow();

protected:
  /*!
    Saves settings before closing the application.
  */
  void closeEvent(QCloseEvent *event);

private slots:
  void open();
  void save();
  void saveAs();
  void setModified(int);
  // application
  void home();
  void preferences();
  void setToolBarDisplayed(bool);
  void setStatusBarDisplayed(bool);
  void about();

private:
  void readSettings(bool firstLaunch = false);
  void writeSettings();

  void createActions();
  void createMenus();
  void createToolBar();

  bool isToolBarDisplayed();
  bool isStatusBarDisplayed();

  // Widgets
  QStackedWidget *m_mainWidget;
  QToolBar *m_mainToolBar;
  CConfModel *m_model;
  QSortFilterProxyModel *m_proxy;
  CCategoriesView *m_categoriesView;
  CKeysView *m_keysView;

  // Settings
  bool m_isToolBarDisplayed;
  bool m_isStatusBarDisplayed;

  // Application actions
  QAction *m_preferencesAct;
  QAction *m_aboutAct;
  QAction *m_exitAct;

  QAction *m_openAct;
  QAction *m_saveAct;
  QAction *m_saveAsAct;
  QAction *m_homeAct;

  // Settings
  QString m_openPath;
  QString m_savePath;
  QStringList m_recentPaths;
};

#endif  // __MAIN_WINDOW_HH__
