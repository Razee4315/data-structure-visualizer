#include "infixtopostfix.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QGroupBox>
#include <QFont>

InfixToPostfix::InfixToPostfix(QWidget *parent)
    : QWidget(parent)
    , currentIndex(0)
    , conversionInProgress(false)
    , isAnimating(false)
    , animationStep(0)
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Create input group
    QGroupBox *inputGroup = new QGroupBox("Input Expression", this);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputGroup);
    inputLayout->setSpacing(5);
    
    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("Enter infix expression (e.g., A+B*C)");
    inputField->setMinimumWidth(200);
    QFont inputFont = inputField->font();
    inputFont.setPointSize(10);
    inputField->setFont(inputFont);

    startButton = new QPushButton("Start", this);
    nextButton = new QPushButton("Next Step", this);
    resetButton = new QPushButton("Reset", this);
    
    // Style buttons
    QString buttonStyle = "QPushButton { padding: 5px 15px; }";
    startButton->setStyleSheet(buttonStyle);
    nextButton->setStyleSheet(buttonStyle);
    resetButton->setStyleSheet(buttonStyle);
    nextButton->setEnabled(false);

    inputLayout->addWidget(inputField, 3);
    inputLayout->addWidget(startButton, 1);
    inputLayout->addWidget(nextButton, 1);
    inputLayout->addWidget(resetButton, 1);

    // Create visualization area
    QGroupBox *visualGroup = new QGroupBox("Visualization", this);
    QVBoxLayout *visualLayout = new QVBoxLayout(visualGroup);
    
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setMinimumHeight(200);
    view->setMaximumHeight(250);
    view->setStyleSheet("QGraphicsView { background-color: #f8f9fa; border: 1px solid #dee2e6; }");
    
    visualLayout->addWidget(view);

    // Create explanation area
    QGroupBox *explanationGroup = new QGroupBox("Step Explanation", this);
    QVBoxLayout *explanationLayout = new QVBoxLayout(explanationGroup);
    
    explanationLabel = new QLabel(this);
    explanationLabel->setWordWrap(true);
    explanationLabel->setStyleSheet("QLabel { background-color: #E3F2FD; padding: 10px; border-radius: 5px; }");
    explanationLabel->setMinimumHeight(50);
    explanationLabel->setMaximumHeight(70);
    QFont explanationFont = explanationLabel->font();
    explanationFont.setPointSize(10);
    explanationLabel->setFont(explanationFont);
    
    resultLabel = new QLabel(this);
    resultLabel->setStyleSheet("QLabel { font-weight: bold; padding: 5px; color: #2196F3; }");
    resultLabel->setFont(explanationFont);

    explanationLayout->addWidget(explanationLabel);
    explanationLayout->addWidget(resultLabel);

    // Add all groups to main layout
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(visualGroup);
    mainLayout->addWidget(explanationGroup);

    // Setup animation timer
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &InfixToPostfix::updateVisualization);

    // Connect signals
    connect(startButton, &QPushButton::clicked, this, &InfixToPostfix::startConversion);
    connect(nextButton, &QPushButton::clicked, this, &InfixToPostfix::nextStep);
    connect(resetButton, &QPushButton::clicked, this, &InfixToPostfix::resetConversion);
}

InfixToPostfix::~InfixToPostfix()
{
    delete scene;
}

void InfixToPostfix::startConversion()
{
    infixExpression = inputField->text().trimmed();
    if (infixExpression.isEmpty()) {
        explanationLabel->setText("Please enter an expression first!");
        return;
    }

    // Initialize conversion
    currentIndex = 0;
    postfixResult.clear();
    operatorStack.clear();
    conversionInProgress = true;
    
    // Update UI state
    inputField->setEnabled(false);
    startButton->setEnabled(false);
    nextButton->setEnabled(true);
    
    // Start visualization
    updateVisualization();
    explanationLabel->setText("Conversion started. Click 'Next Step' to proceed.");
}

void InfixToPostfix::nextStep()
{
    processCurrentCharacter();
}

void InfixToPostfix::resetConversion()
{
    // Reset state
    currentIndex = 0;
    postfixResult.clear();
    operatorStack.clear();
    conversionInProgress = false;
    
    // Reset UI
    inputField->setEnabled(true);
    startButton->setEnabled(true);
    nextButton->setEnabled(false);
    explanationLabel->setText("");
    resultLabel->setText("");
    
    // Clear visualization
    updateVisualization();
}

void InfixToPostfix::processCurrentCharacter()
{
    if (currentIndex >= infixExpression.length()) {
        // Pop all remaining operators from the stack
        while (!operatorStack.isEmpty()) {
            QChar op = operatorStack.pop();
            if (op != '(' && op != ')') {
                postfixResult += op;
                explanationLabel->setText("Popping remaining operator: " + QString(op));
            } else {
                explanationLabel->setText("Found unmatched parenthesis - invalid expression");
            }
            updateVisualization();
        }
        
        // Conversion complete
        nextButton->setEnabled(false);
        startButton->setEnabled(true);
        conversionInProgress = false;
        resultLabel->setText("Final Result: " + postfixResult);
        return;
    }

    QChar currentChar = infixExpression[currentIndex];
    
    if (currentChar.isLetterOrNumber()) {
        postfixResult += currentChar;
        explanationLabel->setText("Added operand: " + QString(currentChar));
    }
    else if (currentChar == '(') {
        operatorStack.push(currentChar);
        explanationLabel->setText("Pushed opening parenthesis onto stack");
    }
    else if (currentChar == ')') {
        while (!operatorStack.isEmpty() && operatorStack.top() != '(') {
            postfixResult += operatorStack.pop();
        }
        if (!operatorStack.isEmpty() && operatorStack.top() == '(') {
            operatorStack.pop(); // Remove the '('
            explanationLabel->setText("Processed closing parenthesis - popped operators until matching '('");
        } else {
            explanationLabel->setText("Error: Unmatched closing parenthesis");
        }
    }
    else if (isOperator(currentChar)) {
        while (!operatorStack.isEmpty() && operatorStack.top() != '(' &&
               getPrecedence(operatorStack.top()) >= getPrecedence(currentChar)) {
            postfixResult += operatorStack.pop();
        }
        operatorStack.push(currentChar);
        explanationLabel->setText("Processed operator: " + QString(currentChar));
    }

    currentIndex++;
    updateVisualization();
}

bool InfixToPostfix::isOperator(QChar ch) const
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

int InfixToPostfix::getPrecedence(QChar op) const
{
    switch (op.toLatin1()) {
        case '^': return 3;
        case '*':
        case '/': return 2;
        case '+':
        case '-': return 1;
        default: return 0;
    }
}

void InfixToPostfix::updateVisualization()
{
    scene->clear();

    // Constants for visualization
    const int BOX_WIDTH = 40;
    const int BOX_HEIGHT = 40;
    const int SPACING = 20;
    const int INPUT_Y = 20;
    const int STACK_Y = 100;
    const int OUTPUT_Y = 180;
    const int START_X = 50;

    // Draw title for input expression
    QGraphicsTextItem *inputTitle = scene->addText("Input Expression:");
    inputTitle->setPos(START_X, INPUT_Y - 20);
    
    // Draw input expression with current position highlighted
    QString input = inputField->text();
    for (int i = 0; i < input.length(); ++i) {
        QGraphicsRectItem *box = scene->addRect(START_X + i * (BOX_WIDTH + 5), INPUT_Y, BOX_WIDTH, BOX_HEIGHT);
        if (i == currentIndex && conversionInProgress) {
            box->setBrush(QColor("#FFB74D")); // Highlight current character
        }
        QGraphicsTextItem *text = scene->addText(QString(input[i]));
        text->setPos(START_X + i * (BOX_WIDTH + 5) + BOX_WIDTH/4, INPUT_Y + BOX_HEIGHT/4);
    }

    // Draw title for stack
    QGraphicsTextItem *stackTitle = scene->addText("Stack:");
    stackTitle->setPos(START_X, STACK_Y - 20);
    
    // Draw stack
    for (int i = 0; i < operatorStack.size(); ++i) {
        QGraphicsRectItem *box = scene->addRect(START_X + i * (BOX_WIDTH + 5), STACK_Y, BOX_WIDTH, BOX_HEIGHT);
        box->setBrush(QColor("#81C784")); // Green for stack
        QGraphicsTextItem *text = scene->addText(QString(operatorStack[i]));
        text->setPos(START_X + i * (BOX_WIDTH + 5) + BOX_WIDTH/4, STACK_Y + BOX_HEIGHT/4);
    }

    // Draw title for output
    QGraphicsTextItem *outputTitle = scene->addText("Output:");
    outputTitle->setPos(START_X, OUTPUT_Y - 20);
    
    // Draw postfix output
    for (int i = 0; i < postfixResult.length(); ++i) {
        QGraphicsRectItem *box = scene->addRect(START_X + i * (BOX_WIDTH + 5), OUTPUT_Y, BOX_WIDTH, BOX_HEIGHT);
        box->setBrush(QColor("#64B5F6")); // Blue for output
        QGraphicsTextItem *text = scene->addText(QString(postfixResult[i]));
        text->setPos(START_X + i * (BOX_WIDTH + 5) + BOX_WIDTH/4, OUTPUT_Y + BOX_HEIGHT/4);
    }

    // Adjust view size to fit content
    view->setSceneRect(scene->itemsBoundingRect());
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
