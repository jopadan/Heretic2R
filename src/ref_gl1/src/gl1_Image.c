//
// gl1_Image.c
//
// Copyright 1998 Raven Software
//

#include "gl1_Image.h"
#include "gl1_Model.h"

image_t gltextures[MAX_GLTEXTURES];
int numgltextures;

#define NUM_HASHED_GLTEXTURES	256
static image_t* gltextures_hashed[NUM_HASHED_GLTEXTURES]; // H2

static byte gammatable[256];

int gl_filter_min = GL_NEAREST_MIPMAP_LINEAR; // Q2: GL_LINEAR_MIPMAP_NEAREST; H2: GL_NEAREST.
int gl_filter_max = GL_LINEAR;

typedef struct
{
	char* name;
	int	minimize;
	int maximize;
} glmode_t;

static glmode_t modes[] =
{
	{ "GL_NEAREST", GL_NEAREST, GL_NEAREST },
	{ "GL_LINEAR", GL_LINEAR, GL_LINEAR },
	{ "GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR },
	{ "GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }
};

#define NUM_GL_MODES ((int)(sizeof(modes) / sizeof(glmode_t))) //mxd. Added int cast.

void R_InitGammaTable(void) // H2: InitGammaTable()
{
	float contrast = 1.0f - vid_contrast->value;

	if (contrast > 0.5f)
		contrast = powf(contrast + 0.5f, 3.0f);
	else
		contrast = powf(contrast + 0.5f, 0.5f);

	gammatable[0] = 0;

	for (int i = 1; i < 256; i++)
	{
		float inf = 255.0f * powf(((float)i + 0.5f) / 255.5f, vid_gamma->value) + 0.5f;
		float sign;

		if (inf > 128.0f)
		{
			inf = 128.0f - inf;
			sign = -1.0f;
		}
		else
		{
			inf -= 128.0f;
			sign = 1.0f;
		}

		inf = (vid_brightness->value * 160.0f - 80.0f) + (powf(inf / 128.0f, contrast) * sign + 1.0f) * 128.0f;

		gammatable[i] = (byte)ClampI((int)inf, 0, 255);
	}
}

// Q2 counterpart
void R_TexEnv(const GLint mode) // Q2: GL_TexEnv()
{
	static GLint lastmodes[] = { -1, -1 };

	if (mode != lastmodes[gl_state.currenttmu])
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode); //mxd. Q2/H2: qglTexEnvf 
		lastmodes[gl_state.currenttmu] = mode;
	}
}

void R_BindImage(const image_t* image) // Q2: GL_BindImage()
{
	NOT_IMPLEMENTED
}

void R_TextureMode(const char* string) // Q2: GL_TextureMode()
{
	int cur_mode;

	for (cur_mode = 0; cur_mode < NUM_GL_MODES; cur_mode++)
		if (!Q_stricmp(modes[cur_mode].name, string))
			break;

	if (cur_mode == NUM_GL_MODES)
	{
		ri.Con_Printf(PRINT_ALL, "Bad texture filter name\n"); // H2: text change.
		return;
	}

	gl_filter_min = modes[cur_mode].minimize;
	gl_filter_max = modes[cur_mode].maximize;

	// Change all the existing mipmap texture objects.
	image_t* glt = &gltextures[0];
	for (int i = 0; i < numgltextures; i++, glt++)
	{
		if (glt->type != it_pic && glt->type != it_sky) // Mipmapped texture.
		{
			R_BindImage(glt); // Q2: GL_Bind(glt->texnum)

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min); // H2_1.07: GL_TEXTURE_MIN_FILTER -> 0x84fe //mxd. Q2/H2: qglTexParameterf
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max); // H2_1.07: GL_TEXTURE_MAG_FILTER -> 0x84fe //mxd. Q2/H2: qglTexParameterf
		} //TODO: add 'texture has no mipmaps' YQ2 logic?
	}
}

void R_ImageList_f(void) // Q2: GL_ImageList_f()
{
	NOT_IMPLEMENTED
}

#pragma region ========================== .M8 LOADING ==========================

static image_t* R_LoadM8(const char* name, const imagetype_t type) // H2: GL_LoadWal().
{
	NOT_IMPLEMENTED
	return NULL;
}

#pragma endregion

#pragma region ========================== .M32 LOADING ==========================

static image_t* R_LoadM32(const char* name, const imagetype_t type) // H2: GL_LoadWal32()
{
	NOT_IMPLEMENTED
	return NULL;
}

#pragma endregion

// Now with name hashing. When no texture found, returns r_notexture instead of NULL.
image_t* R_FindImage(const char* name, const imagetype_t type) // H2: GL_FindImage()
{
	if (name == NULL)
	{
		ri.Con_Printf(PRINT_ALL, "R_FindImage: Invalid null name\n"); //mxd. Com_Printf() -> ri.Con_Printf().
		return r_notexture;
	}

	const uint len = strlen(name);

	if (len < 8)
	{
		ri.Con_Printf(PRINT_ALL, "R_FindImage: Name too short (%s)\n", name); //mxd. Com_Printf() -> ri.Con_Printf().
		return r_notexture;
	}

	// Check for hashed image first.
	const byte hash = name[len - 7] + name[len - 5] * name[len - 6];
	image_t* image = gltextures_hashed[hash];

	if (image != NULL)
	{
		while (strcmp(name, image->name) != 0)
		{
			image = image->next;
			if (image == NULL)
				break;
		}

		if (image != NULL)
		{
			image->registration_sequence = registration_sequence;
			return image;
		}
	}

	// Not hashed. Load image from disk.
	if (strcmp(name + len - 3, ".m8") == 0)
		image = R_LoadM8(name, type);
	else if (strcmp(name + len - 4, ".m32") == 0)
		image = R_LoadM32(name, type);
	else
		ri.Con_Printf(PRINT_ALL, "R_FindImage: Extension not recognized in '%s'\n", name); //mxd. Com_Printf() -> ri.Con_Printf().

	if (image == NULL)
		return r_notexture;

	// Add image to hash.
	image->next = gltextures_hashed[hash];
	gltextures_hashed[hash] = image;

	return image;
}

struct image_s* R_RegisterSkin(const char* name, qboolean* retval)
{
	NOT_IMPLEMENTED
	return NULL;
}

static void R_FreeImage(image_t* image) // H2: GL_FreeImage()
{
	NOT_IMPLEMENTED
}

void R_InitImages(void) // Q2: GL_InitImages()
{
	registration_sequence = 1;
	gl_state.inverse_intensity = 1.0f;
}

void R_ShutdownImages(void) // Q2: GL_ShutdownImages()
{
	image_t* image = &gltextures[0];
	for (int i = 0; i < numgltextures; i++, image++)
		if (image->registration_sequence != 0)
			R_FreeImage(image);
}