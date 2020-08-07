#include "showhtml.h"
#include "ui_showhtml.h"

ShowHtml::ShowHtml(QWidget *parent,QString info_) :
    QDialog(parent),
    ui(new Ui::ShowHtml)
{
    ui->setupUi(this);
    ui->textBrowser->setMarkdown(info_);
    ui->textBrowser->setPlainText(ui->textBrowser->toHtml());
}
ShowHtml::~ShowHtml()
{
    delete ui;
}

void ShowHtml::on_pushButton_clicked()
{
    this->done(0);
}
