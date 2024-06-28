//
// gl_model.c -- model loading and caching
//
// Copyright 1998 Raven Software
//

#include "gl_local.h"
#include "fmodel.h"
#include "Vector.h"

model_t* loadmodel;
int modfilelen;

byte mod_novis[MAX_MAP_LEAFS / 8];

#define MAX_MOD_KNOWN 512
model_t mod_known[MAX_MOD_KNOWN];
int mod_numknown;

// The inline * models from the current map are kept seperate
model_t mod_inline[MAX_MOD_KNOWN];

int registration_sequence;

void Mod_Modellist_f(void)
{
	NOT_IMPLEMENTED
}

// Q2 counterpart
void Mod_Init(void)
{
	memset(mod_novis, 0xff, sizeof(mod_novis));
}

// New in H2 //TODO: byteswapping?
static void Mod_LoadBookModel(model_t* mod, const void* buffer)
{
	int i;
	bookframe_t* frame;
	char frame_name[MAX_QPATH];

	const book_t* book_in = buffer;
	book_t* book_out = Hunk_Alloc(modfilelen);

	if (book_in->bheader.version != BOOK_VERSION)
		Sys_Error("%s has wrong version number (%i should be %i)", mod->name, book_in->bheader.version, BOOK_VERSION);

	if (book_in->bheader.num_segments > MAX_MD2SKINS)
		Sys_Error("%s has too many frames (%i > %i)", mod->name, book_in->bheader.num_segments, MAX_MD2SKINS);

	// Copy everything
	memcpy(book_out, book_in, book_in->bheader.num_segments * sizeof(bookframe_t) + sizeof(bookheader_t));

	// Pre-load frame images
	for (i = 0, frame = book_out->bframes; i < book_out->bheader.num_segments; i++, frame++)
	{
		Com_sprintf(frame_name, sizeof(frame_name), "Book/%s", frame->name);
		mod->skins[i] = GL_FindImage(frame_name, it_pic);
	}

	// Set model type
	mod->type = mod_book;
}

#pragma region ========================== BRUSHMODEL LOADING ==========================

byte* mod_base;

// Q2 counterpart
static void Mod_LoadVertexes(const lump_t* l)
{
	dvertex_t* in = (void*)(mod_base + l->fileofs);

	if (l->filelen % sizeof(dvertex_t) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadVertexes: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dvertex_t);
	mvertex_t* out = Hunk_Alloc(count * (int)sizeof(mvertex_t));

	loadmodel->vertexes = out;
	loadmodel->numvertexes = count;

	for (int i = 0; i < count; i++, in++, out++)
	{
		out->position[0] = LittleFloat(in->point[0]);
		out->position[1] = LittleFloat(in->point[1]);
		out->position[2] = LittleFloat(in->point[2]);
	}
}

// Q2 counterpart
static void Mod_LoadEdges(const lump_t* l)
{
	dedge_t* in = (void*)(mod_base + l->fileofs);

	if (l->filelen % sizeof(dedge_t) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadEdges: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dedge_t);
	medge_t* out = Hunk_Alloc((count + 1) * (int)sizeof(medge_t));

	loadmodel->edges = out;
	loadmodel->numedges = count;

	for (int i = 0; i < count; i++, in++, out++)
	{
		out->v[0] = (ushort)LittleShort(in->v[0]);
		out->v[1] = (ushort)LittleShort(in->v[1]);
	}
}

// Q2 counterpart
static void Mod_LoadSurfedges(const lump_t* l)
{
	const int* in = (void*)(mod_base + l->fileofs);

	if (l->filelen % sizeof(int) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadSurfedges: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(int);

	if (count < 1 || count >= MAX_MAP_SURFEDGES)
		ri.Sys_Error(ERR_DROP, "Mod_LoadSurfedges: bad surfedges count in %s: %i", loadmodel->name, count);

	int* out = Hunk_Alloc(count * (int)sizeof(int));

	loadmodel->surfedges = out;
	loadmodel->numsurfedges = count;

	for (int i = 0; i < count; i++)
		out[i] = LittleLong(in[i]);
}

// Q2 counterpart
static void Mod_LoadLighting(const lump_t* l)
{
	if (l->filelen > 0)
	{
		loadmodel->lightdata = Hunk_Alloc(l->filelen);
		memcpy(loadmodel->lightdata, mod_base + l->fileofs, l->filelen);
	}
	else
	{
		loadmodel->lightdata = NULL;
	}
}

// Q2 counterpart
static void Mod_LoadPlanes(const lump_t* l)
{
	dplane_t* in = (void*)(mod_base + l->fileofs);

	if (l->filelen % sizeof(dplane_t) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadPlanes: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dplane_t);
	cplane_t* out = Hunk_Alloc(count * (int)sizeof(cplane_t) * 2);

	loadmodel->planes = out;
	loadmodel->numplanes = count;

	for (int i = 0; i < count; i++, in++, out++)
	{
		int bits = 0;

		for (int j = 0; j < 3; j++)
		{
			out->normal[j] = LittleFloat(in->normal[j]);
			if (out->normal[j] < 0)
				bits |= 1 << j;
		}

		out->dist = LittleFloat(in->dist);
		out->type = (byte)LittleLong(in->type);
		out->signbits = (byte)bits;
	}
}

static void Mod_LoadTexinfo(const lump_t* l)
{
	char name[MAX_QPATH];

	texinfo_t* in = (void*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(texinfo_t) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadTexinfo: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(texinfo_t);
	mtexinfo_t* out = Hunk_Alloc(count * (int)sizeof(mtexinfo_t));

	loadmodel->texinfo = out;
	loadmodel->numtexinfo = count;

	for (int i = 0; i < count; i++, in++, out++)
	{
		for (int j = 0; j < 8; j++)
			out->vecs[0][j] = LittleFloat(in->vecs[0][j]);

		out->flags = LittleLong(in->flags);
		const int next = LittleLong(in->nexttexinfo);

		if (next > 0)
			out->next = loadmodel->texinfo + next;
		else
			out->next = NULL;

		Com_sprintf(name, sizeof(name), "textures/%s.m8", in->texture); // H2: .wal -> .m8
		out->image = GL_FindImage(name, it_wall2); // H2: type: 2 -> 4

		if (out->image == NULL)
		{
			ri.Con_Printf(PRINT_ALL, "Couldn\'t load %s\n", name);
			out->image = r_notexture;
		}
	}

	// Count animation frames
	for (int i = 0; i < count; i++)
	{
		out = &loadmodel->texinfo[i];
		out->numframes = 1;

		for (const mtexinfo_t* step = out->next; step != NULL && step != out; step = step->next)
			out->numframes++;
	}
}

// Q2 counterpart. Fills in s->texturemins[] and s->extents[]
static void CalcSurfaceExtents(msurface_t* s)
{
	float mins[2];
	float maxs[2];
	mvertex_t* v;

	mins[0] = 999999.0f;
	mins[1] = 999999.0f;

	maxs[0] = -99999.0f;
	maxs[1] = -99999.0f;

	const mtexinfo_t* tex = s->texinfo;

	for (int i = 0; i < s->numedges; i++)
	{
		const int edge = loadmodel->surfedges[s->firstedge + i];
		if (edge >= 0)
			v = &loadmodel->vertexes[loadmodel->edges[edge].v[0]];
		else
			v = &loadmodel->vertexes[loadmodel->edges[-edge].v[1]];

		for (int j = 0; j < 2; j++)
		{
			const float val = v->position[0] * tex->vecs[j][0] + 
							  v->position[1] * tex->vecs[j][1] + 
							  v->position[2] * tex->vecs[j][2] +
							  tex->vecs[j][3];

			mins[j] = min(mins[j], val);
			maxs[j] = max(maxs[j], val);
		}
	}

	for (int i = 0; i < 2; i++)
	{
		const int bmin = (const int)floorf(mins[i] / 16);
		const int bmax = (const int)ceilf(maxs[i] / 16);

		s->texturemins[i] = (short)(bmin * 16);
		s->extents[i] = (short)((bmax - bmin) * 16);
	}
}

// Referenced by Mod_LoadFaces only:
void GL_BeginBuildingLightmaps(model_t* m);
void GL_EndBuildingLightmaps(void);
void GL_BuildPolygonFromSurface(msurface_t* fa);
void GL_CreateSurfaceLightmap(msurface_t* surf);

static void Mod_LoadFaces(const lump_t* l)
{
	dface_t* in = (void*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(dface_t) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadFaces: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dface_t);
	msurface_t* out = Hunk_Alloc(count * (int)sizeof(msurface_t));

	loadmodel->surfaces = out;
	loadmodel->numsurfaces = count;

	currentmodel = loadmodel;

	GL_BeginBuildingLightmaps(loadmodel);

	for (int surfnum = 0; surfnum < count; surfnum++, in++, out++)
	{
		out->firstedge = LittleLong(in->firstedge);
		out->numedges = LittleShort(in->numedges);
		out->flags = 0;
		out->polys = NULL;

		const int planenum = LittleShort(in->planenum);

		if (LittleShort(in->side) != 0)
			out->flags |= SURF_PLANEBACK;

		out->plane = loadmodel->planes + planenum;

		const int texinfo = LittleShort(in->texinfo);
		if (texinfo < 0 || texinfo >= loadmodel->numtexinfo)
			ri.Sys_Error(ERR_DROP, "Mod_LoadFaces: bad texinfo number");

		out->texinfo = loadmodel->texinfo + texinfo;

		CalcSurfaceExtents(out);

		// Lighting info
		for (int i = 0; i < MAXLIGHTMAPS; i++)
			out->styles[i] = in->styles[i];

		const int lightofs = LittleLong(in->lightofs);
		if (lightofs == -1)
			out->samples = NULL;
		else
			out->samples = loadmodel->lightdata + lightofs;

		// Set the drawing flags
		if (out->texinfo->flags & SURF_WARP)
		{
			out->flags |= SURF_DRAWTURB;

			if (out->texinfo->flags & SURF_UNDULATE) // New in H2
				out->flags |= SURF_UNDULATE;

			for (int i = 0; i < 2; i++)
			{
				out->extents[i] = 16384;
				out->texturemins[i] = -8192;
			}

			// Cut up polygon for warps
			GL_SubdivideSurface(out);
		}

		// Create lightmaps and polygons
		if (!(out->texinfo->flags & SURF_FULLBRIGHT)) //mxd. SURF_FULLBRIGHT define
			GL_CreateSurfaceLightmap(out);

		if (!(out->texinfo->flags & SURF_WARP))
			GL_BuildPolygonFromSurface(out);
	}

	GL_EndBuildingLightmaps();
}

// Q2 counterpart
static void Mod_LoadMarksurfaces(const lump_t* l)
{
	const short* in = (void*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(short) != 0)
		ri.Sys_Error(ERR_DROP, "Mod_LoadMarksurfaces: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(short);
	msurface_t** out = Hunk_Alloc(count * (int)sizeof(msurface_t*));

	loadmodel->marksurfaces = out;
	loadmodel->nummarksurfaces = count;

	for (int i = 0; i < count; i++)
	{
		const int j = LittleShort(in[i]);
		if (j < 0 || j >= loadmodel->numsurfaces)
			ri.Sys_Error(ERR_DROP, "Mod_ParseMarksurfaces: bad surface number");

		out[i] = loadmodel->surfaces + j;
	}
}

// Q2 counterpart
static void Mod_LoadVisibility(const lump_t* l)
{
	if (l->filelen == 0)
	{
		loadmodel->vis = NULL;
		return;
	}

	loadmodel->vis = Hunk_Alloc(l->filelen);
	memcpy(loadmodel->vis, mod_base + l->fileofs, l->filelen);

	loadmodel->vis->numclusters = LittleLong(loadmodel->vis->numclusters);
	for (int i = 0; i < loadmodel->vis->numclusters; i++)
	{
		loadmodel->vis->bitofs[i][0] = LittleLong(loadmodel->vis->bitofs[i][0]);
		loadmodel->vis->bitofs[i][1] = LittleLong(loadmodel->vis->bitofs[i][1]);
	}
}

static void Mod_LoadLeafs(const lump_t* l)
{
	dleaf_t* in = (void*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(dleaf_t))
		ri.Sys_Error(ERR_DROP, "Mod_LoadLeafs: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dleaf_t);
	mleaf_t* out = Hunk_Alloc(count * (int)sizeof(mleaf_t));

	loadmodel->leafs = out;
	loadmodel->numleafs = count;

	const float offset = ((int)gl_noartifacts->value ? 32.0f : 0.0f); // New in H2

	for (int i = 0; i < count; i++, in++, out++)
	{
		for (int j = 0; j < 3; j++)
		{
			out->minmaxs[j + 0] = LittleShort(in->mins[j]) - offset; // H2: +offset
			out->minmaxs[j + 3] = LittleShort(in->maxs[j]) + offset; // H2: +offset
		}

		out->contents = LittleLong(in->contents);
		out->cluster = LittleShort(in->cluster);
		out->area = LittleShort(in->area);
		out->firstmarksurface = loadmodel->marksurfaces + LittleShort(in->firstleafface);
		out->nummarksurfaces = LittleShort(in->numleaffaces);
	}
}

// Q2 counterpart
static void Mod_SetParent(mnode_t* node, mnode_t* parent)
{
	node->parent = parent;

	if (node->contents == -1)
	{
		Mod_SetParent(node->children[0], node);
		Mod_SetParent(node->children[1], node);
	}
}

static void Mod_LoadNodes(const lump_t* l)
{
	dnode_t* in = (void*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(dnode_t))
		ri.Sys_Error(ERR_DROP, "Mod_LoadNodes: funny lump size in %s", loadmodel->name);

	const int count = l->filelen / (int)sizeof(dnode_t);
	mnode_t* out = Hunk_Alloc(count * (int)sizeof(mnode_t));

	loadmodel->nodes = out;
	loadmodel->numnodes = count;

	const float offset = ((int)gl_noartifacts->value ? 32.0f : 0.0f); // New in H2

	for (int i = 0; i < count; i++, in++, out++)
	{
		for (int j = 0; j < 3; j++)
		{
			out->minmaxs[j + 0] = LittleShort(in->mins[j]) - offset; // H2: +offset
			out->minmaxs[j + 3] = LittleShort(in->maxs[j]) + offset; // H2: +offset
		}

		out->plane = loadmodel->planes + LittleLong(in->planenum);

		out->firstsurface = LittleShort(in->firstface);
		out->numsurfaces = LittleShort(in->numfaces);
		out->contents = -1;	// Differentiate from leafs

		for (int j = 0; j < 2; j++)
		{
			const int child_index = LittleLong(in->children[j]);
			if (child_index >= 0)
				out->children[j] = loadmodel->nodes + child_index;
			else
				out->children[j] = (mnode_t*)(loadmodel->leafs + (-1 - child_index));
		}
	}

	// Set nodes and leafs
	Mod_SetParent(loadmodel->nodes, NULL);
}

static void Mod_LoadSubmodels(lump_t* l)
{
	NOT_IMPLEMENTED
}

// Q2 counterpart
static void Mod_LoadBrushModel(model_t* mod, void* buffer)
{
	loadmodel->type = mod_brush;
	if (loadmodel != mod_known)
		ri.Sys_Error(ERR_DROP, "Loaded a brush model after the world");

	dheader_t* header = buffer;

	if (header->version != BSPVERSION)
		ri.Sys_Error(ERR_DROP, "Mod_LoadBrushModel: %s has wrong version number (%i should be %i)", mod->name, header->version, BSPVERSION);

	// Swap all the lumps
	mod_base = (byte*)header;

	for (uint i = 0; i < sizeof(dheader_t) / 4; i++)
		((int*)header)[i] = LittleLong(((int*)header)[i]);

	Mod_LoadVertexes(&header->lumps[LUMP_VERTEXES]);
	Mod_LoadEdges(&header->lumps[LUMP_EDGES]);
	Mod_LoadSurfedges(&header->lumps[LUMP_SURFEDGES]);
	Mod_LoadLighting(&header->lumps[LUMP_LIGHTING]);
	Mod_LoadPlanes(&header->lumps[LUMP_PLANES]);
	Mod_LoadTexinfo(&header->lumps[LUMP_TEXINFO]);
	Mod_LoadFaces(&header->lumps[LUMP_FACES]);
	Mod_LoadMarksurfaces(&header->lumps[LUMP_LEAFFACES]);
	Mod_LoadVisibility(&header->lumps[LUMP_VISIBILITY]);
	Mod_LoadLeafs(&header->lumps[LUMP_LEAFS]);
	Mod_LoadNodes(&header->lumps[LUMP_NODES]);
	Mod_LoadSubmodels(&header->lumps[LUMP_MODELS]);

	// Regular and alternate animation
	mod->numframes = 2; 

	// Set up the submodels
	for (int i = 0; i < mod->numsubmodels; i++)
	{
		const mmodel_t* bm = &mod->submodels[i];
		model_t* starmod = &mod_inline[i];

		*starmod = *loadmodel;

		starmod->firstmodelsurface = bm->firstface;
		starmod->nummodelsurfaces = bm->numfaces;
		starmod->firstnode = bm->headnode;

		if (starmod->firstnode >= loadmodel->numnodes)
			ri.Sys_Error(ERR_DROP, "Inline model %i has bad firstnode", i);

		VectorCopy(bm->maxs, starmod->maxs);
		VectorCopy(bm->mins, starmod->mins);
		starmod->radius = bm->radius;

		if (i == 0)
			*loadmodel = *starmod;

		mod_inline[i].numleafs = bm->visleafs;
	}
}

#pragma endregion

static void Mod_LoadSpriteModel(model_t* mod, void* buffer)
{
	NOT_IMPLEMENTED
}

static model_t* Mod_ForName(const char* name, const qboolean crash)
{
	int i;
	model_t* mod;
	
	if (!name[0])
		ri.Sys_Error(ERR_DROP, "Mod_ForName: NULL name");

	// Inline models are grabbed only from worldmodel
	if (name[0] == '*')
	{
		const int index = (int)strtol(name + 1, NULL, 10); //mxd. atoi -> strtol
		if (index < 1 || r_worldmodel == NULL || index >= r_worldmodel->numsubmodels)
			ri.Sys_Error(ERR_DROP, "bad inline model number");

		return &mod_inline[index];
	}

	// Search the currently loaded models
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++)
		if (mod->name[0] && strcmp(mod->name, name) == 0)
			return mod;

	// Find a free model slot
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++)
		if (!mod->name[0])
			break; // Free slot

	if (i == mod_numknown)
	{
		if (mod_numknown == MAX_MOD_KNOWN)
			ri.Sys_Error(ERR_DROP, "Mod_ForName: mod_numknown == MAX_MOD_KNOWN");

		mod_numknown++;
	}

	strcpy_s(mod->name, sizeof(mod->name), name); //mxd. strcpy -> strcpy_s

	// Load the file
	char* buf;
	modfilelen = ri.FS_LoadFile(mod->name, (void**)&buf);

	if (buf == NULL)
	{
		if (crash)
			ri.Sys_Error(ERR_DROP, "Mod_ForName: %s not found", mod->name);

		memset(mod, 0, sizeof(mod->name));
		return NULL;
	}

	loadmodel = mod;

	// H2: check for FlexModel header...
	if (modfilelen > 6 && Q_strncasecmp(buf, "header", 6) == 0)
	{
		mod->skeletal_model = true;

		int datasize = 0x200000;
		if (strstr(name, "players/") || strstr(name, "models/player/"))
			datasize = 0x400000;

		loadmodel->extradata = Hunk_Begin(datasize);
		Mod_LoadFlexModel(mod, buf, modfilelen);
	}
	else
	{
		mod->skeletal_model = false; // H2

		// Call the appropriate loader
		switch (LittleLong(*(uint*)buf))
		{
			// Missing: case IDALIASHEADER

			case IDSPRITEHEADER:
				loadmodel->extradata = Hunk_Begin(0x10000);
				Mod_LoadSpriteModel(mod, buf);
				break;

			case IDBOOKHEADER: // New in H2
				loadmodel->extradata = Hunk_Begin(0x10000);
				Mod_LoadBookModel(mod, buf);
				break;

			case IDBSPHEADER:
				loadmodel->extradata = Hunk_Begin(0x1000000);
				Mod_LoadBrushModel(mod, buf);
				break;

			default:
				Sys_Error("Mod_ForName: unknown file id for %s", mod->name);
				break;
		}
	}

	loadmodel->extradatasize = Hunk_End();
	ri.FS_FreeFile(buf);

	return mod;
}

void R_BeginRegistration(char* model)
{
	char fullname[MAX_QPATH];

	registration_sequence++;
	r_oldviewcluster = -1; // Force markleafs
	
	Com_sprintf(fullname, sizeof(fullname), "maps/%s.bsp", model);

	// Explicitly free the old map if different. This guarantees that mod_known[0] is the world map.
	const cvar_t* flushmap = ri.Cvar_Get("flushmap", "0", 0);
	if (strcmp(mod_known[0].name, fullname) != 0 || (int)flushmap->value)
		Mod_Free(mod_known);

	r_worldmodel = Mod_ForName(fullname, true);
	r_viewcluster = -1;

	GL_FreeUnusedImages(); // New in H2
}

struct model_s* R_RegisterModel(char* name)
{
	char img_name[MAX_OSPATH];

	model_t* mod = Mod_ForName(name, false);

	if (mod == NULL)
		return NULL;

	mod->registration_sequence = registration_sequence;

	switch (mod->type) //mxd. No mod_alias case
	{
		case mod_brush:
			for (int i = 0; i < mod->numtexinfo; i++)
				mod->texinfo[i].image->registration_sequence = registration_sequence;
			break;

		case mod_sprite:
		{
			dsprite_t* sprout = mod->extradata;
			for (int i = 0; i < sprout->numframes; i++)
			{
				Com_sprintf(img_name, sizeof(img_name), "Sprites/%s", sprout->frames[i].name); // H2: extra "Sprites/" prefix
				mod->skins[i] = GL_FindImage(img_name, it_sprite);
			}
		}
			break;

		case mod_fmdl: // H2
			Mod_RegisterFlexModel(mod);
			break;

		case mod_book: // H2
		{
			book_t* book = mod->extradata;
			bookframe_t* bframe = book->bframes;
			for (int i = 0; i < book->bheader.num_segments; i++, bframe++)
			{
				Com_sprintf(img_name, sizeof(img_name), "Book/%s", bframe->name);
				mod->skins[i] = GL_FindImage(img_name, it_pic);
			}
		}
			break;

		default:
			Sys_Error("R_RegisterModel %s failed\n", name);
			return NULL;
	}

	return mod;
}

void R_EndRegistration(void)
{
	NOT_IMPLEMENTED
}

// Q2 counterpart
void Mod_Free(model_t* mod)
{
	Hunk_Free(mod->extradata);
	memset(mod, 0, sizeof(*mod));
}

// Q2 counterpart
void Mod_FreeAll(void)
{
	for (int i = 0; i < mod_numknown; i++)
		if (mod_known[i].extradatasize)
			Mod_Free(&mod_known[i]);
}