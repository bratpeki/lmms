/*
 * WaveAnalyzerControls.h - Declaration of WaveAnalyzerControls class
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

#ifndef WAVEANALYZER_CONTROLS_H
#define WAVEANALYZER_CONTROLS_H

#define BUFFER_MAX_SIZE 88200

#include "ComboBoxModel.h"
#include "EffectControls.h"
#include "WaveAnalyzerControlDialog.h"

namespace lmms
{

namespace gui
{
	class WaveAnalyzerControlDialog;
	class WaveAnalyzerLevelIndicator;
	class WaveAnalyzerClipIndicator;
	class WaveAnalyzerOsc;
	class WaveAnalyzerWaveform;
}

class WaveAnalyzerEffect;

class WaveAnalyzerControls : public EffectControls
{
	Q_OBJECT
public:
	WaveAnalyzerControls(WaveAnalyzerEffect* effect);
	virtual ~WaveAnalyzerControls();

	virtual void saveSettings(QDomDocument & doc, QDomElement & parent);
	virtual void loadSettings(const QDomElement & _this);

	void shiftBuffers(int framesCount);

	inline virtual QString nodeName() const
	{
		return "WaveAnalyzerControls";
	}

	virtual int controlCount()
	{
		return 2;
	}

	virtual lmms::gui::EffectControlDialog* createView()
	{
		return new lmms::gui::WaveAnalyzerControlDialog(this);
	}

signals:
	void bufferChanged(int count);

private:
	WaveAnalyzerEffect* m_effect;

	// Model to start acquiring data for the WaveAnalyzer
	BoolModel m_startModel;
	// Model to snapshot the current wave in the background
	BoolModel m_snapshotModel;

	// Model to set the number of frames that is drawn in the oscilloscope
	FloatModel m_numberOfFrames;

	// Model to set the drawing mode
	ComboBoxModel m_drawingMode;

	// Current input levels
	FloatModel m_leftLevel;
	FloatModel m_rightLevel;

	// Has the audio clipped on the left/right?
	BoolModel m_clippedLeft;
	BoolModel m_clippedRight;

	// Actual buffer with the amplitudes
	float m_ampBufferL[BUFFER_MAX_SIZE];
	float m_ampBufferR[BUFFER_MAX_SIZE];

	friend class WaveAnalyzerEffect;
	friend class gui::WaveAnalyzerControlDialog;
	friend class gui::WaveAnalyzerLevelIndicator;
	friend class gui::WaveAnalyzerClipIndicator;
	friend class gui::WaveAnalyzerOsc;
	friend class gui::WaveAnalyzerWaveform;
};

}

#endif
