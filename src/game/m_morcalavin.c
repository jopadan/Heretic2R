//
// m_morcalavin.c
//
// Copyright 1998 Raven Software
//

#include "m_morcalavin.h"
#include "m_morcalavin_shared.h"
#include "m_morcalavin_anim.h"
#include "g_DefaultMessageHandler.h"
#include "g_monster.h"
#include "g_playstats.h"
#include "mg_ai.h" //mxd
#include "mg_guide.h" //mxd
#include "m_stats.h"
#include "p_main.h"
#include "Random.h"
#include "Utilities.h"
#include "Vector.h"
#include "g_local.h"

#define MORCALAVIN_GRAVITY	0.3f //mxd. Named 'MORK_GRAV' in original logic.

static void MorcalavinProjectileInit(edict_t* self, edict_t* proj); //TODO: remove.
static void MorcalavinProjectile1Blocked(edict_t* self, trace_t* trace); //TODO: remove.
static void MorcalavinProjectile3Blocked(edict_t* self, trace_t* trace); //TODO: remove.
static void MorcalavinPhaseOutInit(edict_t* self); //TODO: remove.

#pragma region ========================== Morcalavin base info ==========================

static const animmove_t* animations[NUM_ANIMS] =
{
	&morcalavin_move_float,
	&morcalavin_move_hurtidle,
	&morcalavin_move_attack1,
	&morcalavin_move_attack2,
	&morcalavin_move_attack2b,
	&morcalavin_move_attack3,
	&morcalavin_move_def1,
	&morcalavin_move_def2,
	&morcalavin_move_walk,
	&morcalavin_move_fly,
	&morcalavin_move_getup,
	&morcalavin_move_retort,
	&morcalavin_move_fall,
	&morcalavin_move_glide,
	&morcalavin_move_ground_attack,
	&morcalavin_move_tracking_attack1,
	&morcalavin_move_attack4,
};

static int sounds[NUM_SOUNDS];

#pragma endregion

void morcalavin_end_retort(edict_t* self)
{
	SetAnim(self, ANIM_WALK);
}

static void MorcalavinLightning2Think(edict_t* self) //mxd. Named 'morcalavin_check_lightning2' in original logic.
{
	if (self->owner->enemy == NULL)
	{
		self->nextthink = level.time + FRAMETIME; //mxd. Use define.
		return;
	}

	vec3_t dir;
	VectorSubtract(self->s.origin, self->owner->enemy->s.origin, dir);
	const float dist = VectorNormalize(dir);

	if (dist < 72.0f)
	{
		T_Damage(self->owner->enemy, self, self->owner, dir, vec3_origin, vec3_origin, 2, 0, DAMAGE_SPELL, MOD_SHIELD);

		vec3_t vel;
		VectorNormalize2(self->velocity, vel);

		vec3_t lightning_start;
		VectorMA(self->s.origin, 600.0f * FRAMETIME, vel, lightning_start);

		vec3_t lightning_end;
		VectorCopy(self->owner->enemy->s.origin, lightning_end);
		lightning_end[2] += flrand(-16.0f, 32.0f); //mxd. irand() in original logic.

		gi.CreateEffect(NULL, FX_LIGHTNING, 0, lightning_start, "vbb", lightning_end, irand(2, 4), 0); //TODO: play SND_LGHTNGHIT sound?
	}

	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

void morcalavin_big_shot(edict_t* self)
{
	edict_t* proj = G_Spawn();

	VectorSet(proj->mins, -4.0f, -4.0f, -4.0f);
	VectorSet(proj->maxs,  4.0f,  4.0f,  4.0f);

	proj->solid = SOLID_BBOX;
	proj->movetype = PHYSICSTYPE_FLY;
	proj->gravity = 0.0f;
	proj->clipmask = MASK_SHOT;

	proj->owner = self;
	self->target_ent = proj;

	proj->isBlocked = MorcalavinProjectile3Blocked;
	proj->isBlocking = MorcalavinProjectile3Blocked;
	proj->bounced = MorcalavinProjectile3Blocked;

	proj->think = MorcalavinLightning2Think;
	proj->nextthink = level.time + FRAMETIME; //mxd. Use define.

	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);
	VectorMA(self->s.origin, 24.0f, forward, proj->s.origin);
	VectorMA(proj->s.origin, 12.0f, right, proj->s.origin);
	proj->s.origin[2] += 32.0f;

	vec3_t dir;
	VectorSubtract(self->enemy->s.origin, proj->s.origin, dir);
	VectorNormalize(dir);
	VectorScale(dir, 600.0f, proj->velocity);

	proj->s.effects = (EF_MARCUS_FLAG1 | EF_CAMERA_NO_CLIP);

	gi.linkentity(proj);
	gi.CreateEffect(&proj->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, proj->s.origin, "bv", FX_MORK_MISSILE, proj->s.origin);
	gi.sound(self, CHAN_AUTO, sounds[SND_PPCHARGE], 1.0f, ATTN_NORM, 0.0f);
}

static void MorcalavinTrackingProjectileThink(edict_t* self) //mxd. Named 'morcalavin_proj_track' in original logic.
{
	// No enemy or enemy is dead, stop tracking.
	if (self->enemy == NULL || self->enemy->health <= 0)
	{
		self->think = NULL;
		return;
	}

	// Timed out?
	if (self->monsterinfo.attack_finished < level.time)
	{
		gi.CreateEffect(&self->s, FX_HP_MISSILE, CEF_OWNERS_ORIGIN, self->s.origin, "vb", vec3_origin, HPMISSILE1_EXPLODE); //TODO: play SND_HOMEHIT sound?

		self->think = G_FreeEdict;
		self->nextthink = level.time + FRAMETIME; //mxd. Use define.

		return;
	}

	vec3_t old_dir;
	VectorCopy(self->velocity, old_dir);
	VectorNormalize(old_dir);

	vec3_t hunt_dir;
	VectorSubtract(self->enemy->s.origin, self->s.origin, hunt_dir);
	VectorNormalize(hunt_dir);

	if (self->delay + 0.05f < 4.0f)
		self->delay += 0.05f;

	if (self->ideal_yaw + 10.0f < 1000.0f)
		self->ideal_yaw += 10.0f;

	const float old_vel_mult = self->delay;
	float new_vel_div = 1.0f / (old_vel_mult + 1.0f);

	VectorScale(old_dir, old_vel_mult, old_dir);

	vec3_t new_dir;
	VectorAdd(old_dir, hunt_dir, new_dir);
	VectorScale(new_dir, new_vel_div, new_dir);

	float speed_mod = DotProduct(old_dir, new_dir);
	speed_mod = max(0.05f, speed_mod);

	new_vel_div *= self->ideal_yaw * speed_mod;

	VectorScale(old_dir, old_vel_mult, old_dir);
	VectorAdd(old_dir, hunt_dir, new_dir);
	VectorScale(new_dir, new_vel_div, new_dir);

	VectorCopy(new_dir, self->velocity);
	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

void morcalavin_tracking_projectile(edict_t* self, float pitch, float yaw, float roll)
{
	edict_t* proj = G_Spawn();

	proj->monsterinfo.attack_finished = level.time + 10.0f;
	proj->svflags |= SVF_ALWAYS_SEND;
	proj->movetype = PHYSICSTYPE_FLY;
	proj->gravity = 0.0f;
	proj->solid = SOLID_BBOX;
	proj->classname = "Morcalavin_Tracking_Missile";
	proj->dmg = 1;
	proj->clipmask = MASK_SHOT;
	proj->delay = 1.0f;
	proj->s.scale = 1.0f;
	proj->s.effects = (EF_MARCUS_FLAG1 | EF_CAMERA_NO_CLIP);

	proj->owner = self;
	proj->enemy = self->enemy;

	VectorSet(proj->mins, -2.0f, -2.0f, -2.0f);
	VectorSet(proj->maxs, 2.0f, 2.0f, 2.0f);
	VectorCopy(self->s.origin, proj->s.origin);

	// Determine the starting velocity of the ball.
	vec3_t angles;
	VectorCopy(self->s.angles, angles);

	angles[PITCH] += pitch;
	angles[YAW] += yaw;
	angles[ROLL] += roll;

	vec3_t forward;
	vec3_t right;
	vec3_t up;
	AngleVectors(angles, forward, right, up);

	VectorMA(self->s.origin, 24.0f, forward, proj->s.origin);
	VectorMA(proj->s.origin, -16.0f, right, proj->s.origin);
	VectorMA(proj->s.origin, 52.0f, up, proj->s.origin);

	VectorScale(up, 16.0f, proj->velocity);
	proj->ideal_yaw = 300.0f;

	proj->bounced = MorcalavinProjectile1Blocked;
	proj->isBlocking = MorcalavinProjectile1Blocked;
	proj->isBlocked = MorcalavinProjectile1Blocked;

	proj->think = MorcalavinTrackingProjectileThink;

	if (SKILL < SKILL_MEDIUM)
		proj->nextthink = level.time + 2.0f;
	else
		proj->nextthink = level.time + flrand(1.5f, 3.0f);

	gi.sound(self, CHAN_AUTO, sounds[SND_HOMING], 1.0f, ATTN_NORM, 0.0f);
	gi.CreateEffect(&proj->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, NULL, "bv", FX_MORK_TRACKING_MISSILE, proj->velocity);
}

static void MorcalavinProjectile2Blocked(edict_t* self, trace_t* trace) //mxd. Named 'morcalavin_proj2_blocked' in original logic.
{
	if (trace->ent == self->owner)
		return;

	// Reflection stuff.
	if (EntReflecting(trace->ent, true, true))
	{
		edict_t* proj = G_Spawn();

		MorcalavinProjectileInit(self, proj);
		proj->owner = self->owner;
		proj->ideal_yaw = self->ideal_yaw;

		Create_rand_relect_vect(self->velocity, proj->velocity);
		Vec3ScaleAssign(proj->ideal_yaw, proj->velocity);
		vectoangles(proj->velocity, proj->s.angles);

		gi.CreateEffect(&self->s, FX_HP_MISSILE, CEF_OWNERS_ORIGIN, self->s.origin, "vb", vec3_origin, HPMISSILE1_EXPLODE); //TODO: play SND_HOMEHIT sound?
		gi.linkentity(proj);

		G_SetToFree(self);

		return;
	}

	if (trace->ent->takedamage != DAMAGE_NO)
	{
		vec3_t hit_dir;
		VectorCopy(self->velocity, hit_dir);
		VectorNormalize(hit_dir);

		T_Damage(trace->ent, self, self->owner, hit_dir, self->s.origin, trace->plane.normal, 40, 0, DAMAGE_SPELL | DAMAGE_NO_KNOCKBACK, MOD_DIED);
	}

	gi.CreateEffect(NULL, FX_M_EFFECTS, 0, self->s.origin, "bv", FX_MORK_MISSILE_HIT, trace->plane.normal); //TODO: play SND_HOMEHIT sound?

	self->think = G_FreeEdict;
	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

static void MorcalavinProjectile3Blocked(edict_t* self, trace_t* trace) //mxd. Named 'morcalavin_proj3_blocked' in original logic.
{
	if (trace->ent == self->owner)
		return;

	// Reflection stuff.
	if (EntReflecting(trace->ent, true, true))
	{
		edict_t* proj = G_Spawn();

		MorcalavinProjectileInit(self, proj);
		proj->owner = self->owner;
		proj->ideal_yaw = self->ideal_yaw;

		Create_rand_relect_vect(self->velocity, proj->velocity);
		Vec3ScaleAssign(proj->ideal_yaw, proj->velocity);
		vectoangles(proj->velocity, proj->s.angles);

		gi.CreateEffect(&self->s, FX_HP_MISSILE, CEF_OWNERS_ORIGIN, self->s.origin, "vb", vec3_origin, HPMISSILE1_EXPLODE); //TODO: play SND_HOMEHIT sound?
		gi.linkentity(proj);

		G_SetToFree(self);

		return;
	}

	if (trace->ent->takedamage != DAMAGE_NO)
	{
		vec3_t hit_dir;
		VectorCopy(self->velocity, hit_dir);
		VectorNormalize(hit_dir);

		T_Damage(trace->ent, self, self->owner, hit_dir, self->s.origin, trace->plane.normal, irand(3, 5), 0, DAMAGE_SPELL | DAMAGE_NO_KNOCKBACK, MOD_DIED); //TODO: 'damage' arg value is the only difference between this and MorcalavinProjectile2Blocked().
	}

	gi.CreateEffect(NULL, FX_M_EFFECTS, 0, self->s.origin, "bv", FX_MORK_MISSILE_HIT, trace->plane.normal); //TODO: play SND_HOMEHIT sound?

	self->think = G_FreeEdict;
	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

static void MorcalavinLightningThink(edict_t* self) //mxd. Named 'morcalavin_check_lightning' in original logic.
{
	if (self->owner == NULL || self->owner->enemy == NULL)
	{
		self->nextthink = level.time + FRAMETIME; //mxd. Use define.
		return;
	}

	vec3_t dir;
	VectorSubtract(self->s.origin, self->owner->enemy->s.origin, dir);
	const float dist = VectorNormalize(dir);

	if (dist < 150.0f)
	{
		T_Damage(self->owner->enemy, self, self->owner, dir, vec3_origin, vec3_origin, irand(2, 4), 0, DAMAGE_SPELL, MOD_SHIELD);

		vec3_t vel;
		VectorNormalize2(self->velocity, vel);

		vec3_t lightning_start;
		VectorMA(self->s.origin, 400.0f * FRAMETIME, vel, lightning_start);

		vec3_t lightning_end;
		VectorCopy(self->owner->enemy->s.origin, lightning_end);
		lightning_end[2] += flrand(-16.0f, 32.0f); //mxd. irand() in original logic.

		gi.CreateEffect(NULL, FX_LIGHTNING, 0, lightning_start, "vbb", lightning_end, irand(2, 4), 0); //TODO: play SND_LGHTNGHIT sound?

		if (SKILL < SKILL_MEDIUM)
		{
			VectorNormalize(self->velocity);
			Vec3AddAssign(vel, self->velocity);
			Vec3ScaleAssign(400.0f, self->velocity);
		}
	}

	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

static void MorcalavinMissileThink(edict_t* self) //mxd. Named 'morcalavin_missile_update' in original logic.
{
	vec3_t forward;
	vec3_t right;
	AngleVectors(self->owner->s.angles, forward, right, NULL);

	switch (self->owner->s.frame)
	{
		case FRAME_atakc3:
			VectorMA(self->owner->s.origin, 16.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 10.0f, right, self->s.origin);
			self->s.origin[2] += 24.0f;
			break;

		case FRAME_atakc4:
			VectorMA(self->owner->s.origin, 16.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 12.0f, right, self->s.origin);
			self->s.origin[2] += 26.0f;
			break;

		case FRAME_atakc5:
			VectorMA(self->owner->s.origin, 16.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 28.0f;
			break;

		case FRAME_atakc6:
			VectorMA(self->owner->s.origin, 15.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 32.0f;
			break;

		case FRAME_atakc7:
			VectorMA(self->owner->s.origin, 15.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 36.0f;
			break;

		case FRAME_atakc8:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 37.0f;
			break;

		case FRAME_atakc9:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 38.0f;
			break;

		case FRAME_atakc10:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 39.0f;
			break;

		case FRAME_atakc11:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 40.0f;
			break;

		case FRAME_atakc12:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 41.0f;
			break;

		case FRAME_atakc13:
		case FRAME_atakc14:
			VectorMA(self->owner->s.origin, 14.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 42.0f;
			break;

		case FRAME_atakc15:
		{
			VectorMA(self->owner->s.origin, 16.0f, forward, self->s.origin);
			VectorMA(self->s.origin, 14.0f, right, self->s.origin);
			self->s.origin[2] += 42.0f;

			const vec3_t end_pos =
			{
				self->owner->enemy->s.origin[0],
				self->owner->enemy->s.origin[1],
				self->owner->enemy->s.origin[2] + (float)self->owner->enemy->viewheight
			};

			vec3_t diff;
			VectorSubtract(end_pos, self->s.origin, diff);
			VectorNormalize(diff);
			VectorScale(diff, 400.0f, self->velocity);

			self->think = MorcalavinLightningThink; //TODO: play SND_LIGHTNING sound?
			self->nextthink = level.time + FRAMETIME; //mxd. Use define.
		} return;
	}

	gi.linkentity(self);

	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

void morcalavin_start_missile(edict_t* self)
{
	edict_t* proj = G_Spawn();

	proj->solid = SOLID_BBOX;
	proj->movetype = PHYSICSTYPE_FLY;
	proj->gravity = 0.0f;
	proj->clipmask = MASK_SHOT;
	proj->s.effects = (EF_MARCUS_FLAG1 | EF_CAMERA_NO_CLIP);

	proj->think = MorcalavinMissileThink;
	proj->nextthink = level.time + FRAMETIME; //mxd. Use define.

	proj->isBlocked = MorcalavinProjectile2Blocked;
	proj->isBlocking = MorcalavinProjectile2Blocked;
	proj->bounced = MorcalavinProjectile2Blocked;

	proj->owner = self;
	self->target_ent = proj;

	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);
	VectorMA(self->s.origin, 16.0f, forward, proj->s.origin);
	VectorMA(proj->s.origin, 10.0f, right, proj->s.origin);
	proj->s.origin[2] += 24.0f;

	VectorSet(proj->mins, -4.0f, -4.0f, -4.0f);
	VectorSet(proj->maxs,  4.0f,  4.0f,  4.0f);

	gi.linkentity(proj);

	// Create the effect.
	gi.CreateEffect(&proj->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, proj->s.origin, "bv", FX_MORK_MISSILE, proj->s.origin);
	gi.sound(self, CHAN_AUTO, sounds[SND_PPCHARGE], 1.0f, ATTN_NORM, 0.0f);
}

void morcalavin_release_missile(edict_t* self)
{
	gi.sound(self, CHAN_AUTO, sounds[SND_PPFIRE], 1.0f, ATTN_NORM, 0.0f);
}

static void MorcalavinProjectile1Blocked(edict_t* self, trace_t* trace) //mxd. Named 'morcalavin_proj1_blocked' in original logic.
{
	if (trace->ent == self->owner || Q_stricmp(trace->ent->classname, "Morcalavin_Missile") == 0) //mxd. stricmp -> Q_stricmp
		return;

	// Reflection stuff.
	if (EntReflecting(trace->ent, true, true))
	{
		edict_t* proj = G_Spawn();

		MorcalavinProjectileInit(self, proj);
		proj->owner = self->owner;
		proj->ideal_yaw = self->ideal_yaw;

		Create_rand_relect_vect(self->velocity, proj->velocity);
		Vec3ScaleAssign(proj->ideal_yaw, proj->velocity);
		vectoangles(proj->velocity, proj->s.angles);

		gi.CreateEffect(&self->s, FX_HP_MISSILE, CEF_OWNERS_ORIGIN, self->s.origin, "vb", vec3_origin, HPMISSILE1_EXPLODE); //TODO: play SND_HOMEHIT sound?

		gi.linkentity(proj);
		G_SetToFree(self);

		return;
	}

	// Do the rest of the stuff.
	if (trace->ent->takedamage != DAMAGE_NO)
	{
		vec3_t hit_dir;
		VectorNormalize2(self->velocity, hit_dir);

		T_Damage(trace->ent, self, self->owner, hit_dir, self->s.origin, trace->plane.normal, irand(4, 8), 0, DAMAGE_SPELL | DAMAGE_NO_KNOCKBACK, MOD_DIED);
	}

	gi.CreateEffect(&self->s, FX_HP_MISSILE, CEF_OWNERS_ORIGIN, self->s.origin, "vb", vec3_origin, HPMISSILE1_EXPLODE); //TODO: play SND_HOMEHIT sound?

	self->think = G_FreeEdict;
	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

// Create the guts of morcalavin's projectile.
static void MorcalavinProjectileInit(edict_t* self, edict_t* proj) //mxd. Named 'create_morcalavin_proj' in original logic.
{
	proj->svflags |= SVF_ALWAYS_SEND;
	proj->movetype = PHYSICSTYPE_FLY;
	proj->gravity = 0.0f;
	proj->solid = SOLID_BBOX;
	proj->classname = "Morcalavin_Missile";
	proj->dmg = 1;
	proj->s.scale = 1.0f;
	proj->clipmask = MASK_SHOT;
	proj->nextthink = level.time + FRAMETIME; //mxd. Use define.

	proj->isBlocked = MorcalavinProjectile1Blocked;
	proj->isBlocking = MorcalavinProjectile1Blocked;
	proj->bounced = MorcalavinProjectile1Blocked;

	proj->s.effects = (EF_MARCUS_FLAG1 | EF_CAMERA_NO_CLIP);
	proj->enemy = self->enemy;

	VectorSet(proj->mins, -2.0f, -2.0f, -2.0f);
	VectorSet(proj->maxs,  2.0f,  2.0f,  2.0f);
	VectorCopy(self->s.origin, proj->s.origin);
}

void morcalavin_taunt_shot(edict_t* self)
{
	if (self->enemy == NULL)
		return;

	// Only predict once for all the missiles.
	vec3_t pred_pos;
	M_PredictTargetPosition(self->enemy, self->enemy->velocity, 5.0f, pred_pos);

	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);

	vec3_t start_pos;
	VectorMA(self->s.origin, -8.0f, forward, start_pos);
	VectorMA(start_pos, -16.0f, right, start_pos);
	start_pos[2] += 8.0f;

	vec3_t diff;
	VectorSubtract(pred_pos, start_pos, diff);
	VectorNormalize(diff);

	vec3_t angles;
	vectoangles(diff, angles);

	angles[PITCH] = -angles[PITCH] + flrand(-4.0f, 4.0f);
	angles[YAW] += flrand(-4.0f, 4.0f);

	// Spawn the projectile.
	edict_t* proj = G_Spawn();

	MorcalavinProjectileInit(self, proj);
	proj->owner = self;

	VectorCopy(start_pos, proj->s.origin);

	vec3_t vel;
	AngleVectors(angles, vel, NULL, NULL);

	VectorScale(vel, flrand(700.0f, 800.0f) + (skill->value * 100.0f), proj->velocity); //mxd. irand() in original logic.
	vectoangles(proj->velocity, proj->s.angles);

	gi.sound(self, CHAN_AUTO, sounds[SND_HOMING], 1.0f, ATTN_NORM, 0.0f);
	gi.CreateEffect(&proj->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, NULL, "bv", FX_MORK_TRACKING_MISSILE, proj->s.origin);
	gi.linkentity(proj);
}

static void MorcalavinPhaseOutPreThink(edict_t* self) //mxd. Named 'morcalavin_phase_out' in original logic.
{
#define PHASE_OUT_INTERVAL	40 //mxd

	if (self->s.color.a > PHASE_OUT_INTERVAL)
	{
		self->s.color.a -= irand(PHASE_OUT_INTERVAL / 2, PHASE_OUT_INTERVAL);
		self->next_pre_think = level.time + 0.05f;
	}
	else
	{
		self->s.color.a = 0;
		self->pre_think = NULL;
		self->next_pre_think = -1.0f;
	}
}

static void MorcalavinPhaseInPreThink(edict_t* self) //mxd. Named 'morcalavin_phase_in' in original logic.
{
#define PHASE_IN_INTERVAL	12 //mxd

	if (self->s.color.a < 255 - PHASE_IN_INTERVAL)
	{
		self->s.color.a += irand(PHASE_IN_INTERVAL / 2, PHASE_IN_INTERVAL);
		self->next_pre_think = level.time + 0.05f;
	}
	else
	{
		self->svflags &= ~SVF_NO_AUTOTARGET;
		self->s.color.c = 0xffffffff;

		if (self->health <= 0 || self->monsterinfo.lefty >= 6)
		{
			self->pre_think = NULL;
			self->next_pre_think = -1.0f;
		}
		else
		{
			MorcalavinPhaseOutInit(self);
		}
	}
}

static void MorcalavinPhaseOutInit(edict_t* self) //mxd. Named 'morcalavin_init_phase_out' in original logic.
{
	// Become tangible once more.
	self->solid = SOLID_NOT;
	self->svflags |= SVF_NO_AUTOTARGET;

	self->pre_think = MorcalavinPhaseOutPreThink;
	self->next_pre_think = level.time + FRAMETIME;
}

static void MorcalavinPhaseInInit(edict_t* self) //mxd. Named 'morcalavin_init_phase_in' in original logic.
{
	self->pre_think = MorcalavinPhaseInPreThink;
	self->next_pre_think = level.time + FRAMETIME;
}

static void MorcalavinLaughPostThink(edict_t* self) //mxd. Named 'mork_laugh' in original logic.
{
	gi.sound(self, CHAN_VOICE, sounds[SND_LAUGH], 1.0f, ATTN_NONE, 0.0f);

	self->post_think = NULL;
	self->next_post_think = -1.0f;
}

static void MorcalavinCheckKilledEnemy(edict_t* attacker) //mxd. Named 'mork_check_killed_enemy' in original logic.
{
	if (attacker->enemy != NULL && attacker->enemy->health <= 0)
	{
		attacker->post_think = MorcalavinLaughPostThink;
		attacker->next_post_think = level.time + flrand(1.3f, 2.3f);
	}
}

//TODO: used only by m_tcheckrik_spells.c. Move there? Merge into MorcalavinProjectileHomeIn()?
static void MorcalavinProjectileVeer(edict_t* self, const float amount) //mxd. Named 'projectile_veer' in original logic.
{
	// Useful code for making projectiles wander randomly to a specified degree.
	const float speed = VectorLength(self->velocity);
	const vec3_t veer_dir = { flrand(-amount, amount), flrand(-amount, amount), flrand(-amount, amount) };

	Vec3AddAssign(veer_dir, self->velocity);
	VectorNormalize(self->velocity);

	Vec3ScaleAssign(speed, self->velocity);
}

//TODO: used only by m_tcheckrik_spells.c. Move there?
void MorcalavinProjectileHomeIn(edict_t* self) //mxd. Named 'projectile_homethink' in original logic.
{
	vec3_t old_dir;
	VectorNormalize2(self->velocity, old_dir);

	vec3_t hunt_dir;
	VectorSubtract(self->enemy->s.origin, self->s.origin, hunt_dir);
	VectorNormalize(hunt_dir);

	const float old_vel_mult = ((self->delay != 0.0f) ? self->delay : 1.3f); //TODO: add projectile_turn_speed custom name?
	Vec3ScaleAssign(old_vel_mult, old_dir);

	vec3_t new_dir;
	VectorAdd(old_dir, hunt_dir, new_dir);

	float new_vel_div = 1.0f / (old_vel_mult + 1.0f);
	Vec3ScaleAssign(new_vel_div, new_dir);

	float speed_mod = DotProduct(old_dir, new_dir);
	speed_mod = max(0.05f, speed_mod);

	new_vel_div *= self->ideal_yaw * speed_mod;

	Vec3ScaleAssign(old_vel_mult, old_dir);
	VectorAdd(old_dir, hunt_dir, new_dir);

	VectorScale(new_dir, new_vel_div, self->velocity);

	if (self->random != 0.0f) //TODO: add add projectile_veer_amount custom name?
		MorcalavinProjectileVeer(self, self->random);
}

static void MorcalavinBeamIsBlocked(edict_t* self, trace_t* trace) //mxd. Named 'beam_blocked' in original logic.
{
	if (trace->ent->takedamage != DAMAGE_NO)
	{
		const int damage = irand(MORK_DMG_PROJ1_MIN, MORK_DMG_PROJ1_MAX) + max(0, self->dmg); //mxd. flrand() in original logic.

		vec3_t hit_dir;
		VectorNormalize2(self->velocity, hit_dir);

		T_Damage(trace->ent, self, self->owner, hit_dir, self->s.origin, trace->plane.normal, damage, 0, DAMAGE_SPELL | DAMAGE_NO_KNOCKBACK, MOD_DIED);
	}

	gi.sound(self, CHAN_WEAPON, gi.soundindex("monsters/seraph/guard/spellhit.wav"), 1.0f, ATTN_NORM, 0.0f); //TODO: precache in sounds[SND_BEAMHIT]?
	gi.CreateEffect(&self->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, self->s.origin, "bv", FX_M_MISC_EXPLODE, vec3_origin);

	G_SetToFree(self);
}

static void MorcalavinBeamThink(edict_t* self) //mxd. Named 'beam_think' in original logic. //TODO: doesn't do anything useful. Can be removed?
{
	self->think = NULL;
	self->nextthink = -1.0f;
}

void morcalavin_beam(edict_t* self)
{
	// Spawn the projectile.
	edict_t* beam = G_Spawn();

	MorcalavinProjectileInit(self, beam);

	beam->reflect_debounce_time = MAX_REFLECT;
	beam->classname = "M_Beam";
	beam->owner = self;

	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);
	VectorMA(beam->s.origin, 48.0f, forward, beam->s.origin);
	VectorMA(beam->s.origin, -20.0f, right, beam->s.origin);
	beam->s.origin[2] += 16.0f;

	const vec3_t end_pos = //mxd. The only difference between this and morcalavin_beam2()...
	{
		self->enemy->s.origin[0],
		self->enemy->s.origin[1],
		self->enemy->s.origin[2] + (float)self->enemy->viewheight
	};

	vec3_t dir;
	VectorSubtract(end_pos, beam->s.origin, dir);
	VectorNormalize(dir);

	VectorScale(dir, 1250.0f, beam->velocity);
	vectoangles(beam->velocity, beam->s.angles);

	beam->dmg = irand(MORK_DMG_BEAM_MIN, MORK_DMG_BEAM_MAX); //mxd. flrand() in original logic.

	beam->isBlocked = MorcalavinBeamIsBlocked;
	beam->think = MorcalavinBeamThink;
	beam->nextthink = level.time + 1.0f;

	gi.sound(self, CHAN_WEAPON, gi.soundindex("monsters/seraph/guard/spell.wav"), 1.0f, ATTN_NORM, 0.0f); //TODO: precache in sounds[SND_BEAM]?
	gi.CreateEffect(&beam->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, vec3_origin, "bv", FX_M_BEAM, beam->s.angles); //FIXME: Spawn a muzzle flash.
	gi.linkentity(beam);
}

void morcalavin_beam2(edict_t* self)
{
	// Spawn the projectile.
	edict_t* beam = G_Spawn();

	MorcalavinProjectileInit(self, beam);

	beam->reflect_debounce_time = MAX_REFLECT;
	beam->classname = "M_Beam";
	beam->owner = self;

	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);
	VectorMA(beam->s.origin, 48.0f, forward, beam->s.origin);
	VectorMA(beam->s.origin, -20.0f, right, beam->s.origin);
	beam->s.origin[2] += 16.0f;

	vec3_t dir;
	VectorSubtract(self->enemy->s.origin, beam->s.origin, dir);
	VectorNormalize(dir);

	VectorScale(dir, 1250.0f, beam->velocity);
	vectoangles(beam->velocity, beam->s.angles);

	beam->dmg = irand(MORK_DMG_BEAM_MIN, MORK_DMG_BEAM_MAX); //mxd. flrand() in original logic.

	beam->isBlocked = MorcalavinBeamIsBlocked;
	beam->think = MorcalavinBeamThink;
	beam->nextthink = level.time + 1.0f;

	gi.sound(self, CHAN_WEAPON, gi.soundindex("monsters/seraph/guard/spell.wav"), 1.0f, ATTN_NORM, 0.0f); //TODO: precache in sounds[SND_BEAM]?
	gi.CreateEffect(&beam->s, FX_M_EFFECTS, CEF_OWNERS_ORIGIN, vec3_origin, "bv", FX_M_BEAM, beam->s.angles); //FIXME: Spawn a muzzle flash.
	gi.linkentity(beam);
}

void morcalavin_quake_pause(edict_t* self)
{
	if (self->monsterinfo.flee_finished)
	{
		self->monsterinfo.flee_finished = false;
		SetAnim(self, ANIM_GROUND_ATTACK);
	}
}

void morcalavin_quake(edict_t* self, float pitch_ofs, float yaw_ofs, float roll_ofs)
{
	// Create the effect.
	vec3_t forward;
	vec3_t right;
	AngleVectors(self->s.angles, forward, right, NULL);

	vec3_t org;
	VectorMA(self->s.origin, 44.0f, forward, org);
	VectorMA(org, -14.0f, right, org);
	org[2] += self->mins[2];

	gi.CreateEffect(NULL, FX_M_EFFECTS, 0, self->s.origin, "bv", FX_QUAKE_RING, org);

	// Check to see if the player is on the ground, and if he is, then knock him down.
	if (self->enemy != NULL && self->enemy->groundentity != NULL && self->enemy->client != NULL)
	{
		// Knock the player down.
		P_KnockDownPlayer(&self->enemy->client->playerinfo);

		// Denote we've done so to follow it with an attack.
		self->monsterinfo.flee_finished = true; //TODO: add qboolean morcalavin_quake_finished name.
	}
}

void morcalavin_rush_sound(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sounds[SND_RUSH], 1.0f, ATTN_NORM, 0.0f);

	vec3_t forward;
	AngleVectors(self->s.angles, forward, NULL, NULL);
	VectorScale(forward, 250.0f, self->velocity);
	self->velocity[2] = 150.0f;
}

void morcalavin_pause(edict_t* self)
{
	if (self->monsterinfo.lefty < 6 && self->health > 0)
	{
		SetAnim(self, ANIM_FLOAT);
		return;
	}

	self->takedamage = DAMAGE_YES;
	self->mood_think(self);

	switch (self->ai_mood)
	{
		case AI_MOOD_ATTACK:
		{
			const int msg_id = ((self->ai_mood_flags & AI_MOOD_FLAG_MISSILE) ? MSG_MISSILE : MSG_MELEE); //mxd
			QPostMessage(self, msg_id, PRI_DIRECTIVE, NULL);
		} break;

		case AI_MOOD_PURSUE:
		case AI_MOOD_NAVIGATE:
			QPostMessage(self, MSG_RUN, PRI_DIRECTIVE, NULL);
			break;

		case AI_MOOD_DELAY:
		case AI_MOOD_STAND:
			QPostMessage(self, MSG_STAND, PRI_DIRECTIVE, NULL);
			break;

		case AI_MOOD_WANDER:
			SetAnim(self, ANIM_WALK);
			break;

		default:
			break;
	}
}

static void MorcalavinDeathMsgHandler(edict_t* self, G_Message_t* msg) //mxd. Named 'morcalavin_death' in original logic.
{
	self->monsterinfo.stepState++; //TODO: used only by m_morcalavin.c rename to morcalavin_state?
}

void morcalavin_retort(edict_t* self)
{
	self->msgHandler = DefaultMsgHandler;
	SetAnim(self, ANIM_RETORT);
}

void morcalavin_getup(edict_t* self) //TODO: rename to morcalavin_get_up.
{
	if (self->monsterinfo.lefty == 7 && self->monsterinfo.attack_finished > 0.0f && self->monsterinfo.attack_finished < level.time)
	{
		self->monsterinfo.attack_finished = -1.0f;
		gi.sound(self, CHAN_VOICE, sounds[SND_REVIVE], 1.0f, ATTN_NORM, 0.0f);
		SetAnim(self, ANIM_GETUP);
	}
}

void morcalavin_hurtidle(edict_t* self) //TODO: rename to morcalavin_hurt_idle.
{
	SetAnim(self, ANIM_HURTIDLE);
}

static void MorcalavinStandMsgHandler(edict_t* self, G_Message_t* msg) //mxd. Named 'morcalavin_stand' in original logic.
{
	if (self->health > 0)
		SetAnim(self, ANIM_FLOAT);
}

void mork_ai_hover(edict_t* self, float distance) //TODO: rename to morcalavin_ai_hover.
{
	if (self->health <= 0)
		return;

	if (self->enemy != NULL)
		ai_charge(self, 0.0f);
	else
		ai_stand(self, 0.0f);

	if (distance > 0.0f)
	{
		vec3_t bottom;
		VectorCopy(self->s.origin, bottom);
		bottom[2] -= distance;

		trace_t trace;
		gi.trace(self->s.origin, self->mins, self->maxs, bottom, self, MASK_SOLID, &trace);

		if (trace.fraction < 1.0f)
		{
			const float desired_vel = (1.0f - trace.fraction) * distance;
			self->velocity[2] = max(desired_vel, self->velocity[2]);
		}
	}
}

void mork_ai_run(edict_t* self, float distance) //TODO: rename to morcalavin_ai_run.
{
	if (self->health <= 0)
		return;

	if (self->curAnimID == ANIM_FLY)
	{
		ai_charge(self, distance);
		return;
	}

	MG_AI_Run(self, distance);

	if (self->groundentity == NULL)
	{
		if (self->curAnimID == ANIM_WALK)
		{
			vec3_t forward;
			AngleVectors(self->s.angles, forward, NULL, NULL);
			VectorScale(forward, 250.0f, self->velocity);
			self->velocity[2] = 150.0f;
		}

		SetAnim(self, ANIM_GLIDE);
	}
	else
	{
		SetAnim(self, ANIM_WALK);
	}
}

static void MorcalavinRunMsgHandler(edict_t* self, G_Message_t* msg) //mxd. Named 'morcalavin_run' in original logic.
{
	// If can't move, go into a float for a bit.
	if (self->health <= 0)
		return;

	if (self->enemy == NULL)
	{
		SetAnim(self, ANIM_FLOAT);
		return;
	}

	if (self->enemy->health <= 0)
	{
		MorcalavinCheckKilledEnemy(self->enemy);
		SetAnim(self, ANIM_FLOAT);

		return;
	}

	if (self->monsterinfo.lefty == 0)
	{
		MorcalavinPhaseOutInit(self);
		SetAnim(self, ANIM_FLOAT);

		gi.sound(self, CHAN_VOICE, sounds[SND_REVIVE], 1.0f, ATTN_NORM, 0.0f);

		self->solid = SOLID_NOT;
		self->monsterinfo.sound_start = level.time + 2.5f;
		self->monsterinfo.lefty++; //TODO: add morcalavin_taunt_counter name.

		return;
	}

	SetAnim(self, ((self->groundentity == NULL) ? ANIM_GLIDE : ANIM_WALK));
}

typedef enum morcalavin_attackID_e
{
	MORK_ATTACK_FADE,
	MORK_ATTACK_TRACKING,
	MORK_ATTACK_SPHERE,
	MORK_ATTACK_BEAM,
	MORK_ATTACK_5SPHERE
} morcalavin_attackID_t;

static void MorcalavinMissileMsgHandler(edict_t* self, G_Message_t* msg) //mxd. Named 'morcalavin_missile' in original logic.
{
	const int chance = irand(0, 100);

	if (chance < 5 && self->wait != MORK_ATTACK_FADE)
	{
		//mxd. Inline morcalavin_attack_fade_out().
		morcalavin_fade_out(self);
		self->monsterinfo.lefty = 8;
		self->wait = MORK_ATTACK_FADE; //TODO: add morcalavin_attackID_t morcalavin_current_attack_id name.
	}
	else if (chance < 25 && self->wait != MORK_ATTACK_TRACKING)
	{
		SetAnim(self, ANIM_TRACKING1);
		self->wait = MORK_ATTACK_TRACKING;
	}
	else if (chance < 50 && self->wait != MORK_ATTACK_SPHERE)
	{
		SetAnim(self, ANIM_ATTACK2B);
		self->wait = MORK_ATTACK_SPHERE;
	}
	else if (chance < 75 && self->wait != MORK_ATTACK_BEAM)
	{
		SetAnim(self, ANIM_ATTACK3);
		self->wait = MORK_ATTACK_BEAM;
	}
	else if (self->monsterinfo.stepState > 1 && self->wait != MORK_ATTACK_5SPHERE)
	{
		SetAnim(self, ANIM_ATTACK4);
		self->wait = MORK_ATTACK_5SPHERE;
	}
}

static void MorcalavinMeleeMsgHandler(edict_t* self, G_Message_t* msg) //mxd. Named 'morcalavin_melee' in original logic.
{
	SetAnim(self, ANIM_ATTACK1);
}

void MorcalavinStaticsInit(void)
{
	static ClassResourceInfo_t res_info; //mxd. Made local static.

	classStatics[CID_MORK].msgReceivers[MSG_STAND] = MorcalavinStandMsgHandler;
	classStatics[CID_MORK].msgReceivers[MSG_MELEE] = MorcalavinMeleeMsgHandler;
	classStatics[CID_MORK].msgReceivers[MSG_MISSILE] = MorcalavinMissileMsgHandler;
	classStatics[CID_MORK].msgReceivers[MSG_RUN] = MorcalavinRunMsgHandler;
	classStatics[CID_MORK].msgReceivers[MSG_DEATH] = MorcalavinDeathMsgHandler;

	res_info.numAnims = NUM_ANIMS;
	res_info.animations = animations;
	res_info.modelIndex = gi.modelindex("models/monsters/morcalavin/tris.fm");

	// Quake attack.
	//sounds[SND_QUAKE] = gi.soundindex("monsters/mork/quake.wav");

	// Straight-fire beam.
	//sounds[SND_BEAM] = gi.soundindex("monsters/mork/beam.wav");
	//sounds[SND_BEAMHIT] = gi.soundindex("monsters/mork/beamhit.wav");

	// Homing balls.
	sounds[SND_HOMING] = gi.soundindex("monsters/mork/homing.wav");
	//sounds[SND_HOMEHIT] = gi.soundindex("monsters/mork/homehit.wav");

	// Power Puff.
	sounds[SND_PPCHARGE] = gi.soundindex("monsters/mork/ppcharge.wav");
	sounds[SND_PPFIRE] = gi.soundindex("monsters/mork/ppfire.wav");
	//sounds[SND_PPEXPLODE] = gi.soundindex("monsters/mork/ppexplode.wav");

	// Lightning from eyes.
	//sounds[SND_LIGHTNING] = gi.soundindex("monsters/mork/lightning.wav");
	//sounds[SND_LGHTNGHIT] = gi.soundindex("monsters/mork/lghtnghit.wav");

	// Shove.
	//sounds[SND_FORCEWALL] = gi.soundindex("monsters/mork/forcewall.wav");

	// Shield.
	sounds[SND_MAKESHIELD] = gi.soundindex("monsters/mork/makeshield.wav");
	//sounds[SND_SHIELDHIT] = gi.soundindex("monsters/mork/shieldhit.wav");
	//sounds[SND_SHIELDPULSE] = gi.soundindex("monsters/mork/shieldpulse.wav");
	//sounds[SND_SHIELDGONE] = gi.soundindex("monsters/mork/shieldgone.wav");
	//sounds[SND_SHIELDBREAK] = gi.soundindex("monsters/mork/shieldbreak.wav");

	// Fly forward.
	sounds[SND_RUSH] = gi.soundindex("monsters/mork/rush.wav");

	// Hurt and get up.
	sounds[SND_FALL] = gi.soundindex("monsters/mork/fall.wav");
	sounds[SND_REVIVE] = gi.soundindex("monsters/mork/revive.wav");

	// Strafing beams attack.
	//sounds[SND_STRAFEON] = gi.soundindex("monsters/mork/strafeon.wav");
	//sounds[SND_STRFSWNG] = gi.soundindex("monsters/mork/strfswng.wav");
	//sounds[SND_STRAFEOFF] = gi.soundindex("monsters/mork/strafeoff.wav");

	// Hurt/kill player laugh.
	sounds[SND_LAUGH] = gi.soundindex("monsters/mork/laugh.wav");

	// Taunts.
	sounds[TAUNT_LAUGH1] = gi.soundindex("monsters/mork/laugh1.wav");
	sounds[TAUNT_LAUGH2] = gi.soundindex("monsters/mork/laugh2.wav");
	sounds[TAUNT_LAUGH3] = gi.soundindex("monsters/mork/laugh3.wav");
	sounds[TAUNT_LAUGH4] = gi.soundindex("monsters/mork/laugh4.wav");

	sounds[TAUNT_BELLY1] = gi.soundindex("monsters/mork/belly.wav");
	sounds[TAUNT_BELLY2] = gi.soundindex("monsters/mork/belly2.wav");
	sounds[TAUNT_BELLY3] = gi.soundindex("monsters/mork/digest.wav");

	res_info.numSounds = NUM_SOUNDS;
	res_info.sounds = sounds;

	classStatics[CID_MORK].resInfo = &res_info;
}

void morcalavin_fade_out(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sounds[SND_REVIVE], 1.0f, ATTN_NORM, 0.0f);

	self->monsterinfo.sound_start = level.time + 2.0f;
	MorcalavinPhaseOutInit(self);
	SetAnim(self, ANIM_FLOAT);
}

static void MorcalavinChooseTeleportDestination(edict_t* self) //mxd. Removed unused return type. Named 'morcalavin_choose_teleport_destination' in original logic.
{
	// Instead of chance, do around self if evade, around other if ambush.
	if (self->enemy == NULL)
	{
		// Phase in and become tangible again.
		MorcalavinPhaseInInit(self);
		self->takedamage = DAMAGE_YES;
		self->monsterinfo.lefty = 10;

		return;
	}

	for (int i = 0; i < 10; i++)
	{
		const vec3_t teleport_angles = { 0.0f, anglemod(flrand(0.0f, 360.0f)), 0.0f }; //TODO: should be flrand(0.0f, 359.0f)?

		vec3_t forward;
		AngleVectors(teleport_angles, forward, NULL, NULL);

		vec3_t start_pos;
		VectorCopy(self->enemy->s.origin, start_pos);
		start_pos[2] += self->enemy->mins[2] - self->mins[2];

		const float trace_dist = flrand(self->min_missile_range, self->missile_range); //mxd. irand() in original logic.

		vec3_t end_pos;
		VectorMA(start_pos, -trace_dist, forward, end_pos);

		trace_t trace;
		gi.trace(start_pos, self->mins, self->maxs, end_pos, self->enemy, MASK_MONSTERSOLID, &trace);

		if (trace.allsolid || trace.startsolid || trace.fraction * trace_dist < 100.0f) // Minimum origin lerp dist.
			continue;

		if (vhlen(trace.endpos, self->enemy->s.origin) >= 128.0f)
		{
			VectorCopy(trace.endpos, start_pos);
			VectorCopy(trace.endpos, end_pos);
			end_pos[2] -= 64.0f;

			gi.trace(start_pos, self->mins, self->maxs, end_pos, self->enemy, MASK_MONSTERSOLID, &trace);

			if (trace.fraction < 1.0f && !trace.allsolid && !trace.startsolid) // The last two should be false if trace.fraction is < 1.0 but doesn't hurt to check.
			{
				VectorCopy(trace.endpos, self->s.origin);
				gi.linkentity(self);

				return;
			}
		}
	}
}

// Teleport in and attack the player quickly, before fading out again.
static void MorcalavinTeleportAttack(edict_t* self) //mxd. Named 'morcalavin_teleport_attack' in original logic.
{
	// Find a valid point away from the player.
	MorcalavinChooseTeleportDestination(self);

	// Start the animation for the attack.
	SetAnim(self, ((self->monsterinfo.lefty == 8) ? ANIM_ATTACK4 : ANIM_ATTACK2));

	// Play the teleport in sound fx.
	gi.sound(self, CHAN_AUTO, sounds[SND_MAKESHIELD], 1.0f, ATTN_NORM, 0.0f);

	// Start phasing back in.
	MorcalavinPhaseInInit(self);

	// Become tangible once more.
	self->solid = SOLID_BBOX;

	// Taunt player?
	if (self->monsterinfo.stepState > 0 && irand(0, 1) == 1)
		gi.sound(self, CHAN_AUTO, sounds[irand(TAUNT_LAUGH2, TAUNT_LAUGH4)], 1.0f, ATTN_NONE, 0.0f);
}

static void MorcalavinPostThink(edict_t* self) //mxd. Named 'morcalavin_postthink' in original logic.
{
	if (self->monsterinfo.lefty == 0)
		MG_CheckEvade(self);

	if (self->enemy != NULL && self->monsterinfo.stepState > 0)
	{
		if (self->dmg < self->max_health)
		{
			M_ShowLifeMeter(self->dmg, self->max_health); //BUGFIX: mxd. M_ShowLifeMeter(self->dmg, self->dmg) in original logic.
			self->dmg += 50;
		}
		else
		{
			M_ShowLifeMeter(self->health, self->max_health);
			MorcalavinCheckKilledEnemy(self->enemy);
		}
	}

	// Check for a teleport razzing.
	if (self->monsterinfo.jump_time > 0.0f && self->monsterinfo.jump_time < level.time)
	{
		if (self->monsterinfo.stepState > 0)
		{
			MorcalavinPhaseInInit(self);
			self->monsterinfo.sound_start = -1.0f; //TODO: add morcalavin_taunt_time name.
		}

		MorcalavinTeleportAttack(self);
		self->monsterinfo.jump_time = -1.0f; //TODO: add morcalavin_teleport_attack_time name.

		return;
	}

	// Check for a pending taunt.
	if (self->monsterinfo.sound_start > 0 && self->monsterinfo.sound_start < level.time)
	{
		switch (self->monsterinfo.lefty)
		{
			case 1:
				gi.sound(self, CHAN_AUTO, sounds[TAUNT_LAUGH1], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 1.0f;
				self->monsterinfo.lefty++;
				break;

			case 2:
				gi.sound(self, CHAN_AUTO, sounds[TAUNT_BELLY1], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 8.0f;
				self->monsterinfo.lefty++;
				break;

			case 3:
				gi.sound(self, CHAN_AUTO, sounds[TAUNT_BELLY2], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 5.0f;
				self->monsterinfo.lefty++;
				break;

			case 4:
				gi.sound(self, CHAN_AUTO, sounds[TAUNT_BELLY3], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 7.0f;
				self->monsterinfo.lefty++;
				break;

			case 5:
				if (irand(0, 1) == 1)
					gi.sound(self, CHAN_AUTO, sounds[irand(TAUNT_LAUGH2, TAUNT_LAUGH4)], 1.0f, ATTN_NONE, 0.0f);

				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 1.0f;
				break;

			case 6:
				gi.sound(self, CHAN_AUTO, sounds[TAUNT_LAUGH1], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.jump_time = -1.0f;

				if (self->targetEnt != NULL)
				{
					self->svflags &= ~SVF_NO_AUTOTARGET;

					if (self->delay > 0.0f)
					{
						self->monsterinfo.attack_finished = level.time + self->delay;
						self->monsterinfo.sound_start = self->monsterinfo.attack_finished;
						self->monsterinfo.sound_start += 1.5f;
						self->targetEnt->monsterinfo.attack_finished = self->monsterinfo.attack_finished;
						self->delay *= 2.0f;
					}
					else
					{
						self->monsterinfo.sound_start = level.time + 3.5f;
						self->monsterinfo.attack_finished = level.time + 2.0f;
						self->delay = 2.0f;
					}

					self->monsterinfo.lefty++;
				}
				break;

			case 7:
				gi.sound(self, CHAN_AUTO, sounds[SND_LAUGH], 1.0f, ATTN_NONE, 0.0f);
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = -1.0f;
				break;

			case 8:
				self->monsterinfo.sound_start = -1.0f;
				self->monsterinfo.jump_time = level.time + 1.0f;
				break;
		}
	}

	self->next_post_think = level.time + 0.05f;
}

static void MorcalavinDie(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, vec3_t point) //mxd. Named 'morcalavin_resist_death' in original logic.
{
	self->msgHandler = DeadMsgHandler;

	gi.sound(self, CHAN_VOICE, sounds[SND_FALL], 1.0f, ATTN_NORM, 0.0f);
	SetAnim(self, ANIM_FALL);

	self->s.color.c = 0xffffffff; //BUGFIX: mxd. Original logic sets color.a to 0xffffffff.
	self->pre_think = NULL;
	self->next_pre_think = -1.0f;

	self->takedamage = DAMAGE_NO;
	self->dmg = 1; //TODO: add morcalavin_charge_meter name.
	self->health = MonsterHealth(MORK_HEALTH);
	self->max_health = self->health;
	self->monsterinfo.sound_start = level.time + 2.5f;
	self->solid = SOLID_BBOX;

	self->monsterinfo.stepState++;
	self->monsterinfo.lefty = 6;

	// Check to release a charging weapon.
	if (self->target_ent != NULL)
	{
		vec3_t right;
		vec3_t forward;
		AngleVectors(self->s.angles, forward, right, NULL);

		VectorMA(self->s.origin, 10.0f, forward, self->target_ent->s.origin);
		VectorMA(self->target_ent->s.origin, 14.0f, right, self->target_ent->s.origin);
		self->target_ent->s.origin[2] += 42.0f;

		VectorScale(forward, 400.0f, self->target_ent->velocity);

		self->target_ent->think = MorcalavinLightningThink;
		self->target_ent->nextthink = level.time + FRAMETIME; //mxd. Use define.
	}

	//FIXME: Create a barrier around him so the player cannot get close to him.
}

// QUAKED monster_morcalavin(1 .5 0) (-24 -24 -50) (24 24 40)
// Morky
// wakeup_target	- Monsters will fire this target the first time it wakes up (only once).
// pain_target		- Monsters will fire this target the first time it gets hurt (only once).
void SP_monster_morcalavin(edict_t* self)
{
	if (!M_WalkmonsterStart(self)) // Failed initialization.
		return;

	self->msgHandler = DefaultMsgHandler;
	self->classname = "monster_morcalavin";

	if (self->health == 0)
		self->health = MORK_HEALTH1;

	// Apply to the end result (whether designer set or not).
	self->health = MonsterHealth(self->health);
	self->max_health = self->health;

	self->mass = MORK_MASS;
	self->yaw_speed = 24.0f;
	self->movetype = PHYSICSTYPE_STEP;
	self->solid = SOLID_BBOX;

	self->viewheight = 36;
	self->materialtype = MAT_FLESH;
	self->s.modelindex = (byte)classStatics[CID_MORK].resInfo->modelIndex;
	self->s.skinnum = 0;

	self->monsterinfo.otherenemyname = "player";

	// This is the number of times he's died (used to calculate window of opportunity for the player).
	self->delay = 0.0f; //TODO: add morcalavin_attack_delay name.

	self->s.origin[2] += 50.0f;
	VectorSet(self->mins, -24.0f, -24.0f, -48.0f);
	VectorSet(self->maxs,  24.0f,  24.0f,  40.0f);

	self->post_think = MorcalavinPostThink;
	self->next_post_think = level.time + FRAMETIME; //mxd. Use define.

	if (self->monsterinfo.scale == 0.0f) //BUGFIX: mxd. 'if (self->monsterinfo.scale)' in original logic.
	{
		self->monsterinfo.scale = MODEL_SCALE;
		self->s.scale = self->monsterinfo.scale;
	}

	MG_InitMoods(self);
	QPostMessage(self, MSG_STAND, PRI_DIRECTIVE, NULL);

	self->s.color.c = 0xffffffff;
	self->s.renderfx |= RF_GLOW;

	self->monsterinfo.stepState = 0;
	self->svflags |= (SVF_BOSS | SVF_FLOAT);
	self->count = self->s.modelindex; // For Cinematic purposes. //TODO: unused?
	self->gravity = MORCALAVIN_GRAVITY;

	self->die = MorcalavinDie;

	gi.linkentity(self);
}

static void MorcalavinBarrierThink(edict_t* self) //mxd. Named 'morcalavin_barrier_think' in original logic.
{
	// If we haven't found an owner yet, find one.
	if (self->owner == NULL)
	{
		edict_t* owner = G_Find(NULL, FOFS(classname), "monster_morcalavin");

		if (owner != NULL)
		{
			self->owner = owner;
			owner->targetEnt = self;
		}
	}

	if (self->monsterinfo.attack_finished > level.time)
	{
		self->count = false;
		self->svflags |= SVF_NOCLIENT;
	}
	else
	{
		self->count = true;
		self->svflags &= ~SVF_NOCLIENT;
	}

	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

static void MorcalavinBarrierTouch(edict_t* self, edict_t* other, cplane_t* plane, csurface_t* surf) //mxd. Named 'morcalavin_barrier_touch' in original logic.
{
	if (strcmp(other->classname, "player") != 0 || !self->count) //TODO: check other->client instead?
		return;

	vec3_t vel;
	VectorSubtract(self->s.origin, other->s.origin, vel);
	VectorNormalize(vel);

	VectorScale(vel, -1.0f, vel);
	VectorScale(vel, 512.0f, other->velocity);

	other->velocity[2] = 128.0f;
	other->client->playerinfo.flags |= PLAYER_FLAG_USE_ENT_POS;

	// NOTENOTE: We should always have an owner. But this is for safety.
	edict_t* attacker = (self->owner != NULL ? self->owner : self);
	T_Damage(other, self, attacker, vel, other->s.origin, vel, irand(5, 10), 250, DAMAGE_AVOID_ARMOR, MOD_DIED);

	if (self->delay < level.time)
	{
		gi.CreateEffect(NULL, FX_WEAPON_STAFF_STRIKE, 0, other->s.origin, "db", vel, 2);
		self->delay = level.time + 0.5f;
	}
}

static void MorcalavinBarrierUse(edict_t* self, edict_t* other, edict_t* activator) //mxd. Named 'morcalavin_barrier_use' in original logic.
{
	self->svflags &= ~SVF_NOCLIENT; // Become visible again.
	self->use = NULL; // Never do this again.

	// Start blocking.
	self->think = MorcalavinBarrierThink;
	self->nextthink = level.time + FRAMETIME; //mxd. Use define.
}

void SP_obj_morcalavin_barrier (edict_t *self)
{
	gi.setmodel(self, self->model);

	self->solid = SOLID_TRIGGER;
	self->movetype = PHYSICSTYPE_NONE;

	self->touch = MorcalavinBarrierTouch;
	self->use   = MorcalavinBarrierUse;

	self->s.color.c = 0xFFFFFFFF;
	self->count = 1;
	self->health = true;

	gi.linkentity(self);

	//Be invisible until used
	self->svflags |= SVF_NOCLIENT;
}

