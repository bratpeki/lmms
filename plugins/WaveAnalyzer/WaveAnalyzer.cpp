/*
 * WaveAnalyzer.cpp - Definition of WaveAnalyzerEffect class
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

#include <cmath>

#include "WaveAnalyzer.h"

#include "embed.h"
#include "lmms_basics.h"
#include "lmms_math.h"
#include "plugin_export.h"

namespace lmms
{

extern "C" {
	Plugin::Descriptor PLUGIN_EXPORT WaveAnalyzer_plugin_descriptor = 
	{
		LMMS_STRINGIFY(PLUGIN_NAME),
		"Wave Analyzer",
		QT_TRANSLATE_NOOP("PluginBrowser", "A simple oscilloscope effect."),
		"Ian Caio <iancaio_dev/at/hotmail.com>",
		0x0112,
		Plugin::Type::Effect,
		new PluginPixmapLoader("logo"),
		nullptr,
		nullptr
	};
}

WaveAnalyzerEffect::WaveAnalyzerEffect(Model* parent, const Plugin::Descriptor::SubPluginFeatures::Key *key) :
	Effect(&WaveAnalyzer_plugin_descriptor, parent, key),
	m_controls(this)
{
}

WaveAnalyzerEffect::~WaveAnalyzerEffect()
{
}

bool WaveAnalyzerEffect::processAudioBuffer(SampleFrame *buffer, const fpp_t frameCount)
{
	if (!isRunning() || !isEnabled()) { return false; }

	// Only do processing when GUI is visible and we haven't paused
	if (m_controls.isViewVisible() && m_controls.m_startModel.value())
	{
		float avgLeft = 0;
		float avgRight = 0;

		// Shift the oscilloscope buffer to fit those new values
		m_controls.shiftBuffers(frameCount);

		// For the oscilloscope buffer
		int lastBufferIndex = static_cast<int>(m_controls.m_numberOfFrames.value()) - 1;
		int firstBufferIndex = lastBufferIndex - (frameCount - 1);

		// Get RMS of left and right levels and also write the values to the
		// oscilloscope buffer
		for (fpp_t f = 0; f < frameCount; ++f)
		{
			m_controls.m_ampBufferL[firstBufferIndex + f] = buffer[f][0];
			m_controls.m_ampBufferR[firstBufferIndex + f] = buffer[f][1];

			// For the level indicator
			avgLeft += buffer[f][0] * buffer[f][0];
			avgRight += buffer[f][1] * buffer[f][1];

			// Check if left or right channels clipped
			if (buffer[f][0] >= 1 || buffer[f][0] <= -1)
			{
				m_controls.m_clippedLeft.setAutomatedValue(true);
			}
			if (buffer[f][1] >= 1 || buffer[f][1] <= -1)
			{
				m_controls.m_clippedRight.setAutomatedValue(true);
			}
		}
		if (frameCount > 0)
		{
			avgLeft = sqrt(avgLeft / frameCount);
			avgRight = sqrt(avgRight / frameCount);
		}

		// Emit signal that the buffer has changed
		m_controls.bufferChanged(frameCount);

		// Update the levels on the control
		m_controls.m_leftLevel.setAutomatedValue(avgLeft);
		m_controls.m_rightLevel.setAutomatedValue(avgRight);
	}
	return true;
}

extern "C"
{
	PLUGIN_EXPORT Plugin * lmms_plugin_main(Model* parent, void* data)
	{
		return new WaveAnalyzerEffect(parent, static_cast<const Plugin::Descriptor::SubPluginFeatures::Key *>(data));
	}
}

}