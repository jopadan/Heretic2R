//
// server.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "qcommon.h"
#include "qfiles.h"
#include "game.h"
#include "q_Physics.h"
#include "SinglyLinkedList.h"

#define MAX_MASTERS	8 // Max. recipients for heartbeat packets

typedef enum
{
	ss_dead,	// No map loaded
	ss_loading,	// Spawning level edicts
	ss_game,	// Actively running
	ss_cinematic,
	ss_demo,
	ss_pic
} server_state_t;

typedef struct
{
	server_state_t state;	// Precache commands are only valid during load

	qboolean attractloop;	// Running cinematics and demos for the local system only
	qboolean loadgame;		// Client begins should reuse existing entity

	uint time; // Always sv.framenum * 100 msec
	int framenum;

	char name[MAX_QPATH]; // Map or cinematic name
	struct cmodel_s* models[MAX_MODELS];

	char configstrings[MAX_CONFIGSTRINGS][512]; //mxd. Increased string size from MAX_QPATH, so CS_STATUSBAR can fit into single slot.
	entity_state_t baselines[MAX_EDICTS];

	// The multicast buffer is used to send a message to a set of clients.
	// It is only used to marshall data until SV_Multicast is called.
	sizebuf_t multicast;
	byte multicast_buf[MAX_MSGLEN];

	// Demo server information
	FILE* demofile;
	qboolean timedemo; // Don't time sync
} server_t;

#define EDICT_NUM(n)		((edict_t *)((byte *)ge->edicts + ge->edict_size * (n)))
#define NUM_FOR_EDICT(e)	(((byte *)(e) - (byte *)ge->edicts) / ge->edict_size)
#define EDICT_MASK(e)		(1 << (NUM_FOR_EDICT(e) - 1)) //mxd

typedef enum
{
	cs_free,		// Can be reused for a new connection.
	cs_zombie,		// Client has been disconnected, but don't reuse connection for a couple seconds.
	cs_connected,	// Has been assigned to a client_t, but not in game yet.
	cs_spawned		// Client is fully in game.
} client_state_e; //mxd. Renamed to avoid confusion with client_state_t struct

typedef enum
{
	cst_default,			// 0
	cst_unknown1,
	cst_unknown2,
	cst_cinematic_freeze,	// 3
	cst_coop_timeout,		// 4
} coop_state_e; // H2 //TODO: remove unknown entries

typedef struct
{
	int areabytes;
	byte areabits[MAX_MAP_AREAS / 8]; // portalarea visibility bits
	player_state_t ps;
	int num_entities;
	int first_entity; // Into the circular sv_packet_entities[]
	int senttime; // For ping calculations
} client_frame_t;

#define LATENCY_COUNTS	32 // Q2: 16
#define RATE_MESSAGES	10

typedef struct client_s
{
	client_state_e state;
	char userinfo[MAX_INFO_STRING]; // name, etc

	int lastframe; // For delta compression
	usercmd_t lastcmd; // For filling in big drops

	int commandMsec; // Every ?? seconds this is reset, if user commands exhaust it, assume time cheating.
	int frame_latency[LATENCY_COUNTS];

	int ping;
	int message_size[RATE_MESSAGES]; // Used to rate drop packets
	int rate;

	edict_t* edict; // EDICT_NUM(clientnum+1)
	char name[32]; // Extracted from userinfo, high bits masked
	int messagelevel; // For filtering printed messages

	// The datagram is written to by sound calls, prints, temp ents, etc.
	// It can be harmlessly overflowed.
	sizebuf_t datagram;
	byte datagram_buf[MAX_MSGLEN];

	client_frame_t frames[UPDATE_BACKUP]; // Updates can be delta'd from here

	byte* download; // File being downloaded
	int downloadsize; // Total bytes (can't use EOF because of paks)
	int downloadcount; // Bytes sent

	int lastmessage; // sv.framenum when packet was last received
	int lastconnect;

	netchan_t netchan;
	int challenge;

	coop_state_e coop_state;
} client_t;

// MAX_CHALLENGES is made large to prevent a denial of service attack
// that could cycle all of them out before legitimate users connected.
#define MAX_CHALLENGES	1024

typedef struct
{
	netadr_t adr;
	int challenge;
	int time;
} challenge_t;

typedef struct
{
	qboolean initialized; // sv_init has completed
	int realtime; // always increasing, no clamping, etc

	char mapcmd[MAX_TOKEN_CHARS]; // ie: *intro.cin+base 

	int spawncount; // Incremented each server start. Used to check late spawns.

	client_t* clients;					// [maxclients->value];
	int num_client_entities;			// maxclients->value*UPDATE_BACKUP*MAX_PACKET_ENTITIES
	int next_client_entities;			// Next client_entity to use
	entity_state_t* client_entities;	// [num_client_entities]

	int last_heartbeat;

	challenge_t challenges[MAX_CHALLENGES]; // To prevent invalid IPs from connecting

	qboolean have_current_save; // H2. More members after this in Q2!
} server_static_t;

extern netadr_t master_adr[MAX_MASTERS]; // Address of the master server

extern server_static_t svs; // Persistent server info
extern server_t sv; // Local server

extern cvar_t* sv_paused;
extern cvar_t* maxclients;
extern cvar_t* r_farclipdist; // H2

extern cvar_t* sv_cooptimeout;
extern cvar_t* sv_cinematicfreeze;
extern cvar_t* sv_welcome_mess; // H2
extern cvar_t* sv_freezeworldset; // H2
extern cvar_t* sv_noreload;
extern cvar_t* sv_pers_fx_send_cut_off; // H2

extern cvar_t* sv_enforcetime;

extern client_t* sv_client;
extern edict_t* sv_player;

extern qboolean is_local_client; // H2

// sv_main.c
void SV_DropClient(client_t* drop);
void SV_UserinfoChanged(client_t* client);
void SV_PrepWorldFrame(void);

// sv_ccmds.c
void SV_ReadLevelFile(void);
void SV_InitOperatorCommands(void);
void SV_WipeSavegame(const char* savename);

// sv_entities.c
void SV_WriteFrameToClient(client_t* client, sizebuf_t* msg);
void SV_BuildClientFrame(client_t* client);

// sv_init.c
int SV_ModelIndex(const char* name);
int SV_SoundIndex(const char* name);
int SV_ImageIndex(const char* name);
void SV_ModelRemove(char* name); // H2
void SV_SoundRemove(char* name); // H2
void SV_Map(qboolean attractloop, const char* levelstring, qboolean loadgame);

// sv_game.c
extern game_export_t* ge;
void SV_InitGameProgs(void);
void SV_ShutdownGameProgs(void);
qboolean PF_inPVS(vec3_t p1, vec3_t p2);

// sv_send.c
void SV_ClientPrintf(client_t* cl, int level, int message_id);
void SV_BroadcastPrintf(int level, char* fmt, ...);
void SV_BroadcastCaption(int printlevel, short stringid);
void SV_BroadcastObituary(int printlevel, short stringid, short client1, short client2);
void SV_BroadcastCommand(char* fmt, ...);
void SV_Multicast(const vec3_t origin, multicast_t to);
void SV_StartSound(vec3_t origin, edict_t* entity, int channel, int soundindex, float volume, float attenuation, float timeofs);
void SV_SendClientMessages(qboolean send_client_data); // H2: + 'send_client_data' arg

// sv_user.c
void SV_Nextserver(void);
void SV_ExecuteClientMessage(client_t* cl);

// sv_world.c
// Called after the world model has been loaded, before linking any entities.
void SV_ClearWorld(void);

// Needs to be called any time an entity changes origin, mins, maxs, or solid. Automatically unlinks if needed.
// sets ent->v.absmin and ent->v.absmax.
// sets ent->leafnums[] for pvs determination even if the entity is not solid.
void SV_LinkEdict(edict_t* ent);

// Call before removing an entity, and before trying to move one, so it doesn't clip against itself.
void SV_UnlinkEdict(edict_t* ent);

// Fills in a table of edict pointers with edicts that have bounding boxes that intersect the given area.
// It is possible for a non-axial bmodel to be returned that doesn't actually intersect the area on an exact test.
// returns the number of pointers filled in.
int SV_AreaEdicts(vec3_t mins, vec3_t maxs, edict_t** list, int maxcount, int areatype);

// New in H2
int SV_FindEntitiesInBounds(vec3_t mins, vec3_t maxs, struct SinglyLinkedList_s* list, int areatype);

// Returns the CONTENTS_* value from the world at the given point.
// Quake 2 extends this to also check entities, to allow moving liquids.
int SV_PointContents(vec3_t p);

// mins and maxs are relative.
// If the entire move stays in a solid volume, trace.startsolid and trace.allsolid will be set, and trace.fraction will be 0.
// If the starting point is in a solid, it will be allowed to move out to an open area.
// passedict is explicitly excluded from clipping checks (normally NULL).
void SV_Trace(vec3_t start, const vec3_t mins, const vec3_t maxs, vec3_t end, edict_t* passent, uint contentmask, trace_t* tr); // H2: different definition

// New in H2
void SV_TraceBoundingForm(struct FormMove_s* formMove);

// New in H2
qboolean SV_ResizeBoundingForm(edict_t* self, struct FormMove_s* formMove);

// New in H2
int SV_GetContentsAtPoint(vec3_t point);

// New in H2
qboolean SV_CheckDistances(vec3_t origin, float dist);