#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include <QPalette>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QToolButton>
#include <QPixmap>
#include <QKeyEvent>

namespace Core {
namespace MainWindow {

class NotificationWidget : public QFrame
{
    Q_OBJECT
public:
    enum Icon {
        // keep this in sync with QMessageBox::Icon
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };

    enum ButtonRole {
        // keep this in sync with QMessageBox::ButtonRole
        InvalidRole = -1,
        AcceptRole,
        RejectRole,
        DestructiveRole,
        ActionRole,
        HelpRole,
        YesRole,
        NoRole,
        ResetRole,
        ApplyRole,

        NRoles
    };

    enum StandardButton {
        // keep this in sync with QMessageBox::StandardButton
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000
    };
    Q_DECLARE_FLAGS(StandardButtons, StandardButton)


    explicit NotificationWidget(QWidget *parent = 0);
    NotificationWidget(const QString &text, Icon icon = NoIcon, StandardButtons buttons = NoButton, QWidget *parent = 0);
    NotificationWidget(const QString &text, Icon icon = NoIcon, StandardButtons buttons = NoButton,
                       const QObject *reciever = NULL, const char *member = NULL, QWidget *parent = 0);
    ~NotificationWidget();

    QString text() const;
    void setText(const QString &text);

    Icon icon() const;
    void setIcon(const Icon &icon);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

    StandardButtons standardButtons() const;
    void setStandardButtons(StandardButtons standardButtons);

    void addButton(StandardButton button);
    QPushButton *button(StandardButton button) const;
    QList<QAbstractButton *> buttons() const;


signals:
    void buttonClicked(StandardButton standardButton);
    void closing();

protected slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_closeButton_clicked();

protected:
    void setupUi();
    void keyReleaseEvent(QKeyEvent *event);


    QLabel *_label;
    QLabel *_iconLabel;
    Icon _icon;
    QDialogButtonBox *_buttonBox;
    QToolButton *_closeButton;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NotificationWidget::StandardButtons)

} // namespace MainWindow
} // namespace Core

#endif // NOTIFICATIONWIDGET_H
