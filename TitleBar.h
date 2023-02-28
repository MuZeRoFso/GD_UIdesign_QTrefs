#ifndef TITLEBAR_H
#define TITLEBAR_H
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QFile>

#define BUTTON_WIDTH 30
#define BUTTON_HEIGHT 30
#define TITLE_BAR_HEIGHT 50

class TitleBar : public QWidget
{
/************************************
 * 该类用于定制系统标题栏
 * 实现 图标 标题 收起 最大/小化 关闭(退出)
************************************/
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar() override;
    void setTitleBarWidth(const QSize &size);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void signalButtonMinClicked();
    void signalButtonExitClicked();

private slots:
    void onButtonMinClicked();
    void onButtonExitClicked();


private:
    QPixmap *titleicon;    //图标
    QPixmap *minicon;
    QPixmap *exiticon;
    QLabel *icon;     //显示图标
    QLabel *l_icon;
    QLabel *title;  //标题
    QPushButton *b_min;     //最小化
    QPushButton *b_exit;    //退出
    QHBoxLayout *baselayout;    //标题栏布局
    int diffX,diffY;            //移动窗体变量

    void init();
    void builtConnections();
};

#endif // TITLEBAR_H
