/*
 * WaveAnalyzerControlDialog.cpp - control dialog for amplifier effect
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

#include "WaveAnalyzerControlDialog.h"
#include "WaveAnalyzerControls.h"
#include "embed.h"
#include "Knob.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

namespace lmms::gui
{

WaveAnalyzerControlDialog::WaveAnalyzerControlDialog(WaveAnalyzerControls* controls) :
	EffectControlDialog(controls)
{

	/* =========================================================================================================================================== */

	setAutoFillBackground(true);
	setFixedSize(900, 400);

	QVBoxLayout* mainLayout = new QVBoxLayout; setLayout(mainLayout);

	QLabel* waveLabel = new QLabel;
	mainLayout->addWidget(waveLabel);
	QPixmap waveContents(900, 400);
	waveContents.fill(0,0,0);
	waveLabel->setPixmap(waveContents);

	/* =========================================================================================================================================== */

	auto makeKnob = [this](int x, int y, const QString& label, const QString& hintText, const QString& unit, FloatModel* model, bool isVolume)
	{
        Knob* newKnob = new Knob(KnobType::Bright26, this);
        newKnob->move(x, y);
        newKnob->setModel(model);
        newKnob->setLabel(label);
        newKnob->setHintText(hintText, unit);
        newKnob->setVolumeKnob(isVolume);
        return newKnob;
    };

	// makeKnob(16, 10, tr("VOL"), tr("Volume:"), "%", &controls->m_volumeModel, true);
	// makeKnob(57, 10, tr("PAN"), tr("Panning:"), "%", &controls->m_panModel, false);
	// makeKnob(16, 65, tr("LEFT"), tr("Left gain:"), "%", &controls->m_leftModel, true);
	// makeKnob(57, 65, tr("RIGHT"), tr("Right gain:"), "%", &controls->m_rightModel, true);
}

} // namespace lmms::gui
