#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class UI_Window : public QWidget
{
    Q_OBJECT

public:
    explicit UI_Window(QWidget *parent = nullptr);

public slots:
    void updateProgressBar(int value);
    void updateStatus(QString text);

signals:
    void startCopyRequested();
    void cancelCopyRequested();

private:
    QProgressBar *progressBar;
    QPushButton *startButton;
    QPushButton *cancelButton;
    QLabel *statusLabel;
};

#endif