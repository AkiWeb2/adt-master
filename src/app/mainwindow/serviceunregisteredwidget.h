#ifndef SERVICEUNREGISTERESWIDGET_H
#define SERVICEUNREGISTERESWIDGET_H

#include <QObject>
#include <QTimer>
#include <QWidget>

const unsigned int INTERVAL         = 500;
const unsigned int NUMBER_OF_CYCLES = 3;

namespace Ui
{
class ServiceUnregisteredWidget;
}

class ServiceUnregisteredWidget : public QWidget
{
    Q_OBJECT
public:
    enum class ButtonPressed
    {
        none,
        closeAndExit,
        close
    };

public:
    ServiceUnregisteredWidget(QWidget *parent = nullptr);
    ~ServiceUnregisteredWidget();

    void show();

    void startAnimation();

    ButtonPressed getPressedButton();

signals:
    void closeAndExit();
    void closeAll();

private:
    Ui::ServiceUnregisteredWidget *ui;

    unsigned int m_counter;

    QTimer *m_animationTimer;

    ButtonPressed m_buttonPressed;

private slots:

    void animationTick();

    void on_closeAndExitButton_clicked();

    void on_closeButton_clicked();

private:
    ServiceUnregisteredWidget(const ServiceUnregisteredWidget &) = delete;
    ServiceUnregisteredWidget(ServiceUnregisteredWidget &&)      = delete;
    ServiceUnregisteredWidget &operator=(const ServiceUnregisteredWidget &) = delete;
    ServiceUnregisteredWidget &operator=(ServiceUnregisteredWidget &&) = delete;
};

#endif // SERVICEUNREGISTERESWIDGET_H
