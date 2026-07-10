#include "ui/UiDecor.h"

#include <QLabel>
#include <QIcon>
#include <QWidget>

namespace printercare::ui
{
QLabel* createIconBadge(
    QWidget* parent,
    const QString& resourcePath,
    const QString& objectName,
    int boxSize,
    int iconSize)
{
    auto* label = new QLabel(parent);
    label->setObjectName(objectName);
    label->setFixedSize(boxSize, boxSize);
    label->setAlignment(Qt::AlignCenter);
    label->setPixmap(QIcon(resourcePath).pixmap(iconSize, iconSize));
    return label;
}

QLabel* createMiniPill(QWidget* parent, const QString& text)
{
    auto* label = new QLabel(text, parent);
    label->setObjectName(QStringLiteral("MiniPill"));
    label->setAlignment(Qt::AlignCenter);
    return label;
}
} // namespace printercare::ui
