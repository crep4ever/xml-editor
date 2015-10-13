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
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QElapsedTimer>

#include "utils/tango-colors.hh"


CConfModel::CConfModel(const QString & filename, QObject *parent)
: QAbstractTableModel(parent)
, m_filename(filename)
, m_rows()
, m_rowCount(0)
, m_columnCount(6)
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
            case 4:
                return tr("Initial value");
            case 5:
                return tr("Default value");
            default:
                return QVariant();
        }
    }
    return QVariant();
}


QModelIndex CConfModel::row(const int p_rowIndex) const
{
    return index(p_rowIndex, 0);
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
                case 4:
                    return data(index, InitialValueRole);
                case 5:
                    return data(index, DefaultValueRole);

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

        case InitialValueRole:
            return m_rows[index.row()][4];

        case DefaultValueRole:
            return m_rows[index.row()][5];

        case Qt::EditRole:
            return  data(index, Qt::DisplayRole);

        case Qt::BackgroundRole:
            if (data(index, ValueRole) != data(index, InitialValueRole))
            {
                return _TangoChameleon1; // value has been modified by user
            }
            else if (data(index, ValueRole) != data(index, DefaultValueRole))
            {
                return _TangoPlum1; // value is different from default value
            }
            return QVariant(); // value is equal to default and has not been modified

        case Qt::ToolTipRole:
            if (index.column() == 3 || index.column() == 5)
            {
                return tr("Current value: %1 \nInitial value: %2 \n Default value: %3")
                                .arg(data(index, ValueRole).toString())
                                .arg(data(index, InitialValueRole).toString())
                                .arg(data(index, DefaultValueRole).toString());
            }
            else
            {
                return QString("%1 / %2 / %3")
                                .arg(data(index, CategoryRole).toString())
                                .arg(data(index, SubCategoryRole).toString())
                                .arg(data(index, ParameterRole).toString());
            }

        default:
            break;
    }

    return QVariant();
}

bool CConfModel::setData(const QModelIndex & p_index, const QVariant & value, int role)
{
    if (!p_index.isValid() || p_index.row() >= m_rowCount)
    {
        return false;
    }

    const QModelIndex & begin = index(p_index.row(), 0);
    const QModelIndex & end   = index(p_index.row(), m_columnCount - 1);

    switch (role)
    {
        case Qt::EditRole:
        {
            if (setData(p_index, value, ValueRole))
            {
                emit(editedValueCountChanged(editedValuesCount()));
                return true;
            }
        }
        return false;

        case CategoryRole:
        {
            m_rows[p_index.row()][0] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;

        case SubCategoryRole:
        {
            m_rows[p_index.row()][1] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;


        case ParameterRole:
        {
            m_rows[p_index.row()][2] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;


        case ValueRole:
        {
            m_rows[p_index.row()][3] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;


        case InitialValueRole:
        {
            m_rows[p_index.row()][4] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;


        case DefaultValueRole:
        {
            m_rows[p_index.row()][5] = value.toString();
            emit(dataChanged(begin, end));
        }
        return true;

        default:
            qWarning() << tr("SetData role not supported yet");
    }

    return false;
}

void CConfModel::addRow(const QStringList & row)
{
    Q_ASSERT(row.count() == m_columnCount);
    m_rows << row;
    ++m_rowCount;
}

void CConfModel::addDefaultValue(const QStringList & p_data)
{
    Q_ASSERT(p_data.count() == 4);

    // find corresponding row in model
    for (int i = 0; i < m_rows.count(); ++i)
    {
        const QModelIndex & idx = row(i);

        // Find the row that matches the columns "category", "subcategory" and "parameter"
        if (data(idx, ParameterRole)   == p_data[2] &&
            data(idx, SubCategoryRole) == p_data[1] &&
            data(idx, CategoryRole)    == p_data[0])
        {
            setData(idx, p_data[3], DefaultValueRole); // parameter default value
            break;
        }
    }
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
    QElapsedTimer timer;
    timer.start();

    const QString baseName = QFileInfo(filename).baseName();
    const bool vitFile = baseName.contains("localconf", Qt::CaseInsensitive) || baseName.contains("persistentconf", Qt::CaseInsensitive);

    if (!vitFile)
    {
        QMessageBox messageBox;
        messageBox.setText(tr("Be sure to select a Vit configuration file (LocalConf files)."));
        messageBox.exec();
        return;
    }

    // open local conf file
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << tr("Can't open file in read mode: %1").arg(filename);
            return;
        }

        QTextStream curStream(&file);
        curStream.setCodec("UTF-8");
        m_rawData = curStream.readAll();
        file.close();
    }

    // Update model with current local conf values
    const QList< QStringList > & currentConfData = parseLocalConfData(VitToXml(m_rawData));
    foreach (const QStringList & r, currentConfData)
    {
        QStringList modelRow;
        modelRow << r[0]; // category
        modelRow << r[1]; // subcategory
        modelRow << r[2]; // param
        modelRow << r[3]; // value
        modelRow << r[3]; // initial value
        modelRow << r[3];   // default value
        addRow(modelRow);
    }

    qDebug() << "Build model from " << QFileInfo(filename).fileName() << " in " << timer.elapsed() << "ms";
    timer.restart();

    // look for potential LocalConf.xml.new in same path
    const QString refFilename = QFileInfo(filename).absolutePath() + QDir::separator() + "LocalConf.xml.new";
    QString refRawData;
    {
        QFile file(refFilename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << tr("Can't open file in read mode: %1").arg(refFilename);
            return;
        }

        QTextStream in(&file);
        in.setCodec("UTF-8");
        refRawData = in.readAll();
        file.close();
    }

    // Update model with default local conf values
    const QList< QStringList > & referenceConfData = parseLocalConfData(VitToXml(refRawData));
    foreach (const QStringList & r, referenceConfData)
    {
        addDefaultValue(r);
    }

    qDebug() << "Update model with default values from " << QFileInfo(refFilename).fileName() << " in " << timer.elapsed() << "ms";

}


QList<QStringList> CConfModel::parseLocalConfData(const QString & p_data)
{
    QList<QStringList> rows;

    QXmlStreamReader xml(p_data);
    while (!xml.hasError() && !xml.atEnd())
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

                    rows.push_back(QStringList() << category
                                   << subcategory
                                   << key
                                   << value);
                }
            }
        }
    }

    if (xml.hasError())
    {
        qWarning() << tr("Badly formed xml document");
        qWarning() << tr("Error: %1").arg(xml.errorString());
    }

    return rows;
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

    // Update raw data
    for (int i = 0; i < m_rowCount; ++i)
    {
        const QModelIndex & idx = row(i);

        const QString param = QString("/Conf/%1/%2/%3::false").arg(data(idx, CategoryRole).toString())
                                                              .arg(data(idx, SubCategoryRole).toString())
                                                              .arg(data(idx, ParameterRole).toString());

        const QString oldValue = data(idx, InitialValueRole).toString();
        const QString curValue = data(idx, ValueRole).toString();

        if (oldValue != curValue)
        {
            m_rawData.replace(QString("%1=%2").arg(param).arg(oldValue),
                              QString("%1=%2").arg(param).arg(curValue));

            // Reset initial value
            setData(idx, curValue, InitialValueRole);
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
        const QModelIndex & r = row(i);
        setData(r, data(r, InitialValueRole), ValueRole);
    }
    emit(editedValueCountChanged(0));
}

void CConfModel::revertToOriginalValue(const QModelIndex & p_index)
{
    Q_ASSERT(p_index.row() < m_rowCount);
    setData(p_index, data(p_index, InitialValueRole), ValueRole);
    emit(editedValueCountChanged(editedValuesCount()));
}

void CConfModel::revertToDefaultValue(const QModelIndex & p_index)
{
    Q_ASSERT(p_index.row() < m_rowCount);
    setData(p_index, data(p_index, DefaultValueRole), ValueRole);
    emit(editedValueCountChanged(editedValuesCount()));
}

int CConfModel::editedValuesCount() const
{
    int count = 0;
    for (int i = 0; i < rowCount(); ++i)
    {
        const QModelIndex & r = row(i);
        if (data(r, ValueRole) != data(r, InitialValueRole))
        {
            ++count;
        }
    }
    return count;
}
