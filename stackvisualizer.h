#ifndef STACKVISUALIZER_H
#define STACKVISUALIZER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QStack>
#include <QString>
#include <QListWidget>
#include <QVector>

class QPushButton;
class QLineEdit;
class QLabel;

class StackVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit StackVisualizer(QWidget *parent = nullptr);
    ~StackVisualizer();

    bool push(int value);
    int pop();
    int peek() const;
    bool isEmpty() const;
    bool isFull() const;
    void clear();
    QVector<int> getCurrentState() const;
    QString getStatusMessage() const;
    void undoOperation();
    void redoOperation();

signals:
    void statusChanged(const QString &message);
    void stateChanged();

private:
    // Operation history struct
    struct Operation {
        enum Type { PUSH, POP, CLEAR } type;
        int value;
        QVector<int> prevState;
    };

    static const int MAX_SIZE = 5;
    int arr[MAX_SIZE];
    int top;
    bool isAnimating;
    int animationStep;
    QString statusMessage;
    QVector<Operation> history;
    int currentHistoryIndex;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *animationTimer;
    QListWidget *historyList;
    
    // UI Elements
    QLineEdit *inputField;
    QPushButton *pushButton;
    QPushButton *popButton;
    QPushButton *peekButton;
    QPushButton *clearButton;
    QPushButton *undoButton;
    QPushButton *redoButton;
    QLabel *statusLabel;

    void setupUI();
    void updateVisualization();
    void setStatusMessage(const QString &message);
    void updateButtons();
    void animateOperation();
    void addToHistory(Operation::Type type, int value);
    void drawBox(int index, int value, const QString &color = "white");
    void updateCapacityIndicator();
};

#endif // STACKVISUALIZER_H
