/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "previewpage.h"
#include "ui_mainwindow.h"
#include"highlight/myhighlighter.h"

#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QTextStream>
#include <QWebChannel>
#include<QFontDialog>
#include<QFont>
#include<QtDebug>
#include <QCloseEvent>
#include<QInputDialog>
#include<QMimeData>



MainWindow::MainWindow(QWidget *parent,const QString &path) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/3rdparty/breeze-icon/default.png"));
    //编辑器用系统字体，禁止webview的菜单
    ui->editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->preview->setContextMenuPolicy(Qt::NoContextMenu);
    //允许拖放
    setAcceptDrops(true);
    //喵高亮系统
    MyHighLighter *high = new MyHighLighter(ui->editor->document());

    //webengine的页面
    PreviewPage *page = new PreviewPage(this);
    ui->preview->setPage(page);

    //如果接收到改变内容的信号，就把新内容发送给m_content，一个继承自Qobject的document类
    connect(ui->editor, &QPlainTextEdit::textChanged,
            [this]() { m_content.setText(ui->editor->toPlainText()); });

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    //使用Index.html
    ui->preview->setUrl(QUrl("qrc:/index.html"));

    //文件菜单
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionNew_2, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionOpen_2, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSave_2, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionFont,&QAction::triggered,this,&MainWindow::ChangeFont);
    connect(ui->actionFind,&QAction::triggered,this,&MainWindow::Findsome);

    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            ui->actionSave, &QAction::setEnabled);

    QLocale loc;
    QString fname;
    //默认打开defalut.md
    if(path!=nullptr)
        openFile(path);
    else if(loc.bcp47Name()=="zh"){
        openFile(":/default.zh.md");
        this->setWindowTitle("MarkDown Editor");
        m_filePath.clear();
    }
    else{
        openFile(":/default.md");
        this->setWindowTitle("MarkDown Editor");
        m_filePath.clear();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &path,const bool isForceCodec)
{//打开文件进程
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
    m_filePath = path;

    //要先把数据取出来，用tried进行存储
    QByteArray fReadAll;
    QString tried="";
    bool flag=false;
    fReadAll=f.readAll();
    //自动获取字节编码，很不稳定
    QTextCodec::ConverterState status;

    tried=m_codec->toUnicode(fReadAll.constData(),fReadAll.size(),&status);
    if(status.invalidChars>0 && !isForceCodec){//如果UTF-8不OK
        m_codec=m_codec->codecForName("GB18030");//排除奇怪的ISO-8859-1
        foreach (QByteArray i, QTextCodec::availableCodecs()) {//先试试
           QTextCodec::ConverterState statu;
           m_codec=m_codec->codecForName(i);
           tried=m_codec->toUnicode(fReadAll.constData(),fReadAll.size(),&statu);
           qDebug()<<i<<statu.invalidChars<<tried;
           if(statu.invalidChars==0){
               flag=true;
               break;
           }
        }
    }else{
        flag=true;
    }
    if(!flag){
        QMessageBox::warning(this, windowTitle(),
                             tr("File %1's Codec Detect failed, try to set another Codec on edit menu").arg(
                                 QDir::toNativeSeparators(path)));
        m_codec=m_codec->codecForName("UTF-8");//实在不行，就还回到UTF-8
        tried=m_codec->toUnicode(fReadAll.constData(),fReadAll.size(),&status);
    }
    ui->editor->setPlainText(tried);
    status.invalidChars=-1;
    this->setWindowTitle("MarkDown Editor - "+m_filePath);
}


bool MainWindow::isModified() const
{//检查是否更改过
    return ui->editor->document()->isModified();
}

void MainWindow::onFileNew()
{//新建文件
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to create a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    m_filePath.clear();
    m_codec->codecForName("UTF-8");//默认以UTF-8新建
    this->setWindowTitle("MarkDown Editor");
    ui->editor->setPlainText(tr("## New document"));
    ui->editor->document()->setModified(false);
}

void MainWindow::onFileOpen()
{//打开文件对话框
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    QString path = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", "MarkDown File (*.md *.markdown);;TxT File (*.txt);;All files(*.*)");
    if (path.isEmpty())
        return;
    m_codec=m_codec->codecForName("UTF-8");
    openFile(path);
}

void MainWindow::onFileSave()
{//保存文件
    if (m_filePath.isEmpty()) {
        onFileSaveAs();
        return;
    }

    QFile f(m_filePath);
    m_filePath=f.fileName();
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(m_filePath), f.errorString()));
        return;
    }
    QTextStream str(&f);
    if(m_codec!=nullptr)
        str.setCodec(m_codec);
    str << ui->editor->toPlainText();

    //保存完成后去除*
    this->setWindowTitle("MarkDown Editor - "+m_filePath);


    ui->editor->document()->setModified(false);
}


//当用户拖动文件到窗口部件上时候，就会触发dragEnterEvent事件
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug()<<event->mimeData()->formats()<<"size"<<event->mimeData()->formats().size();
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}

//当用户放下这个文件后，就会触发dropEvent事件
void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug()<<"drop file:"<<event->mimeData()->urls();
    if(event->mimeData()->urls().size()>1)
        QMessageBox::warning(this, tr("File Drops"),tr("Droping more than 1 file is not allowed"));
    else{
        if (isModified()) {
            QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                                 tr("You have unsaved changes. Do you want to open a new document anyway?"));
            if (button != QMessageBox::Yes)
                return;
        }
        openFile(event->mimeData()->urls()[0].toLocalFile());
    }
}


void MainWindow::onFileSaveAs()
{//另存为文件
    QString path = QFileDialog::getSaveFileName(this,
        tr("Save MarkDown File"), "", tr("MarkDown File (*.md *.markdown)"));
    if (path.isEmpty())
        return;
    m_filePath = path;
    onFileSave();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //退出
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to exit anyway?"));
        if (button != QMessageBox::Yes)
            event->ignore();
        else
            event->accept();
   }
}

void MainWindow::ChangeFont()
{//更改编辑器字体
    bool ok;
    QFont fonta = QFontDialog::getFont(&ok);
    if(ok)
        ui->editor->setFont(fonta);
}


void MainWindow::Findsome()
{//查找
    bool isOK;
    search=QInputDialog::getText(NULL, tr("Search"),tr("Search what?"),QLineEdit::Normal,search,&isOK);
    if(isOK){
        if(!ui->editor->find(search)){
            if(!ui->editor->find(search,QTextDocument::FindBackward)){
                QMessageBox::warning(this, tr("Search"),
                            tr("Can not found %1").arg(search));
            }else{
                ui->preview->findText(search);
            }
        }else{
            ui->preview->findText(search);
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{//关于
    QMessageBox::information(this,tr("About"),tr("Powered by XQQY Meow～Ver0.5"));
}

void MainWindow::on_actionWC_triggered()
{//字数统计
    QMessageBox::information(this,tr("word count"),tr("Total word:%1").arg(ui->editor->toPlainText().size()));
}

void MainWindow::on_editor_selectionChanged()
{//当更改选区时，结束对webview的搜索
    ui->preview->findText("");
}

void MainWindow::on_actionPreview_toggled(bool arg1)
{//打开预览
    ui->preview->setHidden(!arg1);
}

void MainWindow::on_actionView_mode_toggled(bool arg1)
{//打开预览
    ui->editor->setHidden(arg1);
}

void MainWindow::on_actionExit_triggered()
{//退出程序
    close();
}

void MainWindow::on_actionHow_triggered()
{//如何？
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to open a help document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
    QString fname;
    QLocale loc;
    if(loc.bcp47Name()=="zh")
        fname=":/default.zh.md";
    else
        fname=":/default.md";
    openFile(fname);
}

void MainWindow::on_editor_textChanged()
{//给窗口标题加个*
    if(!this->windowTitle().endsWith("*"))
        this->setWindowTitle(this->windowTitle()+"*");
}

void MainWindow::on_actionUTF_8_triggered()
{//使用UTF-8编码
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to open a help document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
    m_codec=m_codec->codecForName("UTF-8");
    openFile(m_filePath,true);

}

void MainWindow::on_actionGBK_triggered()
{//使用GBK编码
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to open a help document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
    m_codec=m_codec->codecForName("GB18030");
    openFile(m_filePath,true);
}

void MainWindow::on_actionCodecOthers_triggered()
{
    //使用其他编码
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                             tr("You have unsaved changes. Do you want to open a help document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
        bool isOK;
        QString codec=QInputDialog::getText(NULL, tr("Codec"),tr("Enter a Codec name"),QLineEdit::Normal,search,&isOK);
        if(isOK){
            m_codec=m_codec->codecForName(codec.toUtf8());
            openFile(m_filePath,true);
        }
}

void MainWindow::on_actionCodecAuto_triggered()
{
    m_codec=m_codec->codecForName("UTF-8");
    openFile(m_filePath,false);
}
