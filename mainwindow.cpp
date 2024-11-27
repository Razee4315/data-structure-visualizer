#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "stackvisualizer.h"
#include "queuevisualizer.h"
#include "infixtopostfix.h"
#include <QInputDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolTip>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Set window icon
    QIcon appIcon(":/resources/logo.svg");
    setWindowIcon(appIcon);
    
    // Set initial window size
    resize(1024, 768);  
    setMinimumSize(800, 600);  

    // Create visualizers
    stackVisualizer = new StackVisualizer(this);
    queueVisualizer = new QueueVisualizer(this);
    infixToPostfix = new InfixToPostfix(this);

    // Add tabs
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    tabWidget->addTab(stackVisualizer, "Stack Visualizer");
    tabWidget->addTab(queueVisualizer, "Queue Visualizer");
    tabWidget->addTab(infixToPostfix, "Infix to Postfix");

    // Setup menu bar
    setupMenuBar();

    // Set window title
    setWindowTitle("Data Structure Visualizer - Created by Saqlan (github@Razee4315)");

    // Create status bar
    statusBar()->showMessage("Ready");

    // Center the window on the screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenuBar()
{
    QMenu *helpMenu = menuBar()->addMenu("Help");

    // Stack help
    QAction *stackHelpAction = helpMenu->addAction("Stack Help");
    connect(stackHelpAction, &QAction::triggered, this, &MainWindow::showStackHelp);

    // Queue help
    QAction *queueHelpAction = helpMenu->addAction("Queue Help");
    connect(queueHelpAction, &QAction::triggered, this, &MainWindow::showQueueHelp);

    // Infix to Postfix help
    QAction *infixToPostfixHelpAction = helpMenu->addAction("Infix to Postfix Help");
    connect(infixToPostfixHelpAction, &QAction::triggered, this, &MainWindow::showInfixToPostfixHelp);

    helpMenu->addSeparator();

    // About action
    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::showStackHelp()
{
    QString helpText = 
        "Stack Operations Help:\n\n"
        "1. Push: Adds a new element to the top of the stack\n"
        "   - If the stack is full, you'll get a 'Stack Overflow' error\n\n"
        "2. Pop: Removes and returns the top element\n"
        "   - If the stack is empty, you'll get a 'Stack Underflow' error\n\n"
        "3. Peek: Views the top element without removing it\n"
        "   - Shows 'Stack is empty' if there are no elements\n\n"
        "4. Clear: Removes all elements from the stack\n\n"
        "Stack Properties:\n"
        "- Maximum capacity: 5 elements\n"
        "- Last-In-First-Out (LIFO) principle\n"
        "- Elements are added and removed from the top";
    
    QMessageBox::information(this, "Stack Help", helpText);
}

void MainWindow::showQueueHelp()
{
    QString helpText = 
        "Queue Operations Help:\n\n"
        "1. Enqueue: Adds a new element to the rear of the queue\n"
        "   - If the queue is full, you'll get a 'Queue Overflow' error\n\n"
        "2. Dequeue: Removes and returns the front element\n"
        "   - If the queue is empty, you'll get a 'Queue Underflow' error\n\n"
        "3. Front: Views the front element without removing it\n"
        "   - Shows 'Queue is empty' if there are no elements\n\n"
        "4. Rear: Views the last element in the queue\n"
        "   - Shows 'Queue is empty' if there are no elements\n\n"
        "5. Clear: Removes all elements from the queue\n\n"
        "Queue Properties:\n"
        "- Maximum capacity: 5 elements\n"
        "- First-In-First-Out (FIFO) principle\n"
        "- Elements are added at rear and removed from front";
    
    QMessageBox::information(this, "Queue Help", helpText);
}

void MainWindow::showInfixToPostfixHelp()
{
    QMessageBox::information(this, "Infix to Postfix Help",
        "Infix to Postfix Converter:\n\n"
        "1. Enter an infix expression (e.g., A+B*C)\n"
        "2. Click 'Start' to begin conversion\n"
        "3. Click 'Next Step' to proceed through each step\n"
        "4. Use 'Reset' to start over\n\n"
        "Operator Precedence:\n"
        "^ (highest)\n"
        "* /\n"
        "+ - (lowest)\n\n"
        "The converter shows the stack operations and builds the postfix expression step by step.");
}

void MainWindow::showAboutDialog()
{
    QString aboutText = 
        "Data Structure Visualizer\n\n"
        "An educational tool for learning about basic data structures.\n\n"
        "Features:\n"
        "- Stack visualization (LIFO)\n"
        "- Queue visualization (FIFO)\n"
        "- Infix to Postfix Conversion\n"
        "- Interactive operations\n"
        "- Visual feedback\n\n"
        "Created with Qt 6";
    
    QMessageBox::about(this, "About Data Structure Visualizer", aboutText);
}
