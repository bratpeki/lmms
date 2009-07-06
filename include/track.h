/*
 * track.h - declaration of classes concerning tracks -> neccessary for all
 *           track-like objects (beat/bassline, sample-track...)
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#ifndef _TRACK_H
#define _TRACK_H

#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtGui/QWidget>

#include "lmms_basics.h"
#include "midi_time.h"
#include "rubberband.h"
#include "journalling_object.h"
#include "automatable_model.h"


class QMenu;
class QPushButton;

class pixmapButton;
class textFloat;
class track;
class trackContentObjectView;
class trackContainer;
class trackContainerView;
class trackContentWidget;
class trackView;

typedef QWidget trackSettingsWidget;



const int DEFAULT_SETTINGS_WIDGET_WIDTH = 224;
const int TRACK_OP_WIDTH = 78;
const int TCO_BORDER_WIDTH = 1;


class trackContentObject : public model, public journallingObject
{
	Q_OBJECT
	mapPropertyFromModel(bool,isMuted,setMuted,m_mutedModel);
public:
	trackContentObject( track * _track );
	virtual ~trackContentObject();

	inline track * getTrack( void )
	{
		return( m_track );
	}

	inline const QString & name( void ) const
	{
		return( m_name );
	}

	inline void setName( const QString & _name )
	{
		m_name = _name;
		emit dataChanged();
	}

	virtual QString displayName( void ) const
	{
		return( name() );
	}


	inline const midiTime & startPosition( void ) const
	{
		return( m_startPosition );
	}

	inline midiTime endPosition( void ) const
	{
		const int sp = m_startPosition;
		return sp + m_length;
	}

	inline const midiTime & length( void ) const
	{
		return( m_length );
	}

	virtual void movePosition( const midiTime & _pos );
	virtual void changeLength( const midiTime & _length );

	virtual trackContentObjectView * createView( trackView * _tv ) = 0;


public slots:
	void copy( void );
	void paste( void );
	void toggleMute( void );


protected:
	virtual void undoStep( journalEntry & _je );
	virtual void redoStep( journalEntry & _je );


signals:
	void lengthChanged( void );
	void positionChanged( void );
	void destroyedTCO( void );


private:
	enum Actions
	{
		NoAction,
		Move,
		Resize
	} ;

	track * m_track;
	QString m_name;

	midiTime m_startPosition;
	midiTime m_length;

	boolModel m_mutedModel;


	friend class trackContentObjectView;

} ;



class trackContentObjectView : public selectableObject, public modelView
{
	Q_OBJECT
public:
	trackContentObjectView( trackContentObject * _tco, trackView * _tv );
	virtual ~trackContentObjectView();

	bool fixedTCOs( void );

	inline trackContentObject * getTrackContentObject( void )
	{
		return( m_tco );
	}


public slots:
	virtual bool close( void );
	void cut( void );
	void remove( void );

protected:
	virtual void constructContextMenu( QMenu * )
	{
	}

	virtual void contextMenuEvent( QContextMenuEvent * _cme );
	virtual void dragEnterEvent( QDragEnterEvent * _dee );
	virtual void dropEvent( QDropEvent * _de );
	virtual void leaveEvent( QEvent * _e );
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void mouseMoveEvent( QMouseEvent * _me );
	virtual void mouseReleaseEvent( QMouseEvent * _me );

	void setAutoResizeEnabled( bool _e = FALSE );
	float pixelsPerTact( void );

	inline trackView * getTrackView( void )
	{
		return( m_trackView );
	}


protected slots:
	void updateLength( void );
	void updatePosition( void );


private:
	enum Actions
	{
		NoAction,
		Move,
		MoveSelection,
		Resize
	} ;

	static textFloat * s_textFloat;

	trackContentObject * m_tco;
	trackView * m_trackView;
	Actions m_action;
	bool m_autoResize;
	Sint16 m_initialMouseX;

	textFloat * m_hint;

	midiTime m_oldTime;// used for undo/redo while mouse-button is pressed

} ;





class trackContentWidget : public QWidget, public journallingObject
{
	Q_OBJECT
public:
	trackContentWidget( trackView * _parent );
	virtual ~trackContentWidget();

	void addTCOView( trackContentObjectView * _tcov );
	void removeTCOView( trackContentObjectView * _tcov );
	void removeTCOView( int _tco_num )
	{
		if( _tco_num >= 0 && _tco_num < m_tcoViews.size() )
		{
			removeTCOView( m_tcoViews[_tco_num] );
		}
	}

	midiTime endPosition( const midiTime & _pos_start );


public slots:
	void update( void );
	void changePosition( const midiTime & _new_pos = midiTime( -1 ) );


protected:
	virtual void dragEnterEvent( QDragEnterEvent * _dee );
	virtual void dropEvent( QDropEvent * _de );
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void paintEvent( QPaintEvent * _pe );
	virtual void resizeEvent( QResizeEvent * _re );

	virtual QString nodeName( void ) const
	{
		return( "trackcontentwidget" );
	}

	virtual void undoStep( journalEntry & _je );
	virtual void redoStep( journalEntry & _je );


private:
	enum Actions
	{
		AddTrackContentObject,
		RemoveTrackContentObject
	} ;

	track * getTrack( void );
	midiTime getPosition( int _mouse_x );

	trackView * m_trackView;

	typedef QVector<trackContentObjectView *> tcoViewVector;
	tcoViewVector m_tcoViews;

	int m_pixelsPerTact;

	QPixmap m_background;

} ;





class trackOperationsWidget : public QWidget
{
	Q_OBJECT
public:
	trackOperationsWidget( trackView * _parent );
	~trackOperationsWidget();


protected:
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void paintEvent( QPaintEvent * _pe );


private slots:
	void cloneTrack( void );
	void removeTrack( void );
	void updateMenu( void );


private:
	static QPixmap * s_grip;

	trackView * m_trackView;

	QPushButton * m_trackOps;
	pixmapButton * m_muteBtn;
	pixmapButton * m_soloBtn;


	friend class trackView;

signals:
	void trackRemovalScheduled( trackView * _t );

} ;





// base-class for all tracks
class EXPORT track : public model, public journallingObject
{
	Q_OBJECT
	mapPropertyFromModel(bool,isMuted,setMuted,m_mutedModel);
public:
	typedef QVector<trackContentObject *> tcoVector;

	enum TrackTypes
	{
		InstrumentTrack,
		BBTrack,
		SampleTrack,
		EventTrack,
		VideoTrack,
		AutomationTrack,
		HiddenAutomationTrack,
		NumTrackTypes
	} ;

	track( TrackTypes _type, trackContainer * _tc );
	virtual ~track();

	static track * create( TrackTypes _tt, trackContainer * _tc );
	static track * create( const QDomElement & _this,
							trackContainer * _tc );
	void clone( void );


	// pure virtual functions
	TrackTypes type( void ) const
	{
		return( m_type );
	}

	virtual bool play( const midiTime & _start, const fpp_t _frames,
				const f_cnt_t _frame_base,
						Sint16 _tco_num = -1 ) = 0;


	virtual trackView * createView( trackContainerView * _view ) = 0;
	virtual trackContentObject * createTCO( const midiTime & _pos ) = 0;

	virtual void saveTrackSpecificSettings( QDomDocument & _doc,
						QDomElement & _parent ) = 0;
	virtual void loadTrackSpecificSettings( const QDomElement & _this ) = 0;


	virtual void saveSettings( QDomDocument & _doc, QDomElement & _this );
	virtual void loadSettings( const QDomElement & _this );

	void setSimpleSerializing( void )
	{
		m_simpleSerializingMode = TRUE;
	}

	// -- for usage by trackContentObject only ---------------
	trackContentObject * addTCO( trackContentObject * _tco );
	void removeTCO( trackContentObject * _tco );
	// -------------------------------------------------------

	int numOfTCOs( void );
	trackContentObject * getTCO( int _tco_num );
	int getTCONum( trackContentObject * _tco );

	const tcoVector & getTCOs( void ) const
	{
		return( m_trackContentObjects );
	}
	void getTCOsInRange( tcoVector & _tco_v, const midiTime & _start,
							const midiTime & _end );
	void swapPositionOfTCOs( int _tco_num1, int _tco_num2 );


	void insertTact( const midiTime & _pos );
	void removeTact( const midiTime & _pos );

	tact_t length( void ) const;


	inline trackContainer * getTrackContainer( void ) const
	{
		return( m_trackContainer );
	}

	// name-stuff
	virtual const QString & name( void ) const
	{
		return( m_name );
	}

	virtual QString displayName( void ) const
	{
		return( name() );
	}

	using model::dataChanged;


public slots:
	virtual void setName( const QString & _new_name )
	{
		m_name = _new_name;
		emit nameChanged();
	}

	void toggleSolo( void );


private:
	trackContainer * m_trackContainer;
	TrackTypes m_type;
	QString m_name;

	boolModel m_mutedModel;
	boolModel m_soloModel;
	bool m_mutedBeforeSolo;

	bool m_simpleSerializingMode;

	tcoVector m_trackContentObjects;


	friend class trackView;


signals:
	void destroyedTrack( void );
	void nameChanged( void );
	void trackContentObjectAdded( trackContentObject * );

} ;




class trackView : public QWidget, public modelView, public journallingObject
{
	Q_OBJECT
public:
	trackView( track * _track, trackContainerView * _tcv );
	virtual ~trackView();

	inline const track * getTrack( void ) const
	{
		return( m_track );
	}

	inline track * getTrack( void )
	{
		return( m_track );
	}

	inline trackContainerView * getTrackContainerView( void )
	{
		return( m_trackContainerView );
	}

	inline trackOperationsWidget * getTrackOperationsWidget( void )
	{
		return( &m_trackOperationsWidget );
	}

	inline trackSettingsWidget * getTrackSettingsWidget( void )
	{
		return( &m_trackSettingsWidget );
	}

	inline trackContentWidget * getTrackContentWidget( void )
	{
		return( &m_trackContentWidget );
	}

	bool isMovingTrack( void ) const
	{
		return( m_action == MoveTrack );
	}

	virtual void update( void );


public slots:
	virtual bool close( void );


protected:
	virtual void modelChanged( void );
	virtual void undoStep( journalEntry & _je );
	virtual void redoStep( journalEntry & _je );

	virtual QString nodeName( void ) const
	{
		return( "trackview" );
	}


	virtual void dragEnterEvent( QDragEnterEvent * _dee );
	virtual void dropEvent( QDropEvent * _de );
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void mouseMoveEvent( QMouseEvent * _me );
	virtual void mouseReleaseEvent( QMouseEvent * _me );
	virtual void paintEvent( QPaintEvent * _pe );
	virtual void resizeEvent( QResizeEvent * _re );


private:
	enum Actions
	{
		NoAction,
		MoveTrack,
		ResizeTrack
	} ;

	track * m_track;
	trackContainerView * m_trackContainerView;

	trackOperationsWidget m_trackOperationsWidget;
	trackSettingsWidget m_trackSettingsWidget;
	trackContentWidget m_trackContentWidget;

	Actions m_action;


	friend class trackLabelButton;


private slots:
	void createTCOView( trackContentObject * _tco );

} ;



#endif
