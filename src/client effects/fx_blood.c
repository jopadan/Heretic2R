//
// fx_blood.c
//
// Copyright 1998 Raven Software
//

#include "fx_blood_local.h" //mxd
#include "Client Effects.h"
#include "Particle.h"
#include "Vector.h"
#include "Random.h"
#include "Reference.h"
#include "Utilities.h"
#include "g_playstats.h"

static struct model_s* splat_models[2];

void PreCacheSplat(void)
{
	splat_models[0] = fxi.RegisterModel("sprites/fx/bsplat.sp2");
	splat_models[1] = fxi.RegisterModel("sprites/fx/ysplat.sp2");
}

#define NUM_INSECT_BLOOD_PARTICLES	12

static int insect_blood_particles[NUM_INSECT_BLOOD_PARTICLES] =
{
	PART_4x4_GREEN,
	PART_4x4_YELLOW,
	PART_8x8_GLOBBIT1,
	PART_8x8_GLOBBIT2,
	PART_16x16_MIST,
	PART_16x16_GLOB,
	PART_16x16_SPARK_G,
	PART_16x16_SPARK_Y,
	PART_32x32_GREENBLOOD,
	PART_16x16_GREENBLOOD,
	PART_4x4_GREENBLOOD1,
	PART_4x4_GREENBLOOD2
};

client_entity_t* DoBloodSplash(vec3_t loc, int amount, const qboolean yellow_blood)
{
	client_particle_t* drop;
	paletteRGBA_t pal_gl;
	paletteRGBA_t pal_soft;

	client_entity_t* splash = ClientEntity_new(FX_BLOOD, CEF_NO_DRAW, loc, NULL, 1000);
	AddEffect(NULL, splash);

	const float speed = (float)amount * 4.0f + 16.0f;
	const float gravity = GetGravity() * 0.5f;

	pal_gl.c = 0xffffffff;
	pal_soft.c = 0xff0000ff;

	amount = min(500, amount);

	for (int i = 0; i < amount; i++)
	{
		const int size = i & 3; // Size = 0-3.

		switch (size)
		{
			case 0:	// Tiny particles.
				for (int j = 0; j < 6; j++)
				{
					if (ref_soft)
					{
						const int bpart = (yellow_blood ? PART_4x4_GREENBLOOD1 : PART_4x4_BLOOD1);
						drop = ClientParticle_new(bpart | PFL_SOFT_MASK, pal_soft, 650);
					}
					else
					{
						const int bpart = (yellow_blood ? irand(PART_4x4_GREENBLOOD1, PART_4x4_GREENBLOOD2) : irand(PART_4x4_BLOOD1, PART_4x4_BLOOD2));
						drop = ClientParticle_new(bpart, pal_gl, 800);
					}

					VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(speed * 2.0f, speed * 4.0f));
					drop->acceleration[2] = gravity;
					drop->d_alpha = 0.0f;
					drop->d_scale = -1.0f;

					AddParticleToList(splash, drop);
				}
				break;

			case 1:	// Some larger globs.
				for (int j = 0; j < 3; j++)
				{
					if (ref_soft)
					{
						const int bpart = (yellow_blood ? PART_8x8_GLOBBIT1 : PART_8x8_BLOOD);
						drop = ClientParticle_new(bpart | PFL_SOFT_MASK, pal_soft, 650);
					}
					else
					{
						const int bpart = (yellow_blood ? irand(PART_8x8_GLOBBIT1, PART_8x8_GLOBBIT2) : PART_8x8_BLOOD);
						drop = ClientParticle_new(bpart, pal_gl, 800);
					}

					VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(speed * 2.0f, speed * 4.0f));
					drop->scale = 2.0f;
					drop->acceleration[2] = gravity;
					drop->d_alpha = 0.0f;
					drop->d_scale = -2.0f;

					AddParticleToList(splash, drop);
				}
				break;

			case 2:	// Some big blobs.
				for (int j = 0; j < 2; j++)
				{
					const int bpart = (yellow_blood ? PART_16x16_GREENBLOOD : PART_16x16_BLOOD);
					drop = ClientParticle_new(bpart, pal_gl, 1000);
					VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(speed, speed * 2.0f));
					drop->scale = 1.0f;
					drop->acceleration[2] = gravity * 0.5f;
					drop->d_scale = flrand(4.0f, 8.0f);
					drop->d_alpha = flrand(-512.0f, -256.0f);

					AddParticleToList(splash, drop);
				}
				break;

			case 3:	// A big splash.
				const int bpart = (yellow_blood ? PART_32x32_GREENBLOOD : PART_32x32_BLOOD);
				drop = ClientParticle_new(bpart, pal_gl, 500);
				VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(0, speed));
				drop->scale = 4.0f;
				drop->acceleration[2] = 0.0f;
				drop->d_scale = flrand(48.0f, 64.0f);
				drop->d_alpha = flrand(-1024.0f, -512.0f);

				AddParticleToList(splash, drop);
				break;
		}
	}

	return splash;
}

void DoBloodTrail(client_entity_t* spawner, int amount)
{
	client_particle_t* drop;
	paletteRGBA_t pal_gl;
	paletteRGBA_t pal_soft;

	const qboolean yellow_blood = ((spawner->SpawnInfo & SIF_FLAG_MASK) == MAT_INSECT); // Insect blood is yellow-green.

	const float speed = (amount == -1 ? 0.0f : (float)amount * 4.0f + 8.0f);
	const float gravity = GetGravity() * 0.5f;
	const float range = (float)amount;

	pal_gl.c = 0xffffffff;
	pal_soft.c = 0xff0000ff;

	amount = min(500, amount);

	for (int i = 0; i < amount; i++)
	{
		const int size = i & 1;	// Size = 0-1.

		switch (size)
		{
			case 0:	// Tiny particles.
				for (int j = 0; j < 3; j++)
				{
					if (ref_soft)
					{
						const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : PART_4x4_BLOOD1); //mxd
						drop = ClientParticle_new(bpart | PFL_SOFT_MASK, pal_soft, 800);
					}
					else
					{
						const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : irand(PART_4x4_BLOOD1, PART_4x4_BLOOD2)); //mxd
						drop = ClientParticle_new(bpart, pal_gl, 800);
					}

					VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(speed * 2.0f, speed * 4.0f));
					VectorMA(drop->velocity, 0.5f, spawner->velocity, drop->velocity);
					VectorSet(drop->origin, flrand(-range, range), flrand(-range, range), flrand(-range, range));
					VectorAdd(drop->origin, spawner->r.origin, drop->origin);
					drop->acceleration[2] = gravity;
					drop->d_alpha = 0.0f;
					drop->d_scale = -1.0f;

					AddParticleToList(spawner, drop);
				}
				break;

			case 1:	// Some larger globs.
				const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : PART_8x8_BLOOD); //mxd
				drop = ClientParticle_new(bpart, pal_gl, 800);
				VectorSet(drop->velocity, flrand(-speed, speed), flrand(-speed, speed), flrand(speed * 2.0f, speed * 4.0f));
				VectorMA(drop->velocity, 0.5f, spawner->velocity, drop->velocity);
				VectorSet(drop->origin, flrand(-range, range), flrand(-range, range), flrand(-range, range));
				VectorAdd(drop->origin, spawner->r.origin, drop->origin);
				drop->acceleration[2] = gravity;
				drop->d_alpha = 0.0f;
				drop->scale = 2.0f;
				drop->d_scale = -2.0f;

				AddParticleToList(spawner, drop);
				break;
		}
	}
}

// Find exact plane to decal the bloodmark to.
static qboolean GetTruePlane(vec3_t origin, vec3_t direction) //TODO: unify with same-named function in fx_scorchmark.c?
{
	trace_t trace;
	vec3_t end;
	vec3_t mins;
	vec3_t maxs;

	VectorClear(mins);
	VectorClear(maxs);

	VectorMA(origin, 16.0f, direction, end);

	fxi.Trace(origin, mins, maxs, end, MASK_DRIP, CEF_CLIP_TO_WORLD, &trace);

	if (trace.fraction == 1.0f)
		return false;

	// Set the new endpos and plane (should be exact).
	VectorCopy(trace.endpos, origin);
	VectorCopy(trace.plane.normal, direction);

	// Raise the decal slightly off the target wall.
	VectorMA(origin, 0.25f, direction, origin);

	return true;
}

static qboolean BloodSplatSplashUpdate(client_entity_t* self, centity_t* owner)
{
	if (self->SpawnInfo > 500)
		return true;

	client_particle_t* p = NULL;
	const qboolean yellow_blood = (self->flags & CEF_FLAG8);
	const paletteRGBA_t color = { .r = 180, .g = 140, .b = 110, .a = 160 };

	while (self->SpawnInfo > 0)
	{
		if (ref_soft)
		{
			const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : PART_4x4_BLOOD1);
			p = ClientParticle_new(bpart | PFL_SOFT_MASK, color, 800);
		}
		else
		{
			const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : irand(PART_4x4_BLOOD1, PART_4x4_BLOOD2));
			p = ClientParticle_new(bpart, color, 800);
		}

		vec3_t vel;
		p->acceleration[2] = GetGravity() * 0.2f;
		VectorRandomCopy(self->endpos2, vel, 10.0f);
		VectorScale(vel, flrand(2.0f, 5.0f), p->velocity);
		p->d_alpha = 0.0f;
		p->scale = flrand(0.4f, 0.8f);
		VectorAdd(self->startpos2, self->startpos, p->origin);
		AddParticleToList(self, p);

		self->SpawnInfo--;
	}

	fxi.S_StartSound(p->origin, -1, CHAN_AUTO, fxi.S_RegisterSound(va("ambient/waterdrop%i.wav", irand(1, 3))), flrand(0.5f, 0.8f), ATTN_STATIC, 0);

	if (!irand(0, 2))
		VectorSet(self->startpos, flrand(-1, 1), flrand(-1, 1), 0);

	self->Update = BloodSplatDripUpdate;
	self->updateTime = irand(400, 1600);

	return true;
}

qboolean BloodSplatDripUpdate(client_entity_t* self, centity_t* owner)
{
	client_particle_t* p;

	if (!AttemptRemoveSelf(self, owner))
		return false;

	const qboolean yellow_blood = (self->flags & CEF_FLAG8);
	const paletteRGBA_t color = { .r = 150, .g = 140, .b = 110, .a = 160 };

	//TODO: make p duration based on self->radius?
	const int drip_time = (int)(self->radius * 6);
	const float scale = flrand(0.2f, 0.4f);
	const int num_drips = irand(7, 15);

	for (int i = 0; i < num_drips; i++)
	{
		if (ref_soft)
		{
			const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : PART_4x4_BLOOD1);
			p = ClientParticle_new(bpart | PFL_SOFT_MASK, color, 1600);
		}
		else
		{
			const int bpart = (yellow_blood ? insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)] : irand(PART_4x4_BLOOD1, PART_4x4_BLOOD2));
			p = ClientParticle_new(bpart, color, 3200);
		}

		const float grav_mod = 0.4f + (float)i * 0.025f;
		p->acceleration[2] = GetGravity() * grav_mod;
		p->d_alpha = 0;
		p->scale = scale + (float)i * 0.08f;
		VectorCopy(self->startpos, p->origin);

		AdvanceParticle(p, i * 7);
		AddParticleToList(self, p);

		if (drip_time >= 17)
		{
			p->duration = (int)((float)drip_time * 3.0f * grav_mod);
			self->SpawnInfo++;
		}
	}

	if (self->SpawnInfo && drip_time >= 17)
	{
		// Splash.
		self->Update = BloodSplatSplashUpdate;
		self->updateTime = drip_time;
	}
	else
	{
		if (!irand(0, 3))
			VectorSet(self->startpos, flrand(-1.0f, 1.0f), flrand(-1.0f, 1.0f), 0);

		self->updateTime = irand(400, 1600);
	}

	return true;
}

void ThrowBlood(vec3_t origin, vec3_t tnormal, const qboolean dark, const qboolean yellow, const qboolean trueplane)
{
	vec3_t normal;

	VectorCopy(tnormal, normal);
	VectorScale(normal, -1.0f, normal);

	if (trueplane || GetTruePlane(origin, normal))
	{
		client_entity_t* bloodmark = ClientEntity_new(FX_BLOOD, CEF_NOMOVE, origin, tnormal, 1000);

		bloodmark->r.angles[ROLL] = flrand(0, ANGLE_360);
		bloodmark->r.model = (yellow ? splat_models + 1 : splat_models);
		bloodmark->r.flags = RF_FIXED | RF_ALPHA_TEXTURE;
		bloodmark->r.frame = irand(0, 4);

		const byte brightness = (byte)(dark ? irand(32, 72) : irand(72, 128));
		bloodmark->r.color.r = brightness;
		bloodmark->r.color.g = brightness;
		bloodmark->r.color.b = brightness;
		bloodmark->r.color.a = 255;

		bloodmark->radius = 10.0f;
		bloodmark->r.scale = flrand(0.2f, 0.45f);

		if (tnormal[2] <= -0.7f && !irand(0, 2) && bloodmark->r.frame != 2 && bloodmark->r.frame != 4)
		{
			trace_t tr;
			vec3_t endpos;

			VectorMA(origin, 256, tnormal, endpos);
			fxi.Trace(origin, vec3_origin, vec3_origin, endpos, MASK_DRIP, CEF_CLIP_TO_WORLD, &tr);

			if (tr.fraction < 1.0f && tr.fraction > 0.0625f && tr.plane.normal[2] >= 0.7f) // Between 16 and 256.
			{
				bloodmark->radius = tr.fraction * 256.0f;
				VectorSubtract(tr.endpos, origin, bloodmark->startpos2);
				VectorCopy(tr.plane.normal, bloodmark->endpos2);

				ThrowBlood(tr.endpos, tr.plane.normal, dark, yellow, false);
			}

			VectorClear(bloodmark->startpos);
			bloodmark->LifeTime = 0;

			if (yellow)
				bloodmark->flags |= CEF_FLAG8;

			bloodmark->Update = BloodSplatDripUpdate;
		}
		else
		{
			bloodmark->Update = AttemptRemoveSelf;
		}

		AddEffect(NULL, bloodmark);
		InsertInCircularList(bloodmark);
	}
}

void FXBloodTrail(centity_t *owner, int type, int flags, vec3_t origin)
{
	vec3_t			normal;
	
	fxi.GetEffect(owner, flags, clientEffectSpawners[FX_BLOOD_TRAIL].formatString, &normal);

	VectorMA(origin, 0.25, normal, origin);
	ThrowBlood(origin, normal, flags&CEF_FLAG7, flags&CEF_FLAG6, true);
}

// ClientEffect FX_BLOOD
void FXBlood(centity_t *owner, int type, int flags, vec3_t origin)
{
	client_entity_t		*spawner;
	byte				amount;
	vec3_t				velocity;
	qboolean			yellow_blood = false;

	fxi.GetEffect(owner, flags, clientEffectSpawners[FX_BLOOD].formatString, &velocity, &amount);
	
	if(flags&CEF_FLAG8)
		yellow_blood = true;

	// lets add level of detail here to the amount we pump out
	switch((int)r_detail->value)
	{
	// half as much
	case DETAIL_LOW:
		amount = ((float)amount * 0.5);	 
		break;

	// 3 quarters
	case DETAIL_NORMAL:
		amount = ((float)amount * 0.75);	 
		break;

	default:
		break;
	}
	if (!amount)
		amount = 1;

	spawner = DoBloodSplash(origin, amount, yellow_blood);
	VectorCopy(velocity, spawner->velocity);
}

// ------------------------------------------------------------------------------

#define	NUM_BLOOD_PARTS		3

static qboolean LinkedBloodThink(client_entity_t *spawner, centity_t *owner)
{
	client_particle_t	*p;
	client_entity_t		*ce;
	vec3_t				org, vel;
	int					i;
	int					bpart;
	qboolean			yellow_blood;

	spawner->updateTime = irand(40, 60);

	spawner->LifeTime -= 50;
	if(spawner->LifeTime < 0)					// Effect finished
		return(false);

	if(spawner->LifeTime < 800)					// Effect needs to stay alive until particles die
		return(true);

	if(spawner->flags&CEF_FLAG8)//yellow
		yellow_blood = true;

	// This tells if we are wasting our time, because the reference points are culled.
	if (!RefPointsValid(owner))
		return false;		// Just end the effect.

	VectorGetOffsetOrigin(owner->referenceInfo->references[spawner->SpawnInfo].placement.origin, owner->current.origin, owner->current.angles[YAW], org);
	ce = ClientEntity_new(-1, 0, org, NULL, 800);
	ce->flags |= CEF_NO_DRAW | CEF_NOMOVE;
	ce->radius = 32.0;
	AddEffect(NULL, ce);

	VectorSubtract(org, spawner->origin, vel);
	Vec3ScaleAssign(5.0, vel);

	for(i = 0; i < NUM_BLOOD_PARTS; i++)
	{
		if(yellow_blood)
			bpart = insect_blood_particles[irand(0, NUM_INSECT_BLOOD_PARTICLES - 1)];
		else
			bpart = irand(PART_4x4_BLOOD1, PART_4x4_BLOOD2);
		p = ClientParticle_new(bpart, spawner->color, 800);
		Vec3AddAssign(vel, p->velocity);
		p->acceleration[2] = GetGravity();
		p->d_alpha = 0.0;
		p->d_scale = -1.0;
		AddParticleToList(ce, p);
	}

	// Remember current origin for calc of velocity
	VectorCopy(org, spawner->origin);
	return(true);
}

void FXLinkedBlood(centity_t *owner, int type, int flags, vec3_t origin)
{
	client_entity_t		*spawner;
	byte				refpointidx, life;
	int					count, i;
	int					lifetime = 0;

	fxi.GetEffect(owner, flags, clientEffectSpawners[FX_LINKEDBLOOD].formatString, &life, &refpointidx);
	lifetime = life * 50;
	count = 1;
	if(life > 1600)
	{
		count = (life - 1600) / 100;
		life = 1600;
		if(count > 10)				// Max out saftey check
			count = 10; 
	}

	for(i = 0; i < count; i++)
	{
		spawner = ClientEntity_new(type, flags, origin, NULL, Q_ftol(fxi.cls->frametime * 2000.0));

		spawner->LifeTime = life;
		spawner->flags |= CEF_NO_DRAW;
		spawner->color.c = 0xFFFFFFFF;
		spawner->Update = LinkedBloodThink;
		spawner->SpawnInfo = refpointidx;
		spawner->AddToView = OffsetLinkedEntityUpdatePlacement;

		AddEffect(owner, spawner);
	}
}

/*
void FXTrailBlood(centity_t *owner, int type, int flags, vec3_t origin)
{
	client_entity_t		*spawner;
	byte				amount;
	vec3_t				velocity;
	qboolean			yellow_blood = false;

	if(flags&CEF_FLAG8)
		yellow_blood = true;

	spawner = DoBloodSplash(origin, amount, yellow_blood);
	VectorCopy(velocity, spawner->velocity);
}
*/
// end
