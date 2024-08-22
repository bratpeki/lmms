/*
 * WaveAnalyzerOsc.h - Declaration of WaveAnalyzerOsc class
 *
 * Copyright (c) 2020 Ian Caio <iancaio_dev/at/hotmail.com>
 *
 * This file is part of LMMS - https://lmms.io
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

#ifndef WAVEANALYZER_OSC_H
#define WAVEANALYZER_OSC_H

#include <QWidget>
#include <QPainterPath>

// CONSTANTS:
// Margins
#define viewportWidth 500
#define viewportHeight 200
#define leftMargin 25
#define rightMargin 10
#define topMargin 15
#define bottomMargin 10
#define clippingMargin 30 // The margin between the clipping line and viewport top/bottom
// Colors
#define borderColor QColor::fromRgb(100, 100, 100)
#define backgroundColor QColor::fromRgb(0, 0, 0)
#define clippingLineColor QColor::fromRgb(255, 100, 100)
#define centerLineColor QColor::fromRgb(100, 100, 100)
#define labelColor QColor::fromRgb(255, 255, 255)
#define waveColor QColor::fromRgb(255, 165, 0, 150)
#define frozenWaveColor QColor::fromRgb(60, 255, 45, 150)
// Sizes
#define labelFontSize 12

namespace lmms
{

class WaveAnalyzerControls;

namespace gui
{

class WaveAnalyzerWaveform : public QWidget
{
	Q_OBJECT
public:
	WaveAnalyzerWaveform(WaveAnalyzerControls* controls, QWidget* parent = nullptr);
	virtual ~WaveAnalyzerWaveform();
	void paintEvent(QPaintEvent* pe) override;
public slots:
	void updatePoints(int count);
	void updateFrozenPoints();
private:
	QPainterPath* generateSmoothedPathL();
	QPainterPath* generateSmoothedPathR();
	void shiftPoints(int count);

	// Controls linked to this indicator
	WaveAnalyzerControls* m_controls;

	// The points representing the sample values of left and right
	QPoint m_pointsL[viewportWidth];
	QPoint m_pointsR[viewportWidth];

	// Peaks and troughs
	QPoint m_peaks[viewportWidth];
	QPoint m_troughs[viewportWidth];

	// Pixmap of the frozen waveform
	QPixmap* m_frozenWave;

	// The points for the frozen waveform
	QPoint m_frozenPointsL[viewportWidth];
	QPoint m_frozenPointsR[viewportWidth];

	// The points for the frozen peaks/troughs
	QPoint m_frozenPeaks[viewportWidth];
	QPoint m_frozenTroughs[viewportWidth];
};

class WaveAnalyzerOsc : public QWidget
{
	Q_OBJECT
public:
	WaveAnalyzerOsc(WaveAnalyzerControls* controls, QWidget* parent = nullptr);
	virtual ~WaveAnalyzerOsc();

	void paintEvent(QPaintEvent* pe) override;
	void paintViewport(QPainter & p);
	void paintLabels(QPainter & p);
	void paintWave(QPainter & p);
private:
	// Waveform itself (drawed separately for performance)
	WaveAnalyzerWaveform* m_wave;

	// Controls linked to this indicator
	WaveAnalyzerControls* m_controls;
};

}

}

#endif
