/*
 * WaveAnalyzerClipIndicator.cpp - Definition of
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

#include "WaveAnalyzerClipIndicator.h"

#include <QPaintEvent>
#include <QPainter>

#include "WaveAnalyzer.h"

#include "embed.h"

namespace lmms::gui
{

WaveAnalyzerClipIndicator::WaveAnalyzerClipIndicator(WaveAnalyzerControls* controls, QWidget* parent) :
	QWidget(parent),
	m_controls(controls)
{
	// Initialize widget
	// Load pixmaps
	m_backgroundPixmap = new QPixmap(PLUGIN_NAME::getIconPixmap("clipping_background"));
	m_clipLedsPixmap = new QPixmap(PLUGIN_NAME::getIconPixmap("clipping_leds"));

	// Set minimum and maximum size
	QSize s = m_backgroundPixmap->size();
	setMinimumSize(s);
	setMaximumSize(s);
	resize(s);
}

WaveAnalyzerClipIndicator::~WaveAnalyzerClipIndicator()
{
}

void WaveAnalyzerClipIndicator::mousePressEvent(QMouseEvent* e)
{
	// Clicking on the clip indicator resets it
	m_controls->m_clippedLeft.setAutomatedValue(false);
	m_controls->m_clippedRight.setAutomatedValue(false);
}

void WaveAnalyzerClipIndicator::paintEvent(QPaintEvent* pe)
{
	QPainter painter(this);

	// Draw background
	painter.drawPixmap(pe->rect(), *m_backgroundPixmap, pe->rect());

	// Draw Clipping indicators
	paintClipLeds(pe, painter);
}

void WaveAnalyzerClipIndicator::paintClipLeds(QPaintEvent* pe, QPainter & painter)
{
	const int width = m_backgroundPixmap->width();
	// Height / 2 since we have two clipping indicators
	const int height = m_backgroundPixmap->height() / 2;

	float clippingL = m_controls->m_clippedLeft.value();
	float clippingR = m_controls->m_clippedRight.value();

	// Draw indicators
	if (clippingL)
	{
		QRect leftLed(0, 0, width, height);
		painter.drawPixmap(leftLed, *m_clipLedsPixmap, leftLed);
	}
	if (clippingR)
	{
		QRect rightLed(0, height, width, height);
		painter.drawPixmap(rightLed, *m_clipLedsPixmap, rightLed);
	}
}

}
