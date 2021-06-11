#include "save.h"

#include <libdragon.h>
#include <string.h>

void save_write(SaveFile save) {
	if (eeprom_present() == EEPROM_NONE)
		return;

	uint8_t buffer[sizeof(save)];
	memcpy(buffer, (uint8_t *)&save, sizeof(save));

	eeprom_write(0, &save.check);
	for (size_t i = 0; i < sizeof(buffer); ++i) {
		eeprom_write(i + 1, &buffer[i]);
	}
}

SaveFile save_read() {
	SaveFile save;

	// no EEPROM, return default save
	if (eeprom_present() == EEPROM_NONE) {
		save.check = 0;
		for (size_t i = 0; i < MINIGAME_MAX - 1; ++i) {
			save.times[i] = 0;
		}
		return save;
	}

	// check first byte to see if it exists, if not, loads default
	eeprom_read(0, &save.check);
	if (save.check <= 0) {
		save.check = 0;
		for (size_t i = 0; i < MINIGAME_MAX - 1; ++i) {
			save.times[i] = 0;
		}
		return save;
	}

	uint8_t buffer[sizeof(save)];
	for (size_t i = 0; i < sizeof(buffer); ++i) {
		eeprom_read(i + 1, &buffer[i]);
	}

	memcpy(&save, buffer, sizeof(save));

	return save;
}