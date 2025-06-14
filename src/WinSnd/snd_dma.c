//
// snd_dma.c -- Main control for any streaming sound output device.
//
// Copyright 1998 Raven Software
//

#include "snd_dma.h"
#include "snd_mem.h"
#include "snd_mix.h"
#include "snd_win.h"
#include "client.h"
#include "g_local.h"
#include "q_clientserver.h"
#include "qcommon.h"
#include "Vector.h"

#define SNDLIB_DECLSPEC __declspec(dllexport)

// Only begin attenuating sound volumes when outside the FULLVOLUME range.
#define SOUND_FULLVOLUME	80
#define ENT_ATTEN_MASK		(255 - ENT_VOL_MASK) //mxd

snd_import_t si; //mxd

// Internal sound data & structures.
channel_t channels[MAX_CHANNELS];

static qboolean sound_started = false; //mxd. int in Q2.

dma_t dma;

static vec3_t listener_origin;
static vec3_t listener_forward;
static vec3_t listener_right;
static vec3_t listener_up;

static int s_registration_sequence;
static qboolean s_registering;

static int soundtime; // Sample PAIRS.
int paintedtime; // Sample PAIRS.

static float snd_attenuations[] = // H2
{
	0.0f,		// ATTN_NONE
	0.0008f,	// ATTN_NORM
	0.002f,		// ATTN_IDLE
	0.006f		// ATTN_STATIC
};

// During registration it is possible to have more sounds than could actually be referenced during gameplay,
// because we don't want to free anything until we are sure we won't need it.
#define MAX_SFX	(MAX_SOUNDS * 2)
static sfx_t known_sfx[MAX_SFX];
static int num_sfx;

#define MAX_PLAYSOUNDS	128
static playsound_t s_playsounds[MAX_PLAYSOUNDS];
static playsound_t s_freeplays;
playsound_t s_pendingplays;

cvar_t* s_volume;
cvar_t* s_sounddir; // H2
cvar_t* s_testsound;
cvar_t* s_loadas8bit;
cvar_t* s_khz;
static cvar_t* s_show;
static cvar_t* s_mixahead;
cvar_t* s_primary;
static cvar_t* s_paused; //mxd

// H2: sound attenuation cvars.
static cvar_t* s_attn_norm;
static cvar_t* s_attn_idle;
static cvar_t* s_attn_static;

#pragma region ========================== Console commands ==========================

// Q2 counterpart.
static void S_Play_f(void) //mxd. Named 'S_Play' in original logic.
{
	for (int i = 1; i < si.Cmd_Argc(); i++)
	{
		char name[256];
		strcpy_s(name, sizeof(name), si.Cmd_Argv(i)); //mxd. strcpy -> strcpy_s.

		if (strrchr(si.Cmd_Argv(i), '.') == NULL)
			strcat_s(name, sizeof(name), ".wav"); //mxd. strcat -> strcat_s.

		sfx_t* sfx = S_RegisterSound(name);
		S_StartSound(NULL, si.cl->playernum + 1, 0, sfx, 1.0f, ATTN_NORM, 0.0f);
	}
}

// Q2 counterpart.
static void S_SoundList_f(void) //mxd. Named 'S_SoundList' in original logic.
{
	int total = 0;

	sfx_t* sfx = &known_sfx[0];
	for (int i = 0; i < num_sfx; i++, sfx++)
	{
		if (sfx->registration_sequence == 0)
			continue;

		const sfxcache_t* sc = sfx->cache;

		if (sc != NULL)
		{
			const int size = sc->length * sc->width * (sc->stereo + 1);
			total += size;

			si.Com_Printf(sc->loopstart >= 0 ? "L" : " ");
			si.Com_Printf("(%2db) %6i : %s\n", sc->width * 8, size, sfx->name);
		}
		else if (sfx->name[0] == '*')
		{
			si.Com_Printf("  placeholder : %s\n", sfx->name);
		}
		else
		{
			si.Com_Printf("  not loaded  : %s\n", sfx->name);
		}
	}

	si.Com_Printf("Total resident: %i bytes\n", total);
}

// Q2 counterpart.
static void S_SoundInfo_f(void)
{
	if (!sound_started)
	{
		si.Com_Printf("Sound system not started\n");
		return;
	}

	si.Com_Printf("sounddir: %s\n", s_sounddir->string); // H2
	si.Com_Printf("%5d stereo\n", dma.channels - 1);
	si.Com_Printf("%5d samples\n", dma.samples);
	si.Com_Printf("%5d samplepos\n", dma.samplepos);
	si.Com_Printf("%5d samplebits\n", dma.samplebits);
	si.Com_Printf("%5d submission_chunk\n", dma.submission_chunk);
	si.Com_Printf("%5d speed\n", dma.speed);
	si.Com_Printf("0x%x dma buffer\n", dma.buffer);
}

#pragma endregion

static void S_Init(void)
{
	si.Com_Printf("\n------- sound initialization -------\n");

	const cvar_t* cv = si.Cvar_Get("s_initsound", "1", 0);
	if (cv->value == 0.0f)
	{
		si.Com_Printf("not initializing.\n");
	}
	else
	{
		s_volume = si.Cvar_Get("s_volume", "0.5", CVAR_ARCHIVE);
		s_sounddir = si.Cvar_Get("s_sounddir", "sound", CVAR_ARCHIVE); // H2
		s_khz = si.Cvar_Get("s_khz", "44", CVAR_ARCHIVE);  // Q2: 11
		s_loadas8bit = si.Cvar_Get("s_loadas8bit", "1", CVAR_ARCHIVE);

		s_mixahead = si.Cvar_Get("s_mixahead", "0.1", CVAR_ARCHIVE); // Q2: 0.2
		s_show = si.Cvar_Get("s_show", "0", 0);
		s_testsound = si.Cvar_Get("s_testsound", "0", 0);
		s_primary = si.Cvar_Get("s_primary", "0", CVAR_ARCHIVE);

		// H2: extra attenuation cvars.
		s_attn_norm = si.Cvar_Get("s_attn_norm", "0.0008", 0);
		s_attn_idle = si.Cvar_Get("s_attn_idle", "0.002", 0);
		s_attn_static = si.Cvar_Get("s_attn_static", "0.006", 0);

		s_paused = si.Cvar_Get("paused", "0", 0); //mxd

		si.Cmd_AddCommand("play", S_Play_f);
		si.Cmd_AddCommand("stopsound", S_StopAllSounds);
		si.Cmd_AddCommand("soundlist", S_SoundList_f);
		si.Cmd_AddCommand("soundinfo", S_SoundInfo_f);

		if (SNDDMA_Init())
		{
			S_InitScaletable();

			sound_started = true;
			num_sfx = 0;
			soundtime = 0;
			paintedtime = 0;

			si.Com_Printf("sound sampling rate: %i\n", dma.speed);
			S_StopAllSounds();
		}
	}

	si.Com_Printf("------------------------------------\n");
}

// Q2 counterpart.
// Shutdown sound engine.
void S_Shutdown(void)
{
	if (!sound_started)
		return;

	SNDDMA_Shutdown();

	si.Cmd_RemoveCommand("play");
	si.Cmd_RemoveCommand("stopsound");
	si.Cmd_RemoveCommand("soundlist");
	si.Cmd_RemoveCommand("soundinfo");

	// Free all sounds.
	sfx_t* sfx = &known_sfx[0];
	for (int i = 0; i < num_sfx; i++, sfx++)
	{
		if (sfx->name[0] == 0)
			continue;

		if (sfx->cache != NULL)
			si.Z_Free(sfx->cache);

		memset(sfx, 0, sizeof(*sfx));
	}

	num_sfx = 0;
	sound_started = false;
}

// Q2 counterpart.
static sfx_t* S_FindName(const char* name, const qboolean create)
{
	if (name == NULL || name[0] == 0) //mxd. Merged checks.
	{
		si.Com_Error(ERR_FATAL, "S_FindName: empty name\n");
		return NULL;
	}

	if (strlen(name) >= MAX_QPATH)
	{
		si.Com_Error(ERR_FATAL, "Sound name too long: %s", name);
		return NULL;
	}

	// See if already loaded.
	for (int i = 0; i < num_sfx; i++)
		if (strcmp(known_sfx[i].name, name) == 0)
			return &known_sfx[i];

	if (!create)
		return NULL;

	// Find a free sfx.
	int free_slot = 0;
	for (; free_slot < num_sfx; free_slot++)
		if (known_sfx[free_slot].name[0] == 0)
			break;

	if (free_slot == num_sfx)
	{
		if (num_sfx == MAX_SFX)
		{
			si.Com_Error(ERR_FATAL, "S_FindName: out of sfx_t");
			return NULL;
		}

		num_sfx++;
	}

	sfx_t* sfx = &known_sfx[free_slot];
	memset(sfx, 0, sizeof(*sfx));
	strcpy_s(sfx->name, sizeof(sfx->name), name); //mxd. strcpy -> strcpy_s.
	sfx->registration_sequence = s_registration_sequence;

	return sfx;
}

// Q2 counterpart.
static void S_BeginRegistration(void)
{
	s_registration_sequence++;
	s_registering = true;
}

// Q2 counterpart.
static sfx_t* S_RegisterSound(const char* name)
{
	if (!sound_started)
		return NULL;

	sfx_t* sfx = S_FindName(name, true);
	sfx->registration_sequence = s_registration_sequence;

	if (!s_registering)
		S_LoadSound(sfx);

	return sfx;
}

// Q2 counterpart.
static void S_EndRegistration(void)
{
	// Free any sounds not from this registration sequence.
	sfx_t* sfx = &known_sfx[0];
	for (int i = 0; i < num_sfx; i++, sfx++)
	{
		// Don't need this sound?
		if (sfx->name[0] != 0 && sfx->registration_sequence != s_registration_sequence)
		{
			// It is possible to have a leftover from a server that didn't finish loading.
			if (sfx->cache != NULL)	
				si.Z_Free(sfx->cache);

			memset(sfx, 0, sizeof(*sfx));
		}
		//mxd. Skip Com_PageInMemory() logic.
	}

	// Load everything in.
	sfx = &known_sfx[0];
	for (int i = 0; i < num_sfx; i++, sfx++)
		if (sfx->name[0] != 0)
			S_LoadSound(sfx);

	s_registering = false;
}

// Q2 counterpart.
static channel_t* S_PickChannel(const int entnum, const int entchannel)
{
	if (entchannel < 0)
	{
		si.Com_Error(ERR_DROP, "S_PickChannel: entchannel<0");
		return NULL;
	}

	// Check for replacement sound, or find the best one to replace.
	int first_to_die = -1;
	int life_left = 0x7fffffff;

	for (int ch_idx = 0; ch_idx < MAX_CHANNELS; ch_idx++)
	{
		if (entchannel != 0 && channels[ch_idx].entnum == entnum && channels[ch_idx].entchannel == entchannel) // Channel 0 never overrides.
		{
			// Always override sound from same entity.
			first_to_die = ch_idx;
			break;
		}

		// Don't let monster sounds override player sounds.
		if (channels[ch_idx].entnum == si.cl->playernum + 1 && entnum != si.cl->playernum + 1 && channels[ch_idx].sfx != NULL)
			continue;

		if (channels[ch_idx].end - paintedtime < life_left)
		{
			life_left = channels[ch_idx].end - paintedtime;
			first_to_die = ch_idx;
		}
	}

	if (first_to_die == -1)
		return NULL;

	channel_t* ch = &channels[first_to_die];
	memset(ch, 0, sizeof(*ch));

	return ch;
}

// Q2 counterpart.
// Used for spatializing channels and autosounds.
static void S_SpatializeOrigin(const vec3_t origin, const float master_vol, const float dist_mult, int* left_vol, int* right_vol)
{
	if (si.cls->state != ca_active)
	{
		*left_vol = 255;
		*right_vol = 255;

		return;
	}

	// calculate stereo separation and distance attenuation.
	vec3_t source_vec;
	VectorSubtract(origin, listener_origin, source_vec);

	float dist = VectorNormalize(source_vec);
	dist = max(0.0f, dist - SOUND_FULLVOLUME); // Close enough to be at full volume.
	dist *= dist_mult; // Different attenuation levels.

	float lscale;
	float rscale;

	if (dma.channels == 1 || dist_mult == 0.0f)
	{
		// No attenuation = no spatialization.
		rscale = 1.0f;
		lscale = 1.0f;
	}
	else
	{
		const float dot = DotProduct(listener_right, source_vec);

		rscale = 0.5f * (1.0f + dot);
		lscale = 0.5f * (1.0f - dot);
	}

	// Add in distance effect.
	float scale = (1.0f - dist) * rscale;
	*right_vol = (int)(master_vol * scale);
	*right_vol = max(0, *right_vol);

	scale = (1.0f - dist) * lscale;
	*left_vol = (int)(master_vol * scale);
	*left_vol = max(0, *left_vol);
}

static void S_Spatialize(channel_t* ch)
{
	// Anything coming from the view entity will always be full volume.
	if (ch->entnum == si.cl->playernum + 1)
	{
		ch->leftvol = ch->master_vol;
		ch->rightvol = ch->master_vol;

		return;
	}

	if (ch->flags == CF_LEFT_ONLY) // H2.
	{
		ch->leftvol = 255;
		ch->rightvol = 0;

		return;
	}

	if (ch->flags == CF_RIGHT_ONLY) // H2.
	{
		ch->leftvol = 0;
		ch->rightvol = 255;

		return;
	}


	if (!ch->fixed_origin && ch->entnum >= 0 && ch->entnum < MAX_EDICTS) //mxd. Inline CL_GetEntitySoundOrigin().
		VectorCopy(si.cl_entities[ch->entnum].lerp_origin, ch->origin); // H2: update ch->origin instead of using separate var.

	S_SpatializeOrigin(ch->origin, (float)ch->master_vol, ch->dist_mult, &ch->leftvol, &ch->rightvol);
}

// Q2 counterpart.
static playsound_t* S_AllocPlaysound(void)
{
	playsound_t* ps = s_freeplays.next;

	if (ps == &s_freeplays)
		return NULL; // No free playsounds.

	// Unlink from freelist.
	ps->prev->next = ps->next;
	ps->next->prev = ps->prev;

	return ps;
}

// Q2 counterpart.
static void S_FreePlaysound(playsound_t* ps)
{
	// Unlink from channel.
	ps->prev->next = ps->next;
	ps->next->prev = ps->prev;

	// Add to free list.
	ps->next = s_freeplays.next;
	s_freeplays.next->prev = ps;
	ps->prev = &s_freeplays;
	s_freeplays.next = ps;
}

// Take the next playsound and begin it on the channel.
// This is never called directly by S_Play*, but only by the update loop.
void S_IssuePlaysound(playsound_t* ps)
{
	if ((int)s_show->value)
		si.Com_Printf("Issue %i\n", ps->begin);

	// Pick a channel to play on.
	channel_t* ch = S_PickChannel(ps->entnum, ps->entchannel);

	if (ch == NULL)
	{
		S_FreePlaysound(ps);
		return;
	}

	// Spatialize.
	ch->dist_mult = snd_attenuations[ps->attenuation_index]; // H2
	ch->master_vol = (int)ps->volume;
	ch->entnum = ps->entnum;
	ch->entchannel = ps->entchannel;
	ch->sfx = ps->sfx;
	ch->flags = ps->attenuation_index; // H2. Never used...
	VectorCopy(ps->origin, ch->origin);
	ch->fixed_origin = ps->fixed_origin;
	ch->pos = 0;

	S_Spatialize(ch);

	const sfxcache_t* sc = S_LoadSound(ch->sfx);
	ch->end = paintedtime + sc->length;

	// Free the playsound.
	S_FreePlaysound(ps);
}

static sfx_t* S_RegisterSexedSound(const entity_state_t* ent, char* base)
{
	char model[MAX_QPATH];

	// Determine what model the client is using.
	model[0] = 0;
	const int n = CS_PLAYERSKINS + ent->number - 1;

	if (si.cl->configstrings[n][0] != 0)
	{
		char* p = strchr(si.cl->configstrings[n], '\\');

		if (p != NULL)
		{
			strcpy_s(model, sizeof(model), p + 1); //mxd. strcpy -> strcpy_s.

			p = strchr(model, '/');
			if (p != NULL)
				*p = 0;
			else
				strcpy_s(model, sizeof(model), "male"); // H2 //mxd. strcpy -> strcpy_s.
		}
	}

	// If we can't figure it out, they're male.
	if (model[0] == 0)
		strcpy_s(model, sizeof(model), "male"); //mxd. strcpy -> strcpy_s.

	// See if we already know of the model specific sound.
	char sexed_filename[MAX_QPATH];
	Com_sprintf(sexed_filename, sizeof(sexed_filename), "#players/%s/%s", model, base + 1);
	struct sfx_s* sfx = S_FindName(sexed_filename, false);

	if (sfx == NULL)
	{
		// No, so see if it exists.
		FILE* f;
		si.FS_FOpenFile(&sexed_filename[1], &f);

		if (f != NULL)
		{
			// Yes, close the file and register it.
			si.FS_FCloseFile(f);
			sfx = S_RegisterSound(sexed_filename);
		}
		else
		{
			// No, revert to the sound in base/player/male folder.
			Com_sprintf(sexed_filename, sizeof(sexed_filename), "player/male/%s", base + 1);
			sfx = S_FindName(sexed_filename, true); // H2: S_AliasName() in Q2.
		}
	}

	return sfx;
}

// Q2 counterpart.
// Validates the params and queues the sound up.
// If origin is NULL, the sound will be dynamically sourced from the entity.
// Entchannel 0 will never override a playing sound.
static void S_StartSound(const vec3_t origin, const int entnum, const int entchannel, sfx_t* sfx, const float fvol, const int attenuation, const float timeofs)
{
	static int s_beginofs = 0; //mxd. Made local static.

	if (!sound_started || sfx == NULL || entnum < 0 || entnum >= MAX_EDICTS) //mxd. Add entnum sanity checks.
		return;

	if (sfx->name[0] == '*')
		sfx = S_RegisterSexedSound(&si.cl_entities[entnum].current, sfx->name);

	// Make sure the sound is loaded.
	if (S_LoadSound(sfx) == NULL)
		return;

	// Make the playsound_t.
	playsound_t* ps = S_AllocPlaysound();

	if (ps == NULL)
		return;

	if (origin != NULL)
	{
		VectorCopy(origin, ps->origin);
		ps->fixed_origin = true;
	}
	else
	{
		ps->fixed_origin = false;
	}

	ps->entnum = entnum;
	ps->entchannel = entchannel;
	ps->attenuation_index = attenuation;
	ps->volume = fvol * 255.0f;
	ps->sfx = sfx;

	// Drift s_beginofs.
	int start = (int)((float)si.cl->frame.servertime * 0.001f * (float)dma.speed) + s_beginofs;

	if (start < paintedtime)
	{
		start = paintedtime;
		s_beginofs = start - (int)((float)si.cl->frame.servertime * 0.001f * (float)dma.speed);
	}
	else if (start > (int)((float)paintedtime + 0.3f * (float)dma.speed))
	{
		start = (int)((float)paintedtime + 0.1f * (float)dma.speed);
		s_beginofs = start - (int)((float)si.cl->frame.servertime * 0.001f * (float)dma.speed);
	}
	else
	{
		s_beginofs -= 10;
	}

	if (timeofs == 0.0f)
		ps->begin = paintedtime;
	else
		ps->begin = (int)((float)start + timeofs * (float)dma.speed);

	// Sort into the pending sound list.
	playsound_t* sort = s_pendingplays.next;
	while (sort != &s_pendingplays && sort->begin < ps->begin)
		sort = sort->next;

	ps->next = sort;
	ps->prev = sort->prev;

	ps->next->prev = ps;
	ps->prev->next = ps;
}

// Q2 counterpart.
static void S_StartLocalSound(const char* sound)
{
	if (!sound_started)
		return;

	sfx_t* sfx = S_RegisterSound(sound);

	if (sfx != NULL)
		S_StartSound(NULL, si.cl->playernum + 1, 0, sfx, 1.0f, ATTN_NORM, 0.0f);
	else
		si.Com_Printf("S_StartLocalSound: can't cache %s\n", sound);
}

static void S_ClearBuffer(void)
{
	if (!sound_started)
		return;

	//H2: no s_rawend reset.
	SNDDMA_BeginPainting();

	if (dma.buffer != NULL)
	{
		const int clear = ((dma.samplebits == 8) ? 0x80 : 0);
		memset(dma.buffer, clear, dma.samples * dma.samplebits / 8);
	}

	SNDDMA_Submit();
}

static void S_StopAllSounds(void)
{
	if (!sound_started)
		return;

	// Clear all the playsounds.
	memset(s_playsounds, 0, sizeof(s_playsounds));

	s_freeplays.next = &s_freeplays;
	s_freeplays.prev = &s_freeplays;
	s_pendingplays.next = &s_pendingplays;
	s_pendingplays.prev = &s_pendingplays;

	for (int i = 0; i < MAX_PLAYSOUNDS; i++)
	{
		s_playsounds[i].prev = &s_freeplays;
		s_playsounds[i].next = s_freeplays.next;
		s_playsounds[i].prev->next = &s_playsounds[i];
		s_playsounds[i].next->prev = &s_playsounds[i];
	}

	// Clear all the channels.
	memset(channels, 0, sizeof(channels));
	// H2: no S_ClearBuffer() call.
}

static void S_StopAllSounds_Sounding(void) // H2
{
	if (sound_started)
		S_ClearBuffer();
}

// Entities with a sound field will generate looped sounds that are automatically started, stopped and merged together as the entities are sent to the client.
static void S_AddLoopSounds(void)
{
	int sounds[MAX_EDICTS];
	float attenuations[MAX_EDICTS]; //H2
	float volumes[MAX_EDICTS]; //H2
	
	if ((int)s_paused->value || si.cls->state != ca_active || !si.cl->sound_prepped)
		return;

	for (int i = 0; i < si.cl->frame.num_entities; i++)
	{
		const int num = (si.cl->frame.parse_entities + i) & (MAX_PARSE_ENTITIES - 1);
		const entity_state_t* ent = &si.cl_parse_entities[num];
		sounds[i] = ent->sound;
		attenuations[i] = snd_attenuations[ent->sound_data & ENT_ATTEN_MASK]; //H2
		volumes[i] = (float)(ent->sound_data & ENT_VOL_MASK); //H2
	}

	for (int i = 0; i < si.cl->frame.num_entities; i++)
	{
		if (sounds[i] == 0)
			continue;

		sfx_t* sfx = si.cl->sound_precache[sounds[i]];

		if (sfx == NULL || sfx->cache == NULL)
			continue; // Bad sound effect.

		int num = (si.cl->frame.parse_entities + i) & (MAX_PARSE_ENTITIES - 1);
		const entity_state_t* ent = &si.cl_parse_entities[num];

		// Find the total contribution of all sounds of this type.
		vec3_t origin;
		VectorAdd(ent->origin, ent->bmodel_origin, origin); // H2. Original logic does Vec3NotZero(bmodel_origin) check before adding, but who cares... --mxd.

		int left_total;
		int right_total;
		S_SpatializeOrigin(origin, volumes[i], attenuations[i], &left_total, &right_total);

		for (int j = i + 1; j < si.cl->frame.num_entities; j++)
		{
			if (sounds[j] != sounds[i])
				continue;

			sounds[j] = 0; // Don't check this again later.

			num = (si.cl->frame.parse_entities + j) & (MAX_PARSE_ENTITIES - 1);
			ent = &si.cl_parse_entities[num];

			int left;
			int right;
			S_SpatializeOrigin(ent->origin, volumes[j], attenuations[j], &left, &right);

			left_total += left;
			right_total += right;
		}

		if (left_total == 0 && right_total == 0)
			continue; // Not audible.

		// Allocate a channel.
		channel_t* ch = S_PickChannel(0, 0);

		if (ch == NULL)
			return;

		ch->leftvol = min(255, left_total);
		ch->rightvol = min(255, right_total);
		ch->autosound = true; // Remove next frame.
		ch->sfx = sfx;
		ch->pos = paintedtime % sfx->cache->length;
		ch->end = paintedtime + sfx->cache->length - ch->pos;
	}
}

// Q2 counterpart.
static void GetSoundtime(void)
{
	static int buffers;
	static int oldsamplepos;

	const int fullsamples = dma.samples / dma.channels;

	// It is possible to miscount buffers if it has wrapped twice between calls to S_Update. Oh well.
	const int samplepos = SNDDMA_GetDMAPos();

	if (samplepos < oldsamplepos)
	{
		buffers++; // Buffer wrapped.

		if (paintedtime > 0x40000000)
		{
			// Time to chop things off to avoid 32 bit limits.
			buffers = 0;
			paintedtime = fullsamples;

			S_StopAllSounds();
		}
	}

	oldsamplepos = samplepos;

	soundtime = buffers * fullsamples + samplepos / dma.channels;
}

// Q2 counterpart.
static void S_Update_(void) //TODO: rename to S_MixSound?
{
	if (!sound_started)
		return;

	SNDDMA_BeginPainting();

	if (dma.buffer == NULL)
		return;

	// Updates DMA time.
	GetSoundtime();

	// Check to make sure that we haven't overshot.
	if (paintedtime < soundtime)
	{
		si.Com_DPrintf("S_Update_ : overflow\n");
		paintedtime = soundtime;
	}

	// Mix ahead of current position.
	uint endtime = soundtime + (int)(s_mixahead->value * (float)dma.speed);

	// Mix to an even submission block size.
	endtime = (endtime + dma.submission_chunk - 1) & ~(dma.submission_chunk - 1);

	const uint samps = dma.samples >> (dma.channels - 1);

	S_PaintChannels(min(endtime, soundtime + samps));
	SNDDMA_Submit();
}

// Called once each time through the main loop.
static void S_Update(const vec3_t origin, const vec3_t forward, const vec3_t right, const vec3_t up)
{
	if (!sound_started)
		return;

	// If the loading plaque is up, clear everything out to make sure we aren't looping a dirty dma buffer while loading.
	if (si.cls->disable_screen)
	{
		S_ClearBuffer();
		return;
	}

	//H2:
	snd_attenuations[ATTN_NORM] = s_attn_norm->value;
	snd_attenuations[ATTN_IDLE] = s_attn_idle->value;
	snd_attenuations[ATTN_STATIC] = s_attn_static->value;

	//H2: missing S_InitScaletable() call.

	VectorCopy(origin, listener_origin);
	VectorCopy(forward, listener_forward);
	VectorCopy(right, listener_right);
	VectorCopy(up, listener_up);

	// Update spatialization for dynamic sounds.
	channel_t* ch = &channels[0];
	for (int i = 0; i < MAX_CHANNELS; i++, ch++)
	{
		if (ch->sfx == NULL)
			continue;

		if (ch->autosound)
			memset(ch, 0, sizeof(*ch)); // Autosounds are regenerated fresh each frame.
		else
			S_Spatialize(ch); // Respatialize channel.

		// H2: missing 'clear channel when it can't be heard' logic. //TODO: re-add? Missing because pre-3.21 Q2 source was used?
	}

	// Add looping sounds.
	S_AddLoopSounds();

	// Debugging output.
	if ((int)s_show->value)
	{
		int total_sounds = 0;

		ch = &channels[0];
		for (int i = 0; i < MAX_CHANNELS; i++, ch++)
		{
			if (ch->sfx != NULL && (ch->leftvol > 0 || ch->rightvol > 0))
			{
				si.Com_Printf("%3i %3i %s\n", ch->leftvol, ch->rightvol, ch->sfx->name);
				total_sounds++;
			}
		}

		si.Com_Printf("----(%i)---- painted: %i\n", total_sounds, paintedtime);
	}

	// Mix some sound.
	S_Update_();
}

//mxd
SNDLIB_DECLSPEC snd_export_t GetSoundAPI(const snd_import_t snd_import)
{
	snd_export_t snd_export;

	si = snd_import;

	snd_export.api_version = SND_API_VERSION;
	snd_export.library_name = "DirectSound";

	snd_export.Init = S_Init;
	snd_export.Shutdown = S_Shutdown;

	snd_export.StartSound = S_StartSound;
	snd_export.StartLocalSound = S_StartLocalSound;

	snd_export.StopAllSounds = S_StopAllSounds;
	snd_export.StopAllSounds_Sounding = S_StopAllSounds_Sounding;

	snd_export.Update = S_Update;
	snd_export.Activate = S_Activate;

	snd_export.BeginRegistration = S_BeginRegistration;
	snd_export.RegisterSound = S_RegisterSound;
	snd_export.EndRegistration = S_EndRegistration;

	snd_export.FindName = S_FindName;

	snd_export.SetEaxEnvironment = NULL;

	return snd_export;
}