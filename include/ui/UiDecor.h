#pragma once

#include <QString>

class QLabel;
class QWidget;

namespace printercare::ui
{
QLabel* createIconBadge(
    QWidget* parent,
    const QString& resourcePath,
    const QString& objectName,
    int boxSize,
    int iconSize);

QLabel* createMiniPill(QWidget* parent, const QString& text);
} // namespace printercare::ui
