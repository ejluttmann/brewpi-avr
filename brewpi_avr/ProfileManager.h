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

#include "Brewpi.h"
#include "TemperatureFormats.h"

#define PROFILE_SIZE		8

#define PROFILE_INTERPOLATION_LINEAR		0
#define PROFILE_INTERPOLATION_SMOOTHSTEP	1
#define PROFILE_INTERPOLATION_SMOOTHERSTEP	2
#define PROFILE_INTERPOLATION_RESERVED		3

struct ProfileStep {
	fixed7_9	setpoint;
	uint16_t	duration;	// Step duration in minutes
};

struct ProfileConfig {

	uint8_t		currentStep;
	uint16_t 	currentDuration;
	uint8_t		reserved			:4;
	uint8_t 	interpolation		:2;
	uint8_t		running				:1;

	ProfileStep profileSteps[PROFILE_SIZE];
};


class ProfileManager {

public:

	ProfileManager();

	static void parseProfileDefinition(Stream& p);

};

extern ProfileManager profileManager;
