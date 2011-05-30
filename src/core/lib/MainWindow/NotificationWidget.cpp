#include "NotificationWidget.h"

#include <QPixmap>
#include <QMovie>
#include <QPalette>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QKeyEvent>
#include <QLabel>
#include <QDialogButtonBox>


namespace Core {
namespace MainWindow {

NotificationWidget::NotificationWidget(QWidget *parent) :
    QFrame(parent)
{
    setupUi();
}


NotificationWidget::NotificationWidget(const QString &text, Icon icon, StandardButtons buttons, QWidget *parent) :
    QFrame(parent)
{
    setupUi();

    setIcon(icon);
    setText(text);
    setStandardButtons(buttons);
}

NotificationWidget::NotificationWidget(const QString &text, Icon icon, StandardButtons buttons,
                                  const QObject *reciever, const char *member, QWidget *parent) :
    QFrame(parent)
{
    setupUi();

    setIcon(icon);
    setText(text);
    setStandardButtons(buttons);

    if(reciever && member) {
        connect(this, SIGNAL(buttonClicked(StandardButton)), reciever, member);
    }
}

NotificationWidget::~NotificationWidget()
{
}

void NotificationWidget::setupUi()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFrameShape(QFrame::Box);

    setStyleSheet("QFrame { background-color: rgb(255,255,225); }");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(9,0,9,0);
    layout->setMargin(0);
    this->setLayout(layout);

    _iconLabel = new QLabel(this);
    _iconLabel->setMaximumHeight(16);
    _iconLabel->setMaximumWidth(16);
    _iconLabel->setScaledContents(true);
    layout->addWidget(_iconLabel);

    _label = new QLabel(this);
    _label->setWordWrap(true);
    layout->addWidget(_label);

    _buttonBox = new QDialogButtonBox(this);
    _buttonBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _buttonBox->setMaximumHeight(16);
    connect(_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)));
    layout->addWidget(_buttonBox);

    _closeButton = new QToolButton(this);
    _closeButton->setMaximumHeight(16);
    _closeButton->setMaximumWidth(16);
    _closeButton->setAutoRaise(true);
    _closeButton->setIcon(QIcon(":/MainWindow/notificationClose.svg"));
    connect(_closeButton, SIGNAL(clicked()), this, SLOT(on_closeButton_clicked()));
    layout->addWidget(_closeButton);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

QString NotificationWidget::text() const
{
    return _label->text();
}
void NotificationWidget::setText(const QString &text)
{
    _label->setText(text);
}

NotificationWidget::Icon NotificationWidget::icon() const
{
    return _icon;
}

void NotificationWidget::setIcon(const Icon &icon)
{
    switch(_icon = icon) {
    case Information:
        _iconLabel->setPixmap(QPixmap(":/MainWindow/information.svg"));
        break;
    case Warning:
        _iconLabel->setPixmap(QPixmap(":/MainWindow/warning.svg"));
        break;
    case Critical:
        _iconLabel->setPixmap(QPixmap(":/MainWindow/critical.svg"));
        break;
    case Question:
        _iconLabel->setPixmap(QPixmap(":/MainWindow/question.svg"));
        break;
    case Loading:
    {
        QMovie *movie = new QMovie(":/MainWindow/loading.gif", QByteArray(), this);
        _iconLabel->setMovie(movie);
        movie->start();
    }
        break;
    default:
        _iconLabel->setPixmap(QPixmap());
    }
}

QPixmap NotificationWidget::pixmap() const
{
    return *_iconLabel->pixmap();
}
void NotificationWidget::setPixmap(const QPixmap &pixmap)
{
    _icon = NoIcon;
    _iconLabel->setPixmap(pixmap);
}

NotificationWidget::StandardButtons NotificationWidget::standardButtons() const
{
    return StandardButtons(int(_buttonBox->standardButtons()));
}
void NotificationWidget::setStandardButtons(StandardButtons standardButtons)
{
    _buttonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(standardButtons)));
}

void NotificationWidget::on_buttonBox_clicked(QAbstractButton *button)
{
    this->hide();
    emit buttonClicked((StandardButton)_buttonBox->standardButton(button));
    emit closing();
    this->close();
}

void NotificationWidget::on_closeButton_clicked()
{
    this->hide();
    emit buttonClicked(Close);
    emit closing();
    this->close();
}


QList<QAbstractButton *> NotificationWidget::buttons() const
{
    return _buttonBox->buttons();
}


void NotificationWidget::addButton(StandardButton button)
{
    _buttonBox->addButton((QDialogButtonBox::StandardButton)button);
}

QPushButton *NotificationWidget::button(StandardButton button) const
{
    return _buttonBox->button((QDialogButtonBox::StandardButton)button);
}


void NotificationWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->count() == 1) {
        if(event->key() == Qt::Key_Escape) {
            _closeButton->animateClick();
            return;
        } else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            if(buttons().count() == 0) {
                _closeButton->animateClick();
                return;
            }
        }
    }

    QFrame::keyReleaseEvent(event);
}




} // namespace MainWindow
} // namespace Core
