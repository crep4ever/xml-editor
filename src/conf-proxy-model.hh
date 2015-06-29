/*!
 * \file   conf-proxy-model.hh
 * \date   Mon, 29 Jun 2015 
 * \author VITECHNOLOGY\\romaing
 */

#ifndef __CONF_PROXY_MODEL_HH__
#define __CONF_PROXY_MODEL_HH__

#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>

class CConfProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public slots:
/*!
    Filter the view according to \a filterString.
    A filter string may contain keywords starting with :
    or negative filters starting with !: (ie :modified or !:modified)
 */
void setFilterWildcard(const QString &filterString);


public:
/// Constructor.
CConfProxyModel(QObject *parent = 0);

/// Destructor.
virtual ~CConfProxyModel();

/*!
    Returns the filter.
    \sa setFilterString
 */
QString filterString() const;

void clearKeywordFilter();
const QStringList & keywordFilter() const;

signals:
 void filteringChanged();


protected:
/*!
    Reimplements QSortFilterProxyModel::filterAcceptsRow
    to display rows matching filterString only for SystemPhase, Rule, Algorithm and Name columns.
 */
bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
QString m_filterString;
QStringList m_keywordFilter;
bool m_onlyModified;

};

#endif // __CONF_PROXY_MODEL_HH__
