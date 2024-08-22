/*
 * WaveAnalyzerControlDialog.h - Declaration of WaveAnalyzerControlDialog class
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

#ifndef WAVEANALYZER_CONTROL_DIALOG_H
#define WAVEANALYZER_CONTROL_DIALOG_H

#include "EffectControlDialog.h"

namespace lmms
{

class WaveAnalyzerControls;

namespace gui
{

class WaveAnalyzerControlDialog : public EffectControlDialog
{
	Q_OBJECT
public:
	WaveAnalyzerControlDialog(WaveAnalyzerControls* controls);
	virtual ~WaveAnalyzerControlDialog();
};

}

}

#endif
