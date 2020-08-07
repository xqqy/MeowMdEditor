#ifndef SHOWHTML_H
#define SHOWHTML_H

#include <QDialog>

namespace Ui {
class ShowHtml;
}

class ShowHtml : public QDialog
{
    Q_OBJECT

public:
    explicit ShowHtml(QWidget *parent = nullptr,QString info_="");
    ~ShowHtml();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ShowHtml *ui;
    QString info;
};

#endif // SHOWHTML_H
