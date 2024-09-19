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
#include "MainWindow.h"
#include "GuiApplication.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

namespace lmms::gui
{

WaveAnalyzerControlDialog::WaveAnalyzerControlDialog(WaveAnalyzerControls* controls) :
	EffectControlDialog(controls),
	m_controls(controls)
{

	/* =========================================================================================================================================== */

	setFixedSize(900, 400);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	QLabel* waveLabel = new QLabel;
	waveLabel->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(waveLabel);
	QPixmap waveContents(900, 400);
	waveContents.fill(cblack);
	waveLabel->setPixmap(waveContents);

	pixmapWidth = waveContents.width();

	connect(getGUI()->mainWindow(), &MainWindow::periodicUpdate, this, &WaveAnalyzerControlDialog::updateDisplay);

	/* =========================================================================================================================================== */

	/* auto makeKnob = [this](int x, int y, const QString& label, const QString& hintText, const QString& unit, FloatModel* model, bool isVolume)
	{
        Knob* newKnob = new Knob(KnobType::Bright26, this);
        newKnob->move(x, y);
        newKnob->setModel(model);
        newKnob->setLabel(label);
        newKnob->setHintText(hintText, unit);
        newKnob->setVolumeKnob(isVolume);
        return newKnob;
    }; */

	// makeKnob(16, 10, tr("VOL"), tr("Volume:"), "%", &controls->m_volumeModel, true);
	// makeKnob(57, 10, tr("PAN"), tr("Panning:"), "%", &controls->m_panModel, false);
	// makeKnob(16, 65, tr("LEFT"), tr("Left gain:"), "%", &controls->m_leftModel, true);
	// makeKnob(57, 65, tr("RIGHT"), tr("Right gain:"), "%", &controls->m_rightModel, true);
}

// TODO: When paused, make it so the user can see how loud the signal is in that point

void WaveAnalyzerControlDialog::updateDisplay() {

	/*
	 * The drawing procedure is as follows:
	 * Get the frames count and amp buffers
	 * Shift the points in the buffer by 256 spaces to the left, making space for 256 incoming frames
	 * Once space has been made, populate the array with new values
	 * Clear the pixmap
	 * Draw the new points
	 */

	// printf("%lf\n", m_controls->m_ampBufferL[0]);

	printf("# of frames: %d\t# of pixels to draw to: %d\n", (m_controls->m_frameModel).value(), pixmapWidth);

	// m_controls->m_ampBufferL, m_controls->m_ampBufferR
	// m_controls->m_drawBufferL, m_controls->m_drawBufferR

/*
Lost's code:
std::vector<float> buf1;
std::vector<float> buf2;
int index = 0;

buf1[index] = input;
if (++index >= size) {index = 0; std::copy(buf1.begin(), buf1.end(), buf2.begin());}

then draw buf2
*/

}

} // namespace lmms::gui
