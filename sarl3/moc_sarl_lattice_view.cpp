/****************************************************************************
** SarlLatticeView meta object code from reading C++ file 'sarl_lattice_view.h'
**
** Created: Fri Jul 15 19:01:02 2005
**      by: The Qt MOC ($Id: moc_sarl_lattice_view.cpp,v 1.3 2005/09/02 08:21:10 rj-cole Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "sarl_lattice_view.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SarlLatticeView::className() const
{
    return "SarlLatticeView";
}

QMetaObject *SarlLatticeView::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SarlLatticeView( "SarlLatticeView", &SarlLatticeView::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SarlLatticeView::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SarlLatticeView", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SarlLatticeView::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SarlLatticeView", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SarlLatticeView::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ "concept_index", &static_QUType_ptr, "sarl_unsigned_int", QUParameter::In },
	{ "event", &static_QUType_ptr, "QMouseEvent", QUParameter::InOut }
    };
    static const QUMethod signal_0 = {"conceptSelected", 2, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ "object_index", &static_QUType_ptr, "sarl_unsigned_int", QUParameter::In },
	{ "event", &static_QUType_ptr, "QMouseEvent", QUParameter::InOut }
    };
    static const QUMethod signal_1 = {"objectSelected", 2, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ "attribute_index", &static_QUType_ptr, "sarl_unsigned_int", QUParameter::In },
	{ "event", &static_QUType_ptr, "QMouseEvent", QUParameter::InOut }
    };
    static const QUMethod signal_2 = {"attributeSelected", 2, param_signal_2 };
    static const QMetaData signal_tbl[] = {
	{ "conceptSelected(sarl_unsigned_int,QMouseEvent&)", &signal_0, QMetaData::Public },
	{ "objectSelected(sarl_unsigned_int,QMouseEvent&)", &signal_1, QMetaData::Public },
	{ "attributeSelected(sarl_unsigned_int,QMouseEvent&)", &signal_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"SarlLatticeView", parentObject,
	0, 0,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SarlLatticeView.setMetaObject( metaObj );
    return metaObj;
}

void* SarlLatticeView::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SarlLatticeView" ) )
	return this;
    return QWidget::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL conceptSelected
void SarlLatticeView::conceptSelected( sarl_unsigned_int t0, QMouseEvent& t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_ptr.set(o+1,&t0);
    static_QUType_ptr.set(o+2,&t1);
    activate_signal( clist, o );
}

// SIGNAL objectSelected
void SarlLatticeView::objectSelected( sarl_unsigned_int t0, QMouseEvent& t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_ptr.set(o+1,&t0);
    static_QUType_ptr.set(o+2,&t1);
    activate_signal( clist, o );
}

// SIGNAL attributeSelected
void SarlLatticeView::attributeSelected( sarl_unsigned_int t0, QMouseEvent& t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_ptr.set(o+1,&t0);
    static_QUType_ptr.set(o+2,&t1);
    activate_signal( clist, o );
}

bool SarlLatticeView::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool SarlLatticeView::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: conceptSelected((sarl_unsigned_int)(*((sarl_unsigned_int*)static_QUType_ptr.get(_o+1))),(QMouseEvent&)*((QMouseEvent*)static_QUType_ptr.get(_o+2))); break;
    case 1: objectSelected((sarl_unsigned_int)(*((sarl_unsigned_int*)static_QUType_ptr.get(_o+1))),(QMouseEvent&)*((QMouseEvent*)static_QUType_ptr.get(_o+2))); break;
    case 2: attributeSelected((sarl_unsigned_int)(*((sarl_unsigned_int*)static_QUType_ptr.get(_o+1))),(QMouseEvent&)*((QMouseEvent*)static_QUType_ptr.get(_o+2))); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool SarlLatticeView::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool SarlLatticeView::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
