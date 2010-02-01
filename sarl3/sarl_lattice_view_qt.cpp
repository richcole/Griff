#include "sarl_lattice_view_qt.h"
#include "sarl_diagram.h"
#include "sarl_lattice.h"
#include "sarl_bit_set.h"
#include "sarl_rect.h"

#include "sarl_lattice_view_label.h"

#include "gc/gc.h"

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <sys/time.h>

#include <iostream>
#include <vector>

using namespace std;

#define sarl_lattice_view_trace_3(msg, x, y, z) \
  std::cout << msg; \
  std::cout << ( #x "=" ) << x; \
  std::cout << (", " #y "=") << y; \
  std::cout << (", " #z "=") << z << endl

// for a vector of pointers
template <class S, class I> 
inline void clear_and_delete_contents(S &s)
{
  I it = s.begin();
  while( it != s.end() ) {
    if ( *it == 0 ) { 
      delete (*it);
      *it = 0;
    };
    ++it;
  }
  s.clear();
};

typedef std::vector<SarlLatticeViewLabel *> LabelVector;

struct sarl_lattice_view_private_t
{
  sarl_lattice_view_private_t() {};

  SarlDiagram       diagram;
  SarlStringTable   object_names;
  SarlStringTable   attribute_names;

  sarl_rect_t*      bbox;
  QWMatrix          tr;
  QWMatrix          inv_tr;

  sarl_unsigned_int selected_concept;
  QPoint            old_pos;

  SarlLatticeViewLabelStyle label_style;
  LabelVector labels;
};

SarlLatticeView::SarlLatticeView(
  SarlDiagram const& diagram,
  SarlStringTable const& object_names,
  SarlStringTable const& attribute_names,
  QWidget *parent, 
  const char *name
)
{
  setWFlags(Qt::WNoAutoErase);
  m_private = new sarl_lattice_view_private_t();
  m_private->diagram = diagram;
  m_private->object_names = object_names;
  m_private->attribute_names = attribute_names;
  m_private->bbox = sarl_rect_new();
  calcBoundingBox();
  constructLabels();
};

void SarlLatticeView::constructLabels()
{
  clear_and_delete_contents<LabelVector, LabelVector::iterator>(
    m_private->labels
  );

  sarl_unsigned_int concept_index;
  sarl_bit_set_t* ob_cont, *at_cont;

  SARL_POWER_MAP_FOR_EACH(
    concept_index, ob_cont, 
    m_private->diagram->lattice->concept_object_cont
  );
  if ( ob_cont && sarl_bit_set_count(ob_cont) > 0 ) {
    SarlLatticeViewLabel* label = 
      new SarlLatticeViewLabel(
        concept_index, ob_cont, m_private->object_names, 
        &(m_private->label_style)
      );
    label->centerTopOn(0, 15);
    m_private->labels.push_back(label);
  }
  SARL_POWER_MAP_END;

  SARL_POWER_MAP_FOR_EACH(
    concept_index, at_cont, 
    m_private->diagram->lattice->concept_attribute_cont
  );
  if ( at_cont && sarl_bit_set_count(at_cont) > 0 ) {
    SarlLatticeViewLabel* label = 
      new SarlLatticeViewLabel(
        concept_index, at_cont, m_private->attribute_names, 
        &(m_private->label_style)
      );
    label->centerBottomOn(0, -15);
    m_private->labels.push_back(label);
  }
  SARL_POWER_MAP_END;
};

SarlLatticeView::~SarlLatticeView()
{
  clear_and_delete_contents<LabelVector, LabelVector::iterator>(
    m_private->labels
  );
  delete m_private; 
};

void SarlLatticeView::paintEvent(QPaintEvent *event)
{
  timeval before, after;

  gettimeofday(&before, 0);
  paintContents(event);
  gettimeofday(&after, 0);

  if ( before.tv_usec > after.tv_usec ) {
    after.tv_sec -= 1;
    after.tv_usec = after.tv_usec + (1000000 - before.tv_usec);
    after.tv_sec -= before.tv_sec;
  }
  else {
    after.tv_sec -= before.tv_sec;
    after.tv_usec -= before.tv_usec;
  }
  // fprintf(stdout, "Paint Creation Time: %2d.%06d\n", after.tv_sec, after.tv_usec);
};

void SarlLatticeView::paintContents(QPaintEvent *event)
{
  QRect ur = event->rect();        // rectangle to update
  QPixmap pix( ur.size() );        // Pixmap for double-buffering
  pix.fill(this, ur.topLeft() );   // fill with widget background
  
  QPainter painter( &pix );
  painter.setWorldXForm(true);
  painter.setWorldMatrix(m_private->tr);
  painter.translate( -ur.x() * m_private->inv_tr.m11(), -ur.y() * m_private->inv_tr.m11()); // use widget coordinate system

  paintLattice(&painter);

  painter.end();
  bitBlt( this, ur.topLeft(), &pix );
};

void SarlLatticeView::paintLattice(QPainter *painter)
{
  // paint connecting lines
  int line_width = sarl_max_of(1, (int)(5 * painter->worldMatrix().m11()));
  painter->setPen(QPen(black,line_width));
  sarl_unsigned_int parent_index, child_index;
  sarl_bit_set_t* children;
  SARL_POWER_MAP_FOR_EACH(
    parent_index, children, m_private->diagram->lattice->lower_covers
  );
    sarl_point_t* parent_pos = 
      sarl_diagram_get_concept_pos(m_private->diagram, parent_index);
    SARL_BIT_SET_FOR_EACH(child_index, children);
      sarl_point_t* child_pos = 
        sarl_diagram_get_concept_pos(m_private->diagram, child_index);
      paintLine(
        child_index, child_pos->x, child_pos->y, 
        parent_index, parent_pos->x, parent_pos->y, 
        painter
      );
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  // paint concepts
  painter->setBrush(QBrush(black));
  painter->setPen(QPen(black, 1));
  sarl_unsigned_int concept_index;
  SARL_LATTICE_FOR_EACH(concept_index, m_private->diagram->lattice);
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(m_private->diagram, concept_index);
    paintConcept(concept_index, concept_pos->x, concept_pos->y, painter);
  SARL_LATTICE_END;

  // paint the labels
  LabelVector::iterator it = m_private->labels.begin();
  m_private->label_style.setStyleOn(painter);
  while( it != m_private->labels.end() ) {
    SarlLatticeViewLabel *label = *it;
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(m_private->diagram, label->concept);
    (*it)->paint(painter, concept_pos->x, concept_pos->y);
    ++it;
  }
};

void SarlLatticeView::calcBoundingBox()
{
  sarl_unsigned_int concept_index;
  sarl_rect_clear(m_private->bbox);
  SARL_LATTICE_FOR_EACH(concept_index, m_private->diagram->lattice);
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(m_private->diagram, concept_index);
    sarl_rect_insert_point(m_private->bbox, concept_pos);
  SARL_LATTICE_END;

  // calculate the transform
  
  double x_margin = 0.05 * width();
  double y_margin = 0.05 * height();

  sarl_rect_t *r = m_private->bbox;
  double dest_width = width() - (2 * x_margin);
  double dest_height = height() - (2 * y_margin);
  double scale_x = dest_width / (double)r->width;
  double scale_y = dest_height / (double)r->height;
  double scale = sarl_min_of(scale_x, scale_y);

  double dx = x_margin + (dest_width - (r->width * scale))/2;
  double dy = y_margin + (dest_height - (r->height * scale))/2;
  
  m_private->tr.setMatrix(
    scale, 0, 0, scale, 
    dx + ((- r->left) * scale), 
    dy + ((- r->top) * scale)
  );
  m_private->inv_tr = m_private->tr.invert();
};

void SarlLatticeView::resizeEvent( QResizeEvent * )
{
  calcBoundingBox();
  update();
};
  

void SarlLatticeView::paintConcept(
  sarl_unsigned_int concept_index, 
  sarl_signed_int x,
  sarl_signed_int y,
  QPainter* painter
)
{
  painter->setBrush(QBrush(black));
  painter->drawEllipse(x - 15, y - 15, 30, 30);
  painter->setBrush(QBrush(gray));
  painter->drawEllipse(x - 12, y - 12, 24, 24);
}

sarl_unsigned_int SarlLatticeView::conceptAt(
  sarl_signed_int x,
  sarl_signed_int y
)
{
  sarl_unsigned_int concept_index;
  SARL_LATTICE_FOR_EACH(concept_index, m_private->diagram->lattice);
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(m_private->diagram, concept_index);
    if ( abs(concept_pos->x - x) < 15 && abs(concept_pos->y - y) < 15) {
      return concept_index;
    }
  SARL_LATTICE_END;
  return 0;
};

void SarlLatticeView::paintLine(
  sarl_unsigned_int child_index, 
  sarl_signed_int cx,
  sarl_signed_int cy,
  sarl_unsigned_int parent_index, 
  sarl_signed_int px,
  sarl_signed_int py,
  QPainter* painter
)
{
  painter->drawLine(cx, cy, px, py);
}

void SarlLatticeView::mousePressEvent( QMouseEvent *event )
{
  QPoint ep = m_private->inv_tr.map(event->pos());
  sarl_unsigned_int concept = conceptAt(ep.x(), ep.y());
  m_private->selected_concept = concept;
  m_private->old_pos = event->pos();
};

void SarlLatticeView::mouseReleaseEvent( QMouseEvent *event )
{
  m_private->selected_concept = 0;
  calcBoundingBox();
  update();
};

void SarlLatticeView::mouseMoveEvent( QMouseEvent *event )
{
  QPoint dp = event->pos() - m_private->old_pos;
  m_private->old_pos = event->pos();

  if ( m_private->selected_concept ) {
    sarl_diagram_move_concept_by(
      m_private->diagram,
      m_private->selected_concept, 
      (int)(m_private->inv_tr.m11() * dp.x()),
      (int)(m_private->inv_tr.m11() * dp.y())
    );
  }
  update();
};
