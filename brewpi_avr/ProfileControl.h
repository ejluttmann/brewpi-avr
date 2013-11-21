/*
 * Copyright 2013 Mats Staffansson
 * Copyright 2013 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <inttypes.h>
#include "TemperatureFormats.h"
#include "ProfileManager.h"

#if PROFILE_CONTROL_STATIC
#define PROFILE_CONTROL_METHOD static
#define PROFILE_CONTROL_FIELD static
#else
#define PROFILE_CONTROL_METHOD
#define PROFILE_CONTROL_FIELD
#endif


class ProfileControl {

private:
	PROFILE_CONTROL_FIELD unsigned long lastTick;
	PROFILE_CONTROL_FIELD ProfileConfig profileConfig;
	PROFILE_CONTROL_METHOD fixed7_9 calcSetpoint();

public:
	ProfileControl();

	PROFILE_CONTROL_METHOD void loadProfile();
	PROFILE_CONTROL_METHOD void updateBeerSetpoint();
};

extern ProfileControl profileControl;
