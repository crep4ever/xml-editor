/*!
 * \file   conf-proxy-model.cc
 * \date   Mon, 29 Jun 2015 
 * \author VITECHNOLOGY\\romaing
 */

#include "conf-proxy-model.hh"

#include <QDebug>

#include "conf-model.hh"

CConfProxyModel::CConfProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
, m_filterString()
, m_keywordFilter()
, m_onlyModified(false)
{}

CConfProxyModel::~CConfProxyModel()
{}

void CConfProxyModel::setFilterWildcard(const QString &filterString)
{
  m_filterString = filterString;

  clearKeywordFilter();

  m_onlyModified = false;

  QString filter = m_filterString;
  if (filter.contains(":modified"))
    {
      m_onlyModified = true;
      filter.remove(":modified");
    }

  m_keywordFilter << filter.split(" ");
  invalidateFilter();

  emit(filteringChanged());
}

QString CConfProxyModel::filterString() const
{
    return m_filterString;
}

void CConfProxyModel::clearKeywordFilter()
{
  m_keywordFilter.clear();
}

const QStringList & CConfProxyModel::keywordFilter() const
{
  return m_keywordFilter;
}

bool CConfProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterString.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    bool accept = true;

    if (!m_keywordFilter.isEmpty())
    {
      foreach (const QString & keyword, m_keywordFilter)
        {
          QRegExp re(keyword,  Qt::CaseInsensitive);
          re.setMinimal(true);

          if (re.indexIn(sourceModel()->data(index, CConfModel::CategoryRole).toString()) == -1 &&
              re.indexIn(sourceModel()->data(index, CConfModel::SubCategoryRole).toString()) == -1 &&
              re.indexIn(sourceModel()->data(index, CConfModel::ParameterRole).toString()) == -1 &&
              re.indexIn(sourceModel()->data(index, CConfModel::ValueRole).toString()) == -1 &&
              re.indexIn(sourceModel()->data(index, CConfModel::DefaultValueRole).toString()) == -1)
            {
              accept = false;
            }
	    }
    }

    if (m_onlyModified)
      {
        const bool isModified = sourceModel() && 
          !sourceModel()->data(index, CConfModel::DefaultValueRole).isNull() && 
          sourceModel()->data(index, CConfModel::DefaultValueRole) != sourceModel()->data(index, CConfModel::ValueRole);

        accept = accept && isModified;
      }

    return accept;
}
