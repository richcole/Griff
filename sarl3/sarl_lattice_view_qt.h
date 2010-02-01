#ifndef SARL_LATTICE_VIEW_H
#define SARL_LATTICE_VIEW_H

#include <qwidget.h>
#include <qpainter.h>

#include "sarl_wrapper.h"

struct sarl_concept_lattice_t;
struct sarl_lattice_view_private_t;

class SarlLatticeView : public QWidget
{
    Q_OBJECT

public:
    SarlLatticeView(
      SarlDiagram const& diagram,
      SarlStringTable const& object_names,
      SarlStringTable const& attr_names,
      QWidget *parent=0, 
      const char *name=0
   );

   ~SarlLatticeView();

public slots:

signals:
    void conceptSelected(
      sarl_unsigned_int concept_index, 
      QMouseEvent& event
    );

    void objectSelected(
      sarl_unsigned_int object_index, 
      QMouseEvent& event
    );

    void attributeSelected(
       sarl_unsigned_int attribute_index, 
       QMouseEvent& event
    );

protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void paintContents( QPaintEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );

    virtual sarl_unsigned_int conceptAt(sarl_signed_int x, sarl_signed_int y);

    virtual void paintLattice(
      QPainter* painter
    );

    virtual void paintConcept(
      sarl_unsigned_int concept_index, 
      sarl_signed_int x,
      sarl_signed_int y,
      QPainter* painter
    );

    virtual void paintLine(
      sarl_unsigned_int child_index, 
      sarl_signed_int cx,
      sarl_signed_int cy,
      sarl_unsigned_int parent_index, 
      sarl_signed_int px,
      sarl_signed_int py,
      QPainter* painter
    );

    void calcBoundingBox();
    void constructLabels();

private:
    sarl_lattice_view_private_t* m_private;
};

#endif // SARL_LATTICE_VIEW_H
