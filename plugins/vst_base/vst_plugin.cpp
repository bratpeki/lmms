/*
 * vst_plugin.cpp - implementation of vstPlugin class
 *
 * Copyright (c) 2005-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#include "vst_plugin.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtGui/QCloseEvent>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#ifdef LMMS_BUILD_LINUX
#include <QtGui/QX11EmbedContainer>
#include <QtGui/QX11Info>
#else
#include <QtGui/QLayout>
#endif
#include <QtXml/QDomDocument>


#ifdef LMMS_BUILD_WIN32
#include <windows.h>
#endif


#include "config_mgr.h"
#include "engine.h"
#include "main_window.h"
#include "templates.h"


class vstSubWin : public QMdiSubWindow
{
public:
	vstSubWin( QWidget * _parent ) :
		QMdiSubWindow( _parent )
	{
		setAttribute( Qt::WA_DeleteOnClose, false );
	}

	virtual ~vstSubWin()
	{
	}

	virtual void closeEvent( QCloseEvent * e )
	{
		// ignore close-events - for some reason otherwise the VST GUI
		// remains hidden when re-opening
		hide();
		e->ignore();
	}
} ;




vstPlugin::vstPlugin( const QString & _plugin ) :
	QObject(),
	journallingObject(),
	remotePlugin( "remote_vst_plugin", false ),
	m_plugin( _plugin ),
	m_pluginWidget( NULL ),
	m_pluginWindowID( 0 ),
	m_name( "" ),
	m_version( 0 ),
	m_vendorString( "" ),
	m_productString( "" )
{
	setSplittedChannels( true );

	lock();
#ifdef LMMS_BUILD_WIN32
	QWidget * helper = new QWidget;
	QWidget * target = new QWidget( helper );
	QHBoxLayout * l = new QHBoxLayout;
	l->setMargin( 0 );
	l->addWidget( target );
	helper->setLayout( l );

	static int k = 0;
	const QString t = QString( "vst%1%2" ).arg( GetCurrentProcessId()<<10 ).
								arg( ++k );
	helper->setWindowTitle( t );

	// we've to call that for making sure, Qt created the windows
	(void) helper->winId();
	(void) target->winId();

	sendMessage( message( IdVstPluginWindowInformation ).
					addString( t.toStdString() ) );
#endif

	VstHostLanguages hlang = LanguageEnglish;
	switch( QLocale::system().language() )
	{
		case QLocale::German: hlang = LanguageGerman; break;
		case QLocale::French: hlang = LanguageFrench; break;
		case QLocale::Italian: hlang = LanguageItalian; break;
		case QLocale::Spanish: hlang = LanguageSpanish; break;
		case QLocale::Japanese: hlang = LanguageJapanese; break;
		default: break;
	}
	sendMessage( message( IdVstSetLanguage ).addInt( hlang ) );


	QString p = m_plugin;
	if( QFileInfo( p ).dir().isRelative() )
	{
		p = configManager::inst()->vstDir() + QDir::separator() + p;
	}

	sendMessage( message( IdVstLoadPlugin ).addString( p.toStdString() ) );

	waitForInitDone();

	unlock();

#ifdef LMMS_BUILD_WIN32
	if( m_pluginWindowID )
	{
		target->setFixedSize( m_pluginGeometry );
		vstSubWin * sw = new vstSubWin(
					engine::getMainWindow()->workspace() );
		sw->setWidget( helper );
		helper->setWindowTitle( name() );
		m_pluginWidget = helper;
	}
	else
	{
		delete helper;
	}
#endif
}




vstPlugin::~vstPlugin()
{
	delete pluginWidget();
}




void vstPlugin::showEditor( QWidget * _parent )
{
	QWidget * w = pluginWidget();
	if( w )
	{
		w->show();
		return;
	}

#ifdef LMMS_BUILD_LINUX
	if( m_pluginWindowID == 0 )
	{
		return;
	}

	m_pluginWidget = new QWidget( _parent );
	m_pluginWidget->setFixedSize( m_pluginGeometry );
	m_pluginWidget->setWindowTitle( name() );
	if( _parent == NULL )
	{
		vstSubWin * sw = new vstSubWin(
					engine::getMainWindow()->workspace() );
		sw->setWidget( m_pluginWidget );
	}

	QX11EmbedContainer * xe = new QX11EmbedContainer( m_pluginWidget );
	xe->embedClient( m_pluginWindowID );
	xe->setFixedSize( m_pluginGeometry );
	xe->show();
#endif

	m_pluginWidget->show();
}




void vstPlugin::hideEditor( void )
{
	QWidget * w = pluginWidget();
	if( w )
	{
		w->hide();
	}
}




void vstPlugin::loadSettings( const QDomElement & _this )
{
	if( pluginWidget() != NULL )
	{
		if( _this.attribute( "guivisible" ).toInt() )
		{
			showEditor();
		}
		else
		{
			hideEditor();
		}
	}

	const int num_params = _this.attribute( "numparams" ).toInt();
	if( num_params > 0 )
	{
		QMap<QString, QString> dump;
		for( int i = 0; i < num_params; ++i )
		{
			const QString key = "param" +
						QString::number( i );
			dump[key] = _this.attribute( key );
		}
		setParameterDump( dump );
	}
}




void vstPlugin::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	if( pluginWidget() != NULL )
	{
		_this.setAttribute( "guivisible", pluginWidget()->isVisible() );
	}
	const QMap<QString, QString> & dump = parameterDump();
	_this.setAttribute( "numparams", dump.size() );
	for( QMap<QString, QString>::const_iterator it = dump.begin();
							it != dump.end(); ++it )
	{
		_this.setAttribute( it.key(), it.value() );
	}
}




void vstPlugin::setTempo( bpm_t _bpm )
{
	lock();
	sendMessage( message( IdVstSetTempo ).addInt( _bpm ) );
	unlock();
}




void vstPlugin::updateSampleRate( void )
{
	lock();
	sendMessage( message( IdSampleRateInformation ).
			addInt( engine::getMixer()->processingSampleRate() ) );
	unlock();
}




const QMap<QString, QString> & vstPlugin::parameterDump( void )
{
	lock();
	sendMessage( IdVstGetParameterDump );
	waitForMessage( IdVstParameterDump );
	unlock();

	return m_parameterDump;
}




void vstPlugin::setParameterDump( const QMap<QString, QString> & _pdump )
{
	message m( IdVstSetParameterDump );
	m.addInt( _pdump.size() );
	for( QMap<QString, QString>::const_iterator it = _pdump.begin();
						it != _pdump.end(); ++it )
	{
		const vstParameterDumpItem item =
		{
			( *it ).section( ':', 0, 0 ).toInt(),
			"",
			( *it ).section( ':', 1, 1 ).toFloat()
		} ;
		m.addInt( item.index );
		m.addString( item.shortLabel );
		m.addFloat( item.value );
	}
	lock();
	sendMessage( m );
	unlock();
}




bool vstPlugin::processMessage( const message & _m )
{
	switch( _m.id )
	{
		case IdVstPluginWindowID:
			m_pluginWindowID = _m.getInt();
			break;

		case IdVstPluginEditorGeometry:
		{
			const int w = _m.getInt( 0 );
			const int h = _m.getInt( 1 );
			m_pluginGeometry = QSize( w, h );
/*			if( m_pluginWidget != NULL )
			{
				m_pluginWidget->setFixedSize(
							m_pluginGeometry );
				if( m_pluginWidget->childAt( 0, 0 ) != NULL )
				{
					m_pluginWidget->childAt( 0, 0
						)->setFixedSize(
							m_pluginGeometry );
				}
			}*/
			break;
		}

		case IdVstPluginName:
			m_name = _m.getQString();
			break;

		case IdVstPluginVersion:
			m_version = _m.getInt();
			break;

		case IdVstPluginVendorString:
			m_vendorString = _m.getQString();
			break;

		case IdVstPluginProductString:
			m_productString = _m.getQString();
			break;

		case IdVstParameterDump:
		{
			m_parameterDump.clear();
			const int num_params = _m.getInt();
			int p = 0;
			for( int i = 0; i < num_params; ++i )
			{
				vstParameterDumpItem item;
				item.index = _m.getInt( ++p );
				item.shortLabel = _m.getString( ++p );
				item.value = _m.getFloat( ++p );
	m_parameterDump["param" + QString::number( item.index )] =
				QString::number( item.index ) + ":" +
//					QString( item.shortLabel ) + ":" +
					QString::number( item.value );
			}
			break;
		}
		default:
			return remotePlugin::processMessage( _m );
	}
	return true;

}



#include "moc_vst_plugin.cxx"

