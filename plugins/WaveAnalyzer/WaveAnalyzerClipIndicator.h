/*
 * WaveAnalyzerClipIndicator.h - Declaration of
 * WaveAnalyzerClipIndicator class
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

#ifndef WAVEANALYZER_CLIP_INDICATOR_H
#define WAVEANALYZER_CLIP_INDICATOR_H

#include <QPixmap>
#include <QWidget>
#include <QMouseEvent>

namespace lmms
{

class WaveAnalyzerControls;

namespace gui
{

class WaveAnalyzerClipIndicator : public QWidget
{
	Q_OBJECT
public:
	WaveAnalyzerClipIndicator(WaveAnalyzerControls* controls, QWidget* parent = nullptr);
	virtual ~WaveAnalyzerClipIndicator();

	void mousePressEvent(QMouseEvent* e) override;

	void paintEvent(QPaintEvent* pe) override;
	void paintClipLeds(QPaintEvent* pe, QPainter & painter);
private:
	// Controls linked to this indicator
	WaveAnalyzerControls* m_controls;

	QPixmap* m_backgroundPixmap;
	QPixmap* m_clipLedsPixmap;
};

}

}

#endif
