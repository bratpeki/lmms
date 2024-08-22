/*
 * WaveAnalyzerControls.cpp - Definition of WaveAnalyzerControls class
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

#include <QDomElement>

#include "WaveAnalyzerControls.h"

#include "WaveAnalyzer.h"

namespace lmms
{

WaveAnalyzerControls::WaveAnalyzerControls(WaveAnalyzerEffect* effect) :
	EffectControls(effect),
	m_effect(effect),
	m_startModel(false, this, tr("Start Acquiring Data")),
	m_snapshotModel(false, this, tr("Snapshot current wave")),
	m_numberOfFrames(1024.0f, 512.0f, BUFFER_MAX_SIZE, 1.0f, this, tr("Number of frames")),
	m_drawingMode(),
	m_leftLevel(0, 0.0f, 1.0f, 0.01f, this, tr("Left Level")),
	m_rightLevel(0, 0.0f, 1.0f, 0.01f, this, tr("Right Level")),
	m_clippedLeft(false, this, tr("Clipped left")),
	m_clippedRight(false, this, tr("Clipped right"))
{
	// Add the drawing modes
	m_drawingMode.addItem(tr("Raw"));
	m_drawingMode.addItem(tr("Peaks/Troughs"));
	m_drawingMode.addItem(tr("Smoothed Bezier"));
	m_drawingMode.setValue(0);
}

WaveAnalyzerControls::~WaveAnalyzerControls()
{
}

/* @brief Shifts the buffer to the left by framesCount number of frames
 * so the new frames can be copied at the right side.
 * @param Int framesCount number of frames to be shifted
 */
void WaveAnalyzerControls::shiftBuffers(int framesCount)
{
	int currentBufferSize = static_cast<int>(m_numberOfFrames.value());
	int lastBufferIndex = currentBufferSize - 1;

	for (int i = 0; i <= lastBufferIndex; ++i)
	{
		// If the frame we are shifting is out of bound return
		if (i + framesCount > lastBufferIndex) { return; }
		// Else we shift the frames to the beginning of the buffer
		m_ampBufferL[i] = m_ampBufferL[i + framesCount];
		m_ampBufferR[i] = m_ampBufferR[i + framesCount];
	}
}

void WaveAnalyzerControls::loadSettings(const QDomElement & _this)
{
}

void WaveAnalyzerControls::saveSettings(QDomDocument & doc, QDomElement & parent)
{
}

}
