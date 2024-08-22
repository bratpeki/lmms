/*
 * WaveAnalyzerControlDialog.cpp - Definition of WaveAnalyzerControlDialog class
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

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "WaveAnalyzerControlDialog.h"
#include "WaveAnalyzerLevelIndicator.h"
#include "WaveAnalyzerClipIndicator.h"
#include "WaveAnalyzerOsc.h"

#include "ComboBox.h"
#include "embed.h"
#include "Knob.h"
#include "PixmapButton.h"
#include "WaveAnalyzerControls.h"

namespace lmms::gui
{

WaveAnalyzerControlDialog::WaveAnalyzerControlDialog(WaveAnalyzerControls* controls) :
	EffectControlDialog(controls)
{
	/*  ____________________________________
	 *  | Controls      Level indicator     |
	 *  |___________________________________|
	 *  |                                   |
	 *  |          Oscilloscope             |
	 *  |                                   |
	 *  |                                   |
	 *  |                                   |
	 *  |                                   |
	 *  |                                   |
	 *  |___________________________________|
	 */

	// Layout to separate oscilloscope from the top bar
	QVBoxLayout* mainLayout = new QVBoxLayout;
	setLayout(mainLayout);

	// Layout for the top bar (controls + level indicator)
	QHBoxLayout* topBar = new QHBoxLayout;

	// Start button
	PixmapButton* startButton = new PixmapButton(this, tr("Start acquiring data"));
	QPixmap* startOnPixMap = new QPixmap(PLUGIN_NAME::getIconPixmap("pause"));
	QPixmap* startOffPixMap = new QPixmap(PLUGIN_NAME::getIconPixmap("play"));
	startButton->setActiveGraphic(*startOnPixMap);
	startButton->setInactiveGraphic(*startOffPixMap);
	startButton->setCheckable(true);
	startButton->setModel(&controls->m_startModel);

	// Snapshot button
	PixmapButton* snapshotButton = new PixmapButton(this, tr("Snapshot current wave"));
	QPixmap* snapshotOnPixMap = new QPixmap(PLUGIN_NAME::getIconPixmap("snapshotOn"));
	QPixmap* snapshotOffPixMap = new QPixmap(PLUGIN_NAME::getIconPixmap("snapshotOff"));
	snapshotButton->setActiveGraphic(*snapshotOnPixMap);
	snapshotButton->setInactiveGraphic(*snapshotOffPixMap);
	snapshotButton->setCheckable(true);
	snapshotButton->setModel(&controls->m_snapshotModel);

	// Adds drawing mode Combobox + Label
	QVBoxLayout* comboBoxLayout = new QVBoxLayout;

	QLabel* drawingModeTitle = new QLabel("Drawing mode:");

	ComboBox* drawingMode = new ComboBox(this);
	drawingMode->setModel(&controls->m_drawingMode);
	drawingMode->setFixedSize(130, ComboBox::DEFAULT_HEIGHT);
	drawingMode->setToolTip(tr("Drawing mode"));

	comboBoxLayout->setSpacing(0);

	comboBoxLayout->addWidget(drawingModeTitle);
	comboBoxLayout->addWidget(drawingMode);

	// Adds numberOfFrames Knob
	Knob* framesKnob = new Knob(KnobType::Bright26);
	framesKnob->setLabel("Frames");
	framesKnob->setModel(&controls->m_numberOfFrames);

	QHBoxLayout* levelIndicatorLayout = new QHBoxLayout;

	// Add level indicator
	WaveAnalyzerLevelIndicator* levelIndicator = new WaveAnalyzerLevelIndicator(controls, this);
	// Level changes on the controls will trigger updates on the level indicator
	connect(&controls->m_leftLevel, SIGNAL(dataChanged()), levelIndicator, SLOT(update()));
	connect(&controls->m_rightLevel, SIGNAL(dataChanged()), levelIndicator, SLOT(update()));

	// Add clipping indicator
	WaveAnalyzerClipIndicator* clipIndicator = new WaveAnalyzerClipIndicator(controls, this);
	// Clipping models will trigger the leds
	connect(&controls->m_clippedLeft, SIGNAL(dataChanged()), clipIndicator, SLOT(update()));
	connect(&controls->m_clippedRight, SIGNAL(dataChanged()), clipIndicator, SLOT(update()));

	levelIndicatorLayout->setSpacing(0);
	levelIndicatorLayout->addWidget(levelIndicator);
	levelIndicatorLayout->addWidget(clipIndicator);

	topBar->setContentsMargins(20, 5, 20, 5);
	topBar->setSpacing(20);

	topBar->addWidget(startButton);
	topBar->addWidget(snapshotButton);
	topBar->addLayout(comboBoxLayout);
	topBar->addWidget(framesKnob);
	topBar->addStretch();
	topBar->addLayout(levelIndicatorLayout);

	// Horizontal layout for the oscilloscope
	QHBoxLayout* bottomLayout = new QHBoxLayout;

	// Oscilloscope
	WaveAnalyzerOsc* osc = new WaveAnalyzerOsc(controls, this);

	bottomLayout->addWidget(osc);

	mainLayout->addLayout(topBar);
	mainLayout->addLayout(bottomLayout);
}

WaveAnalyzerControlDialog::~WaveAnalyzerControlDialog()
{
}

}