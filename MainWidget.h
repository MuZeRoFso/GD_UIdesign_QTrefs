#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QWidget>
#include <QSystemTrayIcon>
#include "TitleBar.h"
#include "subFunc/Funcwidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;

private:
    QIcon *ico;
    QSystemTrayIcon *sysico;
    TitleBar *titlebar;
    FunctionalWidget *funcwidget;
    QSize *winsize;
    QVBoxLayout *mainlayout;
    int diffX,diffY;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onButtonMinClicked();
    void onButtonExitClicked();
};
#endif // MAINWIDGET_H
