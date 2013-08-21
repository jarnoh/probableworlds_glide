#ifndef _PROBE_INC
#define _PROBE_INC

#include <glide.h>
#include "Vector3.h"

#ifndef NOMUSIC
#include "midasdll.h"
#endif

class Probe 
{
public:
// constructors
	Probe();
	~Probe();

// methods
	void prepare();
	void randomScene();
	void run();

// properties
public:
#ifndef NOMUSIC
		MIDASmodule module;
#endif
	FxU32 fogColor;
	FxU32 framesTotal;
	FxU8 blurAm;
	float cameraRotate;
	float cameraSpeed;
	Vector3 rotSpeed;

private:
	int initok;
	int kala;
	FxU8 fadetab[256];
	DWORD timeold;

	FxI32 frames;
	FxU8 *bmp;
	int bmpsize;

	int objvertAm;
	int objfaceAm;

	Vector3 *objfn;
	Vector3 *objvn;

	GrVertex *rotvert;
	GrVertex texVerts[4];

	GrMipMapId_t texMap, envMap;
	GuTexPalette texPal, envPal, realPal;
	GrFog_t fogtable[GR_FOG_TABLE_SIZE];
	GrState defState, bgState, klunssiState;

};

#endif