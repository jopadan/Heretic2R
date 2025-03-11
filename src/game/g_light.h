//
// g_light.h
//
// Copyright 2025 mxd
//

#pragma once

#include "g_Edict.h"

extern void SP_light(edict_t* self);
extern void SP_env_fire(edict_t* self);
extern void SP_light_walltorch(edict_t* self);
extern void SP_light_floortorch(edict_t* self);
extern void SP_light_torch1(edict_t* self);
extern void SP_light_gem2(edict_t* self);
extern void SP_light_chandelier1(edict_t* self);
extern void SP_light_chandelier2(edict_t* self);
extern void SP_light_chandelier3(edict_t* self);
extern void SP_light_lantern1(edict_t* self);
extern void SP_light_lantern2(edict_t* self);
extern void SP_light_lantern3(edict_t* self);
extern void SP_light_lantern4(edict_t* self);
extern void SP_light_lantern5(edict_t* self);
extern void SP_light_buglight(edict_t* self);

extern void LightStaticsInit(void);
extern void SpawnFlame(edict_t* self, const vec3_t origin);

//mxd. Local forward declarations for g_light.c:
static void TorchUse(edict_t* self, edict_t* other, edict_t* activator);
static void TorchStart(edict_t* self);