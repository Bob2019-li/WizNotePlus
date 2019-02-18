﻿#ifndef CORE_CELLBUTTON_H
#define CORE_CELLBUTTON_H

#include <QToolButton>
#include <QIcon>

class QSize;
class QPaintEvent;
class QString;
class QPropertyAnimation;

class WizCellButton : public QToolButton
{
    Q_OBJECT

public:
    enum ButtonType {
        ImageOnly,
        WithCountInfo
    };

    enum State {
        Normal,
        Checked,
        Badge
    };

    explicit WizCellButton(ButtonType type, QWidget* parent);
    void setNormalIcon(const QIcon& icon, const QString& strTips);
    void setCheckedIcon(const QIcon& icon, const QString& strTips);
    void setBadgeIcon(const QIcon& icon, const QString& strTips);
    int state() const { return m_state; }
    void setUserObject(QObject* obj) { m_userObject = obj; }
    QObject* userObject() const { return m_userObject; }

public slots:
    void setState(int state);
    void setCount(int count);

protected:
    ButtonType m_buttonType;
    int m_state;
    int m_count;
    QIcon m_iconNomal;
    QIcon m_iconChecked;
    QIcon m_iconBadge;
    QSize m_iconSize;
    QString m_strTipsNormal;
    QString m_strTipsChecked;
    QString m_strTipsBagde;
    QObject* m_userObject;

    //QIcon m_backgroundIcon;

protected:
    void paintEvent(QPaintEvent* event);
    QSize sizeHint() const;
    QString countInfo() const;
};

class WizRoundCellButton : public WizCellButton
{
    Q_OBJECT
public:
    explicit WizRoundCellButton(QWidget* parent = nullptr);
    ~WizRoundCellButton();

    void setNormalIcon(const QIcon& icon, const QString& text, const QString& strTips);
    void setCheckedIcon(const QIcon& icon, const QString& text, const QString& strTips);
    void setBadgeIcon(const QIcon& icon, const QString& text, const QString& strTips);

    QString text() const;
    int buttonWidth() const;

public slots:
    void setState(int state);

protected:
    void paintEvent(QPaintEvent* event);
    QSize sizeHint() const;

    void applyAnimation();
protected:
    QString m_textNormal;
    QString m_textChecked;
    QString m_textBadge;

    QPropertyAnimation* m_animation;
};


class WizToolButton : public QToolButton
{
    Q_OBJECT

public:
    enum ButtonType {
        ImageOnly = 0x00000000,
        WithCountInfo = 0x00000001,
        WithTextLabel = 0x00000002,
        WithMenu = 0x00000004
    };

    enum State {
        Normal,
        Checked,
        Badge
    };

    explicit WizToolButton(QWidget* parent, int type = WithTextLabel);
    void setNormalIcon(const QIcon& icon, const QString& strTips);
    void setCheckedIcon(const QIcon& icon, const QString& strTips);
    void setBadgeIcon(const QIcon& icon, const QString& strTips);
    int state() const { return m_state; }
    void setUserObject(QObject* obj) { m_userObject = obj; }
    QObject* userObject() const { return m_userObject; }

public slots:
    void setState(int state);
    void setCount(int count);

protected:
    int m_buttonType;
    int m_state;
    int m_count;
    QSize m_iconSize;
    QIcon m_icon;
    QIcon m_iconNomal;
    QIcon m_iconChecked;
    QIcon m_iconBadge;
    QString m_strTipsNormal;
    QString m_strTipsChecked;
    QString m_strTipsBadge;
    QObject* m_userObject;

protected:
    virtual void paintEvent(QPaintEvent* event);
    QSize sizeHint() const;
    QString countInfo() const;
};

class WizEditButton : public WizToolButton
{
    Q_OBJECT

private:
    QPropertyAnimation* m_animation;
    QString m_textNormal;
    QString m_textChecked;
    QString m_textBadge;

public:
    explicit WizEditButton(QWidget* parent);
    //
    void setStatefulIcon(const QIcon& icon, WizToolButton::State state);
    void setStatefulText(const QString& text, const QString& strTips, WizToolButton::State state);
    void setState(WizToolButton::State state);

    QString text() const;
    QString tips()const;
    int iconWidth() const;
    int buttonWidth() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    void applyAnimation();
};

#endif // CORE_CELLBUTTON_H
