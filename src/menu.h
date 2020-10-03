#ifndef MENU_H
#define MENU_H

#include <QQuickView>
#include <QMenu>

class Menu : public QMenu
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);

    Q_INVOKABLE void popup();
};

#endif // MENU_H
