#ifndef LATTICE_VIEW_LABEL_H
#define LATTICE_VIEW_LABEL_H

#include <qobject.h>
#include <qpainter.h>

#include <iostream>

using namespace std;

#ifndef SARL_STRING_TABLE_H
#include "sarl_string_table.h"
#endif

#ifndef SARL_BIT_SET_H
#include "sarl_bit_set.h"
#endif

class SarlLatticeViewLabelStyle {
public:
  QFont        font;
  QFontMetrics metrics;
  int          margin;

  SarlLatticeViewLabelStyle(QFont a_font = QFont("Arial", 18)) 
    : font(a_font), metrics(a_font), margin(5) {};

  QRect boundingBox(sarl_string_ptr str) {
    return metrics.boundingRect(str);
  }

  void setStyleOn(QPainter* painter) {
    painter->setBrush(QBrush(QBrush::white));
    painter->setFont(font);
  }
};

class SarlLatticeViewLabel : public QObject
{
  Q_OBJECT;
public:
  sarl_unsigned_int    concept;
  sarl_bit_set_t*      bset;
  sarl_string_table_t* stab;

  QRect                      boundingBox;
  SarlLatticeViewLabelStyle* style;


  SarlLatticeViewLabel(
     sarl_unsigned_int   a_concept, 
     sarl_bit_set_t      *a_bset, 
     sarl_string_table_t *a_stab,
     SarlLatticeViewLabelStyle      *a_style
  ) 
    : concept(a_concept), bset(a_bset), stab(a_stab), boundingBox(0, 0, 0, 0), 
      style(a_style)
  {
    updateBoundingBox();
  };

  void centerTopOn(int x, int y)
  {
    boundingBox.moveTopLeft(QPoint(x - (boundingBox.width()/2), y));
  };

  void centerBottomOn(int x, int y)
  {
    boundingBox.moveBottomRight(QPoint(x + (boundingBox.width()/2), y));
  };

  void updateBoundingBox() 
  {
    sarl_unsigned_int i, count = 0;
    SARL_BIT_SET_FOR_EACH(i, bset);
      QRect textBox = style->boundingBox(sarl_string_table_get(stab, i));
      boundingBox.setRight(
        sarl_max_of(
          boundingBox.right(), 
          textBox.right())
      );
      boundingBox.setLeft(
        sarl_min_of(
          boundingBox.left(), 
          textBox.left())
      );
      ++count;
    SARL_BIT_SET_END;
    boundingBox.setBottom(style->metrics.height() * count);
    boundingBox.addCoords(-style->margin, -style->margin, style->margin, style->margin);
  };

  void paint(QPainter* painter, int dx, int dy) 
  {
    sarl_unsigned_int i;
    QRect shiftBox = boundingBox;
    shiftBox.moveTopLeft(QPoint(shiftBox.left() + dx, shiftBox.top() + dy));
    int x = shiftBox.left() + style->margin;
    int height = style->metrics.height();
    int descent = style->metrics.descent();
    int y = shiftBox.top() + height + style->margin - descent;
    painter->fillRect(shiftBox, QBrush::white);
    painter->drawRect(shiftBox);
    SARL_BIT_SET_FOR_EACH(i, bset);
    //      cout << "Paint Label: '" << sarl_string_table_get(stab, i) << "'";
    //      cout << " at (" << x << "," << y << "), i=" << i << endl;
      painter->drawText(x, y, sarl_string_table_get(stab, i));
      y += height;
    SARL_BIT_SET_END;
  }
};

#endif
