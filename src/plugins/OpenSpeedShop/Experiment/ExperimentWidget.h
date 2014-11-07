#ifndef PLUGINS_OPENSPEEDSHOP_EXPERIMENTWIDGET_H
#define PLUGINS_OPENSPEEDSHOP_EXPERIMENTWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QUuid>

#include <ModelManager/FilterDescriptor.h>

class QAbstractItemView;
class QSortFilterProxyModel;

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui { class ExperimentWidget; }

class ExperimentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExperimentWidget(QWidget *parent = 0);
    ~ExperimentWidget();


    void create();
    bool load();

signals:
    void windowTitleChanged();

protected:
    void loadModelDescriptors(QString experimentType = QString());
    void readSettings();
    void writeSettings();

    QString sourceFilePath(int row, bool rewrite = true);
    QString rewriteSourceFilePath(int row);
    void refreshSourceIcon(int row = -1);

protected slots:
    // Processes tab
    void on_trvNodeList_selectionChanged();
    void on_trvNodeList_doubleClicked(QString);
    void on_trvProcesses_doubleClicked(QModelIndex);

    // Views tab
    void on_cmbViews_currentIndexChanged(int index);
    void on_btnAddModel_clicked();
    void on_txtViewFilter_textChanged(const QString &text);
    void on_cmbViewFilterColumn_currentIndexChanged(int index);

    // Source tab
    void on_btnSourcePath_clicked();
    void on_lstSource_currentRowChanged(int row);
    void on_lstSource_customContextMenuRequested(const QPoint &position);
    void on_txtSourcePath_editingFinished();

    void viewItemActivated(QModelIndex);

    void getModel(QUuid descriptorUid);
    void refreshProcessTree();
    void refreshSourcePaths();

    void refreshSourcePath();
    void rewriteSourcePath();
    void resetSourcePath();

    void closeEvent(QCloseEvent *);


private:
    Ui::ExperimentWidget *ui;
    QUuid m_ExperimentUid;
    QAbstractItemView *m_CurrentView;
    QAbstractItemModel *m_CurrentModel;
    QMap<QString, QString> m_SourceFileCache;
    QString m_CommonPath;

    QSortFilterProxyModel *proxyModelProcesses;

    FilterDescriptor m_CurrentFilter;

    int m_lstSourceContextMenuRow;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // PLUGINS_OPENSPEEDSHOP_EXPERIMENTWIDGET_H
