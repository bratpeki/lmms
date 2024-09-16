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

#include <QDomElement>


namespace lmms
{

WaveAnalyzerControls::WaveAnalyzerControls(WaveAnalyzerEffect* effect) :
	EffectControls(effect),
	m_effect(effect),
	/* Very arbitrarily chosen value of 8000 */
	m_frameModel(DEFAULT_BUFFER_SIZE, MINIMUM_BUFFER_SIZE, 8000, this, "Frames"),
	m_pauseModel(false, this, "Pause")
{
}


void WaveAnalyzerControls::loadSettings(const QDomElement& parent)
{
	/* Maybe don't load and save the pause? */
	m_frameModel.loadSettings(parent, "frames");
	m_pauseModel.loadSettings(parent, "pause");
}


void WaveAnalyzerControls::saveSettings(QDomDocument& doc, QDomElement& parent)
{
	m_frameModel.loadSettings(parent, "frames");
	m_pauseModel.loadSettings(parent, "pause");
}


} // namespace lmms
