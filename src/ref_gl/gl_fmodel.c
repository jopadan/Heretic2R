//
// fmodel.c -- Heretic 2 FlexModel loading
//
// Copyright 1998 Raven Software
//

#include "gl_local.h"
#include "gl_fmodel.h"
#include "anorms.h"
#include "anormtab.h"
#include "m_Reference.h"
#include "m_Skeleton.h"
#include "Reference.h"
#include "r_SkeletonLerp.h"
#include "r_Skeletons.h"
#include "Vector.h"

#pragma region ========================== FLEX MODEL LOADING ==========================

fmdl_t* fmodel;

static qboolean fmLoadHeader(model_t* model, const int version, const int datasize, const void* buffer)
{
	if (version != FM_HEADER_VER)
		ri.Sys_Error(ERR_DROP, "invalid HEADER version for block %s: %d != %d\n", FM_HEADER_NAME, FM_HEADER_VER, version);

	// Read header...
	memcpy(&fmodel->header, buffer, sizeof(fmheader_t));

	// Sanity checks...
	if (fmodel->header.skinheight > SKINPAGE_HEIGHT)
		ri.Sys_Error(ERR_DROP, "model %s has a skin taller than %d", model->name, SKINPAGE_HEIGHT);

	if (fmodel->header.num_xyz < 1)
		ri.Sys_Error(ERR_DROP, "model %s has no vertices", model->name);

	if (fmodel->header.num_xyz > MAX_FM_VERTS)
		ri.Sys_Error(ERR_DROP, "model %s has too many vertices", model->name);

	if (fmodel->header.num_st < 1)
		ri.Sys_Error(ERR_DROP, "model %s has no st vertices", model->name);

	if (fmodel->header.num_tris < 1)
		ri.Sys_Error(ERR_DROP, "model %s has no triangles", model->name);

	if (fmodel->header.num_frames < 1)
		ri.Sys_Error(ERR_DROP, "model %s has no frames", model->name);

	//TODO: skinwidth, MAX_FM_TRIANGLES, MAX_FM_FRAMES checks?

	VectorSet(model->mins, -32.0f, -32.0f, -32.0f);
	VectorSet(model->maxs, 32.0f, 32.0f, 32.0f);

	return true;
}

static qboolean fmLoadSkin(model_t* model, const int version, const int datasize, const void* buffer)
{
	if (version != FM_SKIN_VER)
		ri.Sys_Error(ERR_DROP, "invalid SKIN version for block %s: %d != %d\n", FM_SKIN_NAME, FM_SKIN_VER, version);

	const int skin_names_size = fmodel->header.num_skins * MAX_FRAMENAME;
	if (skin_names_size != datasize)
	{
		ri.Con_Printf(PRINT_ALL, "skin sizes do not match: %d != %d\n", datasize, skin_names_size);
		return false;
	}

	fmodel->skin_names = (char*)Hunk_Alloc(skin_names_size);
	memcpy(fmodel->skin_names, buffer, skin_names_size);

	// Precache skins...
	char* skin_name = fmodel->skin_names;
	for (int i = 0; i < fmodel->header.num_skins; i++, skin_name += MAX_FRAMENAME)
		model->skins[i] = GL_FindImage(skin_name, it_skin);

	return true;
}

static qboolean fmLoadST(model_t* model, const int version, const int datasize, const void* buffer)
{
	return true;
}

static qboolean fmLoadTris(model_t* model, const int version, const int datasize, const void* buffer)
{
	return true;
}

static qboolean fmLoadFrames(model_t* model, const int version, const int datasize, const void* buffer)
{
	if (version != FM_FRAME_VER)
		ri.Sys_Error(ERR_DROP, "invalid FRAMES version for block %s: %d != %d\n", FM_FRAME_NAME, FM_FRAME_VER, version);

	fmodel->frames = Hunk_Alloc(fmodel->header.num_frames * fmodel->header.framesize);

	for (int i = 0; i < fmodel->header.num_frames; i++)
	{
		const fmaliasframe_t* in = (const fmaliasframe_t*)((const byte*)buffer + i * fmodel->header.framesize);
		fmaliasframe_t* out = (fmaliasframe_t*)((byte*)fmodel->frames + i * fmodel->header.framesize);

		VectorCopy(in->scale, out->scale);
		VectorCopy(in->translate, out->translate);

		memcpy(out->name, in->name, sizeof(out->name));
		memcpy(out->verts, in->verts, fmodel->header.num_xyz * sizeof(fmtrivertx_t));
	}

	return true;
}

static qboolean fmLoadGLCmds(model_t* model, const int version, const int datasize, const void* buffer)
{
	if (version != FM_GLCMDS_VER)
		ri.Sys_Error(ERR_DROP, "invalid GLCMDS version for block %s: %d != %d\n", FM_GLCMDS_NAME, FM_GLCMDS_VER, version);

	const uint size = fmodel->header.num_glcmds * sizeof(int);
	fmodel->glcmds = Hunk_Alloc((int)size);
	memcpy(fmodel->glcmds, buffer, size);

	return true;
}

static qboolean fmLoadMeshNodes(model_t* model, const int version, const int datasize, const void* buffer)
{
	int i;
	const fmmeshnode_t* in;
	fmmeshnode_t* out;
	short tri_indices[MAX_FM_TRIANGLES];
	
	if (version != FM_MESH_VER)
		ri.Sys_Error(ERR_DROP, "invalid MESH version for block %s: %d != %d\n", FM_MESH_NAME, FM_MESH_VER, version);

	if (fmodel->header.num_mesh_nodes < 1)
		return true;

	//TODO: check for fmodel->header.num_tris >= MAX_FM_TRIANGLES?

	fmodel->mesh_nodes = Hunk_Alloc(fmodel->header.num_mesh_nodes * (int)sizeof(fmmeshnode_t));

	for (i = 0, in = buffer, out = fmodel->mesh_nodes; i < fmodel->header.num_mesh_nodes; i++, in++, out++)
	{
		// Copy tris
		int num_tris = 0;
		for (int tri_index = 0; tri_index < fmodel->header.num_tris; tri_index++)
			if ((1 << (tri_index & 7)) != 0 && in->tris[tri_index >> 3] != 0)
				tri_indices[num_tris++] = (short)tri_index;

		out->num_tris = num_tris;
		out->triIndicies = Hunk_Alloc(num_tris * (int)sizeof(short));
		memcpy(out->triIndicies, tri_indices, num_tris);

		// Copy verts
		memcpy(out->verts, in->verts, sizeof(out->verts));

		// Copy glcmds
		out->start_glcmds = in->start_glcmds;
		out->num_glcmds = in->num_glcmds;
	}

	return true;
}

static qboolean fmLoadShortFrames(model_t* model, const int version, const int datasize, const void* buffer)
{
	NOT_IMPLEMENTED
	return false;
}

static qboolean fmLoadNormal(model_t* model, const int version, const int datasize, const void* buffer)
{
	NOT_IMPLEMENTED
	return false;
}

static qboolean fmLoadComp(model_t* model, const int version, const int datasize, const void* buffer)
{
	NOT_IMPLEMENTED
	return false;
}

static qboolean fmLoadSkeleton(model_t* model, const int version, const int datasize, const void* buffer)
{
	if (version != FM_SKELETON_VER)
	{
		ri.Con_Printf(PRINT_ALL, "invalid SKELETON version for block %s: %d != %d\n", FM_SKELETON_NAME, FM_SKELETON_VER, version);
		return false;
	}

	const int* in_i = buffer;

	fmodel->skeletalType = *in_i;
	fmodel->rootCluster = CreateSkeleton(fmodel->skeletalType);

	const int num_clusters = *(++in_i);

	// Count and allocate verts...
	int num_verts = 0;
	for (int cluster = num_clusters - 1; cluster > -1; cluster--)
	{
		num_verts += *(++in_i);

		const int cluster_index = fmodel->rootCluster + cluster;
		SkeletalClusters[cluster_index].numVerticies = num_verts;
		SkeletalClusters[cluster_index].verticies = Hunk_Alloc(num_verts * (int)sizeof(int));
	}

	int start_vert_index = 0;
	for (int cluster = num_clusters - 1; cluster > -1; cluster--)
	{
		for (int v = start_vert_index; v < SkeletalClusters[fmodel->rootCluster + cluster].numVerticies; v++)
		{
			const int vert_index = *(++in_i);
			for (int c = 0; c <= cluster; c++)
				SkeletalClusters[fmodel->rootCluster + c].verticies[v] = vert_index;
		}

		start_vert_index = SkeletalClusters[fmodel->rootCluster + cluster].numVerticies;
	}

	// Check for duplicates...
	for (int i = 0; i < num_clusters; i++)
	{
		const int c = fmodel->rootCluster + i;
		for (int v1 = 0; v1 < SkeletalClusters[c].numVerticies - 1; v1++)
			for (int v2 = v1 + 1; v2 < SkeletalClusters[c].numVerticies; v2++)
				if (SkeletalClusters[c].verticies[v1] == SkeletalClusters[c].verticies[v2])
					Com_Printf("Warning duplicate vertex:  %d\n", SkeletalClusters[c].verticies[v1]);
	}

	const qboolean have_skeleton = *(++in_i);

	// Create skeleton
	if (have_skeleton)
	{
		const float* in_f = (const float*)in_i;

		fmodel->skeletons = Hunk_Alloc(fmodel->header.num_frames * (int)sizeof(ModelSkeleton_t));

		for (int i = 0; i < fmodel->header.num_frames; i++)
		{
			CreateSkeletonAsHunk(fmodel->skeletalType, fmodel->skeletons + i);

			for (int c = 0; c < num_clusters; c++)
			{
				fmodel->skeletons[i].rootJoint[c].model.origin[0] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.origin[1] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.origin[2] = *(++in_f);

				fmodel->skeletons[i].rootJoint[c].model.direction[0] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.direction[1] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.direction[2] = *(++in_f);

				fmodel->skeletons[i].rootJoint[c].model.up[0] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.up[1] = *(++in_f);
				fmodel->skeletons[i].rootJoint[c].model.up[2] = *(++in_f);

				VectorCopy(fmodel->skeletons[i].rootJoint[c].model.origin, fmodel->skeletons[i].rootJoint[c].parent.origin);
				VectorCopy(fmodel->skeletons[i].rootJoint[c].model.direction, fmodel->skeletons[i].rootJoint[c].parent.direction);
				VectorCopy(fmodel->skeletons[i].rootJoint[c].model.up, fmodel->skeletons[i].rootJoint[c].parent.up);
			}
		}
	}
	else
	{
		fmodel->header.num_xyz -= num_clusters * 3;
	}

	return true;
}

static qboolean fmLoadReferences(model_t* model, const int version, const int datasize, const void* buffer)
{
	//mxd. Helper data type...
	typedef struct
	{
		int referenceType;
		qboolean haveRefs;
		M_Reference_t* refsForFrame;
	} dmreferences_t;

	if (version != FM_REFERENCES_VER)
	{
		ri.Con_Printf(PRINT_ALL, "invalid REFERENCES version for block %s: %d != %d\n", FM_REFERENCES_NAME, FM_REFERENCES_VER, version);
		return false;
	}

	const dmreferences_t* in = buffer;
	fmodel->referenceType = in->referenceType;

	if (!in->haveRefs)
	{
		fmodel->header.num_xyz -= numReferences[fmodel->referenceType] * 3;
		return true;
	}

	const int num_refs = numReferences[fmodel->referenceType];
	fmodel->refsForFrame = Hunk_Alloc(fmodel->header.num_frames * num_refs * (int)sizeof(M_Reference_t));

	if (fmodel->header.num_frames < 1)
		return true;

	const M_Reference_t* ref_in = (const M_Reference_t*)&in->refsForFrame;
	M_Reference_t* ref_out = fmodel->refsForFrame;

	for (int i = 0; i < fmodel->header.num_frames; i++)
	{
		for (int j = 0; j < num_refs; j++, ref_in++, ref_out++)
		{
			for (int c = 0; c < 3; c++)
			{
				//TODO: done this way to perform byte-swapping (otherwise we can just memcpy the whole thing)?
				ref_out->placement.origin[c] = ref_in->placement.origin[c];
				ref_out->placement.direction[c] = ref_in->placement.direction[c];
				ref_out->placement.up[c] = ref_in->placement.up[c];
			}
		}
	}

	return true;
}

// FlexModel block loaders
typedef struct
{
	char ident[FMDL_BLOCK_IDENT_SIZE];
	qboolean(*load)(model_t* model, int version, int datasize, const void* buffer);
} fmdl_loader_t;

fmdl_loader_t fmblocks[] =
{
	{ FM_HEADER_NAME,		fmLoadHeader },
	{ FM_SKIN_NAME,			fmLoadSkin },
	{ FM_ST_NAME,			fmLoadST },
	{ FM_TRI_NAME,			fmLoadTris },
	{ FM_FRAME_NAME,		fmLoadFrames },
	{ FM_GLCMDS_NAME,		fmLoadGLCmds },
	{ FM_MESH_NAME,			fmLoadMeshNodes },
	{ FM_SHORT_FRAME_NAME,	fmLoadShortFrames },
	{ FM_NORMAL_NAME,		fmLoadNormal },
	{ FM_COMP_NAME,			fmLoadComp },
	{ FM_SKELETON_NAME,		fmLoadSkeleton },
	{ FM_REFERENCES_NAME,	fmLoadReferences },
	{ "",					NULL },
};

void Mod_LoadFlexModel(model_t* mod, void* buffer, int length)
{
	mod->type = mod_fmdl;

	fmodel = Hunk_Alloc(sizeof(fmdl_t));
	fmodel->skeletalType = -1;
	fmodel->referenceType = -1;

	byte* in = buffer;

	while (length > 0)
	{
		fmdl_blockheader_t* header = (fmdl_blockheader_t*)in;
		in += sizeof(fmdl_blockheader_t); // Block data is stored after block header.

		// Find appropriate loader...
		fmdl_loader_t* loader;
		for (loader = fmblocks; loader->ident[0] != 0; loader++)
		{
			if (Q_stricmp(loader->ident, header->ident) == 0)
			{
				if (!loader->load(mod, header->version, header->size, in)) //mxd. Added sanity check
					Com_Error(ERR_DROP,"Mod_LoadFlexModel: failed to load block %s\n", header->ident);

				break;
			}
		}

		if (loader->ident[0] == 0)
			ri.Con_Printf(PRINT_ALL, "Mod_LoadFlexModel: unknown block %s\n", header->ident);

		in += header->size; // Skip to next block header...
		length -= (header->size + (int)sizeof(fmdl_blockheader_t));
	}
}

void Mod_RegisterFlexModel(model_t* mod)
{
	fmodel = (fmdl_t*)mod->extradata;

	// Precache skins... //TODO: also done in fmLoadSkin(). One of these isn't needed?
	char* skin_name = fmodel->skin_names;
	for (int i = 0; i < fmodel->header.num_skins; i++, skin_name += MAX_QPATH)
		mod->skins[i] = GL_FindImage(skin_name, it_skin);
}

#pragma endregion

#pragma region ========================== FLEX MODEL RENDERING ==========================

static vec3_t shadelight;
static vec3_t shadevector;

//mxd. Somewhat similar to R_CullAliasModel from Q2
static qboolean R_CullFlexModel(const fmdl_t* model, entity_t* e)
{
	vec3_t mins;
	vec3_t maxs;

	if (e->frame < 0 || e->frame >= model->header.num_frames)
		e->frame = 0;

	if (e->oldframe < 0 || e->oldframe >= model->header.num_frames)
		e->oldframe = 0;

	// Compute axially aligned mins and maxs
	if (model->frames == NULL)
	{
		for (int i = 0; i < 3; i++)
		{
			mins[i] = model->compdata[model->frame_to_group[e->frame]].bmin[i];
			maxs[i] = model->compdata[model->frame_to_group[e->frame]].bmax[i];
		}
	}
	else
	{
		fmaliasframe_t* pframe = (fmaliasframe_t*)((byte*)model->frames + e->frame * model->header.framesize);
		fmaliasframe_t* poldframe = (fmaliasframe_t*)((byte*)model->frames + e->oldframe * model->header.framesize);

		if (pframe == poldframe)
		{
			for (int i = 0; i < 3; i++)
			{
				mins[i] = pframe->translate[i];
				maxs[i] = mins[i] + pframe->scale[i] * 255;
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				const float thismins = pframe->translate[i];
				const float thismaxs = thismins + pframe->scale[i] * 255;

				const float oldmins = poldframe->translate[i];
				const float oldmaxs = oldmins + poldframe->scale[i] * 255;

				mins[i] = min(thismins, oldmins);
				maxs[i] = max(thismaxs, oldmaxs);
			}
		}
	}

	// Apply model scale
	if (e->cl_scale != 0.0f && e->cl_scale != 1.0f)
	{
		VectorScale(mins, e->cl_scale, mins);
		VectorScale(maxs, e->cl_scale, maxs);
	}

	// Compute a full bounding box
	vec3_t bbox[8];
	for (int i = 0; i < 8; i++)
	{
		bbox[i][0] = (i & 1 ? mins[0] : maxs[0]);
		bbox[i][1] = (i & 2 ? mins[1] : maxs[1]);
		bbox[i][2] = (i & 4 ? mins[2] : maxs[2]);
	}

	// Rotate the bounding box
	const vec3_t angles =
	{
		e->angles[0] * RAD_TO_ANGLE,
		e->angles[1] * RAD_TO_ANGLE * -1.0f,
		e->angles[2] * RAD_TO_ANGLE,
	};

	vec3_t vectors[3];
	AngleVectors(angles, vectors[0], vectors[1], vectors[2]);

	for (int i = 0; i < 8; i++)
	{
		vec3_t tmp;
		VectorCopy(bbox[i], tmp);

		bbox[i][0] = DotProduct(vectors[0], tmp);
		bbox[i][1] = -DotProduct(vectors[1], tmp);
		bbox[i][2] = DotProduct(vectors[2], tmp);

		VectorAdd(e->origin, bbox[i], bbox[i]);
	}

	int aggregatemask = -1;

	for (int p = 0; p < 8; p++)
	{
		int mask = 0;

		for (int f = 0; f < 4; f++)
		{
			const float dp = DotProduct(frustum[f].normal, bbox[p]);
			if (dp - frustum[f].dist < 0.0f)
				mask |= 1 << f;
		}

		aggregatemask &= mask;
	}

	return aggregatemask != 0;
}

//TODO: rewrite to use entity_t* arg instead of 'currententity'
static image_t* GetSkin(void)
{
	if (currententity->skin != NULL)
		return currententity->skin;

	const int skinnum = (currententity->skinnum < MAX_FRAMES ? currententity->skinnum : 0);
	if (currentmodel->skins[skinnum] != NULL)
		return currentmodel->skins[skinnum];

	if (currentmodel->skins[0] != NULL)
		return currentmodel->skins[0];

	return r_notexture;
}

static image_t* GetSkinFromNode(const int index)
{
	image_t* skin;

	if (currententity->skin != NULL && currententity->skins != NULL)
		skin = currententity->skins[currententity->fmnodeinfo[index].skin];
	else
		skin = currentmodel->skins[currententity->fmnodeinfo[index].skin];

	if (skin != NULL)
		return skin;

	if (currententity->skin != NULL)
		return currententity->skin;

	if (currentmodel->skins[0] != NULL)
		return currentmodel->skins[0];

	return r_notexture;
}

static void InterpolateVertexNormals(const int num_xyz, const float lerp_inv, const float lerp, const fmtrivertx_t* verts, const fmtrivertx_t* old_verts, vec3_t* normals)
{
	int i;
	const fmtrivertx_t* v;
	const fmtrivertx_t* ov;
	vec3_t* n;

	for (i = 0, v = verts, ov = old_verts, n = normals; i < num_xyz; i++, v++, ov++, n++)
		for (int j = 0; j < 3; j++)
			(*n)[j] = lerp * bytedirs[ov->lightnormalindex][j] + lerp_inv * bytedirs[v->lightnormalindex][j];
}

static void GL_DrawFlexFrameLerp(void)
{
	int i;
	fmnodeinfo_t* nodeinfo;
	static vec3_t normals_array[MAX_FM_VERTS]; //mxd. Made static

	const qboolean draw_reflection = (currententity->flags & RF_REFLECTION); //mxd. Skipped gl_envmap_broken check
	const image_t* skin = GetSkin();
	float alpha = 1.0f; //mxd. Set in Loki Linux version, but not in Windows version.

	if (currententity->color.a != 255 || currententity->flags & RF_TRANS_ANY || skin->has_alpha)
	{
		if (currententity->flags & RF_TRANS_GHOST)
			alpha = shadelight[0] * 0.5f;
		else
			alpha = (float)currententity->color.a / 255.0f;

		HandleTrans(currententity);
	}

	if (!(int)r_frameswap->value)
		currententity->swapFrame = -1;

	FrameLerp();

	if (draw_reflection)
	{
		if (fmodel->frames != NULL)
			InterpolateVertexNormals(fmdl_num_xyz, framelerp_inv, framelerp, sfl_cur_skel.verts, sfl_cur_skel.old_verts, normals_array);

		qglEnable(GL_TEXTURE_GEN_S);
		qglEnable(GL_TEXTURE_GEN_T);
		qglTexGeni(MEM_RESERVE, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		qglTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		GL_BindImage(r_reflecttexture);
	}

	for (i = 0, nodeinfo = currententity->fmnodeinfo; i < fmodel->header.num_mesh_nodes; i++, nodeinfo++)
	{
		qboolean use_color = false;
		qboolean use_skin = false;
		qboolean use_reflect = false;

		if (nodeinfo != NULL)
		{
			if (nodeinfo->flags & FMNI_NO_DRAW)
				continue;

			use_color = (nodeinfo->flags & FMNI_USE_COLOR);
			if (use_color)
			{
				qglEnable(GL_BLEND);
				qglColor4ub(nodeinfo->color.r, nodeinfo->color.g, nodeinfo->color.b, nodeinfo->color.a);
			}

			if (draw_reflection || !(nodeinfo->flags & FMNI_USE_REFLECT))
			{
				if (nodeinfo->flags & FMNI_USE_SKIN)
				{
					use_skin = true;
					GL_BindImage(GetSkinFromNode(i));
				}
			}
			else
			{
				qglEnable(GL_TEXTURE_GEN_S);
				qglEnable(GL_TEXTURE_GEN_T);
				qglTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				qglTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

				use_skin = true;
				use_reflect = true;
				GL_BindImage(r_reflecttexture);
			}
		}

		int* order = fmodel->glcmds + fmodel->mesh_nodes[i].start_glcmds;

		while (true)
		{
			// Get the vertex count and primitive type
			int count = *order++;
			if (count == 0)
				break; // Done

			if (count < 0)
			{
				count = -count;
				qglBegin(GL_TRIANGLE_FAN);
			}
			else
			{
				qglBegin(GL_TRIANGLE_STRIP);
			}

			do
			{
				const int index_xyz = order[2];

				if (draw_reflection || use_reflect)
				{
					vec3_t* normal;
					if (fmodel->frames == NULL)
						normal = &bytedirs[fmodel->lightnormalindex[index_xyz]];
					else if (draw_reflection)
						normal = &normals_array[index_xyz];
					else
						normal = &bytedirs[sfl_cur_skel.verts[index_xyz].lightnormalindex];

					qglNormal3f((*normal)[0], (*normal)[1], (*normal)[2]);
				}
				else
				{
					// Texture coordinates come from the draw list
					qglTexCoord2f(((float*)order)[0], ((float*)order)[1]);
				}

				order += 3;

				if (!use_color && !(currententity->flags & RF_FULLBRIGHT))
				{
					float l;
					if (fmodel->frames == NULL)
						l = shadedots[fmodel->lightnormalindex[index_xyz]];
					else
						l = shadedots[sfl_cur_skel.verts[index_xyz].lightnormalindex];

					qglColor4f(l * shadelight[0], l * shadelight[1], l * shadelight[2], alpha);
				}

				qglVertex3fv(s_lerped[index_xyz]);
			} while (--count);

			qglEnd();
		}

		if (use_reflect)
		{
			qglDisable(GL_TEXTURE_GEN_S);
			qglDisable(GL_TEXTURE_GEN_T);
		}

		if (use_skin)
			GL_BindImage(skin);
	}

	if (draw_reflection)
	{
		qglDisable(GL_TEXTURE_GEN_S);
		qglDisable(GL_TEXTURE_GEN_T);
		GL_BindImage(skin);
	}

	CleanupTrans(currententity);
}

//mxd. Somewhat similar to R_DrawAliasModel from Q2. Original code used 'currententity' global var instead of 'e' arg.
void R_DrawFlexModel(entity_t* e)
{
	fmodel = (fmdl_t*)(*e->model)->extradata; //mxd. Original code used 'currentmodel' global var here
	
	if (R_CullFlexModel(fmodel, e))
		return;

	// Get lighting information
	if (e->flags & RF_TRANS_ADD_ALPHA)
	{
		const float alpha = (float)e->color.a / 255.0f;
		VectorSet(shadelight, alpha, alpha, alpha);
	}
	else if (e->flags & RF_FULLBRIGHT)
	{
		VectorSet(shadelight, 1.0f, 1.0f, 1.0f);
	}
	else if (e->absLight.r != 0 || e->absLight.g != 0 || e->absLight.b != 0)
	{
		VectorSet(shadelight, (float)e->absLight.r / 255.0f, (float)e->absLight.g / 255.0f, (float)e->absLight.b / 255.0f);
	}
	else
	{
		R_LightPoint(e->origin, shadelight);

		// Player lighting hack for communication back to server. Big hack!
		if (e->flags & RF_WEAPONMODEL)
		{
			// Pick the greatest component, which should be the same as the mono value returned by software
			r_lightlevel->value = max(shadelight[0], max(shadelight[1], shadelight[2])) * 150.0f;
		}

		//mxd. Skip gl_monolightmap logic
	}

	shadelight[0] *= (float)e->color.r / 255.0f;
	shadelight[1] *= (float)e->color.g / 255.0f;
	shadelight[2] *= (float)e->color.b / 255.0f;

	if (e->flags & RF_MINLIGHT)
	{
		int c;
		for (c = 0; c < 3; c++)
			if (shadelight[c] > 0.1f)
				break;

		if (c == 3)
			VectorSet(shadelight, 0.1f, 0.1f, 0.1f);
	}

	if (e->flags & RF_GLOW)
	{
		// Bonus items will pulse with time
		const float val = sinf(r_newrefdef.time * 7.0f) * 0.3f + 0.7f;
		VectorSet(shadelight, val, val, val);
	}

	shadedots = r_avertexnormal_dots[((int)(e->angles[1] * (SHADEDOT_QUANT / 360.0f * RAD_TO_ANGLE)) & (SHADEDOT_QUANT - 1))];

	VectorSet(shadevector, cosf(-e->angles[1]), sinf(-e->angles[1]), 1.0f);
	VectorNormalize(shadevector);

	// Locate the proper data
	c_alias_polys += fmodel->header.num_tris;

	// Draw all the triangles
	if (e->flags & RF_DEPTHHACK) // Hack the depth range to prevent view model from poking into walls
		qglDepthRange(gldepthmin, (gldepthmax - gldepthmin) * 0.3f + gldepthmin);

	qglPushMatrix();
	R_RotateForEntity(e);

	// Select skin
	GL_BindImage(GetSkin());

	// Draw it
	qglShadeModel(GL_SMOOTH);
	GL_TexEnv(GL_MODULATE);

	if (e->frame < 0 || e->frame >= fmodel->header.num_frames)
	{
		e->frame = 0;
		e->oldframe = 0;
	}

	if (e->oldframe < 0 || e->oldframe >= fmodel->header.num_frames)
	{
		ri.Con_Printf(PRINT_ALL, "R_DrawFlexModel: no such oldframe %d\n");
		e->frame = 0;
		e->oldframe = 0;
	}

	if (!(int)r_lerpmodels->value)
		e->backlerp = 0.0f;

	framelerp = e->backlerp;

	GL_DrawFlexFrameLerp();

	GL_TexEnv(GL_REPLACE);
	qglShadeModel(GL_FLAT);
	qglPopMatrix();

	if (e->flags & RF_TRANS_ANY)
		qglDisable(GL_BLEND);

	if (e->flags & RF_DEPTHHACK)
		qglDepthRange(gldepthmin, gldepthmax);

	qglColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void GL_LerpVert(vec3_t newPoint, vec3_t oldPoint, vec3_t interpolatedPoint, float move[3], float frontv[3], float backv[3])
{
	for (int i = 0; i < 3; i++)
		interpolatedPoint[i] = newPoint[i] * frontv[i] + oldPoint[i] * backv[i] + move[i];
}

#pragma endregion