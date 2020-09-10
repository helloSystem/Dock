#include "mainwindow.h"
#include "iconthemeimageprovider.h"
#include "processprovider.h"
#include "popuptips.h"

#include <QGuiApplication>
#include <QScreen>
#include <QAction>
#include <QPainter>
#include <QImage>
#include <QRegion>

#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QMetaEnum>

#include <NETWM>
#include <KWindowSystem>

MainWindow::MainWindow(QQuickView *parent)
    : QQuickView(parent),
      m_appModel(new ApplicationModel),
      m_resizeAnimation(new QVariantAnimation(this))
{
    setDefaultAlphaBuffer(true);
    setColor(Qt::transparent);

    setFlags(Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    KWindowSystem::setState(winId(), NET::SkipTaskbar | NET::SkipPager);
    KWindowSystem::setOnDesktop(winId(), NET::OnAllDesktops);
    KWindowSystem::setType(winId(), NET::Dock);

    engine()->rootContext()->setContextProperty("appModel", m_appModel);
    engine()->rootContext()->setContextProperty("process", new ProcessProvider);
    engine()->rootContext()->setContextProperty("popupTips", new PopupTips);
    engine()->addImageProvider("icontheme", new IconThemeImageProvider);

    setResizeMode(QQuickView::SizeRootObjectToView);
    setClearBeforeRendering(true);
    setScreen(qGuiApp->primaryScreen());
    setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    resizeWindow();

    connect(this, &QQuickView::xChanged, this, &MainWindow::updatePosition);
    connect(this, &QQuickView::yChanged, this, &MainWindow::updatePosition);
    connect(m_appModel, &ApplicationModel::countChanged, this, &MainWindow::resizeWindow);
    connect(m_resizeAnimation, &QVariantAnimation::valueChanged, this, &MainWindow::onResizeValueChanged);
    // connect(m_resizeAnimation, &QVariantAnimation::finished, this, &MainWindow::updateViewStruts);
}

void MainWindow::updatePosition()
{
    const QRect screenGeometry = screen()->geometry();
    QPoint position = {0, 0};
    int margin = 10;

    // bottom
    position = { screenGeometry.x(), screenGeometry.y() + screenGeometry.height() - height()};
    m_maxLength = screenGeometry.width();

    position.setX((screenGeometry.width() - geometry().width()) / 2);

    // left
    // position = {screenGeometry.x(), screenGeometry.y()};
    // m_maxLength = screenGeometry.height();

    setX(position.x());
    setY(position.y() - margin / 2);
}

void MainWindow::resizeWindow()
{
    setVisible(true);

    QSize screenSize = screen()->size();

    // vertical
    // const QSize size{m_iconSize, screenSize.height()};

    // horizontal
    QSize newSize { screenSize.width(), m_appModel->iconSize() };
    newSize.setWidth((m_appModel->rowCount() + 2) * m_appModel->iconSize());

    if (m_resizeAnimation->state() == QVariantAnimation::Running) {
        m_resizeAnimation->stop();
    }

    if (newSize.width() > size().width()) {
        m_resizeAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    } else {
        m_resizeAnimation->setEasingCurve(QEasingCurve::InCubic);
    }

    XWindowInterface::instance()->enableBlurBehind(this, false);
    m_resizeAnimation->setDuration(250);
    m_resizeAnimation->setStartValue(this->size());
    m_resizeAnimation->setEndValue(newSize);
    m_resizeAnimation->start();
}

void MainWindow::updateBlurRegion()
{
    const QRect rect { 0, 0, size().width(), size().height() };
    XWindowInterface::instance()->enableBlurBehind(this, true, cornerMask(rect, rect.height() * 0.3));
}

void MainWindow::updateViewStruts()
{
    XWindowInterface::instance()->setViewStruts(this, geometry());
}

void MainWindow::onResizeValueChanged(const QVariant &value)
{
    const QSize &s = value.toSize();
    setMinimumSize(s);
    setMaximumSize(s);
    resize(s);
    updatePosition();
    updateBlurRegion();
    updateViewStruts();
}

QRegion MainWindow::cornerMask(const QRect &rect, const int r)
{
    QRegion region;
    // middle and borders
    region += rect.adjusted(r, 0, -r, 0);
    region += rect.adjusted(0, r, 0, -r);
    // top left
    QRect corner(rect.topLeft(), QSize(r * 2, r * 2));
    region += QRegion(corner, QRegion::Ellipse);
    // top right
    corner.moveTopRight(rect.topRight());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom left
    corner.moveBottomLeft(rect.bottomLeft());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom right
    corner.moveBottomRight(rect.bottomRight());
    region += QRegion(corner, QRegion::Ellipse);

    return region;
}
