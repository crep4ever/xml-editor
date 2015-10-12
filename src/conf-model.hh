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

#ifndef __CONF_MODEL_HH__
#define __CONF_MODEL_HH__

#include <QAbstractTableModel>
#include <QString>
#include <QStringList>

/*!
  \file conf-model.hh
  \class CConfModel
  \brief Main model of the application.

  A CConfModel stores configuration parameters in a table.
  Each row corresponds to a parameter.
  The four columns store its:
  \li category
  \li sub-category
  \li name
  \li value

  This model is built when parsing an xml file.
  Due to ViT specific and non-standard xml representation
  of its configuration file. The following hacks are necessary:

  \li preprocess the file with \a VitToXml method to have a valid xml document
  \li store the xml document as a raw string (m_rawData) for saving purposes

  Keeping a raw version of the file allows to keep in sync
  modifications of the model when invoking the method \a setData.
 */

class CConfModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    enum Roles
    {
        CategoryRole = Qt::UserRole + 1,
        SubCategoryRole = Qt::UserRole + 2,
        ParameterRole = Qt::UserRole + 3,
        ValueRole = Qt::UserRole + 4,
        DefaultValueRole = Qt::UserRole + 5
    };

    /// Constructor
    CConfModel(const QString & filename, QObject *parent = 0);

    /// Destructor
    virtual ~CConfModel();

    /*!
    Return a valid xml document from a ViT configuration file.
    Modifications include:
    \li adding a root <Settings> node for the whole file
    \li ensure that xml nodes don't start with a digit
     */
    static QString VitToXml(const QString & filename);

    /*!
    Reimplements QAbstractTableModel::headerData.
    Display the four columns labels: category / subcategory / parameter / value
    \sa data
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
    Reimplements QAbstractTableModel::data.
    Returns the data at position \a index for the given \a role.
    \sa headerData
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /*!
    Reimplements QAbstractTableModel::setData.
    Edition is only allowed for the "value" column.
    Both the TableModel and m_rawData are modified accordingly.
    Before setting the new parameter value, its original value is kept
    for the \a revert() action.
    \sa data, headerData
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role);

    /*!
    Reimplements QAbstractTableModel::flags.
    Edition is only allowed for the "value" column.
    \sa data, headerData
     */
    Qt::ItemFlags flags(const QModelIndex & index) const;

    /*!
    Reimplements QAbstractTableModel::rowCount.
    Returns the number of rows in the model.
    \sa columnCount
     */
    virtual int rowCount(const QModelIndex &index = QModelIndex()) const;

    /*!
    Reimplements QAbstractTableModel::columnCount.
    \sa rowCount
     */
    virtual int columnCount(const QModelIndex &index = QModelIndex()) const;

    /*!
    Add a new row to the model.
    A \a row contains 4 strings (category / subcategory / name / value).
     */
    void addRow(const QStringList & row);

    /*!
    Returns the path of the file corresponding to the model.
    This path is automatically set when building the model from a file (\a load())
    but may be changed for saveAs actions.
    \sa setFilename, load, save
     */
    QString filename() const;

    /*!
    Defines a path for the model.
    This path is used by the \a save action.
    \sa filename, load, save
     */
    void setFilename(const QString & path);

    /*!
    Build the model by parsing an xml file.
    \sa save
     */
    void load(const QString & filename);

    /*!
    Save the model to a file.
    The destination is stored in \a filename()
    \sa filename, setFilename, load
     */
    void save();

    /*!
    Return the number of parameter values that have changed.
    \sa revert, editedValueCountChanged
     */
    int editedValuesCount() const;

public slots:
/*!
    Revert modified parameter values to their original state.
    \sa editedValuesCount, editedValueCountChanged
 */
void revert();

private:

QList< QStringList > parseLocalConfData(const QString & p_data);
//void parseOriginalLocalConfData(const QString & p_data);

void updateLocalConfRow(const QStringList & p_originalConfValues);

signals:

/*!
    This signal is emitted whenever the number of edited parameter values changes.
    \sa revert, editedValueCount
 */
void editedValueCountChanged(int count);

private:

QString m_filename;
QString m_rawData;
QList< QStringList > m_rows;
QStringList m_originalValues;

int m_rowCount;
int m_columnCount;
};

#endif // __CONF_MODEL_HH__
