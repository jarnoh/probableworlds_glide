/****************************************************************************

Probable Worlds

Copyright 1997 (c) Complex

Design and Programming by Jarno Heikkinen (jarnoh@yomimedia.fi)

****************************************************************************/


#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

// uncomment this to disable MIDAS music
//#define NOMUSIC

#include "saver.h"
#include "test.h"

#pragma warning( once : 4305 4244 )
#include "klunssi.h"

extern TCHAR szConfig[]; //=_T("Config");

// quick macro to scale 32bit ABGR color (uses fadetab scaling table)
#define FADE(q) (fadetab[q&0xff]|(fadetab[(q&0xff00)>>8]<<8)|(fadetab[(q&0xff0000)>>16]<<16))

// different fog colors
FxU32 fogColors[]={ 
	0xc0c0c0c0, 
	0x00000000, 
	0x00302000, 
	0x00307070, 
	0x00804020, 
	0x001050e0, 
	0x00405050, 
	0x00c01060, 
	0x0000d0d0,
	0x00ff0040,
	0x000040ff 
};


/****************************************************************************/

Probe::Probe() 
{
	GrHwConfiguration hwconfig;

#ifndef NOMUSIC
	MIDASstartup();
#endif

    // Initialize Glide 
    grGlideInit();
    grSstQueryHardware( &hwconfig );
    grSstSelect( 0 );

	frames=-1;
	kala=0;
	initok=0;
}

/****************************************************************************/
void Probe::prepare()
{
    int i;
	float asdf=0.;

	// load texture data from resources
	// 666 is our resource ID
	HRSRC hRes;
	hRes = FindResource(NULL, "#666", "texture");

	if(hRes)
	{
		bmpsize=SizeofResource(NULL, hRes);
		bmp=new FxU8[bmpsize];
		memcpy(bmp, LockResource(LoadResource(NULL, hRes)), bmpsize);
	}
	else 
	{
		bmp=NULL;
		bmpsize=0;
//		MessageBox(NULL, "perkele", "", MB_OK);
	}

//	FreeResource(hRes); 

	srand( GetTickCount() );

	// scale the object and calculate normals...
	objvertAm=sizeof(objvert)/sizeof(objvert[0])/3;
	objfaceAm=sizeof(objface)/sizeof(objface[0])/3;

	rotvert=new GrVertex[objvertAm];
    objvn=new Vector3[objvertAm];

	memset(rotvert, 0, sizeof(GrVertex)*objvertAm);
	memset(objvn, 0, sizeof(Vector3)*objvertAm);

	for(i=0;i<objvertAm*3;i++) if(objvert[i] > asdf) asdf=objvert[i];
	for(i=0;i<objvertAm*3;i++) objvert[i]=objvert[i]*(1.f/asdf);

    objfn=new Vector3[objfaceAm];
    for(i=0; i<objfaceAm; i++)
    {
        Vector3 tn;
        Vector3 a=Vector3(&objvert[3*objface[i*3]]);
        Vector3 b=Vector3(&objvert[3*objface[i*3+1]]);
        Vector3 c=Vector3(&objvert[3*objface[i*3+2]]);
        tn.x=  (b.y-a.y)*(c.z-a.z)-(c.y-a.y)*(b.z-a.z);
        tn.y=-((b.x-a.x)*(c.z-a.z)-(c.x-a.x)*(b.z-a.z));
        tn.z=  (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
        objfn[i]=tn.normalize();
    }

    for(i=0; i<objfaceAm*3; i++) objvn[objface[i]] += objfn[i/3];
    for(i=0; i<objvertAm; i++) objvn[i]=objvn[i].normalize();

	// then some glide preparations... i dont have a clue about supporting rush
	// by providing a hwnd to glide, tried it but it didnt work...
//	if(!grSstWinOpen(0, GR_RESOLUTION_640x480, GR_REFRESH_60Hz,
	if(!grSstWinOpen( (unsigned long)AfxGetMainWnd()->m_hWnd, GR_RESOLUTION_640x480, GR_REFRESH_60Hz,
		GR_COLORFORMAT_ABGR, GR_ORIGIN_UPPER_LEFT, GR_SMOOTHING_ENABLE, 1 ))
	{
		MessageBox(NULL, "failed", "", MB_OK);
		return;
	};

	float sw=(float)grSstScreenWidth();
	float sh=(float)grSstScreenHeight();

	texVerts[0].x = 0.f; texVerts[0].y = sh;
	texVerts[1].x = sw; texVerts[1].y = sh;
	texVerts[2].x = sw; texVerts[2].y = 0.f; 
	texVerts[3].x = 0.f; texVerts[3].y = 0.f;
	texVerts[0].oow=texVerts[1].oow=0.29f;
	texVerts[2].oow=texVerts[3].oow=0.29f/24;

	// allocate space for texture (3dfx)
	envMap = guTexAllocateMemory( 0, GR_MIPMAPLEVELMASK_BOTH, 256, 256,
		GR_TEXFMT_P_8, GR_MIPMAP_NEAREST, GR_LOD_256, GR_LOD_256,
		GR_ASPECT_1x1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR, 0.0F, FXFALSE );

	texMap = guTexAllocateMemory( 0, GR_MIPMAPLEVELMASK_BOTH, 128, 128,
		GR_TEXFMT_P_8, GR_MIPMAP_NEAREST, GR_LOD_128, GR_LOD_128,
		GR_ASPECT_1x1, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR, 0.0F, FXFALSE );

	// first texture is the envmap...
	for(i=0;i<256;i++) envPal.data[i]=(bmp[i*3+32]<<16)+(bmp[i*3+1+32]<<8)+(bmp[i*3+2+32])&0xffffff;
	guTexDownloadMipMap( envMap, bmp+800, NULL );

	memset(&texPal, 0, sizeof(texPal));

    for( i = 0; i < objvertAm; i++ ) rotvert[i].oow=1.f;

 	grDepthMask(1);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);

//	grDitherMode(GR_DITHER_DISABLE);
 	grCullMode(GR_CULL_POSITIVE);

	// save default rendering state
    grGlideGetState(&defState);

	// generate fog table 
    guFogGenerateExp( fogtable, .05f );
    grFogTable( fogtable );

	// setup background rendering features
    grGlideSetState(&defState);
	guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );
	grAlphaCombine( 
		GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_NONE,FXFALSE );

	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
		GR_BLEND_ZERO, GR_BLEND_ZERO );

	grColorCombine(
		GR_COMBINE_FUNCTION_SCALE_OTHER,
		GR_COMBINE_FACTOR_ONE, GR_COMBINE_LOCAL_NONE,
		GR_COMBINE_OTHER_TEXTURE, FXFALSE);

	grTexCombine( GR_TMU0,
		GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_NONE, FXFALSE, FXFALSE);

	guTexSource(texMap);
    grFogMode( GR_FOG_WITH_TABLE );
//    grFogMode( GR_FOG_DISABLE );
	grGlideGetState(&bgState);

	// setup klunssi object
	grGlideSetState(&defState);

	grAlphaCombine( 
		GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_NONE,FXFALSE );
	grAlphaBlendFunction(
		GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);

	grTexCombine(GR_TMU0,
		 GR_COMBINE_FUNCTION_LOCAL,
		 GR_COMBINE_FACTOR_NONE,
		 GR_COMBINE_FUNCTION_LOCAL,
		 GR_COMBINE_FACTOR_NONE,
		 FXFALSE, FXFALSE);

	guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);
	guTexSource(envMap);
    grFogMode( GR_FOG_DISABLE );
	grGlideGetState(&klunssiState);

#ifndef NOMUSIC

    /* Initialize MIDAS and start background playback: */
	module=NULL;
    if( (AfxGetApp()->GetProfileInt(szConfig, _T("Mute"), 0)==0) && MIDASinit())
	{
	    if(MIDASstartBackgroundPlay(100))
		{
			/* Load the module and start playing: */
//			module = MIDASloadModule("d:\\projects\\glidetest\\trans.xm");
			LPCTSTR name = AfxGetApp()->GetProfileString(szConfig, _T("FileName"), "");
			module = MIDASloadModule((char*)name);
			if(module) MIDASplayModule(module, 0);
		}
	}
#endif

	// run at idle priority...
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
	initok=1;
}

/****************************************************************************/
// randomize rotation and fog color
void Probe::randomScene()
{
	if(!initok) this->prepare();

	float rd=1.f/(float)RAND_MAX; 
	cameraRotate=(rand()*rd-.5f)*.015f;
	cameraSpeed=(rand()*rd-.5f)*2.24f;
	rotSpeed=Vector3(rand()*rd-.5f, rand()*rd-.5f, rand()*rd-.5f)*4.f;

	fogColor = fogColors[ rand() % (sizeof(fogColors)/sizeof(fogColors[0])) ];

	// get new texture .. there's 15 128x128 textures and 256x256 texture for envmap
	FxU8 *pic=bmp+(rand()%15)*17184+66336;

	// download texture
	guTexDownloadMipMap( texMap, pic+800, NULL );
	// copy palette
	for(int i=0;i<256;i++) texPal.data[i]=(pic[32+i*3]<<16)+(pic[32+i*3+1]<<8)+(pic[32+i*3+2])&0xffffff;

	// generate fog table 
    guFogGenerateExp( fogtable, .0025f + ((rand()%100)*.001f) );
    grFogTable( fogtable );


	blurAm = rand()%192+64;
}

/****************************************************************************/
void Probe::run() 
{
	int i, j;
	if(!initok) this->prepare();

	if(frames<0)
	{
		timeold=GetTickCount();
		kala=0;
		frames=0;	
		randomScene();
	}

	float t=(GetTickCount()-timeold)*0.04f;

	j=frames;
	if(j<0) j=0;
	if(j>256) j=256;
		
	for(i=0;i<256;i++) fadetab[i]=(i*j)>>8;

	if(kala==0) frames+=1;
	if(kala==1) frames-=3;

	if(frames>700) kala=1;

	Vector3 v=rotSpeed * (t*60.f*.00015f);

	// rotation matrix
	Vector3 m0, m1, m2;
	m0.x = (float)(cos(v.y) * cos(v.z) - sin(v.y) * sin(v.x) * sin(v.z));
	m0.y = (float)(-cos(v.x) * sin(v.z));
	m0.z = (float)(sin(v.y) * cos(v.z) + cos(v.y) * sin(v.x) * sin(v.z));
	m1.x = (float)(cos(v.y) * sin(v.z) + sin(v.y) * sin(v.x) * cos(v.z));
	m1.y = (float)(cos(v.x) * cos(v.z));
	m1.z = (float)(sin(v.y) * sin(v.z) - cos(v.y) * sin(v.x) * cos(v.z));
	m2.x = (float)(-sin(v.y) * cos(v.x));
	m2.y = (float)(sin(v.x));
	m2.z = (float)(cos(v.y) * cos(v.x));

    for( i = 0; i < objvertAm; i++ ) 
	{
		// add pivot
		Vector3 q=Vector3(objvert[i*3]+.8f, objvert[i*3+1], objvert[i*3+2]);

		v.x=m0.x*q.x+m0.y*q.y+m0.z*q.z;
		v.y=m1.x*q.x+m1.y*q.y+m1.z*q.z;
		v.z=m2.x*q.x+m2.y*q.y+m2.z*q.z+2.5f;

		if(v.z<1.f) v.z=1.f;

		v.x=v.x/v.z*320.f*2.00f;
		v.y=v.y/v.z*240.f*2.45f;
		v=v+Vector3(320.f, 240.f, 0);

	// vertex snapping to 4bits subpixel accuracy.... HOAX!
		int qx, qy;
		qx=(int)(v.x*16.f);
		qy=(int)(v.y*16.f);

		rotvert[i].x = qx/16.f;	
		rotvert[i].y = qy/16.f;
		rotvert[i].oow=1.f/v.z;
		// rotate vertex normals
		v.x=m0.x*objvn[i].x+m0.y*objvn[i].y+m0.z*objvn[i].z;
		v.y=m1.x*objvn[i].x+m1.y*objvn[i].y+m1.z*objvn[i].z;
		
		// use them for envmap
		rotvert[i].tmuvtx[0].sow = (1-v.x)*128.f*rotvert[i].oow;
		rotvert[i].tmuvtx[0].tow = (1-v.y)*128.f*rotvert[i].oow;
	}

//		float ang=(float)sin(t*cameraRotate)*3.f;
	float ang=(float)t*cameraRotate;
	float pos=t*cameraSpeed;
	for(i=0;i<4;i++)
	{
		// this looks good enough :)
		texVerts[i].tmuvtx[0].sow = (float)sin(ang)*13.f;
		texVerts[i].tmuvtx[0].tow = (float)cos(ang)*13.f+pos*texVerts[i].oow; 
		ang+=2*3.1415/4;
	}

	// clear only depth buffer
	grColorMask(0,0);
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST );
	grColorMask(1,0);

	grGlideSetState(&bgState);
	grFogColorValue( FADE(fogColor) );
	for(i=0;i<256;i++) realPal.data[i]=FADE(texPal.data[i]);
	grTexDownloadTable(GR_TMU0, GR_TEXTABLE_PALETTE, &realPal);

	// fix the blur bugs by writing 100% solid if fading is very dark
	if(frames>2)
		grConstantColorValue( blurAm<<24 );
	else
		grConstantColorValue(~0);

	// draw the background
	grDrawTriangle(&texVerts[0], &texVerts[1], &texVerts[2]);
	grDrawTriangle(&texVerts[2], &texVerts[3], &texVerts[0]); 

	grGlideSetState(&klunssiState);

	for(i=0;i<256;i++) realPal.data[i]=FADE(envPal.data[i]);
	grTexDownloadTable(GR_TMU0, GR_TEXTABLE_PALETTE, &realPal);

	grConstantColorValue( 250<<24 );
    for( i = 0; i < objfaceAm; i++ ) 
	{
//			guDrawTriangleWithClip(
        grDrawTriangle(
			rotvert+objface[i*3+0], 
			rotvert+objface[i*3+1], 
			rotvert+objface[i*3+2]);
    }
	framesTotal++;

	grSstIdle();
	grBufferSwap(1);

}

/****************************************************************************/

Probe::~Probe()
{
    grGlideShutdown();

#ifndef NOMUSIC
    // Stop playing and deallocate module: 
    if(module) MIDASstopModule(module);
    if(module) MIDASfreeModule(module);

	module=NULL;

    // Stop background playback and uninitialize MIDAS: 
    MIDASstopBackgroundPlay();
    MIDASclose();
#endif	

	return;
}

