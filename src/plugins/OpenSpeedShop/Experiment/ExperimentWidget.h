#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <QWidget>
#include <QUuid>
#include <QAbstractItemView>
#include <QTextDocument>
#include "SourceView.h"

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui { class ExperimentWidget; }

class ExperimentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExperimentWidget(QWidget *parent = 0);
    ~ExperimentWidget();

    void readSettings();
    void writeSettings();

    void create();
    void load();

signals:
    void windowTitleChanged();

protected:
    void loadModelDescriptors(QString experimentType = QString());

protected slots:
    void on_cmbViews_currentIndexChanged(int index);
    void on_btnAddModel_clicked();
    void on_txtViewFilter_textChanged(const QString &text);
    void on_cmbViewFilterColumn_currentIndexChanged(int index);
    void on_lstSource_currentRowChanged(int row);
    void on_btnSourcePath_clicked();

    void viewItemActivated(QModelIndex);

    void getModel(QUuid descriptorUid);
    void refreshProcessTree();
    void refreshSourcePaths();

    void closeEvent(QCloseEvent *);

private:
    Ui::ExperimentWidget *ui;
    QUuid m_ExperimentUid;
    QAbstractItemView *m_CurrentView;
    QAbstractItemModel *m_CurrentModel;
    QMap<QString, QString> m_SourceFileCache;
    SourceView m_SourceView;


};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // EXPERIMENTWIDGET_H
