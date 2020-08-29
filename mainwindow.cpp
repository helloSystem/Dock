#include "mainwindow.h"
#include "iconthemeimageprovider.h"

#include <QGuiApplication>
#include <QScreen>

#include <QAction>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QMetaEnum>

#include <NETWM>
#include <KWindowSystem>

MainWindow::MainWindow(QQuickView *parent)
    : QQuickView(parent),
      m_appModel(new ApplicationModel)
{
    setDefaultAlphaBuffer(true);
    // setOpacity(0.8);

    KWindowSystem::setType(winId(), NET::Dock);
    KWindowSystem::setState(winId(), NET::SkipTaskbar | NET::SkipPager);

    engine()->rootContext()->setContextProperty("appModel", m_appModel);
    engine()->addImageProvider("icontheme", new IconThemeImageProvider);

    setResizeMode(QQuickView::SizeRootObjectToView);
    setClearBeforeRendering(true);

    adaptToScreen(qGuiApp->primaryScreen());

    m_timerGeometry.setSingleShot(true);
    m_timerGeometry.setInterval(500);

    m_lockGeometry.setSingleShot(true);
    m_lockGeometry.setInterval(700);

    init();

    connect(m_appModel, &ApplicationModel::countChanged, this, &MainWindow::initWindow);
}

void MainWindow::init()
{
    connect(&m_timerGeometry, &QTimer::timeout, this, &MainWindow::initWindow);
    connect(&m_lockGeometry, &QTimer::timeout, this, &MainWindow::updatePosition);

    // rekols
    connect(this, &QQuickView::xChanged, this, &MainWindow::updatePosition);
    connect(this, &QQuickView::yChanged, this, &MainWindow::updatePosition);

    setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    m_timerGeometry.start();
}

void MainWindow::initWindow()
{
    resizeWindow();
    updatePosition();
}

void MainWindow::updatePosition()
{
    const QRect screenGeometry = screen()->geometry();
    QPoint position = {0, 0};

    // bottom
    position = { screenGeometry.x(), screenGeometry.y() + screenGeometry.height() - height()};
    m_maxLength = screenGeometry.width();

    // left
    // position = {screenGeometry.x(), screenGeometry.y()};
    // m_maxLength = screenGeometry.height();

    setX(position.x());
    setY(position.y());
}

void MainWindow::resizeWindow()
{
    setVisible(true);

    QSize screenSize = screen()->size();

    // vertical
    // const QSize size{m_iconSize, screenSize.height()};

    // horizontal
    const QSize size{screenSize.width(), m_appModel->iconSize()};
    // size.setWidth((m_appModel->rowCount() + 2)* m_appModel->iconSize());

    setMinimumSize(size);
    setMaximumSize(size);
    resize(size);
}

void MainWindow::adaptToScreen(QScreen *screen)
{
    setScreen(screen);

    m_timerGeometry.start();
}

