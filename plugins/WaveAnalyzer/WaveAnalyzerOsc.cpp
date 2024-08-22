/*
 * WaveAnalyzerOsc.cpp - Definition of WaveAnalyzerOsc class
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

#include "WaveAnalyzerOsc.h"
#include "WaveAnalyzerControls.h"

#include <algorithm>

#include <QColor>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

namespace lmms::gui
{

WaveAnalyzerOsc::WaveAnalyzerOsc(WaveAnalyzerControls* controls, QWidget* parent) :
	QWidget(parent),
	m_controls(controls)
{
	// Initialize widget
	// Set minimum and maximum size
	int totalWidth = viewportWidth + leftMargin + rightMargin;
	int totalHeight = viewportHeight + topMargin + bottomMargin;

	QSize s(totalWidth, totalHeight);
	setMinimumSize(s);
	setMaximumSize(s);
	resize(s);

	m_wave = new WaveAnalyzerWaveform(controls, this);

	// Connect signal to repaint when buffer changes
	connect(m_controls, SIGNAL(bufferChanged(int)), m_wave, SLOT(updatePoints(int)));

	// Connect signal to update frozen waveform points when button is clicked
	connect(&m_controls->m_snapshotModel, SIGNAL(dataChanged()), m_wave, SLOT(updateFrozenPoints()));
}

WaveAnalyzerOsc::~WaveAnalyzerOsc()
{
}

void WaveAnalyzerOsc::paintEvent(QPaintEvent* pe)
{
	QPainter painter(this);

	// Draw viewport
	paintViewport(painter);

	// Draw labels
	paintLabels(painter);

	// Waveform is drawn by WaveAnalyzerWaveform
}

void WaveAnalyzerOsc::paintViewport(QPainter & p)
{
	// Fill a rect behind the background slightly
	// bigger just to make a small edge
	p.fillRect(
		leftMargin - 2,
		topMargin - 2,
		viewportWidth + 4,
		viewportHeight + 4,
		borderColor
	);
	// Fill the background
	p.fillRect(
		leftMargin,
		topMargin,
		viewportWidth,
		viewportHeight,
		backgroundColor
	);

	// Draw clipping lines
	p.setPen(clippingLineColor);
	// Top clipping line
	p.drawLine(
		leftMargin,
		topMargin + clippingMargin,
		leftMargin + viewportWidth,
		topMargin + clippingMargin
	);
	// Bottom clipping line
	p.drawLine(
		leftMargin,
		topMargin + viewportHeight - clippingMargin,
		leftMargin + viewportWidth,
		topMargin + viewportHeight - clippingMargin
	);

	// Draw center line
	p.setPen(centerLineColor);
	p.drawLine(
		leftMargin,
		topMargin + viewportHeight / 2,
		leftMargin + viewportWidth,
		topMargin + viewportHeight / 2
	);
}

void WaveAnalyzerOsc::paintLabels(QPainter & p)
{
	// Paint the labels from the viewport
	p.setPen(labelColor);
	QFont font;
	font.setPixelSize(labelFontSize);
	p.setFont(font);

	p.drawText(
		leftMargin / 2,
		topMargin + clippingMargin + (labelFontSize / 2),
		"1"
	);
	p.drawText(
		leftMargin / 2,
		topMargin + viewportHeight / 2 + (labelFontSize / 2),
		"0"
	);
	p.drawText(
		leftMargin / 2,
		topMargin + viewportHeight - clippingMargin + (labelFontSize / 2),
		"-1"
	);
}

WaveAnalyzerWaveform::WaveAnalyzerWaveform(WaveAnalyzerControls* controls, QWidget* parent) :
	QWidget(parent),
	m_controls(controls)
{
	move(leftMargin, topMargin);
	QSize s(viewportWidth, viewportHeight);
	setMinimumSize(s);
	setMaximumSize(s);
	resize(s);

	// Create frozen wave pixmap (for storing the snapshot instead of redrawing it everytime)
	m_frozenWave = new QPixmap(viewportWidth, viewportHeight);
}

WaveAnalyzerWaveform::~WaveAnalyzerWaveform()
{
	delete m_frozenWave;
}

void WaveAnalyzerWaveform::updateFrozenPoints()
{
	// If we are disabling don't bother updating
	if (!m_controls->m_snapshotModel.value()) { return; }

	// Clear the frozenWave pixmap
	m_frozenWave->fill(QColor("Transparent"));

	// Painter to fill the pixmap
	QPainter p(m_frozenWave);
	p.setPen(frozenWaveColor);

	switch(m_controls->m_drawingMode.value())
	{
		// Raw
		case 0:
		{
			for (int i = 0; i < viewportWidth; ++i)
			{
				m_frozenPointsL[i] = m_pointsL[i];
				m_frozenPointsR[i] = m_pointsR[i];
			}
			p.drawPolyline(m_frozenPointsL, viewportWidth);
			p.drawPolyline(m_frozenPointsR, viewportWidth);
			break;
		}
		// Peaks / Troughs
		case 1:
		{
			// Draw first points
			m_frozenPeaks[viewportWidth - 1] = m_peaks[viewportWidth - 1];
			m_frozenTroughs[viewportWidth - 1] = m_troughs[viewportWidth - 1];
			p.drawLine(m_frozenPeaks[viewportWidth - 1], m_frozenTroughs[viewportWidth - 1]);
			// Draw remaining ones, connecting the previous troughs to the current peaks
			for (int i = 1; i < viewportWidth; ++i)
			{
				m_frozenPeaks[i] = m_peaks[i];
				m_frozenTroughs[i] = m_troughs[i];

				// Connect previous trough to the this peak
				p.drawLine(m_frozenTroughs[i - 1], m_frozenPeaks[i]);
				// Draw the line between peak and trough
				p.drawLine(m_frozenPeaks[i], m_frozenTroughs[i]);
			}
			break;
		}
		// Smoothed Bezier
		case 2:
		{
			QPainterPath* path;
			path = generateSmoothedPathL();
			p.drawPath(*path);
			delete path;
			path = generateSmoothedPathR();
			p.drawPath(*path);
			delete path;
		}
		default:
			qWarning("WaveAnalyzer: Invalid drawing mode!");
			return;
	}

	for (int i = 0; i < viewportWidth; ++i)
	{
		m_frozenPointsL[i] = m_pointsL[i];
		m_frozenPointsR[i] = m_pointsR[i];

		m_frozenPeaks[i] = m_peaks[i];
		m_frozenTroughs[i] = m_troughs[i];
	}
}

void WaveAnalyzerWaveform::updatePoints(int count)
{
	int totalPixels = viewportWidth;
	int totalFrames = static_cast<int>(m_controls->m_numberOfFrames.value());
	int lastFrame = totalFrames - 1;
	int framesPerPixel = std::max(totalFrames / totalPixels, 1);

	// Shift existing points
	int pixelsShift = count / framesPerPixel;
	shiftPoints(pixelsShift);

	// Add new points
	// References for the points
	int baseY = viewportHeight / 2;
	int ySpace = (viewportHeight / 2) - clippingMargin;

	// Go to the first altered frame
	int currentFrame = totalFrames - count;
	int currentPixel = totalPixels - (count / framesPerPixel);

	// Raw mode will get the value of the immediate frame
	// while Peaks/Troughs will get the peaks and troughs of the range
	switch(m_controls->m_drawingMode.value())
	{
		// Raw and Smoothed Bezier
		case 0:
		case 2:
		{
			for (; currentPixel < totalPixels; ++currentPixel)
			{
				float valueL = 0;
				float valueR = 0;

				if (currentFrame <= lastFrame)
				{
					valueL = m_controls->m_ampBufferL[currentFrame];
					valueR = m_controls->m_ampBufferR[currentFrame];
					currentFrame += framesPerPixel;
				}

				int leftY = baseY - (valueL * ySpace);
				int rightY = baseY - (valueR * ySpace);
				m_pointsL[currentPixel] = QPoint(currentPixel, leftY);
				m_pointsR[currentPixel] = QPoint(currentPixel, rightY);

				// Peaks and troughs are "zeroed" here (we don't have that information
				// on that mode, to prevent the processing time of gathering it)
				m_peaks[currentPixel] = QPoint(currentPixel, baseY);
				m_troughs[currentPixel] = QPoint(currentPixel, baseY);
			}
			break;
		}
		// Peaks/Troughs
		case 1:
		{
			for (; currentPixel < totalPixels; ++currentPixel)
			{
				float peak = std::numeric_limits<float>::min();
				float trough = std::numeric_limits<float>::max();

				for
				(
					int i = 0;
					(currentFrame + i) <= lastFrame && i < framesPerPixel;
					++i
				)
				{
					auto lAmp = m_controls->m_ampBufferL[currentFrame + i];
					auto rAmp = m_controls->m_ampBufferR[currentFrame + i];

					// Get peaks and troughs
					if (lAmp > peak) { peak = lAmp; }
					if (rAmp > peak) { peak = rAmp; }
					if (lAmp < trough) { trough = lAmp; }
					if (rAmp < trough) { trough = rAmp; }
				}

				currentFrame += framesPerPixel;

				// Set our peaks and troughs points
				int peakY = baseY - (peak * ySpace);
				int troughY = baseY - (trough * ySpace);
				m_peaks[currentPixel] = QPoint(currentPixel, peakY);
				m_troughs[currentPixel] = QPoint(currentPixel, troughY);

				// Regular points are "zeroed".
				// TODO: We could also update them, I'm mostly zeroing for consistent
				// behavior. Think about this later.
				m_pointsL[currentPixel] = QPoint(currentPixel, baseY);
				m_pointsR[currentPixel] = QPoint(currentPixel, baseY);
			}
			break;
		}
		default:
			qWarning("WaveAnalyzer: Invalid drawing mode!");
			return;
	}

	repaint();
}

void WaveAnalyzerWaveform::shiftPoints(int count)
{
	int size = viewportWidth;
	int lastIndex = size - 1;

	for (int i = 0; i < size; ++i)
	{
		// If point being shifted is out of bounds return
		if (i + count > lastIndex) { return; }
		// Else we shift the points
		m_pointsL[i] = QPoint(i, m_pointsL[i + count].y());
		m_pointsR[i] = QPoint(i, m_pointsR[i + count].y());

		m_peaks[i] = QPoint(i, m_peaks[i + count].y());
		m_troughs[i] = QPoint(i, m_troughs[i + count].y());
	}
}

QPainterPath* WaveAnalyzerWaveform::generateSmoothedPathL()
{
	QPainterPath* p = new QPainterPath();

	// si -> c1 -> c2 -> sn
	// S0 is the first point itself
	QPointF si(m_pointsL[0].x(), m_pointsL[0].y());
	QPointF c1;
	QPointF c2;
	QPointF sn;

	p->moveTo(si);

	int lastP = viewportWidth - 1;

	for (int i = 0; i < lastP; ++i)
	{
		// Calculate c1, c2 and sn
		c1 = (2 * m_pointsL[i] / 3) + (m_pointsL[i + 1] / 3);
		c2 = (m_pointsL[i] / 3) + (2 * m_pointsL[i + 1] / 3);
		// sn will be the last point on the last bezier or be calculated
		if (i == lastP - 1)
		{
			sn = m_pointsL[lastP];
		}
		else
		{
			// The next sn depends on the c1 of the next bezier curve
			QPointF nc1 = (2 * m_pointsL[i + 1] / 3) + (m_pointsL[i + 2] / 3);
			sn = c2 / 2 + nc1 / 2;
		}
		p->cubicTo(c1, c2, sn);
		// For next iteration
		si = sn;
	}
	return p;
}

QPainterPath* WaveAnalyzerWaveform::generateSmoothedPathR()
{
	QPainterPath* p = new QPainterPath();

	// si -> c1 -> c2 -> sn
	// S0 is the first point itself
	QPointF si(m_pointsR[0].x(), m_pointsR[0].y());
	QPointF c1;
	QPointF c2;
	QPointF sn;

	p->moveTo(si);

	int lastP = viewportWidth - 1;

	for (int i = 0; i < lastP; ++i)
	{
		// Calculate c1, c2 and sn
		c1 = (2 * m_pointsR[i] / 3) + (m_pointsR[i + 1] / 3);
		c2 = (m_pointsR[i] / 3) + (2 * m_pointsR[i + 1] / 3);
		// sn will be the last point on the last bezier or be calculated
		if (i == lastP - 1)
		{
			sn = m_pointsR[lastP];
		}
		else
		{
			// The next sn depends on the c1 of the next bezier curve
			QPointF nc1 = (2 * m_pointsR[i + 1] / 3) + (m_pointsR[i + 2] / 3);
			sn = c2 / 2 + nc1 / 2;
		}
		p->cubicTo(c1, c2, sn);
		// For next iteration
		si = sn;
	}
	return p;
}

void WaveAnalyzerWaveform::paintEvent(QPaintEvent* pe)
{
	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing);

	switch(m_controls->m_drawingMode.value())
	{
		// Raw mode
		case 0:
		{
			if (m_controls->m_snapshotModel.value())
			{
				p.drawPixmap(0, 0, *m_frozenWave);
			}
			p.setPen(waveColor);
			p.drawPolyline(m_pointsL, viewportWidth);
			p.drawPolyline(m_pointsR, viewportWidth);
			break;
		}
		// Peaks/Troughs
		case 1:
		{
			if (m_controls->m_snapshotModel.value())
			{
				p.drawPixmap(0, 0, *m_frozenWave);
			}
			p.setPen(waveColor);
			for (int i = 0; i < viewportWidth - 1; ++i)
			{
				p.drawLine(m_peaks[i], m_troughs[i]);
				p.drawLine(m_troughs[i], m_peaks[i + 1]);
			}
			p.drawLine(m_peaks[viewportWidth - 1], m_troughs[viewportWidth - 1]);
			break;
		}
		// Smoothed bezier
		case 2:
		{
			if (m_controls->m_snapshotModel.value())
			{
				p.drawPixmap(0, 0, *m_frozenWave);
			}

			p.setPen(waveColor);
			QPainterPath* path;
			path = generateSmoothedPathL();
			p.drawPath(*path);
			delete path;
			path = generateSmoothedPathR();
			p.drawPath(*path);
			delete path;
			break;
		}
		default:
			qWarning("WaveAnalyzer: Invalid drawing mode");
			return;
	}
}

}
