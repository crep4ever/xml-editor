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

#include "conf-model.hh"

#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>

#include "utils/tango-colors.hh"

CConfModel::CConfModel(const QString & filename, QObject *parent)
  : QAbstractTableModel(parent)
  , m_filename(filename)
  , m_rows()
  , m_originalValues()
  , m_rowCount(0)
  , m_columnCount(4)
{
  load(filename);
}

CConfModel::~CConfModel()
{
  m_rows.clear();
}

QVariant CConfModel::headerData (int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
      switch (section)
        {
	case 0:
	  return tr("Category");
	case 1:
	  return tr("SubCategory");
	case 2:
	  return tr("Parameter");
	case 3:
	  return tr("Value");
	default:
	  return QVariant();
        }
    }
  return QVariant();
}

QVariant CConfModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  switch (role)
    {
    case Qt::DisplayRole:
      {
	switch (index.column())
	  {
	  case 0:
	    return data(index, CategoryRole);
	  case 1:
	    return data(index, SubCategoryRole);
	  case 2:
	    return data(index, ParameterRole);
	  case 3:
	    return data(index, ValueRole);

	  default:
	    return QVariant();
	  }
      }
      break;

    case CategoryRole:
      return m_rows[index.row()][0];

    case SubCategoryRole:
      return m_rows[index.row()][1];

    case ParameterRole:
      return m_rows[index.row()][2];

    case ValueRole:
      return m_rows[index.row()][3];

    case Qt::EditRole:
      return  data(index, Qt::DisplayRole);

    case Qt::BackgroundRole:
      return !m_originalValues[index.row()].isEmpty() ? _TangoChameleon1 : QVariant();

    case Qt::ToolTipRole:
      if (index.column() == 3)
	return !m_originalValues[index.row()].isEmpty() ? tr("Original value: %1").arg(m_originalValues[index.row()]) : QVariant();
      else
	return QString("%1 / %2 / %3")
	  .arg(data(index, CategoryRole).toString())
	  .arg(data(index, SubCategoryRole).toString())
	  .arg(data(index, ParameterRole).toString());

    default:
      break;
    }

  return QVariant();
}

bool CConfModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  switch (role)
    {
    case Qt::EditRole:
      {
	m_rows[index.row()][index.column()] = value.toString();

	const QString param = QString("/Conf/%1/%2/%3::false")
	  .arg(m_rows[index.row()][0])
	  .arg(m_rows[index.row()][1])
	  .arg(m_rows[index.row()][2]);

	const QRegExp rxParam(QString("%1=([^<]+)").arg(param));
	rxParam.indexIn(m_rawData);
	const QString oldValue = rxParam.cap(1);
	const QString newValue = value.toString();

	if (oldValue != newValue)
	  {
	    if (m_originalValues[index.row()].isEmpty())
	      {
		m_originalValues[index.row()] = oldValue;
	      }
	    m_rawData.replace(QString("%1=%2").arg(param).arg(oldValue),
			      QString("%1=%2").arg(param).arg(value.toString()));

	    emit(editedValueCountChanged(editedValuesCount()));
	    emit(dataChanged(index, index));
	  }
      }
      return true;

    default:
      qWarning() << tr("SetData role not supported yet");
    }

  return false;
}

void CConfModel::addRow(const QStringList & row)
{
  if (row.count() != columnCount())
    {
      qWarning() << "Invalid row: " << row;
      return;
    }

  m_rows << row;
  m_originalValues << "";
  ++m_rowCount;
}

int CConfModel::rowCount(const QModelIndex &index) const
{
  Q_UNUSED(index);
  return m_rowCount;
}

int CConfModel::columnCount(const QModelIndex &index) const
{
  Q_UNUSED(index);
  return m_columnCount;
}

void CConfModel::load(const QString & filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qWarning() << tr("Can't open file in read mode: %1").arg(filename);
      return;
    }

  QTextStream in(&file);
  in.setCodec("UTF-8");
  m_rawData = in.readAll();
  file.close();

  QXmlStreamReader xml(VitToXml(m_rawData));
  while (!xml.atEnd())
    {
      xml.readNext();
      const QString str = xml.text().toString();
      if (!str.isEmpty())
	{
	  QStringList tokens = str.split("=");
	  if (tokens.size() > 1)
	    {
	      QString value = tokens[1];
	      QStringList tokens2 = tokens[0].split("/");
	      if (tokens2.size() > 4)
		{
		  QString category = tokens2[2];
		  QString subcategory = tokens2[3];
		  QString key = tokens2[4].remove("::false");

		  addRow(QStringList() << category
			 << subcategory
			 << key
			 << value);
		}
	    }
	}
    }

  if (xml.hasError())
    {
      qWarning() << tr("Badly formed xml document: %1").arg(filename);
      qWarning() << tr("Error: %1").arg(xml.errorString());
    }
}

Qt::ItemFlags CConfModel::flags(const QModelIndex & index) const
{
  if (index.column() == 3) // only editable column is "values"
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
  else
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QString CConfModel::VitToXml(const QString & vit)
{
  QStringList lines = vit.split("\n");

  // Add xml parent node
  lines.insert(lines.begin() + 1, "<Settings>");
  lines.insert(lines.end(), "</Settings>");

  // Ensure xml nodes start with letters 
  QString xml = lines.join("\n");

  xml.replace("<2", "<Two");
  xml.replace("<3", "<Three");

  xml.replace("</2", "</Two");
  xml.replace("</3", "</Three");

  return xml;
}

QString CConfModel::filename() const
{
  return m_filename;
}

void CConfModel::setFilename(const QString & path)
{
  m_filename = path;
}

void CConfModel::save()
{
  // Open file
  QFile file(filename());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      qWarning() << tr("Can't open file in write mode: %1").arg(filename());
      return;
    }

  // Reset original values
  for (int i = 0; i < rowCount(); ++i)
    {
      if (!m_originalValues[i].isEmpty())
	{
	  m_originalValues[i] = "";
	  emit(dataChanged(index(i, 0), index(i, 3)));
	}
    }
  emit(editedValueCountChanged(0));

  // Write file
  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << m_rawData;
}

void CConfModel::revert()
{
  for (int i = 0; i < rowCount(); ++i)
    {
      if (!m_originalValues[i].isEmpty())
	{
	  setData(index(i, 3), m_originalValues[i], Qt::EditRole);
	  m_originalValues[i] = "";
	}
    }
  emit(editedValueCountChanged(0));
}

int CConfModel::editedValuesCount() const
{
  int count = 0;
  for (int i = 0; i < rowCount(); ++i)
    {
      if (!m_originalValues[i].isEmpty())
	{
	  ++count;
	}
    }
  return count;
}
