//
// m_assassin_local.h -- Local forward declarations for m_assassin.c.
//
// Copyright 2025 mxd
//

#pragma once

#include "q_Typedef.h"

static void AssassinDaggerInit(edict_t* dagger);
static qboolean AssassinChooseTeleportDestination(edict_t* self, int type, qboolean imperative, qboolean instant);
static void AssassinInitDeCloak(edict_t* self);