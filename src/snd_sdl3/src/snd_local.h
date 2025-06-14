//
// snd_local.h -- Structures local to sound library.
//
// Copyright 2025 mxd
//

#pragma once

#include "client.h"

extern snd_import_t si;

#pragma region ========================== Sound structures ==========================

//mxd. Moved from snd_loc.h. Not used outside of sound library.
typedef struct sfxcache_s
{
	int length;
	int loopstart;
	int speed; // Not needed, because converted on load?
	int width;
	int stereo;
	byte data[1]; // Variable-sized.
} sfxcache_t;

//mxd. Moved from snd_loc.h.
typedef struct sfx_s
{
	char name[MAX_QPATH];
	int registration_sequence;
	sfxcache_t* cache;
	//char* truename; //mxd. Missing in H2.
} sfx_t;

#pragma endregion