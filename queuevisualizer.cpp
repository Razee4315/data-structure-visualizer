#include "queuevisualizer.h"
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
#include <QListWidget>

QueueVisualizer::QueueVisualizer(QWidget *parent)
    : QWidget(parent)
    , frontIndex(-1)
    , rearIndex(-1)
    , size(0)
    , isAnimating(false)
    , animationStep(0)
    , currentHistoryIndex(-1)
{
    setupUI();

    // Setup animation timer
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &QueueVisualizer::animateOperation);

    // Connect signals
    connect(enqueueButton, &QPushButton::clicked, this, [this]() {
        bool ok;
        int value = inputField->text().toInt(&ok);
        if (ok) {
            enqueue(value);
            inputField->clear();
        } else {
            setStatusMessage("Please enter a valid number");
        }
    });

    connect(dequeueButton, &QPushButton::clicked, this, [this]() {
        if (!isEmpty()) {
            dequeue();
        } else {
            setStatusMessage("Queue is empty!");
        }
    });

    connect(frontButton, &QPushButton::clicked, this, [this]() {
        if (!isEmpty()) {
            setStatusMessage(QString("Front element: %1").arg(front()));
        } else {
            setStatusMessage("Queue is empty!");
        }
    });

    connect(rearButton, &QPushButton::clicked, this, [this]() {
        if (!isEmpty()) {
            setStatusMessage(QString("Rear element: %1").arg(rear()));
        } else {
            setStatusMessage("Queue is empty!");
        }
    });

    connect(clearButton, &QPushButton::clicked, this, &QueueVisualizer::clear);
    connect(undoButton, &QPushButton::clicked, this, &QueueVisualizer::undoOperation);
    connect(redoButton, &QPushButton::clicked, this, &QueueVisualizer::redoOperation);

    // Initialize
    updateVisualization();
    updateButtons();
}

QueueVisualizer::~QueueVisualizer()
{
    delete scene;
}

bool QueueVisualizer::enqueue(int value)
{
    if (isFull()) {
        setStatusMessage("Queue Overflow!");
        return false;
    }

    addToHistory(Operation::ENQUEUE, value);

    if (isEmpty()) {
        frontIndex = 0;
        rearIndex = 0;
    } else {
        rearIndex = (rearIndex + 1) % MAX_SIZE;
    }
    
    arr[rearIndex] = value;
    size++;
    
    updateVisualization();
    setStatusMessage(QString("Enqueued value: %1").arg(value));
    
    // Start animation after updating
    if (!isAnimating) {
        animationStep = 0;
        isAnimating = true;
        animationTimer->start(100);
    }
    
    emit stateChanged();
    updateButtons();
    return true;
}

int QueueVisualizer::dequeue()
{
    if (isEmpty()) {
        setStatusMessage("Queue Underflow!");
        return -1;
    }

    int value = arr[frontIndex];
    addToHistory(Operation::DEQUEUE, value);
    
    if (frontIndex == rearIndex) {
        // Last element being removed
        frontIndex = -1;
        rearIndex = -1;
        size = 0;
    } else {
        frontIndex = (frontIndex + 1) % MAX_SIZE;
        size--;
    }
    
    updateVisualization();
    setStatusMessage(QString("Dequeued value: %1").arg(value));
    
    // Start animation after updating
    if (!isAnimating) {
        animationStep = 0;
        isAnimating = true;
        animationTimer->start(100);
    }
    
    emit stateChanged();
    updateButtons();
    return value;
}

int QueueVisualizer::front() const
{
    if (isEmpty()) {
        return -1;
    }
    return arr[frontIndex];
}

int QueueVisualizer::rear() const
{
    if (isEmpty()) {
        return -1;
    }
    return arr[rearIndex];
}

void QueueVisualizer::clear()
{
    if (!isEmpty()) {
        addToHistory(Operation::CLEAR, 0);
        frontIndex = -1;
        rearIndex = -1;
        size = 0;
        updateVisualization();
        setStatusMessage("Queue cleared");
        emit stateChanged();
        updateButtons();
    }
}

bool QueueVisualizer::isEmpty() const
{
    return size == 0;
}

bool QueueVisualizer::isFull() const
{
    return size == MAX_SIZE;
}

void QueueVisualizer::updateVisualization()
{
    scene->clear();

    const int BOX_WIDTH = 60;
    const int BOX_HEIGHT = 40;
    const int SPACING = 15;  // Increased spacing between boxes
    const int BASE_Y = 120;  // Moved boxes down a bit
    const int BASE_X = 50;
    const int FRAME_PADDING = 15;  // Padding around the frame

    // Draw queue frame
    QPen framePen(Qt::black, 2);
    scene->addLine(BASE_X - FRAME_PADDING, BASE_Y - FRAME_PADDING, 
                  BASE_X + (MAX_SIZE * (BOX_WIDTH + SPACING)) - SPACING + FRAME_PADDING, BASE_Y - FRAME_PADDING, 
                  framePen);
    scene->addLine(BASE_X - FRAME_PADDING, BASE_Y + BOX_HEIGHT + FRAME_PADDING, 
                  BASE_X + (MAX_SIZE * (BOX_WIDTH + SPACING)) - SPACING + FRAME_PADDING, BASE_Y + BOX_HEIGHT + FRAME_PADDING, 
                  framePen);

    // Draw vertical lines
    scene->addLine(BASE_X - FRAME_PADDING, BASE_Y - FRAME_PADDING,
                  BASE_X - FRAME_PADDING, BASE_Y + BOX_HEIGHT + FRAME_PADDING,
                  framePen);
    scene->addLine(BASE_X + (MAX_SIZE * (BOX_WIDTH + SPACING)) - SPACING + FRAME_PADDING, BASE_Y - FRAME_PADDING,
                  BASE_X + (MAX_SIZE * (BOX_WIDTH + SPACING)) - SPACING + FRAME_PADDING, BASE_Y + BOX_HEIGHT + FRAME_PADDING,
                  framePen);

    // Draw all boxes
    for (int i = 0; i < MAX_SIZE; i++) {
        QString color = "#f0f0f0"; // Default empty color
        int value = 0;
        bool isOccupied = false;

        if (!isEmpty()) {
            // Check if this position is occupied in the circular queue
            int relativePos = i - frontIndex;
            if (relativePos < 0) relativePos += MAX_SIZE;
            
            if (frontIndex <= rearIndex) {
                isOccupied = (i >= frontIndex && i <= rearIndex);
            } else {
                isOccupied = (i >= frontIndex || i <= rearIndex);
            }

            if (isOccupied) {
                color = "#81C784"; // Green for occupied
                value = arr[i];
            }
        }

        // Draw box with appropriate color and value
        QGraphicsRectItem *box = scene->addRect(
            BASE_X + i * (BOX_WIDTH + SPACING),
            BASE_Y,
            BOX_WIDTH,
            BOX_HEIGHT
        );
        box->setBrush(QColor(color));
        box->setPen(QPen(Qt::black));

        // Add index number above box
        QGraphicsTextItem *indexText = scene->addText(QString::number(i));
        QFont indexFont = indexText->font();
        indexFont.setPointSize(10);
        indexText->setFont(indexFont);
        indexText->setDefaultTextColor(Qt::darkGray);
        indexText->setPos(
            BASE_X + i * (BOX_WIDTH + SPACING) + (BOX_WIDTH - indexText->boundingRect().width()) / 2,
            BASE_Y - 35  // Moved index numbers up by 5 more pixels
        );

        if (isOccupied) {
            QGraphicsTextItem *valueText = scene->addText(QString::number(value));
            QFont valueFont = valueText->font();
            valueFont.setPointSize(12);
            valueText->setFont(valueFont);
            valueText->setDefaultTextColor(Qt::black);
            // Center the value in the box
            valueText->setPos(
                BASE_X + i * (BOX_WIDTH + SPACING) + (BOX_WIDTH - valueText->boundingRect().width()) / 2,
                BASE_Y + (BOX_HEIGHT - valueText->boundingRect().height()) / 2
            );
        }

        // Add Front/Rear indicators
        if (!isEmpty()) {
            if (i == frontIndex) {
                QGraphicsTextItem *frontText = scene->addText("Front");
                QFont frontFont = frontText->font();
                frontFont.setPointSize(10);
                frontText->setFont(frontFont);
                frontText->setDefaultTextColor(Qt::blue);
                frontText->setPos(
                    BASE_X + i * (BOX_WIDTH + SPACING) + (BOX_WIDTH - frontText->boundingRect().width()) / 2,
                    BASE_Y - 55  // Moved up to avoid overlap
                );
            }
            if (i == rearIndex) {
                QGraphicsTextItem *rearText = scene->addText("Rear");
                QFont rearFont = rearText->font();
                rearFont.setPointSize(10);
                rearText->setFont(rearFont);
                rearText->setDefaultTextColor(Qt::red);
                rearText->setPos(
                    BASE_X + i * (BOX_WIDTH + SPACING) + (BOX_WIDTH - rearText->boundingRect().width()) / 2,
                    BASE_Y + BOX_HEIGHT + 20
                );
            }
        }
    }

    // Update capacity indicator
    updateCapacityIndicator();

    // Adjust view
    QRectF bounds = scene->itemsBoundingRect();
    bounds.adjust(-20, -20, 20, 20);
    view->setSceneRect(bounds);
    view->fitInView(bounds, Qt::KeepAspectRatio);
}

void QueueVisualizer::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create visualization group
    QGroupBox *visualizationGroup = new QGroupBox("Queue Visualization");
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

    enqueueButton = new QPushButton("Enqueue");
    dequeueButton = new QPushButton("Dequeue");
    frontButton = new QPushButton("Front");
    rearButton = new QPushButton("Rear");
    clearButton = new QPushButton("Clear");

    controlsLayout->addWidget(enqueueButton);
    controlsLayout->addWidget(dequeueButton);
    controlsLayout->addWidget(frontButton);
    controlsLayout->addWidget(rearButton);
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
    setStatusMessage("Queue is empty");
    updateButtons();
    updateCapacityIndicator();
}

void QueueVisualizer::updateCapacityIndicator()
{
    QString style = isEmpty() ? "color: red;" : (isFull() ? "color: orange;" : "color: green;");
    statusLabel->setStyleSheet(style);
    statusLabel->setText(QString("Size: %1/%2").arg(size).arg(MAX_SIZE));
}

void QueueVisualizer::updateButtons()
{
    enqueueButton->setEnabled(!isFull());
    dequeueButton->setEnabled(!isEmpty());
    frontButton->setEnabled(!isEmpty());
    rearButton->setEnabled(!isEmpty());
    clearButton->setEnabled(!isEmpty());
    undoButton->setEnabled(currentHistoryIndex >= 0);
    redoButton->setEnabled(currentHistoryIndex < history.size() - 1);
}

void QueueVisualizer::addToHistory(Operation::Type type, int value)
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
        case Operation::ENQUEUE:
            operationText = QString("Enqueue %1").arg(value);
            break;
        case Operation::DEQUEUE:
            operationText = QString("Dequeue %1").arg(value);
            break;
        case Operation::CLEAR:
            operationText = "Clear";
            break;
    }
    historyList->addItem(operationText);
    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
}

void QueueVisualizer::undoOperation()
{
    if (currentHistoryIndex < 0 || isAnimating) return;

    Operation &op = history[currentHistoryIndex];
    switch (op.type) {
        case Operation::ENQUEUE:
            if (rearIndex == frontIndex) {
                frontIndex = rearIndex = -1;
            } else {
                rearIndex = (rearIndex - 1 + MAX_SIZE) % MAX_SIZE;
            }
            size--;
            break;
        case Operation::DEQUEUE:
            frontIndex = (frontIndex - 1 + MAX_SIZE) % MAX_SIZE;
            arr[frontIndex] = op.value;
            size++;
            break;
        case Operation::CLEAR:
            for (int i = 0; i < op.prevState.size(); i++) {
                arr[i] = op.prevState[i];
            }
            frontIndex = 0;
            rearIndex = op.prevState.size() - 1;
            size = op.prevState.size();
            break;
    }

    currentHistoryIndex--;
    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
    updateVisualization();
    setStatusMessage("Undo: " + historyList->item(currentHistoryIndex + 1)->text());
}

void QueueVisualizer::redoOperation()
{
    if (currentHistoryIndex >= history.size() - 1 || isAnimating) return;

    currentHistoryIndex++;
    Operation &op = history[currentHistoryIndex];
    switch (op.type) {
        case Operation::ENQUEUE:
            if (isEmpty()) {
                frontIndex = rearIndex = 0;
            } else {
                rearIndex = (rearIndex + 1) % MAX_SIZE;
            }
            arr[rearIndex] = op.value;
            size++;
            break;
        case Operation::DEQUEUE:
            frontIndex = (frontIndex + 1) % MAX_SIZE;
            size--;
            if (size == 0) {
                frontIndex = rearIndex = -1;
            }
            break;
        case Operation::CLEAR:
            frontIndex = rearIndex = -1;
            size = 0;
            break;
    }

    historyList->setCurrentRow(currentHistoryIndex);
    updateButtons();
    updateVisualization();
    setStatusMessage("Redo: " + historyList->item(currentHistoryIndex)->text());
}

void QueueVisualizer::animateOperation()
{
    if (!isAnimating) return;

    // Animation logic here
    animationStep++;
    if (animationStep >= 10) {
        isAnimating = false;
        animationTimer->stop();
        animationStep = 0;
    }
    updateVisualization();
}

QVector<int> QueueVisualizer::getCurrentState() const
{
    QVector<int> state;
    if (!isEmpty()) {
        if (frontIndex <= rearIndex) {
            for (int i = frontIndex; i <= rearIndex; i++) {
                state.append(arr[i]);
            }
        } else {
            for (int i = frontIndex; i < MAX_SIZE; i++) {
                state.append(arr[i]);
            }
            for (int i = 0; i <= rearIndex; i++) {
                state.append(arr[i]);
            }
        }
    }
    return state;
}

QString QueueVisualizer::getStatusMessage() const
{
    return statusMessage;
}

void QueueVisualizer::setStatusMessage(const QString &message)
{
    statusMessage = message;
    emit statusChanged(message);
}
