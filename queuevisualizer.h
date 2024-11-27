#ifndef QUEUEVISUALIZER_H
#define QUEUEVISUALIZER_H

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

class QueueVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit QueueVisualizer(QWidget *parent = nullptr);
    ~QueueVisualizer();

    bool enqueue(int value);
    int dequeue();
    int front() const;
    int rear() const;
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
    // Constants
    static const int MAX_SIZE = 5;

    // Queue state
    int arr[MAX_SIZE];
    int frontIndex;
    int rearIndex;
    int size;

    // Animation state
    bool isAnimating;
    int animationStep;

    // Status message
    QString statusMessage;

    // Operation history
    struct Operation {
        enum Type { ENQUEUE, DEQUEUE, CLEAR } type;
        int value;
        QVector<int> prevState;
    };
    QVector<Operation> history;
    int currentHistoryIndex;

    // UI Elements
    QLineEdit *inputField;
    QPushButton *enqueueButton;
    QPushButton *dequeueButton;
    QPushButton *frontButton;
    QPushButton *rearButton;
    QPushButton *clearButton;
    QPushButton *undoButton;
    QPushButton *redoButton;
    QLabel *statusLabel;
    QListWidget *historyList;

    // Graphics and timer
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *animationTimer;

    void setupUI();
    void updateVisualization();
    void setStatusMessage(const QString &message);
    void updateButtons();
    void animateOperation();
    void addToHistory(Operation::Type type, int value);
    void drawBox(int index, int value, const QString &color = "white");
    void updateCapacityIndicator();
};

#endif // QUEUEVISUALIZER_H
