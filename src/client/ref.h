//
// ref.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "qcommon.h"

// These are the maximum number that maybe rendered on any given frame
#define	MAX_DLIGHTS			32
#define	MAX_ENTITIES		128	
#define MAX_ALPHA_ENTITIES	MAX_ENTITIES * 2
#define	MAX_SERVER_ENTITIES	MAX_ENTITIES
#define	MAX_PARTICLES		4096
#define	MAX_LIGHTSTYLES		256

#define NUM_PARTICLE_TYPES	62	// This doesn't use the macro because of referencing weirdness.

typedef struct entity_s
{
	struct model_s** model; // Opaque type outside refresh
	float angles[3];
	float origin[3];
	int frame;

	// Model scale
	float scale;

	// Scale of model - but only for client entity models - not server side models.
	// Required for scaling mins and maxs that are used to cull models - mins and maxs
	// are scaled on the server side, but not on the client side when the models are loaded in.
	float cl_scale;

	// Distance to the camera origin, gets set every frame by AddEffectsToView 
	float depth;

	paletteRGBA_t color;
	int flags;

	union
	{
		int rootJoint;	// rootJoint of the entities skeleton
		int spriteType;
	};

	union
	{
		// Info for fmodels and bmodels
		struct
		{
			float oldorigin[3];	
			int oldframe;

			float backlerp;	// 0.0 = current, 1.0 = old

			int skinnum;
			struct image_s* skin;		// NULL for inline skin
			struct image_s** skins;		// Pointer to the list of clientinfo skins.
			char skinname[MAX_QPATH];	// For specific path to skin

			paletteRGB_t absLight;
			byte padFor_3byte_absLight;

			// Client entities which use a flexible model will need to fill this in, and then release it when they die.
			// Happily most client entities are sprites.
			fmnodeinfo_t* fmnodeinfo;

			int swapCluster;	// Cluster to swap on

			int swapFrame;		// Frame to swap clustered verts in for
			int oldSwapFrame;	// Previous frame to swap clustered verts in for

			struct LERPedReferences_s* referenceInfo;
			struct LERPedReferences_s* oldReferenceInfo;

			int padToUnionSize[4];	// Use this space up to add any more nonsprite fields that may be needed
		};

		// Info for dynamic sprites
		float verts[4][4];			// verts for dynamic sprites [x, y, s, t]

		// Info for variable sprites
		struct
		{
			float (*verts_p)[4];		// Pointer to verts for variable sprites
			int numVerts;
			int padToUnionSize2[11];	// Use this space up to add any more variable sprite fields
		};

		// info for line sprites
		struct
		{
			float startpos[3];
			float endpos[3];
			float scale2;
			float tile;
			float tileoffset;
			int padToUnionSize3[7];		// Use this space up to add any more line sprite fields
		};
	};
} entity_t;

#define ENTITY_FLAGS  68

typedef struct
{
	vec3_t origin;
	paletteRGBA_t color;
	float intensity;
} dlight_t;

typedef struct
{
	float rgb[3];	// 0.0 - 2.0
	float white;	// Highest of rgb
} lightstyle_t;

typedef struct
{
	vec3_t origin;
	paletteRGBA_t color;
	float scale;
	int type;
} particle_t;

typedef struct
{
	int x;	// In virtual screen coordinates
	int y;
	int width;
	int height;
	int area;

	float fov_x;
	float fov_y;
	float vieworg[3];
	float clientmodelorg[3];
	float viewangles[3];
	float blend[4];	// rgba 0-1 full screen blend
	float time;		// Time is uesed to auto animate
	int rdflags;	// RDF_UNDERWATER, etc

	byte* areabits;	// If not NULL, only areas with set bits will be drawn

	lightstyle_t* lightstyles;	// [MAX_LIGHTSTYLES]

	int num_entities;
	entity_t** entities;

	int num_alpha_entities;
	entity_t** alpha_entities;

	int num_dlights;
	dlight_t* dlights;

	int num_particles;
	particle_t* particles;

	int anum_particles;
	particle_t* aparticles;
} refdef_t;

#define	API_VERSION		3

// Functions exported by the refresh module
typedef struct
{
	// If api_version is different, the dll cannot be used
	int api_version;

	// Set this to false if you don`t want any rendering commands issued
	qboolean render;

	// Called when the library is loaded
	qboolean (*Init)(void* hinstance, void* wndproc);

	// Called before the library is unloaded
	void (*Shutdown)(void);

	// All data that will be used in a level should be
	// registered before rendering any frames to prevent disk hits,
	// but they can still be registered at a later time if necessary.

	// EndRegistration will free any remaining data that wasn't registered.
	// Any model_s or skin_s pointers from before the BeginRegistration
	// are no longer valid after EndRegistration.

	// Skins and images need to be differentiated, because skins
	// are flood filled to eliminate mip map edge errors, and pics have
	// an implicit "pics/" prepended to the name. (a pic name that starts with a
	// slash will not use the "pics/" prefix or the ".pcx" postfix)
	void (*BeginRegistration)(char* map);
	struct model_s* (*RegisterModel)(char* name);
	struct image_s* (*RegisterSkin)(char* name, qboolean* retval);
	struct image_s* (*RegisterPic)(char* name);
	void (*SetSky)(char* name, float rotate, vec3_t axis);
	void (*EndRegistration)(void);

	int (*GetReferencedID)(struct model_s* model);

	int (*RenderFrame)(refdef_t* fd);

	void (*DrawGetPicSize)(int* w, int* h, char* name);
	void (*DrawPic)(int x, int y, char* name, float alpha);
	void (*DrawStretchPic)(int x, int y, int w, int h, char* name, float alpha, qboolean scale);
	void (*DrawChar)(int x, int y, int c, paletteRGBA_t color);
	void (*DrawTileClear)(int x, int y, int w, int h, char* name);
	void (*DrawFill)(int x, int y, int w, int h, byte r, byte g, byte b);
	void (*DrawFadeScreen)(paletteRGBA_t color);
	void (*DrawBigFont)(int x, int y, char* text, float alpha);
	int (*BF_Strlen)(char* text);
	void (*BookDrawPic)(int w, int h, char* name, float scale);

	// Draw images for cinematic rendering (which can have a different palette). Note that calls
	void (*DrawInitCinematic)(int w, int h, char* overlay, char* backdrop);
	void (*DrawCloseCinematic)(void);
	void (*DrawCinematic)(int cols, int rows, byte* data, paletteRGB_t* palette, float alpha);
	void (*Draw_Name)(vec3_t origin, char* name, paletteRGBA_t color);

	// Video mode and refresh state management entry points
	void (*BeginFrame)(float camera_separation);
	void (*EndFrame)(void);

	void (*AppActivate)(qboolean activate);

	int (*FindSurface)(vec3_t start, vec3_t end, struct Surface_s* surface);

#ifdef __A3D_GEOM
	void (*A3D_RenderGeometry)(void* pA3D, void* pGeom, void* pMat, void* pGeomStatus);
#endif
} refexport_t;

// Functions imported by the refresh module
typedef struct
{
	struct CL_SkeletalJoint_s* skeletalJoints;
	struct ArrayedListNode_s* jointNodes;

	void (*Sys_Error)(int err_level, char* str, ...);
	void (*Com_Error)(int code, char* fmt, ...);
	void (*Con_Printf)(int print_level, char* str, ...);

	cvar_t* (*Cvar_Get)(char* name, char* value, int flags);
	cvar_t* (*Cvar_FullSet)(char* name, char* value, int flags);
	cvar_t* (*Cvar_Set)(char* name, char* value);
	void (*Cvar_SetValue)(char* name, float value);

	void (*Cmd_AddCommand)(char* name, void (*cmd)(void));
	void (*Cmd_RemoveCommand)(char* name);
	int (*Cmd_Argc)(void);
	char* (*Cmd_Argv)(int i);
	void (*Cmd_ExecuteText)(int exec_when, char* text);

	// This is used for the screen flash - there is a reason for doing this...
	int (*Is_Screen_Flashing)(void);
	void (*Deactivate_Screen_Flash)(void);

	// Files will be memory mapped read only.
	// The returned buffer may be part of a larger pak file,
	// or a discrete file from anywhere in the quake search path.
	// A -1 return means the file does not exist.
	// NULL can be passed for buf to just determine existence.
	int (*FS_LoadFile)(char* name, void** buf);
	void (*FS_FreeFile)(void* buf);

	// gamedir will be the current directory that generated files should be stored to, ie: "f:\quake\id1"
	char* (*FS_Gamedir)(void);
	char* (*FS_Userdir)(void);
	void (*FS_CreatePath)(char* path);

	qboolean (*Vid_GetModeInfo)(int* width, int* height, int mode);
	void (*Vid_MenuInit)(void);
	void (*Vid_NewWindow)(int width, int height);
} refimport_t;

// This is the only function actually exported at the linker level
typedef refexport_t (*GetRefAPI_t)(refimport_t);