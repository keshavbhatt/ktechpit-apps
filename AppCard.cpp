// SPDX-FileCopyrightText: 2026 Keshav Bhatt (Ktechpit)
// SPDX-License-Identifier: MIT
#include "AppCard.h"

#include "AppLinks.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Qt::StringLiterals;

namespace ktechpit {

namespace {
constexpr int kIconSize = 40;
}

AppCard::AppCard(const AppInfo& app, const QString& place, QWidget* parent)
    : QFrame(parent)
    , m_app(app)
    , m_place(place)
{
    setObjectName(u"ktechpitAppCard"_s);
    setFrameShape(QFrame::StyledPanel);
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("%1\nClick for details").arg(app.title));
    setAttribute(Qt::WA_Hover);
    // Tab reaches the card, and Enter or Space opens its page, as a click does.
    setFocusPolicy(Qt::TabFocus);
    setAccessibleName(app.name);
    setAccessibleDescription(app.summary);

    m_icon = new QLabel(this);
    m_icon->setFixedSize(kIconSize, kIconSize);
    m_icon->setAlignment(Qt::AlignCenter);
    updateIcon();

    auto* name = new QLabel(app.name, this);
    name->setObjectName(u"ktechpitAppName"_s); // object names for the host's style sheet
    QFont bold = name->font();
    bold.setBold(true);
    name->setFont(bold);

    auto* summary = new QLabel(app.summary, this);
    summary->setObjectName(u"ktechpitAppSummary"_s);
    summary->setWordWrap(true);
    summary->setForegroundRole(QPalette::PlaceholderText);
    // Two lines at most: the full text is on the product page.
    const int lineHeight = summary->fontMetrics().lineSpacing();
    summary->setMaximumHeight(lineHeight * 2 + 2);
    summary->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    if (AppLinks::isInstalled(app)) {
        m_get = new QPushButton(tr("Installed"), this);
        m_get->setEnabled(false);
        m_get->setToolTip(tr("%1 is installed on this computer").arg(app.name));
    } else {
        m_get = new QPushButton(tr("Get"), this);
        m_get->setCursor(Qt::PointingHandCursor);
        m_get->setToolTip(tr("Install %1").arg(app.name));
        connect(m_get, &QPushButton::clicked, this, [this] { AppLinks::openGet(m_app, m_place); });
    }

    m_get->setObjectName(u"ktechpitAppGet"_s);

    auto* text = new QVBoxLayout;
    text->setSpacing(2);
    text->addWidget(name);
    text->addWidget(summary);
    text->addStretch();

    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(10, 10, 10, 10);
    row->setSpacing(10);
    row->addWidget(m_icon, 0, Qt::AlignTop);
    row->addLayout(text, 1);
    row->addWidget(m_get, 0, Qt::AlignVCenter);

    connect(AppCatalog::instance(), &AppCatalog::iconReady, this, [this](const QString& id) {
        if (id == m_app.id) {
            updateIcon();
        }
    });
}

void AppCard::updateIcon()
{
    m_icon->setPixmap(AppCatalog::instance()->icon(m_app, kIconSize));
}

void AppCard::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        AppLinks::openPage(m_app, m_place);
    }
    QFrame::mouseReleaseEvent(event);
}

void AppCard::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
        AppLinks::openPage(m_app, m_place);
        event->accept();
        return;
    }
    QFrame::keyPressEvent(event);
}

void AppCard::enterEvent(QEnterEvent* event)
{
    setBackgroundRole(QPalette::AlternateBase);
    setAutoFillBackground(true);
    QFrame::enterEvent(event);
}

void AppCard::leaveEvent(QEvent* event)
{
    setAutoFillBackground(false);
    QFrame::leaveEvent(event);
}

} // namespace ktechpit
