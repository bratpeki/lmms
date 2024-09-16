/*
 * WaveAnalyzer.cpp - A native amplifier effect plugin with sample-exact amplification
 *
 * Copyright (c) 2014 Vesa Kivimäki <contact/dot/diizy/at/nbl/dot/fi>
 * Copyright (c) 2006-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "WaveAnalyzer.h"

#include "embed.h"
#include "plugin_export.h"

namespace lmms
{

extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT waveanalzyer_plugin_descriptor =
{
	LMMS_STRINGIFY(PLUGIN_NAME),
	"Wave Analyzer",
	QT_TRANSLATE_NOOP("PluginBrowser", "poopity scoop poopity woop di poop di scoop"),
	"Vesa Kivimäki <contact/dot/diizy/at/nbl/dot/fi>",
	0x0100,
	Plugin::Type::Effect,
	new PluginPixmapLoader("logo"),
	nullptr,
	nullptr,
} ;

}


WaveAnalyzerEffect::WaveAnalyzerEffect(Model* parent, const Descriptor::SubPluginFeatures::Key* key) :
	Effect(&waveanalzyer_plugin_descriptor, parent, key),
	m_ampControls(this)
{
}


bool WaveAnalyzerEffect::processAudioBuffer(SampleFrame* buf, const fpp_t frames)
{
	if (!isEnabled() || !isRunning()) { return false ; }

	double outSum = 0.0;
	const float d = dryLevel();
	const float w = wetLevel();

	// const ValueBuffer* volumeBuf = m_ampControls.m_volumeModel.valueBuffer();
	// const ValueBuffer* panBuf = m_ampControls.m_panModel.valueBuffer();
	// const ValueBuffer* leftBuf = m_ampControls.m_leftModel.valueBuffer();
	// const ValueBuffer* rightBuf = m_ampControls.m_rightModel.valueBuffer();

	for (fpp_t f = 0; f < frames; ++f)
	{
		m_ampControls.m_ampBufferL[f] = buf[f][0];
		m_ampControls.m_ampBufferR[f] = buf[f][1];
	}

	checkGate(outSum / frames);

	return isRunning();
}


extern "C"
{

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin* lmms_plugin_main(Model* parent, void* data)
{
	return new WaveAnalyzerEffect(parent, static_cast<const Plugin::Descriptor::SubPluginFeatures::Key*>(data));
}

}

} // namespace lmms
