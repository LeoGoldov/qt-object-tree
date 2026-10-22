#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Qt Object Tree Demo");
    resize(500, 600);
    setupUi();
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    m_rootLayout = new QVBoxLayout(this);

    m_title = new LogLabel("Object Tree Demo: Owner and Children", this);
    m_rootLayout->addWidget(m_title.data());

    m_input = new LogLineEdit(this);
    m_input->setPlaceholderText("Child widget of MainWindow");
    m_rootLayout->addWidget(m_input.data());

    m_status = new LogLabel("Status: window created", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status.data());

    // Создаём панель (родитель - MainWindow)
    m_panel = new LogPanel("PanelOwner", this);
    m_rootLayout->addWidget(m_panel.data());

    m_panelLayout = new QVBoxLayout(m_panel.data());

    // Статическая кнопка внутри панели
    LogButton* staticButton = new LogButton("Static button in panel", m_panel.data());
    m_panelLayout->addWidget(staticButton);

    // Перемещаемая кнопка (сейчас в панели)
    m_movable = new LogButton("Movable widget (in panel)", m_panel.data());
    m_panelLayout->addWidget(m_movable.data());

    // Кнопка перемещения
    m_moveButton = new LogButton("Move widget: panel <-> window", this);
    connect(m_moveButton.data(), &QPushButton::clicked, this, &MainWindow::moveMovableWidget);
    m_rootLayout->addWidget(m_moveButton.data());

    // Кнопка удаления панели
    m_deleteButton = new LogButton("Delete panel (deleteLater)", this);
    connect(m_deleteButton.data(), &QPushButton::clicked, this, &MainWindow::deletePanel);
    m_rootLayout->addWidget(m_deleteButton.data());

    // Кнопка демонстрации setParent(nullptr)
    LogButton* detachButton = new LogButton("Demo setParent(nullptr)", this);
    connect(detachButton, &QPushButton::clicked, this, &MainWindow::demoDetaching);
    m_rootLayout->addWidget(detachButton);

    m_rootLayout->addStretch();

    setStatus("Ready. All widgets created with parents. Deleting parent will delete children.");
}

void MainWindow::setStatus(const QString& text)
{
    if (!m_status.isNull()) {
        m_status->setText(text);
    }
}

void MainWindow::moveMovableWidget()
{
    if (m_movable.isNull()) {
        setStatus("Movable widget already destroyed.");
        return;
    }
    if (m_panel.isNull()) {
        setStatus("Panel already deleted. Cannot move widget back.");
        return;
    }

    if (m_movable->parentWidget() == m_panel.data()) {
        // Перемещаем из панели в главное окно
        if (!m_panelLayout.isNull()) {
            m_panelLayout->removeWidget(m_movable.data());
        }
        m_movable->setParent(this);
        if (!m_rootLayout.isNull()) {
            m_rootLayout->addWidget(m_movable.data());
        }
        m_movable->setText("Movable widget (outside panel)");
        setStatus("setParent(this) made MainWindow the new owner. Panel will not delete this widget.");
    }
    else {
        // Перемещаем обратно в панель
        if (!m_rootLayout.isNull()) {
            m_rootLayout->removeWidget(m_movable.data());
        }
        m_movable->setParent(m_panel.data());
        if (!m_panelLayout.isNull()) {
            m_panelLayout->addWidget(m_movable.data());
        }
        m_movable->setText("Movable widget (in panel)");
        setStatus("Widget is a child of panel again. It will be deleted with the panel.");
    }

    m_movable->show();
    qDebug() << "[SET_PARENT] New owner of widget:" << m_movable->parentWidget()->objectName();
}

void MainWindow::deletePanel()
{
    if (m_panel.isNull()) {
        setStatus("Panel already deleted.");
        return;
    }

    if (!m_movable.isNull() && m_movable->parentWidget() == m_panel.data()) {
        setStatus("Panel will be deleted. Movable widget is its child, so it will be destroyed automatically.");
    }
    else {
        setStatus("Panel will be deleted. Movable widget is NOT its child, so it will survive.");
    }

    if (!m_deleteButton.isNull()) m_deleteButton->setEnabled(false);
    if (!m_moveButton.isNull()) m_moveButton->setEnabled(false);

    // Безопасное удаление
    m_panel->deleteLater();
}

void MainWindow::demoDetaching()
{
    // Создаём временную панель
    LogPanel* tempOwner = new LogPanel("TempOwner", this);
    tempOwner->hide(); // Скрываем, чтобы не мешала визуально

    LogLabel* child1 = new LogLabel("TempOwner's child", tempOwner);
    LogLabel* child2 = new LogLabel("Detached child", tempOwner);

    qDebug() << "[DEMO] Children count before detach:" << tempOwner->children().count();

    // Отвязываем child2
    child2->setParent(nullptr);
    qDebug() << "[DEMO] Children count after detach:" << tempOwner->children().count();

    // Удаляем tempOwner (child1 удалится автоматически, child2 - нет)
    tempOwner->deleteLater();

    // child2 теперь без владельца - удаляем вручную
    child2->deleteLater();

    setStatus("Demo completed. Check console output for CREATE/DESTROY logs.");
}