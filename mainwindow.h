#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QQuickView>
#include <QTimer>
#include "applicationmodel.h"

class MainWindow : public QQuickView
{
    Q_OBJECT

public:
    explicit MainWindow(QQuickView *parent = nullptr);

private:
    void init();
    void initWindow();
    void updatePosition();
    void resizeWindow();
    void adaptToScreen(QScreen *screen);

private:
    ApplicationModel *m_appModel;
    QTimer m_timerGeometry;
    QTimer m_lockGeometry;
    int m_maxLength;
};

#endif // MAINWINDOW_H
