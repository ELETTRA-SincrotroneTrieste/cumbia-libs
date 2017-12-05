#ifndef EINPUTOUTPUTWIDGET_H
#define EINPUTOUTPUTWIDGET_H

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QAbstractItemView>

class EReaderWriterPrivate;
class QPushButton;

class Container : public QFrame
{
  Q_OBJECT
public:
    Container(QWidget *parent);

signals:
    void visibilityChanged(bool);

protected:
    void hideEvent(QHideEvent *e);
};

class EInputOutputWidget : public QFrame
{
    Q_OBJECT
public:

    EInputOutputWidget(QWidget *outputw, QWidget *parent);

    EInputOutputWidget(QWidget *parent);

    void setInputWidget(QWidget *inputw);

    void setOutputWidget(QWidget *outputw);

    QSize minimumSizeHint() const;

    QSize sizeHint() const;

    QWidget *outputWidget() const;

    QWidget *inputWidget() const;

    QFrame *getContainer() const;

    virtual QPushButton *getApplyButton();

public slots:

    void setInputValue(double val);

    void setInputValue(int val);

    void setInputText(const QString& text);

    void setOutputValue(int val);

    void setOutputValue(double val);

    void setOutputText(const QString& text);


signals:

    void editButtonToggled(bool toggled);

    void applyClicked();

    void applyClicked(double val);

    void applyClicked(const QString& text);

    void editButtonClicked();

private slots:
    void m_editToggled(bool en);
    void m_applyClicked();

protected:

private:
    EReaderWriterPrivate *d;

    void m_init(QWidget *outputw);
    void m_createContainer();
    void m_showWriter();
    void m_show();
    void m_setValue(const QVariant &d, QWidget *w);
    void m_setText(const QString& s,  QWidget *w);
};



#endif // EREADERWRITER_H
