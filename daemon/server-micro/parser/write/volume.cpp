#include "daemon/server-micro/parser/write/volume.h"
#include "daemon/server-micro/response.h"
#include "core/sound/player.h"
#include "core/property.h"
#include "core/predefined.h"

MHD_Result write_volume::parse(MHD_Connection *connection, std::string param) {
	snd_mixer_t *mixer = nullptr;
	snd_mixer_elem_t *elem = nullptr;
	long volume_min = 0;
	long volume_max = 0;
	long volume_now = 0;
	int volume = std::stoi(param);
	if (snd_mixer_open(&mixer, 0) < 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	} else if (snd_mixer_attach(
		       mixer,
		       CONFIG_STRING("alsa", "mixer", "name").c_str()) < 0) {
		snd_mixer_close(mixer);
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	snd_mixer_selem_register(mixer, NULL, NULL);
	snd_mixer_load(mixer);
	for (elem = snd_mixer_first_elem(mixer);
	     elem != nullptr;
	     elem = snd_mixer_elem_next(elem)) {
		if (snd_mixer_selem_is_active(elem) == 0) {
			continue;
		} else if (snd_mixer_selem_has_playback_volume(elem)) {
			break;
		}
	}

	if (elem == nullptr) {
		snd_mixer_close(mixer);
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	snd_mixer_selem_get_playback_volume_range(elem, &volume_min, &volume_max);
	snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume_now);
	if (volume == 0) {
		snd_mixer_selem_set_playback_volume_all(elem, volume_min);
	} else {
		volume_now += volume;
		if (volume_now < volume_min) {
			volume_now = volume_min;
		} else if (volume_now > volume_max) {
			volume_now = volume_max;
		}

		snd_mixer_selem_set_playback_volume_all(elem, volume_now);
	}

	snd_mixer_close(mixer);
	return response::empty(connection, MHD_HTTP_OK);
}