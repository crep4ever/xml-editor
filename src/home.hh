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

#ifndef __HOME_HH__
#define __HOME_HH__

#include <QWidget>

#include <QStringList>
#include <QList>

class QPushButton;
class QGroupBox;

/*!
  \file home.hh
  \class CHome
  \brief Home screen of the application.
  
  Displays recently opened files and
  xml files found in specified directories.
  Those directories can be defined in the preferences
  menu.

  \image html home.png
*/
class CHome : public QWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CHome(QWidget *parent);

  /// Destructor.
  ~CHome();

  /*!
    Look for xml files within \a directory
  */
  QStringList scanFiles(const QString & directory);

signals:
  /*!
    This signal is emitted when the user requests
    files under a directory section to be hidden / displayed.
  */
  void collapseViewClicked(int i);

private slots:
  /*!
    Hide / display files under a directory section.
  */
  void collapseView(int);

  /*!
    Launch file edition.
    This slot guess the filename from its sender
    and emits the \a requestEditFile signal in return.
    \see requestEditFile
  */
  void editFile();

signals:
  /*!
    This signal is emitted when the user requests a file to be edited.
    \param path filename to be edited
  */
  void requestEditFile(const QString & path);

private:
  QList<QGroupBox*> m_boxes;
  QList<QPushButton*> m_collapseButtons;
};

#endif  // __HOME_HH__
