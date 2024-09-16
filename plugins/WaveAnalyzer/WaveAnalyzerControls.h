/*
 * WaveAnalyzerControls.h - controls for bassboosterx -effect
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

#ifndef LMMS_WAVE_ANALYZER_CONTROLS_H
#define LMMS_WAVE_ANALYZER_CONTROLS_H

#include "EffectControls.h"
#include "WaveAnalyzerControlDialog.h"
#include "AudioEngine.h"

#define WAVE_ANALYZER_MAXFRAMES 8000

namespace lmms
{

class WaveAnalyzerEffect;

class WaveAnalyzerControls : public EffectControls
{
	Q_OBJECT
public:
	WaveAnalyzerControls(WaveAnalyzerEffect* effect);
	~WaveAnalyzerControls() override = default;

	void saveSettings(QDomDocument& doc, QDomElement& parent) override;
	void loadSettings(const QDomElement& parent) override;
	inline QString nodeName() const override
	{
		return "WaveAnalyzerControls";
	}
	gui::EffectControlDialog* createView() override
    {
        m_view = new gui::WaveAnalyzerControlDialog(this);
        return m_view;
    }
	int controlCount() override { return 2; }

private:

	gui::WaveAnalyzerControlDialog* m_view;
	WaveAnalyzerEffect* m_effect;

	IntModel m_frameModel;
	BoolModel m_pauseModel;

	float m_ampBufferL[DEFAULT_BUFFER_SIZE];
	float m_ampBufferR[DEFAULT_BUFFER_SIZE];

	float m_drawBufferL[WAVE_ANALYZER_MAXFRAMES];
	float m_drawBufferR[WAVE_ANALYZER_MAXFRAMES];

	friend class gui::WaveAnalyzerControlDialog;
	friend class WaveAnalyzerEffect;
};

} // namespace lmms

#endif // LMMS_WAVE_ANALYZER_CONTROLS_H
