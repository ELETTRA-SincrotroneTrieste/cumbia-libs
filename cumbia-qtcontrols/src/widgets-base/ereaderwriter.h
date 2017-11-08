#ifndef EREADERWRITER_H
#define EREADERWRITER_H

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include "elabel.h"

class EReaderWriterPrivate;



class EReaderWriter : public ELabel
{
    Q_OBJECT
    Q_PROPERTY(WriterType writerType READ writerType WRITE setWriterType  DESIGNABLE true)
public:
    enum WriterType { SpinBox, DoubleSpinBox, ComboBox, LineEdit,  Numeric};
    enum WriterPosition { AutoPosition, North, South, East, West };

    EReaderWriter(QWidget *parent);

    WriterType writerType() const;

    QSize minimumSizeHint() const;

    QSize sizeHint() const;


public slots:

    void setWriterType(WriterType t);

    void setValue(double d);

    void setValue(int i);

    void setValue(const QString& s);

signals:

    void editButtonToggled(bool toggled);

    void applyClicked();

    void editButtonClicked();

private slots:
    void m_editToggled(bool en);

protected:
  virtual void enterEvent(QEvent *e);
  virtual void leaveEvent(QEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mousePressEvent(QMouseEvent *);

private:
    EReaderWriterPrivate *d;

    void m_setData(const QVariant& d);
    void m_init();
    void m_showWriter();
    void m_repositionAll();
    void m_showEditButton();
};



#endif // EREADERWRITER_H
