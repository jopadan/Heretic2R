//
// gl_light.c
//
// Copyright 1998 Raven Software
//

#include "gl_local.h"

static int r_dlightframecount; //mxd. Made static
static float s_blocklights[34 * 34 * 3];

#pragma region ========================== DYNAMIC LIGHTS RENDERING ==========================

static void R_RenderDlight(dlight_t* light)
{
	NOT_IMPLEMENTED
}

void R_RenderDlights(void)
{
	int i;
	dlight_t* l;

	if ((int)gl_flashblend->value) //TODO: H2 check is opposite to Q2 check! A bug?
		return;

	r_dlightframecount = r_framecount + 1; // Because the count hasn't advanced yet for this frame

	qglDepthMask(0);
	qglDisable(GL_TEXTURE_2D);
	qglShadeModel(GL_SMOOTH);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);

	for (i = 0, l = r_newrefdef.dlights; i < r_newrefdef.num_dlights; i++, l++)
		R_RenderDlight(l);

	qglColor3f(1.0f, 1.0f, 1.0f);
	qglDisable(GL_BLEND);
	qglEnable(GL_TEXTURE_2D);
	qglBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); // Q2: GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
	qglDepthMask(1);
}

#pragma endregion

#pragma region ========================== DYNAMIC LIGHTS ==========================

void R_MarkLights(dlight_t* light, int bit, mnode_t* node)
{
	NOT_IMPLEMENTED
}

// Q2 counterpart
void R_PushDlights(void)
{
	int i;
	dlight_t* l;

	if (!(int)gl_flashblend->value)
	{
		r_dlightframecount = r_framecount + 1; // Because the count hasn't advanced yet for this frame

		for (i = 0, l = r_newrefdef.dlights; i < r_newrefdef.num_dlights; i++, l++)
			R_MarkLights(l, 1 << i, r_worldmodel->nodes);
	}
}

#pragma endregion

void R_LightPoint(vec3_t p, vec3_t color)
{
	NOT_IMPLEMENTED
}

static void R_AddDynamicLights(msurface_t* surf)
{
	NOT_IMPLEMENTED
}

// Q2 counterpart
void R_SetCacheState(msurface_t* surf)
{
	for (int maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255; maps++)
		surf->cached_light[maps] = r_newrefdef.lightstyles[surf->styles[maps]].white;
}

// Q2 counterpart (in H2, except for extra SURF_TALL_WALL flag)
void R_BuildLightMap(msurface_t* surf, byte* dest, int stride)
{
	if (surf->texinfo->flags & SURF_FULLBRIGHT) //mxd. SURF_FULLBRIGHT define
		ri.Sys_Error(ERR_DROP, "R_BuildLightMap called for non-lit surface");

	const int smax = (surf->extents[0] >> 4) + 1;
	const int tmax = (surf->extents[1] >> 4) + 1;
	const int size = smax * tmax;

	if (size > (int)sizeof(s_blocklights) >> 4)
		ri.Sys_Error(ERR_DROP, "Bad s_blocklights size");

	// Set to full bright if no light data
	if (surf->samples == NULL)
	{
		for (int i = 0; i < size * 3; i++)
			s_blocklights[i] = 255;
	}
	else
	{
		//mxd. Don't count the number of maps
		const byte* lightmap = surf->samples;

		// Add all the lightmaps //mxd. Skip nummaps == 1 logic
		memset(s_blocklights, 0, sizeof(s_blocklights[0]) * size * 3);

		for (int maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255; maps++)
		{
			float scale[3];
			for (int i = 0; i < 3; i++)
				scale[i] = gl_modulate->value * r_newrefdef.lightstyles[surf->styles[maps]].rgb[i];

			//mxd. Skip scale == 1.0 logic
			float* bl = s_blocklights;
			for (int i = 0; i < size; i++, bl += 3)
				for (int j = 0; j < 3; j++)
					bl[j] += (float)lightmap[i * 3 + j] * scale[j];

			// Skip to next lightmap
			lightmap += size * 3;
		}

		// Add all the dynamic lights
		if (surf->dlightframe == r_framecount)
			R_AddDynamicLights(surf);
	}

	// Put into texture format
	stride -= (smax << 2);
	const float* bl = s_blocklights;

	//mxd. Skip gl_monolightmap logic.
	for (int i = 0; i < tmax; i++, dest += stride)
	{
		for (int j = 0; j < smax; j++, bl += 3, dest += 4)
		{
			int r = Q_ftol(bl[0]);
			int g = Q_ftol(bl[1]);
			int b = Q_ftol(bl[2]);

			// Catch negative lights
			r = max(r, 0);
			g = max(g, 0);
			b = max(b, 0);

			// Determine the brightest of the three color components
			const int max = max(r, max(g, b));

			// Alpha is ONLY used for the mono lightmap case.
			// For this reason we set it to the brightest of the color components so that things don't get too dim.
			int a = max;

			// Rescale all the color components if the intensity of the greatest channel exceeds 1.0
			if (max > 255)
			{
				const float t = 255.0f / (float)max;

				r = (int)((float)r * t);
				g = (int)((float)g * t);
				b = (int)((float)b * t);
				a = (int)((float)a * t);
			}

			dest[0] = (byte)r;
			dest[1] = (byte)g;
			dest[2] = (byte)b;
			dest[3] = (byte)a;
		}
	}
}