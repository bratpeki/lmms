/*
 * WaveAnalyzerControls.cpp - controls for amplifier effect
 *
 * Copyright (c) 2014 Vesa Kivimäki <contact/dot/diizy/at/nbl/dot/fi>
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "WaveAnalyzerControls.h"

#include "WaveAnalyzer.h"
#include "AudioEngine.h"

#include <algorithm>

#include <QDomElement>


namespace lmms
{

WaveAnalyzerControls::WaveAnalyzerControls(WaveAnalyzerEffect* effect) :
	EffectControls(effect),
	m_effect(effect),
	m_frameModel(DEFAULT_BUFFER_SIZE, MINIMUM_BUFFER_SIZE, WAVE_ANALYZER_MAXFRAMES, this, "Frames"),
	m_pauseModel(false, this, "Pause")
{
	std::fill(std::begin(m_drawBufferL), std::end(m_drawBufferL), 0.0f);
	std::fill(std::begin(m_drawBufferR), std::end(m_drawBufferR), 0.0f);
}


void WaveAnalyzerControls::loadSettings(const QDomElement& parent)
{
	m_frameModel.loadSettings(parent, "frames");
	// m_pauseModel.loadSettings(parent, "pause");
}


void WaveAnalyzerControls::saveSettings(QDomDocument& doc, QDomElement& parent)
{
	m_frameModel.loadSettings(parent, "frames");
	// m_pauseModel.loadSettings(parent, "pause");
}


} // namespace lmms
