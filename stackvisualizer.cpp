#include "stackvisualizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>

StackVisualizer::StackVisualizer(QWidget *parent)
    : QWidget(parent)
    , top(-1)
    , isAnimating(false)
    , animationStep(0)
    , currentHistoryIndex(-1)
{
    setupUI();

    // Setup animation timer
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &StackVisualizer::animateOperation);

    // Connect signals
    connect(pushButton, &QPushButton::clicked, this, [this]() {
        bool ok;
        int value = inputField->text().toInt(&ok);
        if (ok) {
            push(value);
            inputField->clear();
        } else {
            setStatusMessage("Please enter a valid number");
        }
    });

    connect(popButton, &QPushButton::clicked, this, [this]() {
        if (!isEmpty()) {
            pop();
        }
    });

    connect(peekButton, &QPushButton::clicked, this, [this]() {
        if (!isEmpty()) {
            setStatusMessage(QString("Top element: %1").arg(peek()));
        }
    });

    connect(clearButton, &QPushButton::clicked, this, &StackVisualizer::clear);
    connect(undoButton, &QPushButton::clicked, this, &StackVisualizer::undoOperation);
    connect(redoButton, &QPushButton::clicked, this, &StackVisualizer::redoOperation);

    // Initialize
    updateVisualization();
    updateButtons();
}

StackVisualizer::~StackVisualizer()
{
    delete scene;
}

bool StackVisualizer::push(int value)
{
    if (isFull()) {
        setStatusMessage("Stack Overflow!");
        return false;
    }

    addToHistory(Operation::PUSH, value);
    arr[++top] = value;
    updateVisualization();
    setStatusMessage(QString("Pushed value: %1").arg(value));
    
    // Start animation after updating
    if (!isAnimating) {
        animationStep = 0;
        isAnimating = true;
        animationTimer->start(100);
    }
    
    emit stateChanged();
    return true;
}

int StackVisualizer::pop()
{
    if (isEmpty()) {
        setStatusMessage("Stack Underflow!");
        return -1;
    }

    int value = arr[top];
    addToHistory(Operation::POP, value);
    top--;
    updateVisualization();
    setStatusMessage(QString("Popped value: %1").arg(value));
    
    // Start animation after updating
    if (!isAnimating) {
        animationStep = 0;
        isAnimating = true;
        animationTimer->start(100);
    }
    
    emit stateChanged();
    return value;
}

int StackVisualizer::peek() const
{
    if (isEmpty()) {
        return -1;
    }
    return arr[top];
}

void StackVisualizer::addToHistory(Operation::Type type, int value)
{
    // Remove any operations after current index if we're in the middle of history
    while (history.size() > currentHistoryIndex + 1) {
        history.removeLast();
    }

    Operation op;
    op.type = type;
    op.value = value;
    op.prevState = getCurrentState();
    history.append(op);
    currentHistoryIndex++;

    // Update history list widget
    QString operationText;
    switch (type) {
        case Operation::PUSH:
            operationText = QString("Push %1").arg(value);
            break;
        case Operation::POP:
            operationText = QString("Pop %1").arg(value);
            break;
        case Operation::CLEAR:
            operationText = "Clear";
            break;
    }
    historyList->addItem(operationText);
    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
}

void StackVisualizer::undoOperation()
{
    if (currentHistoryIndex < 0 || isAnimating) return;

    Operation &op = history[currentHistoryIndex];
    switch (op.type) {
        case Operation::PUSH:
            top--;
            break;
        case Operation::POP:
            top++;
            arr[top] = op.value;
            break;
        case Operation::CLEAR:
            for (int i = 0; i < op.prevState.size(); i++) {
                arr[i] = op.prevState[i];
            }
            top = op.prevState.size() - 1;
            break;
    }

    currentHistoryIndex--;
    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
    updateVisualization();
    setStatusMessage("Undo: " + historyList->item(currentHistoryIndex + 1)->text());
}

void StackVisualizer::redoOperation()
{
    if (currentHistoryIndex >= history.size() - 1 || isAnimating) return;

    currentHistoryIndex++;
    Operation &op = history[currentHistoryIndex];
    switch (op.type) {
        case Operation::PUSH:
            top++;
            arr[top] = op.value;
            break;
        case Operation::POP:
            top--;
            break;
        case Operation::CLEAR:
            top = -1;
            break;
    }

    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
    updateVisualization();
    setStatusMessage("Redo: " + historyList->item(currentHistoryIndex)->text());
}

void StackVisualizer::animateOperation()
{
    if (!isAnimating) return;

    // Animation logic here
    animationStep++;
    if (animationStep >= 5) { // Reduced from 10 to 5 for faster animation
        isAnimating = false;
        animationTimer->stop();
        animationStep = 0;
    }
    updateVisualization();
}

void StackVisualizer::drawBox(int index, int value, const QString &color)
{
    const int BOX_WIDTH = 60;
    const int BOX_HEIGHT = 40;
    const int SPACING = 10;
    const int BASE_Y = 200; // Increased from 150 to give more space
    const int BASE_X = 100; // Added to center the stack

    QGraphicsRectItem *box = new QGraphicsRectItem(BASE_X, BASE_Y - index * (BOX_HEIGHT + SPACING), BOX_WIDTH, BOX_HEIGHT);
    box->setBrush(QColor(color));
    box->setPen(QPen(Qt::black));
    scene->addItem(box);

    QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(value));
    text->setDefaultTextColor(Qt::black);
    text->setPos(box->pos().x() + BOX_WIDTH/4, box->pos().y() + BOX_HEIGHT/4);
    scene->addItem(text);
}

void StackVisualizer::updateCapacityIndicator()
{
    QString style = isEmpty() ? "color: red;" : (isFull() ? "color: orange;" : "color: green;");
    statusLabel->setStyleSheet(style);
    statusLabel->setText(QString("Capacity: %1/%2").arg(top + 1).arg(MAX_SIZE));
}

void StackVisualizer::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create visualization group
    QGroupBox *visualizationGroup = new QGroupBox("Stack Visualization");
    QVBoxLayout *visualizationLayout = new QVBoxLayout;
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setMinimumHeight(200);
    view->setRenderHint(QPainter::Antialiasing);
    visualizationLayout->addWidget(view);
    visualizationGroup->setLayout(visualizationLayout);
    mainLayout->addWidget(visualizationGroup);

    // Create controls group
    QGroupBox *controlsGroup = new QGroupBox("Controls");
    QHBoxLayout *controlsLayout = new QHBoxLayout;

    inputField = new QLineEdit;
    inputField->setPlaceholderText("Enter a number");
    controlsLayout->addWidget(inputField);

    pushButton = new QPushButton("Push");
    popButton = new QPushButton("Pop");
    peekButton = new QPushButton("Peek");
    clearButton = new QPushButton("Clear");

    controlsLayout->addWidget(pushButton);
    controlsLayout->addWidget(popButton);
    controlsLayout->addWidget(peekButton);
    controlsLayout->addWidget(clearButton);

    controlsGroup->setLayout(controlsLayout);
    mainLayout->addWidget(controlsGroup);

    // Create history group
    QGroupBox *historyGroup = new QGroupBox("Operation History");
    QVBoxLayout *historyLayout = new QVBoxLayout;
    historyList = new QListWidget;
    historyLayout->addWidget(historyList);

    QHBoxLayout *historyButtonLayout = new QHBoxLayout;
    undoButton = new QPushButton("Undo");
    redoButton = new QPushButton("Redo");
    historyButtonLayout->addWidget(undoButton);
    historyButtonLayout->addWidget(redoButton);
    historyLayout->addLayout(historyButtonLayout);

    historyGroup->setLayout(historyLayout);
    mainLayout->addWidget(historyGroup);

    // Create status group
    QGroupBox *statusGroup = new QGroupBox("Status");
    QVBoxLayout *statusLayout = new QVBoxLayout;
    statusLabel = new QLabel;
    statusLayout->addWidget(statusLabel);
    statusGroup->setLayout(statusLayout);
    mainLayout->addWidget(statusGroup);

    setLayout(mainLayout);

    // Initialize status
    setStatusMessage("Stack is empty");
    updateButtons();
    updateCapacityIndicator();
}

bool StackVisualizer::isEmpty() const
{
    return top == -1;
}

bool StackVisualizer::isFull() const
{
    return top == MAX_SIZE - 1;
}

void StackVisualizer::clear()
{
    top = -1;
    addToHistory(Operation::CLEAR, 0);
    setStatusMessage("Stack cleared");
    updateVisualization();
    emit stateChanged();
}

QVector<int> StackVisualizer::getCurrentState() const
{
    QVector<int> state;
    for (int i = 0; i <= top; i++) {
        state.append(arr[i]);
    }
    return state;
}

QString StackVisualizer::getStatusMessage() const
{
    return statusMessage;
}

void StackVisualizer::setStatusMessage(const QString &message)
{
    statusMessage = message;
    statusLabel->setText(message);
    emit statusChanged(message);
}

void StackVisualizer::updateButtons()
{
    pushButton->setEnabled(!isFull());
    popButton->setEnabled(!isEmpty());
    peekButton->setEnabled(!isEmpty());
    clearButton->setEnabled(!isEmpty());
    undoButton->setEnabled(currentHistoryIndex >= 0);
    redoButton->setEnabled(currentHistoryIndex < history.size() - 1);
}

void StackVisualizer::updateVisualization()
{
    scene->clear();
    
    const int boxWidth = 60;
    const int boxHeight = 30;
    const int spacing = 5;
    const int startX = 10;
    const int startY = scene->height() - boxHeight - 10;
    
    // Draw stack base
    scene->addLine(startX - 5, startY + boxHeight + 5, 
                  startX + boxWidth + 5, startY + boxHeight + 5,
                  QPen(Qt::black, 2));

    // Draw stack elements
    QFont elementFont;
    elementFont.setPointSize(9);
    
    for (int i = 0; i <= top; i++) {
        int y = startY - (i * (boxHeight + spacing));
        
        // Draw box
        QGraphicsRectItem *box = scene->addRect(startX, y, boxWidth, boxHeight);
        box->setPen(QPen(QColor("#dee2e6")));
        box->setBrush(QBrush(QColor("#f8f9fa")));
        
        // Draw value
        QGraphicsTextItem *valueText = scene->addText(QString::number(arr[i]));
        valueText->setFont(elementFont);
        valueText->setDefaultTextColor(QColor("#2196F3"));
        
        // Center text in box
        QRectF textBounds = valueText->boundingRect();
        valueText->setPos(startX + (boxWidth - textBounds.width()) / 2,
                         y + (boxHeight - textBounds.height()) / 2);
        
        // Draw index
        QGraphicsTextItem *indexText = scene->addText(QString::number(i));
        indexText->setFont(elementFont);
        indexText->setDefaultTextColor(Qt::gray);
        indexText->setPos(startX - 20, y + (boxHeight - textBounds.height()) / 2);
    }

    // Adjust view
    QRectF bounds = scene->itemsBoundingRect();
    bounds.adjust(-20, -10, 20, 10);
    view->setSceneRect(bounds);
    view->fitInView(bounds, Qt::KeepAspectRatio);
}
