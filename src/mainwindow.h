#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QQuickView>
#include <QVariantAnimation>
#include <QTimer>

#include "docksettings.h"
#include "applicationmodel.h"
#include "popuptips.h"

class MainWindow : public QQuickView
{
    Q_OBJECT

public:
    explicit MainWindow(QQuickView *parent = nullptr);

private:
    void updatePosition();
    void resizeWindow();
    void updateBlurRegion();
    void updateViewStruts();
    void onResizeValueChanged(const QVariant &value);

    QRegion cornerMask(const QRect &rect, const int r);

private:
    DockSettings *m_settings;
    ApplicationModel *m_appModel;
    PopupTips *m_popupTips;
    QVariantAnimation *m_resizeAnimation;
    int m_maxLength;
};

#endif // MAINWINDOW_H
