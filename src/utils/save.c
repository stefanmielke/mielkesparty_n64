#include "save.h"

#include <libdragon.h>
#include <string.h>

#define MAGIC_NUMBER (uint8_t)300

uint8_t checksum(SaveFile *save) {
	size_t save_size = sizeof(save->body);
	char *ptr = (char *)&save->body;
	uint8_t chk = 0;
	while (save_size-- != 0)
		chk -= *ptr++;
	return chk;
}

SaveFile new_save() {
	SaveFile save;
	save.check = 0;
	for (size_t i = 0; i < MINIGAME_MAX - 1; ++i) {
		save.body.times[i] = 0;
	}
	return save;
}

void save_write(SaveFile *save) {
	if (eeprom_present() == EEPROM_NONE)
		return;

	uint8_t buffer[sizeof(save->body)];
	memcpy(buffer, (uint8_t *)&save->body, sizeof(save->body));

	uint8_t check = checksum(save);
	const uint8_t magic = MAGIC_NUMBER;

	eeprom_write(0, &check);
	eeprom_write(1, &magic);
	for (size_t i = 0; i < sizeof(buffer); ++i) {
		eeprom_write(i + 2, &buffer[i]);
	}
}

SaveFile save_read() {
	SaveFile save;

	// no EEPROM, return default save
	if (eeprom_present() == EEPROM_NONE) {
		return new_save();
	}

	uint8_t magic;

	// check first byte to see if it exists, if not, loads default
	eeprom_read(0, &save.check);
	eeprom_read(1, &magic);
	if (magic != MAGIC_NUMBER) {
		return new_save();
	}

	uint8_t buffer[sizeof(save.body)];
	for (size_t i = 0; i < sizeof(buffer); ++i) {
		eeprom_read(i + 2, &buffer[i]);
	}

	memcpy(&save.body, buffer, sizeof(save.body));

	uint8_t check = checksum(&save);
	if (save.check != check) {
		return new_save();
	}

	return save;
}