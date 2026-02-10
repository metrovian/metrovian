#include "daemon/server-micro/parser/write/volume.h"
#include "daemon/server-micro/response.h"
#include "core/sound/player.h"

MHD_Result write_volume::parse(MHD_Connection *connection, std::string param) {
	snd_mixer_t *mixer = nullptr;
	snd_mixer_elem_t *elem = nullptr;
	snd_mixer_selem_id_t *sid = nullptr;
	long volume_min = 0;
	long volume_max = 0;
	long volume_now = 0;
	int volume = std::stoi(param);
	if (snd_mixer_open(&mixer, 0) < 0) {
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	} else if (snd_mixer_attach(mixer, "default") < 0) {
		snd_mixer_close(mixer);
		return response::empty(connection, MHD_HTTP_BAD_REQUEST);
	}

	snd_mixer_selem_register(mixer, NULL, NULL);
	snd_mixer_load(mixer);
	snd_mixer_selem_id_malloc(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, "PCM");
	elem = snd_mixer_find_selem(mixer, sid);
	if (elem == nullptr) {
		snd_mixer_selem_id_free(sid);
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

	snd_mixer_selem_id_free(sid);
	snd_mixer_close(mixer);
	return response::empty(connection, MHD_HTTP_OK);
}