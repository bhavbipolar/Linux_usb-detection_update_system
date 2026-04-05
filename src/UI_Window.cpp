#include "UI_Window.h"

UI_Window::UI_Window(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,100);

    statusLabel = new QLabel("Waiting for USB...", this);

    startButton = new QPushButton("Update Now", this);
    cancelButton = new QPushButton("Cancel", this);

    layout->addWidget(progressBar);
    layout->addWidget(statusLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(startButton);

    layout->addLayout(buttonLayout);

    connect(startButton,&QPushButton::clicked,this,[this](){
        emit startCopyRequested();
    });

    connect(cancelButton,&QPushButton::clicked,this,[this](){
        emit cancelCopyRequested();
    });
}

void UI_Window::updateProgressBar(int value)
{
    progressBar->setValue(value);
}

void UI_Window::updateStatus(QString text)
{
    statusLabel->setText(text);
}