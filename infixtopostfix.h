#ifndef INFIXTOPOSTFIX_H
#define INFIXTOPOSTFIX_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStack>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class InfixToPostfix : public QWidget
{
    Q_OBJECT

public:
    explicit InfixToPostfix(QWidget *parent = nullptr);
    ~InfixToPostfix();

private slots:
    void startConversion();
    void nextStep();
    void resetConversion();

private:
    // UI Elements
    QGraphicsScene *scene;
    QGraphicsView *view;
    QLineEdit *inputField;
    QPushButton *startButton;
    QPushButton *nextButton;
    QPushButton *resetButton;
    QLabel *explanationLabel;
    QLabel *resultLabel;

    // Conversion state
    QString infixExpression;
    QString postfixResult;
    QStack<QChar> operatorStack;
    int currentIndex;
    bool conversionInProgress;

    // Animation
    QTimer *animationTimer;
    bool isAnimating;
    int animationStep;

    // Helper functions
    int getPrecedence(QChar op) const;
    bool isOperator(QChar ch) const;
    void updateVisualization();
    void drawOperatorStack();
    void drawExpressionProgress();
    void drawCurrentStep(const QString &stepExplanation);
    void highlightCurrentCharacter();
    void processCurrentCharacter();
    QString getCurrentStepExplanation() const;
};

#endif // INFIXTOPOSTFIX_H
