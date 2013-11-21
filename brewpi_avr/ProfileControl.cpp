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

#include "ProfileControl.h"
#include "EepromManager.h"
#include "TempControl.h"
#include "Ticks.h"

ProfileControl profileControl;

typedef uint16_t fixed0_16;
typedef uint32_t fixed16_16;


#if PROFILE_CONTROL_STATIC
unsigned long ProfileControl::lastTick;
ProfileConfig ProfileControl::profileConfig;
#endif

ProfileControl::ProfileControl() {
	lastTick = 0;
}

fixed7_9 ProfileControl::calcSetpoint(){
	fixed7_9 setpoint;

	if(profileConfig.currentStep == PROFILE_SIZE-1 || profileConfig.profileSteps[profileConfig.currentStep].duration == 0){
		setpoint = profileConfig.profileSteps[profileConfig.currentStep].setpoint;
	} else {
		// fixed point 0.16
		fixed0_16 t2 = ((fixed16_16)profileConfig.currentDuration << 16) / profileConfig.profileSteps[profileConfig.currentStep].duration;

		fixed7_9 sp1 = profileConfig.profileSteps[profileConfig.currentStep].setpoint;
		fixed7_9 sp2 = profileConfig.profileSteps[profileConfig.currentStep+1].setpoint;

		// Some wizardy required to implement smoothstep/smootherstep in fixed point
		if(profileConfig.interpolation == PROFILE_INTERPOLATION_SMOOTHSTEP){
			// smoothstep (x*x*(3-2*x))
			t2 = (fixed0_16)(((((fixed16_16)t2) * (fixed16_16)t2) >> 16) * ((3UL<<16) - ((fixed16_16) t2 << 1)) >> 16);
		} else if (profileConfig.interpolation == PROFILE_INTERPOLATION_SMOOTHERSTEP) {
			// smootherstep (x*x*x*(x*(x*6-15)+10))
			t2 = ((((t2 * ((fixed16_16)t2 * 6UL - (15UL<<16))) >> 16) + (10UL<<16)) * (((((fixed16_16)t2 * t2) >> 16) * t2) >> 16)) >> 16;
		}

		fixed0_16 t1 = (1UL<<16)-t2;

		setpoint = ((fixed23_9)sp1 * t1 + (fixed23_9)sp2 * t2) >> 16;
	}
	return setpoint;
}

void ProfileControl::loadProfile(){
	eepromManager.fetchProfile(profileConfig);
}

void ProfileControl::updateBeerSetpoint() {

	if(profileConfig.running && tempControl.getMode() == MODE_BEER_PROFILE){
		unsigned long currTick = ticks.millis();

		if(currTick-lastTick >= 60000){
			lastTick = currTick;

			if(profileConfig.currentDuration >= profileConfig.profileSteps[profileConfig.currentStep].duration){
				if(profileConfig.profileSteps[profileConfig.currentStep].duration == 0 || profileConfig.currentStep == PROFILE_SIZE-1){
					tempControl.setMode(MODE_BEER_CONSTANT,true);
					tempControl.setBeerTemp(profileConfig.profileSteps[profileConfig.currentStep].setpoint);
					return;
				} else {
					profileConfig.currentDuration = 0;
					profileConfig.currentStep = min(PROFILE_SIZE-1, profileConfig.currentStep+1);
				}
			}

			tempControl.setBeerTemp(profileControl.calcSetpoint());

			// Save progress to EEPROM every 30 mins
			if(profileConfig.currentDuration % 30 == 0){
				eepromManager.storeProfile(profileConfig);
			}

			profileConfig.currentDuration++;
		}
	}
}
