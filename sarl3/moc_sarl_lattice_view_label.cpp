/****************************************************************************
** SarlLatticeViewLabel meta object code from reading C++ file 'sarl_lattice_view_label.h'
**
** Created: Thu Jul 14 11:13:30 2005
**      by: The Qt MOC ($Id: moc_sarl_lattice_view_label.cpp,v 1.3 2005/09/02 08:21:10 rj-cole Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "sarl_lattice_view_label.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SarlLatticeViewLabel::className() const
{
    return "SarlLatticeViewLabel";
}

QMetaObject *SarlLatticeViewLabel::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SarlLatticeViewLabel( "SarlLatticeViewLabel", &SarlLatticeViewLabel::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SarlLatticeViewLabel::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SarlLatticeViewLabel", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SarlLatticeViewLabel::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SarlLatticeViewLabel", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SarlLatticeViewLabel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"SarlLatticeViewLabel", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SarlLatticeViewLabel.setMetaObject( metaObj );
    return metaObj;
}

void* SarlLatticeViewLabel::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SarlLatticeViewLabel" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool SarlLatticeViewLabel::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool SarlLatticeViewLabel::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SarlLatticeViewLabel::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool SarlLatticeViewLabel::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
