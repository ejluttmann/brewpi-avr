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

#if BREWPI_TEMP_PROFILE
#include "ProfileManager.h"
#include "PiLink.h"
#include "BrewpiStrings.h"
#include "EepromManager.h"
#include "ProfileControl.h"

ProfileManager profileManager;

typedef ProfileConfig ProfileDefinition;

const char PROFILE_ATTRIB_SETPOINT = 's'; // s0, s1 ... s7
const char PROFILE_ATTRIB_DURATION = 'd'; // d0, d1 ... d7
const char PROFILE_ATTRIB_CURRENT_DURATION = 't';
const char PROFILE_ATTRIB_CURRENT_STEP = 'x';
const char PROFILE_ATTRIB_INTERPOLATION = 'p';
const char PROFILE_ATTRIB_RUN_PROFILE = 'r';

static const char STR_PROFILE_ATTRIB_INT[] PROGMEM = ",\"%c\":%d";


static void printProfile(ProfileConfig& config, const char* value, Print& p)
{
	char buf[17];
	uint8_t i;

	p.print('{');

	sprintf_P(buf, PSTR("\"%c\":%d"), PROFILE_ATTRIB_CURRENT_STEP, config.currentStep);
	p.print(buf);

	sprintf_P(buf, STR_PROFILE_ATTRIB_INT, PROFILE_ATTRIB_CURRENT_DURATION, config.currentDuration);
	p.print(buf);

	sprintf_P(buf, STR_PROFILE_ATTRIB_INT, PROFILE_ATTRIB_RUN_PROFILE, config.running);
	p.print(buf);

	sprintf_P(buf, STR_PROFILE_ATTRIB_INT, PROFILE_ATTRIB_INTERPOLATION, config.interpolation);
	p.print(buf);

	for(i=0; i<PROFILE_SIZE; i++){
		char tbuff[9];
		tempToString(tbuff, config.profileSteps[i].setpoint, 3, sizeof(tbuff)/sizeof(tbuff[0]));
		sprintf_P(buf, PSTR(",\"%c%d\":%s"), PROFILE_ATTRIB_SETPOINT, i, tbuff);
		p.print(buf);
		sprintf_P(buf, STR_PROFILE_ATTRIB_INT, PROFILE_ATTRIB_DURATION, i, config.profileSteps[i].duration);
		p.print(buf);
	}
	p.print('}');
}

static void handleProfileDefinition(const char* key, const char* val, void* pv)
{
	ProfileDefinition* def = (ProfileDefinition*) pv;

	if(key[0] == PROFILE_ATTRIB_CURRENT_DURATION){
		def->currentDuration = (uint16_t) atol(val);
	} else if(key[0] == PROFILE_ATTRIB_CURRENT_STEP){
		def->currentStep = (uint8_t) atol(val);
	} else if(key[0] == PROFILE_ATTRIB_RUN_PROFILE){
		def->running = (uint8_t) atol(val);
	} else if(key[0] == PROFILE_ATTRIB_INTERPOLATION){
		def->interpolation = (uint8_t) atol(val);
	} else if (key[0] == PROFILE_ATTRIB_SETPOINT) {
		uint8_t i = (uint8_t) atol(&key[1]);
		if (i < PROFILE_SIZE) {
			def->profileSteps[i].setpoint = stringToTemp(val);
		}
	} else if (key[0] == PROFILE_ATTRIB_DURATION) {
		uint8_t i = (uint8_t) atol(&key[1]);
		if (i < PROFILE_SIZE) {
			def->profileSteps[i].duration = (uint16_t) atol(val);
		}
	}
}

ProfileManager::ProfileManager() {
}

void ProfileManager::parseProfileDefinition(Stream& p){
	ProfileConfig cfg;
	bool valid = true;

	eepromManager.fetchProfile(cfg);

	piLink.parseJson(&handleProfileDefinition, &cfg);

	// TODO improve validation before saving?
	valid = (cfg.currentStep >= PROFILE_SIZE) ? false : valid;

	if(valid){
		eepromManager.storeProfile(cfg);
		profileControl.loadProfile();
	}

	piLink.printResponse('p');
	printProfile(cfg, NULL, p);
	piLink.printNewLine();

}
#endif //BREWPI_TEMP_PROFILE
