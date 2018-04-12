#ifndef QUMBIANEWCONTROLW_H
#define QUMBIANEWCONTROLW_H

#include <QWidget>
#include <QMap>

class QLineEdit;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

private slots:
    void nameTextChanged(const QString& txt);
    void setTextEdited(const QString& s = QString());

    void changeOutDir();

    void generate();

    void outDirTextChanged(const QString &s);

    void parentClassNameChanged(const QString& cn);

    void m_genIncludeMap();

    void updateBrief();

private:
    QMap<QString, QString> m_includeMap;

    void m_add_to_pro();

    QString m_replace_from_template(const QString& templ);

    QString m_getTemplateContents(const QLineEdit *le);

    bool m_error;

    QString m_message;
};

#endif // QUMBIANEWCONTROLW_H
