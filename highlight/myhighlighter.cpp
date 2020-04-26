#include "myhighlighter.h"
#include<QtDebug>

MyHighLighter::MyHighLighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)//重写了new时的方法
{
    HighlightingRule rule;

    //用于一大长串的关键字匹配
    /*keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\b\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }*/

    //对于HTML标签，标为深蓝色
    HTMLFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = HTMLFormat;
    highlightingRules.append(rule);

    //对于标题，标粗
    TitleFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("^(#+)(.*)");
    rule.format = TitleFormat;
    highlightingRules.append(rule);

    //对于链接括号，标为棕色下划线
    LinkFormat.setForeground(Qt::darkRed);
    LinkFormat.setUnderlineColor(Qt::darkRed);
    LinkFormat.setFontUnderline(true);
    rule.pattern = QRegExp("(\\[.+\\]\\([^\\)]+\\))");
    rule.format = LinkFormat;
    highlightingRules.append(rule);




    //对于列表，绿
    ListFormat.setForeground(Qt::darkGreen);
    rule.format = ListFormat;
    rule.pattern = QRegExp("^[\\s]*[0-9]+\\.(.*)");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("^[\\s]*[-\\*\\+] +(.*)");
    highlightingRules.append(rule);

    //对于选择列表，蓝
    TodoFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("- \\[( |X|x)\\] ");
    rule.format = TodoFormat;
    highlightingRules.append(rule);

    //对于斜体
    ItalicFormat.setFontItalic(true);
    rule.pattern = QRegExp("\\*\\S+\\*");
    rule.format = ItalicFormat;
    highlightingRules.append(rule);

    //对于粗体
    TitleFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\*\\*.+\\*\\*");
    rule.format = TitleFormat;
    highlightingRules.append(rule);

    //对于引用块,灰色
    QuoteFormat.setFontItalic(true);
    QuoteFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("^(&gt;|\\>)(.*)");
    rule.format = QuoteFormat;
    highlightingRules.append(rule);


    //对于多行的标红，在highlightblock那块
    multiLineCommentFormat.setForeground(Qt::red);
}

void MyHighLighter::highlightBlock(const QString &text)
{
    //对于所有HighLightRule规则
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);//从头开始，到脚标上
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }


    //对于多行的标红,666代表从该行红
    if(text!="```"){
        if(previousBlockState()==666){//上一块是红的，就跟着红
            setFormat(0, text.length(), multiLineCommentFormat);
            setCurrentBlockState(666);
        }else{
            setCurrentBlockState(0);
        }
    }else{
        setFormat(0, text.length(), multiLineCommentFormat);
        if(previousBlockState()==666)//上一块是红的，注释结束，否则就是开始
            setCurrentBlockState(0);
        else
            setCurrentBlockState(666);

    }


    /*while (startIndex >= 0) {//找到所有开始标识符
        int endIndex = commentEndExpression.indexIn(text, startIndex+1);
        int commentLength;
        if (endIndex == -1) {//终止标识符
            setCurrentBlockState(1);//表示这行搞定了
            commentLength = text.length() - startIndex;//从头红到尾
            startIndex = -1;

        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();//到终止标识符
            startIndex = commentStartExpression.indexIn(text, endIndex+1);//继续找下一个有开始标识符的位置
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);//设为红色
    }*/
}
