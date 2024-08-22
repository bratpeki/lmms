/*
 * WaveAnalyzerLevelIndicator.cpp - Definition of
 * WaveAnalyzerLevelIndicator class
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

#include "WaveAnalyzerLevelIndicator.h"

#include <QPaintEvent>
#include <QPainter>

#include "WaveAnalyzer.h"

#include "embed.h"
#include "lmms_math.h"

namespace lmms::gui
{

WaveAnalyzerLevelIndicator::WaveAnalyzerLevelIndicator(WaveAnalyzerControls* controls, QWidget* parent) :
	QWidget(parent),
	m_controls(controls),
	m_minPeak(0.01f),
	m_maxPeak(1.00f)
{
	// Initialize widget
	// Load pixmaps
	m_backgroundPixmap = new QPixmap(PLUGIN_NAME::getIconPixmap("fader_background"));
	m_levelsPixmap = new QPixmap(PLUGIN_NAME::getIconPixmap("fader_leds"));

	// Set minimum and maximum size
	QSize s = m_backgroundPixmap->size();
	setMinimumSize(s);
	setMaximumSize(s);
	resize(s);
}

WaveAnalyzerLevelIndicator::~WaveAnalyzerLevelIndicator()
{
}

void WaveAnalyzerLevelIndicator::paintEvent(QPaintEvent* pe)
{
	QPainter painter(this);

	// Draw background
	painter.drawPixmap(pe->rect(), *m_backgroundPixmap, pe->rect());

	// Draw levels
	paintPeaks(pe, painter);
}

void WaveAnalyzerLevelIndicator::paintPeaks(QPaintEvent* pe, QPainter & painter)
{
	const int width = m_backgroundPixmap->width();
	// Height / 2 since we have two level indicators
	const int height = m_backgroundPixmap->height() / 2;

	// Constant and lambda function to get the proportion of
	// the value given in relation to the min/max
	const float pScale = 1 / (m_maxPeak - m_minPeak);
	auto getProportion = [this, pScale](float value)
	{
		// TODO: Maybe can be approximated to value * pScale
		float proportion = (value - m_minPeak) * pScale;
		return proportion;
	};

	// TODO: Replace with std::clamp when we have C++17
	// Lambda function to replace std::clamp
	auto clamp = [](float value, float min, float max)
	{
		if (value < min) { return min; }
		else if (value > max) { return max; }
		else { return value; }
	};

	float leftPeak = m_controls->m_leftLevel.value();
	float rightPeak = m_controls->m_rightLevel.value();

	// Draw levels
	float leftSize = getProportion(clamp(leftPeak, m_minPeak, m_maxPeak)) * width;
	QRect leftLevel(0, 0, leftSize, height);
	painter.drawPixmap(leftLevel, *m_levelsPixmap, leftLevel);

	float rightSize = getProportion(clamp(rightPeak, m_minPeak, m_maxPeak)) * width;
	QRect rightLevel(0, height, rightSize, height);
	painter.drawPixmap(rightLevel, *m_levelsPixmap, rightLevel);
}

}
