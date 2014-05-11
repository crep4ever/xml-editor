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

#include "filter-lineedit.hh"

#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QDebug>

CClearButton::CClearButton(QWidget *parent)
  : QToolButton(parent)
{
  setCursor(Qt::ArrowCursor);
  setFocusPolicy(Qt::NoFocus);
  setToolTip(tr("Clear"));
  setMinimumSize(22, 22);
  setVisible(false);

  // First check for a style icon
  if (m_icon.isNull())
    {
      QIcon icon = QIcon::fromTheme("edit-clear", QIcon(":/icons/tango/src/edit-clear.svg"));
      if (!icon.isNull())
	m_icon = icon.pixmap(16, 16).toImage();
    }
}

void CClearButton::textChanged(const QString &text)
{
  setVisible(!text.isEmpty());
}

void CClearButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);

  if (!m_icon.isNull())
    {
      int x = (width() - m_icon.width()) / 2 - 1;
      int y = (height() - m_icon.height()) / 2 - 1;
      painter.drawImage(x, y, m_icon);
      return;
    }

  // Fall back to boring circle X
  painter.setRenderHint(QPainter::Antialiasing, true);

  QPalette p = palette();
  QColor circleColor = isDown() ? p.color(QPalette::Dark) : p.color(QPalette::Mid);
  QColor xColor = p.color(QPalette::Window);

  // draw circle
  painter.setBrush(circleColor);
  painter.setPen(circleColor);
  int padding = width() / 5;
  int circleRadius = width() - (padding * 2);
  painter.drawEllipse(padding, padding, circleRadius, circleRadius);

  // draw X
  painter.setPen(xColor);
  padding *= 2;
  painter.drawLine(padding, padding, width() - padding, width() - padding);
  painter.drawLine(padding, height() - padding, width() - padding, padding);
}

CMagButton::CMagButton(QWidget *parent)
  : QToolButton(parent)
{
  setCursor(Qt::ArrowCursor);
  setFocusPolicy(Qt::NoFocus);
  setToolTip(tr("Filter"));
  setMinimumSize(22, 22);
  setVisible(true);
  setPopupMode(QToolButton::InstantPopup);
}

void CMagButton::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);

  if (!m_icon.isNull())
    {
      int x = (width() - m_icon.width()) / 2 - 1;
      int y = (height() - m_icon.height()) / 2 - 1;
      painter.drawImage(x, y, m_icon);
      return;
    }

  // Fall back to boring circle X
  painter.setRenderHint(QPainter::Antialiasing, true);

  QPalette p = palette();

  QPen pen(QColor::fromRgb(85,85,85));
  pen.setWidth(2);

  // draw circle
  painter.setPen(pen);
  int padding = width() / 4;
  int circleRadius = (width() - (padding * 2))*3/4;
  painter.drawEllipse(padding, padding, circleRadius, circleRadius);

  // draw
  painter.drawLine(padding+circleRadius, padding+circleRadius, width() - padding, width() - padding);
}

CFilterLineEdit::CFilterLineEdit(QWidget *parent)
  : LineEdit(parent)
  , m_menu(new QMenu(this))
{
  CClearButton *clearButton = new CClearButton(this);
  CMagButton *magButton = new CMagButton(this);
  QString style("QListView, QLineEdit {"
		"selection-color: white; "
		"border: 2px groove gray;"
		"border-radius: 13px;"
		"padding: 2px 2px;"
		"background-image: url(:/icons/xxx.png);"
		"background-position: top right;"
		"padding-right: 0px;"
		"}"
		"QLineEdit:focus {"
		"selection-color: white;   "
		"border: 2px groove gray;"
		"border-radius: 13px;"
		"padding: 2px 2px;"
		"background-image: url(:/icons/xxx.png);"
		"padding-right: 0px;"
		"}"
		"QLineEdit:edit-focus {"
		"selection-color: white;   "
		"border: 2px groove gray;"
		"border-radius: 13px;"
		"padding: 2px 2px;"
		"background-image: url(:/icons/xxx.png);"
		"padding-right: 0px;"
		"}");

  setStyleSheet(style);
  setAttribute(Qt::WA_MacShowFocusRect, 0);
  addWidget(magButton, LeftSide);
  magButton->setMenu(m_menu);
  connect(magButton, SIGNAL(clicked()), magButton, SLOT(showMenu()));

  connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
  connect(this, SIGNAL(textChanged(const QString&)),
	  clearButton, SLOT(textChanged(const QString&)));
  addWidget(clearButton, RightSide);
  addWidget(magButton, LeftSide);

  updateTextMargins();
  setInactiveText(tr("Filter"));
}

CFilterLineEdit::~CFilterLineEdit()
{}

void CFilterLineEdit::addAction(QAction* action)
{
  m_menu->addAction(action);
}
