#ifndef WizMainTabBrowserView_H
#define WizMainTabBrowserView_H

#include <QTabWidget>
#include <QAbstractButton>
#include <QQueue>
#include "share/WizWebEngineView.h"
#include "WizDocumentWebView.h"

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

class WizExplorerApp;
class WizDatabaseManager;
class WIZDOCUMENTDATA;
class WizDocumentView;
class WizDocumentWebView;
class WizWebsiteView;
class QStyleOptionButton;
class QStyleOptionTabBarBase;

class TabButton : public QAbstractButton
{
    Q_OBJECT

public:
    explicit TabButton(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override
        { return sizeHint(); }
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void drawTabBtn(const QStyleOptionButton *option, QPainter *painter, const QWidget *widget = nullptr) const;
};

class WizTabPageRecycler : public QThread
{
    Q_OBJECT

public:
    WizTabPageRecycler(QObject *parent);
    ~WizTabPageRecycler();

    void addPageWidget(QWidget *wgt);
    void stop();

protected:
    void run();

private:
    QQueue<QWidget *> m_widgets;
    volatile bool m_stopped;
    QWaitCondition m_widgetAdded;
    QMutex m_mutex;
};

class WizMainTabBrowserView : public QTabWidget
{
    Q_OBJECT

public:

    typedef QMap<QString, QVariant> TabStatusData;

    WizMainTabBrowserView(WizExplorerApp& app, QWidget *parent = nullptr);

    //WizDocumentView* currentDocView();
    WizWebEngineView* currentWebView() const;
    WizWebEngineView* getWebView(int index) const;

private:
    WizExplorerApp& m_app;
    WizDatabaseManager& m_dbMgr;
    QString m_strTheme;
    WizTabPageRecycler *m_recycler;

signals:
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void loadProgress(int progress);
    void linkHovered(const QString &link);

public slots:
    void handleCurrentChanged(int index);
    void handleContextMenuRequested(const QPoint &pos);
    int createTab(WizDocumentView *docView);
    int createTab(const QUrl &url);
    void closeTab(int index);
    void closeOtherTabs(int index);
    void closeAllTabs();
    void closeLeftTabs(int index);
    void closeRightTabs(int index);
    void lockTab(int index);
    void unlockTab(int index);
    void onViewNoteRequested(WizDocumentView* view, const WIZDOCUMENTDATAEX& doc, bool forceEditing);
    void on_document_deleted(const WIZDOCUMENTDATA&);

private:
    void setupView(WizWebEngineView* view);
    void setupWebsiteView(WizWebsiteView *websiteView);
    void setupDocView(WizDocumentView *docView);
    void setupTab(QWidget* wgt);
    void paintEvent(QPaintEvent *);
    void initStyleBaseOption(QStyleOptionTabBarBase *optTabBase, QTabBar *tabbar, QSize size);
};

#endif // WIZTABWIDGET_H
