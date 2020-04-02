#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
class MyHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MyHighLighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;//多行注释用开始标识符
    QRegExp commentEndExpression;//多行注释用结束标识符

    QTextCharFormat HTMLFormat;//HTML关键字
    QTextCharFormat TitleFormat;//#标题
    QTextCharFormat LinkFormat;//链接
    QTextCharFormat QuoteFormat;//引用块
    QTextCharFormat TodoFormat;//TODO列表
    QTextCharFormat ListFormat;//有序列表无序列表
    QTextCharFormat multiLineCommentFormat;//多行注释
    QTextCharFormat ItalicFormat;//方法标识符
};
#endif // MYHIGHLIGHTER_H
