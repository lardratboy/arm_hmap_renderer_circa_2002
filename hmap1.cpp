// ----------------------------------------------------------------------------
/*
The MIT License (MIT)

Copyright (c) 2000- Brad P. Taylor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// ----------------------------------------------------------------------------

#include "mytypes.h"
#include "GBARegs.h"
#include "sin_cos.h"

// ----------------------------------------------------------------------------
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define TEST_AUDIO 
//#define STEREO_TEST
#define LOOPING_SAMPLE_TEST
#define BPT_ADPCM_TEST
#define ONLY_MONO_AUDIO
//#define HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ
#define ADPCM_SAMPLE_FREQUENCY		16384

//#define BIG_AUDIO_TEST

#if defined(TEST_AUDIO)

	#if defined(BPT_ADPCM_TEST)

//		#include "./audio/hiphop_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/devil_casual_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/soulwish_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/hardest_button_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/seven_nation_army_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/sexdwarf_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/whatiam_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/testditty4brad2_(bpt)Adpcm.cpp"
//		#include "./audio/bpt_really_funked_out_(bpt)Adpcm.cpp"
		#include "./audio/balloonman_mono_8bit_16384Hz_(4bit)Adpcm.cpp"

#if defined(BIG_AUDIO_TEST)

//		#include "./audio/little_acorns_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/soulwish_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/knockwood_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/withoutme_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/hiphop_mono_8bit_16384Hz_(4bit)Adpcm.cpp"
//		#include "./audio/truth_mono_8bit_16384Hz_(4bit)Adpcm.cpp"

#endif // defined(BIG_AUDIO_TEST)

	#endif

#endif // defined(TEST_AUDIO)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ----------------------------------------------------------------------------

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "hackdata.h"
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define REPORT_RENDER_TIME

// ----------------------------------------------------------------------------
// TEST CASES
// ----------------------------------------------------------------------------

#define HALF_RES_16BPP_MODE
#define ROTATED_DISPLAY_MODE
//#define TEST_50_50_BLEND

#if defined(HALF_RES_16BPP_MODE)
	#define BYTES_PER_PIXEL		2
#else
	#define BYTES_PER_PIXEL		1
#endif

#if !defined(TEST_50_50_BLEND)
	#define HACK_RENDER_DUP_COLS
#endif

#if defined(ROTATED_DISPLAY_MODE)
	#define HACK_DUP_COL_COUNT	2
#else
	#define HACK_DUP_COL_COUNT	4
#endif

#define SPEED_UP_MOVEMENT	1
#define SUPPORT_SPECIAL_TILE_BITS

//#define OFFSCREEN_RENDERING
#define SUPPORT_FLEXIBLE_RENDER_HEIGHT
#define TEST_TILED_RENDERER
#define TEST_COMMAND_STREAM_RENDERER

//#define HACK_ODD_EVEN_RENDERER
//#define HACK_SKIP_RENDERER

//#define SPHERE_LIKE_PROJECTION
#define SPHERE_PROJECTION_FLIP
#define SPHERE_PROJECTION_AMOUNT	8

// ----------------------------------------------------------------------------
// UVMAP info
// ----------------------------------------------------------------------------

#define UVMAP_U_POW2				8
#define UVMAP_V_POW2				8

#define UVMAP_CX					(1 << UVMAP_U_POW2)
#define UVMAP_CY					(1 << UVMAP_V_POW2)

#define UVMAP_U_MASK				((UVMAP_CX)-1)
#define UVMAP_V_MASK				((UVMAP_CY)-1)
#define UVMAP_U_SHIFT				0
#define UVMAP_V_SHIFT				(UVMAP_U_POW2)

// ------------------------------------------------------------------------------
// constants
// ------------------------------------------------------------------------------

#if defined(HALF_RES_16BPP_MODE)
	#define VRAM_PITCH					320
	#define VRAM_ACTUAL_WIDTH			160
	#define VRAM_ACTUAL_HEIGHT			128
#else
	#define VRAM_PITCH					240
	#define VRAM_ACTUAL_WIDTH			240
	#define VRAM_ACTUAL_HEIGHT			160
#endif

#if defined(ROTATED_DISPLAY_MODE)
	#if defined(TEST_50_50_BLEND)
		#define REND_WIDTH				60
	#else
		#define REND_WIDTH				120
//		#define REND_WIDTH				60
	#endif
#else
	#define REND_WIDTH					240
#endif

//#define REND_WIDTH				60
//#define REND_WIDTH				30
#define REND_HEIGHT					160
//#define REND_HEIGHT				80

//#define VIEW_ANGLE					(TRIG_ANGLE_60)
//#define VIEW_ANGLE					(TRIG_ANGLE_30)
#define VIEW_ANGLE					((TRIG_ANGLE_90)/2)
#define HALF_VIEW_ANGLE				((VIEW_ANGLE)/2)
#define LERP_ANGLE_SHIFT			10

#if defined(HACK_RENDER_DUP_COLS)
#define PER_COL_ANGLE_INC			((((VIEW_ANGLE) << LERP_ANGLE_SHIFT) / (REND_WIDTH)) * HACK_DUP_COL_COUNT)
#else
#define PER_COL_ANGLE_INC			(((VIEW_ANGLE) << LERP_ANGLE_SHIFT) / (REND_WIDTH))
#endif

#define MAX_ALTITUDE				1024
#define DEFAULT_PITCH				((REND_HEIGHT)/2)
#define V_SCALE						0x800
#define DEPTH_CLIP					((((V_SCALE)*256)*3)/4)

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
// ----------------------------------------------------------------------------

extern "C" {

	// ------------------------------------------------------------------------

	struct S_HeightMapData {

		s32 xOrg;
		s32 yOrg;
		s32 altitude;
		s32 yaw;
		s32 pitch;
		s32 speed;

#if defined(TEST_COMMAND_STREAM_RENDERER)
		u32 commandStream[ 4 * (REND_WIDTH + 1) ];
#endif

	};

	// ------------------------------------------------------------------------

	struct SGlobalVars {

		pu8 pBackBuffer;

		u32 frame;

		S_HeightMapData HD;

		bool bSelectToggle;

		u16 lastButtonsOnceAFrame;
		u16 buttonsOnceAFrame;

#if defined(TEST_TILED_RENDERER)
		void * pTilemap;
		void * pTileset;
#endif

		u16 tileset;

	};

	SGlobalVars g; // will be mapped to ERAM eventually...

};

// ----------------------------------------------------------------------------
// static code
// ----------------------------------------------------------------------------

//
//	CastRay()
//

template< class T >
inline void CastRay(
	T * pDst, T * pEnd, int x, int y, int z, int dx, int dy, int dz
) {

	int ph = 0;

	while ( ph < DEPTH_CLIP ) {

		x += dx;
		y += dy;
		z += dz;

		ph += V_SCALE;

		int texelOffset = 
			(((x >> BPT_TRIG_SHIFT) & UVMAP_U_MASK) << UVMAP_U_SHIFT) | 
			(((y >> BPT_TRIG_SHIFT) & UVMAP_V_MASK) << UVMAP_V_SHIFT) ;

		int h = (*((const T *)0x2000000 + texelOffset)) << 16;

		if ( h > z ) {

			int c = *((const T *)0x2010000 + texelOffset);

			do {

				if ( 1 & ((u32)++pDst) ) {

					*(pDst - 1) = c;

#if defined(HACK_RENDER_DUP_COLS) && (!defined(HACK_SKIP_RENDERER))
					*(pDst + (VRAM_PITCH - 1)) = c;
#endif

				}

				if ( pEnd <= pDst ) return;

				dz += V_SCALE;

			} while ( h > (z += ph) );

		}

	}

}

#if defined(TEST_COMMAND_STREAM_RENDERER)

// ----------------------------------------------------------------------------

template< class T >
struct THeightStreamCommand {
	T *		pDst;
	T *		pEnd;
	s32		dx;
	s32		dy;
};

// ----------------------------------------------------------------------------

extern "C" void ARM_RenderHeightmapCommandStream(
	const u32 * pCommandStream
	,const void * pHeightMap
	,const void * pTextureMap
	,const int dz
	,const int x
	,const int y
	,const int z
);

// ----------------------------------------------------------------------------

#if 0

//
//	THeightMapRendererFactory<>
//

template<
	class PIXEL = u8
	,class MAP_ENTRY = u16
	,class HEIGHT_TILE_PIXEL = u8
	,class TEXTURE_TILE_PIXEL = u8
	,const int MAP_CX_POW2 = 7
	,const int MAP_CY_POW2 = 7
	,const int TILE_CX_POW2 = 3
	,const int TILE_CY_POW2 = 3
>
class THeightMapRendererFactory {

private:

	enum {

		UNKNOWN

		,MAP_CX = (1 << MAP_CX_POW2)
		,MAP_CY = (1 << MAP_CY_POW2)
		,MAP_CX_MASK = (MAP_CX - 1)
		,MAP_CY_MASK = (MAP_CY - 1)
		,MAP_SIZE_POW2 = (MAP_CX_POW2 + MAP_CY_POW2)
		,MAP_SIZE = (1 << MAP_SIZE_POW2)

		,TILE_CX = (1 << TILE_CX_POW2)
		,TILE_CY = (1 << TILE_CY_POW2)
		,TILE_CX_MASK = (TILE_CX - 1)
		,TILE_CY_MASK = (TILE_CY - 1)
		,TILE_SIZE_POW2 = (TILE_CX_POW2 + TILE_CY_POW2)
		,TILE_SIZE = (1 << TILE_SIZE_POW2)

	};

public:

	static void T_RenderHeightmapCommandStream(
		const void * pStream
		,const void * pTilemaps
		,const void * pTilesets
		,const int initial_dz
		,const int initial_x
		,const int initial_y
		,const int initial_z
	)
	{

		typedef THeightStreamCommand<PIXEL> command_type;

		const command_type * pCommandStream = reinterpret_cast<const command_type *>( pStream );

		for ( ;; ) {

			// fetch the command
			// ----------------------------------------------------------------

			command_type cmd = *pCommandStream++;

			if ( 0 == cmd.pDst ) return;

			// Setup the 'ray' variables
			// ----------------------------------------------------------------

			int dz = initial_dz
			int x = initial_x;
			int y = initial_y;
			int z = initial_z;

			// ----------------------------------------------------------------

			for ( int ph = 0; ph < DEPTH_CLIP; ) {

				// advance the ray and projected pixel height
				// ------------------------------------------------------------

				x += dx;
				y += dy; 
				z += dz;

				ph += V_SCALE;

				// calculate the tilemap & tile offsets
				// ------------------------------------------------------------

				int mapOffset = 
					((x >> (BPT_TRIG_SHIFT + TILE_CX_POW2)) & MAP_CX_MASK) |
					(((y >> (BPT_TRIG_SHIFT + TILE_CY_POW2)) & MAP_CY_MASK) << MAP_CX_POW2)
					;

				int tileOffset = 
					((x >> BPT_TRIG_SHIFT) & MAP_CX_MASK) |
					(((y >> BPT_TRIG_SHIFT) & MAP_CY_MASK) << TILE_CX_POW2)
					;

				// lookup the height tile & render if tall enough
				// ------------------------------------------------------------

				int heightTileOffset = (*((MAP_ENTRY *)pTilemaps + mapOffset)) << TILE_SIZE_POW2;

				int h = (*((TEXTURE_TILE_PIXEL *)pTilesets + tileOffset)) << 16;

				if ( h > z ) {

					int c = (*((HEIGHT_TILE_PIXEL *)pTilesets + tileOffset)) << 16;
					int c = *((const T *)0x2010000 + texelOffset);

					do {

						if ( 1 & ((u32)++pDst) ) {

							*(pDst - 1) = c;

#if defined(HACK_RENDER_DUP_COLS) && (!defined(HACK_SKIP_RENDERER))
							*(pDst + (VRAM_PITCH - 1)) = c;
#endif

						}

						if ( pEnd <= pDst ) return;

						dz += V_SCALE;

					} while ( h > (z += ph) );

				}

			}


		}
	}

}; // class THeightMapRendererFactory<>

#endif

#endif // defined(TEST_COMMAND_STREAM_RENDERER)

// ----------------------------------------------------------------------------

//
//	RenderHeightfield()
//

void RenderHeightfield() __attribute__ ((section(".iwram"))); 

void RenderHeightfield() {

	int dz = ((g.HD.pitch - 40) - (REND_HEIGHT - 1)) * V_SCALE;

#if defined(TEST_COMMAND_STREAM_RENDERER)
	pu32 pCmd = g.HD.commandStream;
#endif

	pu8 pDst = (pu8)g.pBackBuffer;

#if defined(HACK_ODD_EVEN_RENDERER)
	if ( g.frame & 1 ) {

#if defined(ROTATED_DISPLAY_MODE)
		pDst += VRAM_PITCH;
#else
		pDst += 2;
#endif

	}
#endif

	int angleA = (g.HD.yaw - HALF_VIEW_ANGLE);

	if ( 0 > angleA ) angleA = TRIG_ANGLE_360 + angleA;

	angleA <<= LERP_ANGLE_SHIFT;

#if !defined(TEST_COMMAND_STREAM_RENDERER)
	int z = g.HD.altitude << 16;
#endif

#if defined(HACK_RENDER_DUP_COLS) || defined(HACK_ODD_EVEN_RENDERER)
	for ( int col = 0; col < REND_WIDTH; col += HACK_DUP_COL_COUNT ) {
#else
	for ( int col = 0; col < REND_WIDTH; col++ ) {
#endif

#if defined(STORE_SINCOS_TABLE_TWICE)
		int angle = (angleA >> LERP_ANGLE_SHIFT);
#else
		int angle = (angleA >> LERP_ANGLE_SHIFT) & TRIG_ANGLE_MASK;
#endif

#if defined(TEST_COMMAND_STREAM_RENDERER)

#if defined(SPHERE_LIKE_PROJECTION)

		int hackPhase = (((col*TRIG_ANGLE_180)/(REND_WIDTH-1))) & TRIG_ANGLE_MASK;
		int hackOffset = (SPHERE_PROJECTION_AMOUNT * (*(g_SinAndCosTable + hackPhase))) >> BPT_TRIG_SHIFT;

		hackOffset *= BYTES_PER_PIXEL;

#if defined(SPHERE_PROJECTION_FLIP)
		hackOffset = SPHERE_PROJECTION_AMOUNT - hackOffset;
#endif

#if defined(SUPPORT_FLEXIBLE_RENDER_HEIGHT)
		*pCmd++ = (u32)(pDst); // + hackOffset;
#else
		*pCmd++ = (u32)(pDst) + hackOffset;
#endif

#else

		*pCmd++ = (u32)(pDst);

#endif

#if 1 || defined(SCALE_ANGLE_DELTAS)

		*pCmd++ = (u32)(( (*((g_SinAndCosTable + TRIG_ANGLE_90) + angle)) *3)/8);
		*pCmd++ = (u32)(( (*(g_SinAndCosTable + angle)) *3)/8);

#else

		*pCmd++ = (u32)(*((g_SinAndCosTable + TRIG_ANGLE_90) + angle));
		*pCmd++ = (u32)(*(g_SinAndCosTable + angle));

#endif

#if defined(SUPPORT_FLEXIBLE_RENDER_HEIGHT)

	#if defined(ROTATED_DISPLAY_MODE)

		#if defined(SPHERE_LIKE_PROJECTION)
			*pCmd++ = (u32)(pDst + (REND_HEIGHT * BYTES_PER_PIXEL) + hackOffset);
		#else
			*pCmd++ = (u32)(pDst + (REND_HEIGHT * BYTES_PER_PIXEL));
		#endif

	#else

		#if defined(SPHERE_LIKE_PROJECTION)

			*pCmd++ = (u32)(pDst + REND_HEIGHT * VRAM_PITCH + (hackOffset * VRAM_PITCH));

		#else

			*pCmd++ = (u32)(pDst + REND_HEIGHT * VRAM_PITCH);

		#endif

	#endif

#endif

#else

		CastRay(
			pDst
			,pDst + REND_HEIGHT
			,g.HD.xOrg
			,g.HD.yOrg
			,z // g.HD.altitude << 16
			,*((g_SinAndCosTable + TRIG_ANGLE_90) + angle) // cos angle
			,*(g_SinAndCosTable + angle) // sin angle
			,dz
		);

#endif

		angleA += PER_COL_ANGLE_INC;

#if defined(ROTATED_DISPLAY_MODE)

	#if defined(HACK_RENDER_DUP_COLS) || defined(HACK_ODD_EVEN_RENDERER)
			pDst += (VRAM_PITCH * HACK_DUP_COL_COUNT); // surface pitch
	#else
			pDst += VRAM_PITCH; // surface pitch
	#endif

#else

	#if defined(HACK_RENDER_DUP_COLS) || defined(HACK_ODD_EVEN_RENDERER)

		pDst += HACK_DUP_COL_COUNT;

	#else

		++pDst;

	#endif


#endif

	}

	// ------------------------------------------------------------------------

#if defined(TEST_COMMAND_STREAM_RENDERER)

	*pCmd = 0;

	ARM_RenderHeightmapCommandStream(
		g.HD.commandStream
#if defined(TEST_TILED_RENDERER)
		,(void *)g.pTilemap
		,(void *)g.pTileset
#else
		,(void *)0x2000000
		,(void *)0x2010000
#endif
		,dz
		,g.HD.xOrg & (~TRIG_ANGLE_MASK)
		,g.HD.yOrg & (~TRIG_ANGLE_MASK)
		,g.HD.altitude << 16
	);

#endif

}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// public code
// ----------------------------------------------------------------------------

extern "C" {

	// support routines
	// ------------------------------------------------------------------------

	//
	//	DMA3Copy()
	//

	void 
	DMA3Copy( void * pDst, void * pSrc, const int nSize )
	{
		TWrite<u32>( REG_DMA3SAD, (u32)pSrc );
		TWrite<u32>( REG_DMA3DAD, (u32)pDst );

		if ( (nSize & 2) || ((u32)pDst & 2) || ((u32)pSrc & 2) ) {

			TWrite<u32>( REG_DMA3CNT, (nSize >> 1) | 0x80000000 );

		} else {

			TWrite<u32>( REG_DMA3CNT, (nSize >> 2) | 0x84000000 );

		}
	}

	//
	//	DisplayDecimalNumber()
	//

	void
	DisplayDecimalNumber( pu8 pBackBuffer, const u32 value, const bool bDisplayOnlyWhenPaused = true )
	{

		u32 v = value;

#if defined(HALF_RES_16BPP_MODE)
		pu16 pDst = (pu16)(pBackBuffer + 64 );
#else
		pu32 pDst = (pu32)(pBackBuffer + 32 );
#endif

		for ( int digit = 0; digit < 8; digit++ ) { // could unrolll if needed...

#if defined(HALF_RES_16BPP_MODE)
			pDst -= 4; // -= VRAM_PITCH;
#else
			--pDst;
#endif

			int adigit = (v % 10);

			v /= 10;

			pu32 pSrc = (pu32)(g_4x5x8bpp_Numbers + (adigit << 4) + (adigit << 2));

#if defined(ROTATED_DISPLAY_MODE)

#if defined(HALF_RES_16BPP_MODE)

			pu16 pWrite = pDst;

			for ( int i = 0; i < 5; i++ ) {

				u32 pa = *pSrc++;

				*(pWrite+0) = ( 0x000000ff & pa ) ? 0x0000 : 0xffff;
				*(pWrite+1) = ( 0x0000ff00 & pa ) ? 0x0000 : 0xffff;
				*(pWrite+2) = ( 0x00ff0000 & pa ) ? 0x0000 : 0xffff;
				*(pWrite+3) = ( 0xff000000 & pa ) ? 0x0000 : 0xffff;

				pWrite -= VRAM_PITCH/2;

			}

#else

			*(pDst + ((VRAM_PITCH/4) * 4) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 3) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 2) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 1) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 0) ) = *pSrc++;

#endif

#else
			*(pDst + ((VRAM_PITCH/4) * 0) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 1) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 2) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 3) ) = *pSrc++;
			*(pDst + ((VRAM_PITCH/4) * 4) ) = *pSrc++;
#endif

		}

	}

	// ------------------------------------------------------------------------

	extern "C" void ARM_4x4ColorizedDisplayList( pu8 pDst, const void * pSrc );
	extern "C" void ARM_4x4SingleImageDisplayList( pu8 pDst, const void * pSrc );

	void 
	RenderParticles()
	{

#if 0

		// fill in the particle display list
		// --------------------------------------------------------------------

		vAngle

		for ( int v = 0; v < TRIG_ANGLE_360; v += vAngle ) {

			for ( int u = 0; u < TRIG_ANGLE_360; u += uAngle ) {

			}

		}

		// call the renderer
		// --------------------------------------------------------------------

#if defined(COLORED_PARTICLES_TEST)

		ARM_4x4ColorizedDisplayList( g.pBackBuffer, gHack_4x4DisplayList );

#else

		ARM_4x4SingleImageDisplayList( g.pBackBuffer, gHack_4x4DisplayList );

#endif

#endif

	}

	// ------------------------------------------------------------------------

// ============================================================================
// ============================================================================
#if defined(TEST_AUDIO)

#if defined(LOOPING_SAMPLE_TEST)

	// ------------------------------------------------------------------------

#if defined(BPT_ADPCM_TEST)

	struct ADPCMState {

		s32 index;				// Index into step size table
		s32 previousValue;		// Most recent sample value

	};

#endif // defined(BPT_ADPCM_TEST)

	// ------------------------------------------------------------------------

	//
	//	SChannelInfo
	//

	struct SChannelInfo {

		pu8 pAudioBackBuffer;
		u32 dwBackBufferOffset;
		pu8 pAudioSrc;
		pu8 pAudioSrcStart;
		pu8 pAudioSrcEnd;

#if defined(BPT_ADPCM_TEST)

		u32 nSamplesPerByte;

		ADPCMState adpcmState;

#endif // defined(BPT_ADPCM_TEST)

	};

	// ------------------------------------------------------------------------

	//
	//	SAudioSystem
	//

	struct SAudioSystem {

		// ALL AUDIO buffers MUST be 32 bit aligned!

		// Audio double buffered memory
		// --------------------------------------------------------------------

		pu8 pAudioBufferA;
		pu8 pAudioBufferB;

		pu8 pAudioBufferC;
		pu8 pAudioBufferD;

		// audio buffer page size & process count information
		// should it be interrupt driven?
		// --------------------------------------------------------------------

		s32 nAudioPageSize;

		s32 nPerServiceCallProcessSampleCount;

		// current double buffered back and front buffers
		// --------------------------------------------------------------------

		// playback information
		// --------------------------------------------------------------------

		bool bStereoPlayback;
		bool bAudioPlaying;
		s32 nFrequency;

		// currently playing audio
		// --------------------------------------------------------------------

		SChannelInfo L_Channel;
		SChannelInfo R_Channel;

	}; // struct SAudioSystem

	// ------------------------------------------------------------------------

	SAudioSystem g_Audio;

	// ------------------------------------------------------------------------

#if defined(BPT_ADPCM_TEST)

	// ------------------------------------------------------------------------

	s32 g_IndexAdjustTable[ 16 ] = {

		-1, -1, -1, -1,			// +0 - +3, decrease step size
		2, 4, 6, 8,				// +4 - +7, increase step size
		-1, -1, -1, -1,			// -0 - -3, decrease step size
		2, 4, 6, 8				// -4 - -7, increase step size

	};

	// ------------------------------------------------------------------------

	//
	//	g_QuantizedStepTable
	//

	#define BPT_ADPCM_STEP_TABLE_SIZE	128

	s32 g_QuantizedStepTable[ BPT_ADPCM_STEP_TABLE_SIZE ] = {

			  7
		,     8
		,     9
		,    10
		,    11
		,    12
		,    13
		,    14
		,    15
		,    16
		,    17
		,    18
		,    19
		,    20
		,    21
		,    22
		,    23
		,    24
		,    25
		,    26
		,    27
		,    28
		,    29
		,    30
		,    31
		,    32
		,    33
		,    34
		,    35
		,    36
		,    37
		,    38
		,    39
		,    40
		,    41
		,    42
		,    43
		,    44
		,    45
		,    46
		,    47
		,    48
		,    49
		,    50
		,    51
		,    52
		,    53
		,    54
		,    55
		,    56
		,    57
		,    58
		,    59
		,    60
		,    61
		,    62
		,    63
		,    64
		,    65
		,    67
		,    68
		,    69
		,    70
		,    71
		,    72
		,    73
		,    74
		,    75
		,    77
		,    78
		,    79
		,    80
		,    81
		,    83
		,    84
		,    85
		,    87
		,    88
		,    89
		,    91
		,    92
		,    94
		,    95
		,    97
		,    98
		,   100
		,   102
		,   103
		,   105
		,   107
		,   109
		,   111
		,   113
		,   115
		,   117
		,   119
		,   121
		,   124
		,   126
		,   129
		,   131
		,   134
		,   137
		,   140
		,   143
		,   146
		,   149
		,   153
		,   157
		,   160
		,   164
		,   168
		,   172
		,   177
		,   182
		,   186
		,   191
		,   197
		,   202
		,   208
		,   214
		,   220
		,   226
		,   233
		,   240
		,   247
		,   255
		,   262

	}; // g_QuantizedStepTable[]

	// ------------------------------------------------------------------------

	int ADPCM_DecodeSAMPLE( ADPCMState & state, const int deltaCode ) __attribute__ ((section(".iwram")));

	//
	//	DecodeSAMPLE()
	//

	int ADPCM_DecodeSAMPLE( ADPCMState & state, const int deltaCode ) {

		// Get the current step size

		int step = g_QuantizedStepTable[ state.index ];

		// Construct the difference by scaling the current step size
		// This is approximately: difference = (deltaCode+.5)*step/4

		int difference = step >> 3;

		if ( 1 & deltaCode ) { difference += step >> 2; }
		if ( 2 & deltaCode ) { difference += step >> 1; }
		if ( 4 & deltaCode ) { difference += step; }
		if ( 8 & deltaCode ) { difference = -difference; }

		// Build new sample

		state.previousValue += difference;

		// limit sample to valid sample range

		if ( 127 < state.previousValue ) {
			
			state.previousValue = 127;

		} else if ( -128 > state.previousValue ) {
			
			state.previousValue = -128;

		}

		// Update the state foe the next sample

		state.index += g_IndexAdjustTable[ deltaCode ];

		if ( 0 > state.index ) {

			state.index = 0;

		} else if ( BPT_ADPCM_STEP_TABLE_SIZE <= state.index ) {

			state.index = (BPT_ADPCM_STEP_TABLE_SIZE - 1);

		}

		return ((u32)state.previousValue & 0xff);

	}

	// ------------------------------------------------------------------------

	void ADPCM_DecodeChannel( SChannelInfo & channel, const int nSamples ) __attribute__ ((section(".iwram")));

	//
	//	ADPCM_DecodeChannel()
	//

	void ADPCM_DecodeChannel( SChannelInfo & channel, const int nSamples ) {

		int (*pfnDecoder)( ADPCMState & state, const int deltaCode ) = ADPCM_DecodeSAMPLE;

		pu32 pOutput = (pu32)(channel.pAudioBackBuffer + channel.dwBackBufferOffset);

		channel.dwBackBufferOffset += nSamples; // output bytes per sample...

		for ( int decodeSamples = nSamples; 0 < decodeSamples; ) {

			// figure out how many samples are left inside the src data
			// ----------------------------------------------------------------

			int remainingSrcSamples = 
				(channel.pAudioSrcEnd - channel.pAudioSrc) * channel.nSamplesPerByte;

			int decodeN = decodeSamples;

			if ( decodeN > remainingSrcSamples ) {

				decodeN = remainingSrcSamples;

			}

			decodeSamples -= decodeN;

			// decode channel data
			// ----------------------------------------------------------------

			pu32 pSource = (pu32)channel.pAudioSrc;

			// number of samples per 32 bit word

			for ( int decodeGroups = (decodeN/8); 0 <= --decodeGroups; ) {

				u32 srcData = *pSource++;

				*pOutput++ = 
						(pfnDecoder( channel.adpcmState, srcData & 0x0f ))
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x08)
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x10)
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x18);

				*pOutput++ = 
						(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ))
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x08)
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x10)
					|	(pfnDecoder( channel.adpcmState, (srcData >>= 4) & 0x0f ) << 0x18);

			}

			channel.pAudioSrc = (pu8)pSource;

			// check to see if we are at the end of our audio samples
			// ----------------------------------------------------------------

			if ( (u32)channel.pAudioSrc >= (u32)channel.pAudioSrcEnd ) {

				channel.adpcmState.index = 0;

				channel.adpcmState.previousValue = 0;

				channel.pAudioSrc = channel.pAudioSrcStart;

			}

		}

	}

#endif // defined(BPT_ADPCM_TEST)

	// ------------------------------------------------------------------------

	void PCM_DMAChannel( SChannelInfo & channel, const int nSamples ) __attribute__ ((section(".iwram")));

	//
	//	PCM_DMAChannel()
	//

	void PCM_DMAChannel( SChannelInfo & channel, const int nSamples ) {

		pu8 pOutput = channel.pAudioBackBuffer + channel.dwBackBufferOffset;

		channel.dwBackBufferOffset += nSamples; // output bytes per sample...

		for ( int decodeSamples = nSamples; 0 < decodeSamples; ) {

			// figure out how many samples are left inside the src data
			// ----------------------------------------------------------------

			int remainingSrcSamples = 
				(channel.pAudioSrcEnd - channel.pAudioSrc) * channel.nSamplesPerByte;

			int decodeN = decodeSamples;

			if ( decodeN > remainingSrcSamples ) {

				decodeN = remainingSrcSamples;

			}

			decodeSamples -= decodeN;

			// DMA channel data
			// ----------------------------------------------------------------

			TWrite<u32>( REG_DMA3SAD, (u32)channel.pAudioSrc );
			TWrite<u32>( REG_DMA3DAD, (u32)pOutput );
			TWrite<u32>( REG_DMA3CNT, (decodeN/4) | 0x84000000 );

			// Advance & check to see if we are at the end of our audio data
			// ----------------------------------------------------------------

			channel.pAudioSrc += decodeN;

			pOutput += decodeN;

			if ( (u32)channel.pAudioSrc >= (u32)channel.pAudioSrcEnd ) {

				channel.pAudioSrc = channel.pAudioSrcStart;

			}

		}

	}

	// ------------------------------------------------------------------------

	void CODE_ServiceAudio( const bool bForceBufferFill ) __attribute__ ((section(".iwram")));

	//
	//	CODE_ServiceAudio()
	//

	void
	CODE_ServiceAudio( const bool bForceBufferFill )
	{

		// There is an assumption here that the L channel is the 'controlling' channel
		// if the channels were ever to be unsync'd then this code would need to change...

		int samplesLeft = g_Audio.nAudioPageSize - g_Audio.L_Channel.dwBackBufferOffset;

		if ( 0 < samplesLeft ) {

			// How many samples to process?

			if ( (!bForceBufferFill) && (samplesLeft > g_Audio.nPerServiceCallProcessSampleCount) ) {
					
				samplesLeft = g_Audio.nPerServiceCallProcessSampleCount;

			}

#if defined(BPT_ADPCM_TEST)

			// decode could be part of the

			ADPCM_DecodeChannel( g_Audio.L_Channel, samplesLeft );

			if ( g_Audio.bStereoPlayback ) {

				ADPCM_DecodeChannel( g_Audio.R_Channel, samplesLeft );

			}

#else // defined(BPT_ADPCM_TEST)

			PCM_DMAChannel( g_Audio.L_Channel, samplesLeft );

			if ( g_Audio.bStereoPlayback ) {

				PCM_DMAChannel( g_Audio.R_Channel, samplesLeft );

			}
#endif

		}

	}

	void (*ServiceAudio)( const bool bForceBufferFill ) = CODE_ServiceAudio;

#endif // defined(LOOPING_SAMPLE_TEST)

	// ------------------------------------------------------------------------

	//
	//	InitAudioHardware()
	//

	void
	InitAudioHardware()
	{
		// should this setup the memory regions?

		TWrite<u32>( REG_SOUNDCNT_X, 0x00000000 ); // turn sound chip off

	    // make sure sound channels 1-4 are turned off

		TWrite<u16>( REG_SOUNDCNT_L, 0 ); 

	}

	// ------------------------------------------------------------------------

	//
	//	StopAudio()
	//

	void StopAudio() {

		if ( g_Audio.bAudioPlaying ) {

			g_Audio.bAudioPlaying = false;

			TWrite<u32>( REG_SOUNDCNT_X, 0x00000000 ); // turn sound chip off

#if defined(LOOPING_SAMPLE_TEST)

			TWrite<u16>( REG_IME, 0 );			// disable interrupt
			TWrite<u16>( REG_IE, 0 );			// disable irq for timer 1 overflow
			TWrite<u16>( REG_TM1D, 0 );			// stop timer 1 (audio double buffer handler)
			TWrite<u16>( REG_TM1CNT, 0 );		// ""

#endif

			TWrite<u16>( REG_TM0CNT, 0 );		// stop timer 0
			TWrite<u16>( REG_TM0D, 0 );			// ""
			TWrite<u32>( REG_DMA1CNT, 0 );		// stop dma 1
			TWrite<u32>( REG_DMA2CNT, 0 );		// stop dma 2

		}

	}

// sample granularity must be a power of 2 for the mask to work
#define SERVICE_SAMPLE_GRANULARITY			32
#define SERVICE_GRANULARITY_MASK			(SERVICE_SAMPLE_GRANULARITY - 1)

	//
	//	StartAudio()
	//

	void StartAudio(
			void * pLAudioData
		,	void * pRAudioData
		,	const int nFrequency
		,	const int nSamples
	) {

		StopAudio();
		
		// general info
		// --------------------------------------------------------------------

		g_Audio.bAudioPlaying = true;

		g_Audio.bStereoPlayback = (pLAudioData != pRAudioData);

		g_Audio.nFrequency = nFrequency;

		// Setup the channel information
		// --------------------------------------------------------------------

		g_Audio.L_Channel.dwBackBufferOffset = 0;
		g_Audio.L_Channel.pAudioSrc = (pu8)pLAudioData;
		g_Audio.L_Channel.pAudioSrcStart = (pu8)pLAudioData;

#if defined(BPT_ADPCM_TEST)
		g_Audio.L_Channel.pAudioSrcEnd = g_Audio.L_Channel.pAudioSrcStart + nSamples/2;
#else
		g_Audio.L_Channel.pAudioSrcEnd = g_Audio.L_Channel.pAudioSrcStart + nSamples;
#endif

		g_Audio.R_Channel.dwBackBufferOffset = 0;
		g_Audio.R_Channel.pAudioSrc = (pu8)pRAudioData;
		g_Audio.R_Channel.pAudioSrcStart = (pu8)pRAudioData;

#if defined(BPT_ADPCM_TEST)
		g_Audio.R_Channel.pAudioSrcEnd = g_Audio.R_Channel.pAudioSrcStart + nSamples/2;
#else
		g_Audio.R_Channel.pAudioSrcEnd = g_Audio.R_Channel.pAudioSrcStart + nSamples;
#endif

		// 
		// --------------------------------------------------------------------

		// setup the audio double buffer addresses
		// --------------------------------------------------------------------

#if defined(LOOPING_SAMPLE_TEST)

		u32 baseAudioAddress = 0x2030000 + sizeof(g_Texture3_ShadeTable);

		if ( g_Audio.bStereoPlayback ) {

			// ensure that the audio page is multiple of the granularity in size

			g_Audio.nAudioPageSize = 
				((nFrequency/2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);

			// now setup the audio buffers (this really needs to be a call)

			g_Audio.pAudioBufferA = (pu8)((baseAudioAddress)+((0)*(g_Audio.nAudioPageSize)));
			g_Audio.pAudioBufferB = (pu8)((baseAudioAddress)+((1)*(g_Audio.nAudioPageSize)));
			g_Audio.pAudioBufferC = (pu8)((baseAudioAddress)+((2)*(g_Audio.nAudioPageSize)));
			g_Audio.pAudioBufferD = (pu8)((baseAudioAddress)+((3)*(g_Audio.nAudioPageSize)));

		} else {

			// ensure that the audio page is multiple of the granularity in size

			g_Audio.nAudioPageSize = 
				((nFrequency) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);

			// now setup the audio buffers (this really needs to be a call)

			g_Audio.pAudioBufferA = (pu8)((baseAudioAddress)+((0)*(g_Audio.nAudioPageSize)));
			g_Audio.pAudioBufferB = g_Audio.pAudioBufferA;
			g_Audio.pAudioBufferC = (pu8)((baseAudioAddress)+((1)*(g_Audio.nAudioPageSize)));
			g_Audio.pAudioBufferD = g_Audio.pAudioBufferC;

		}


		// setup the samples to process per service call
		// --------------------------------------------------------------------
	
#if defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ)

		g_Audio.nPerServiceCallProcessSampleCount = nFrequency / 60;

#else

		g_Audio.nPerServiceCallProcessSampleCount = nFrequency / 16;

#endif

		// Ensure that a multiple of 32 samples are processed per call
		// --------------------------------------------------------------------

		g_Audio.nPerServiceCallProcessSampleCount =
			(g_Audio.nPerServiceCallProcessSampleCount + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);

		// --------------------------------------------------------------------

#if defined(BPT_ADPCM_TEST)

		g_Audio.L_Channel.nSamplesPerByte = 2;
		g_Audio.L_Channel.adpcmState.index = 0;
		g_Audio.L_Channel.adpcmState.previousValue = 0;

		g_Audio.R_Channel.nSamplesPerByte = 2;
		g_Audio.R_Channel.adpcmState.index = 0;
		g_Audio.R_Channel.adpcmState.previousValue = 0;

#else // defined(BPT_ADPCM_TEST)

		g_Audio.L_Channel.nSamplesPerByte = 1;
		g_Audio.R_Channel.nSamplesPerByte = 1;

#endif // defined(BPT_ADPCM_TEST)

#endif // defined(LOOPING_SAMPLE_TEST)

		// ----------------------------------------------------------------

		// turn timer 0 off (controls audio frequency)

		TWrite<u16>( REG_TM0CNT, 0 );

		// turn off the dma channel & reset the FIFO's

		TWrite<u32>( REG_DMA1CNT, 0 );
		TWrite<u32>( REG_DMA2CNT, 0 );

		u16 resetDSAFIFO = 
				TRead<u16>( REG_SOUNDCNT_H )
			|	0x0800		// DSA_FIFO_RESET
			|	0x8000		// DSB_FIFO_RESET
		;  

		TWrite<u16>( REG_SOUNDCNT_H, resetDSAFIFO ); 

#if defined(LOOPING_SAMPLE_TEST)

		// Really need to fill in the first buffer
		// --------------------------------------------------------------------

		g_Audio.L_Channel.pAudioBackBuffer = g_Audio.pAudioBufferA;
		g_Audio.R_Channel.pAudioBackBuffer = g_Audio.pAudioBufferB;

		ServiceAudio( true );

		// setup the next back buffer
		// --------------------------------------------------------------------

		g_Audio.L_Channel.dwBackBufferOffset = 0;
		g_Audio.L_Channel.pAudioBackBuffer = g_Audio.pAudioBufferC;

		g_Audio.R_Channel.dwBackBufferOffset = 0;
		g_Audio.R_Channel.pAudioBackBuffer = g_Audio.pAudioBufferD;

		// point the DMA address

		TWrite<u32>( REG_DMA1SAD, (u32)g_Audio.pAudioBufferA );

		if ( g_Audio.bStereoPlayback ) {

			TWrite<u32>( REG_DMA2SAD, (u32)g_Audio.pAudioBufferB );

		}

#else // defined(LOOPING_SAMPLE_TEST)

		TWrite<u32>( REG_DMA1SAD, (u32)pLAudioData );

		if ( g_Audio.bStereoPlayback ) {

			TWrite<u32>( REG_DMA2SAD, (u32)pRAudioData );

		}

#endif // defined(LOOPING_SAMPLE_TEST)

		TWrite<u32>( REG_DMA1DAD, (u32)REG_FIFO_A );
		TWrite<u32>( REG_DMA1CNT, DMA_ENABLE | DMA_START_ON_FIFO_EMPTY | DMA_32 | DMA_REPEAT );

		if ( g_Audio.bStereoPlayback ) {

			TWrite<u32>( REG_DMA2DAD, (u32)REG_FIFO_B );
			TWrite<u32>( REG_DMA2CNT, DMA_ENABLE | DMA_START_ON_FIFO_EMPTY | DMA_32 | DMA_REPEAT );

		}

#if defined(LOOPING_SAMPLE_TEST)

		// setup the interupt timing frequency to call back for the audio buffer switch

		TWrite<u16>( REG_TM1D, 0xffff - (u16)g_Audio.nAudioPageSize ); // sample buffer size
		TWrite<u16>( REG_TM1CNT, TIME_IRQ_ENABLE | TIME_ENABLE | TIME_OVERFLOW ); 

#endif // defined(LOOPING_SAMPLE_TEST)

		// start timer 0 to playback at desired frequency
		// --------------------------------------------------------------------
		
#if 0

		// this code doesn't support all of the frequencies as the code below.

		TWrite<u16>( REG_TM0D, 0xffff - ((16384/nFrequency) - 1) ); // sample buffer size
		TWrite<u16>( REG_TM0CNT, TIME_IRQ_ENABLE | TIME_ENABLE | TIME_FREQUENCY_1024 ); 

#else

		TWrite<u16>( REG_TM0D, 0xffff - (u16)((1 << 24) / nFrequency) );
		TWrite<u16>( REG_TM0CNT, TIME_ENABLE | TIME_FREQUENCY_SYSTEM );

#endif

		// --------------------------------------------------------------------

#if defined(LOOPING_SAMPLE_TEST)

#if defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ)

		u16 dispStat = TRead<u16>( REG_DISPSTAT ) | (1 << 3); // enable V blank IRQ
		TWrite<u16>( REG_DISPSTAT, dispStat );
		TWrite<u16>( REG_IE, INTERUPT_TIMER_1 | INTERUPT_VBLANK );	// enable irq for timer 1 overflow

#else

		TWrite<u16>( REG_IE, INTERUPT_TIMER_1 );	// enable irq for timer 1 overflow

#endif // defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ)

		TWrite<u16>( REG_IME, 1 );		// enable interrupts

#endif // defined(LOOPING_SAMPLE_TEST)

	    // set the direct sound output control register

		u16 audioControl;

		if ( g_Audio.bStereoPlayback ) {

			audioControl =
					0x0002					// SND_OUTPUT_RATIO_100
				|	0x0004					// DSA_OUTPUT_RATIO_100
				|	0x0200					// DSA_OUTPUT_TO_LEFT
				|	0x0000					// DSA_TIMER0
				|	0x0800					// DSA_FIFO_RESET
				|	0x0008					// DSB_OUTPUT_RATIO_100
				|	0x1000					// DSB_OUTPUT_TO_RIGHT
				|	0x0000					// DSB_TIMER0
				|	0x8000					// DSB_FIFO_RESET
			;

		} else {

			audioControl =
					0x0002						// SND_OUTPUT_RATIO_100
				|	0x0004						// DSA_OUTPUT_RATIO_100
				|	0x0300						// DSA_OUTPUT_TO_LEFT | DSA_OUTPUT_TO_RIGHT
				|	0x0000						// DSA_TIMER0
				|	0x0800						// DSA_FIFO_RESET
			;

		}

		TWrite<u16>( REG_SOUNDCNT_H, audioControl );

		// finally turn on the sound chip

		TWrite<u32>( REG_SOUNDCNT_X, 0x00000080 );

	}

	// ------------------------------------------------------------------------

//#define RELOAD_ONLY_DMA_SRC_ADDRESS_IN_IRQ

	void InterruptProcess(void) __attribute__ ((section(".iwram"))); 

	//
	//	InterruptProcess()
	//

	void 
	InterruptProcess(void) {

		u32 request = TRead<u16>( REG_IF );

#if defined(LOOPING_SAMPLE_TEST)

		if ( INTERUPT_TIMER_1 & request ) {

			// stop the DMA

#if defined(RELOAD_ONLY_DMA_SRC_ADDRESS_IN_IRQ)

			TWrite<u32>( REG_DMA1SAD, (u32)g_Audio.L_Channel.pAudioBackBuffer );

#else

			TWrite<u32>( REG_DMA1CNT, 0 );

			if ( g_Audio.bStereoPlayback ) {

				TWrite<u32>( REG_DMA2CNT, 0 );

			}

			// start the DMA at the current back buffer

			TWrite<u32>( REG_DMA1SAD, (u32)g_Audio.L_Channel.pAudioBackBuffer );
			TWrite<u32>( REG_DMA1DAD, (u32)REG_FIFO_A );
			TWrite<u32>( REG_DMA1CNT, DMA_ENABLE | DMA_START_ON_FIFO_EMPTY | DMA_32 | DMA_REPEAT );

#endif

			if ( g_Audio.bStereoPlayback ) {

	#if 0
				// ------------------------------------------------------------
				// NEED TO PAUSE A FEW CYCLES BEFORE ISSUING ANOTHER DMA REQUEST
				// ------------------------------------------------------------

				if ( TRead<u16>( REG_TM0D ) == TRead<u16>( REG_TM1D ) ) {

					TRead<u16>( REG_TM2D );
					TRead<u16>( REG_TM3D );

				}
	#endif

#if defined(RELOAD_ONLY_DMA_SRC_ADDRESS_IN_IRQ)

				TWrite<u32>( REG_DMA2SAD, (u32)g_Audio.R_Channel.pAudioBackBuffer );

#else

				TWrite<u32>( REG_DMA2SAD, (u32)g_Audio.R_Channel.pAudioBackBuffer );
				TWrite<u32>( REG_DMA2DAD, (u32)REG_FIFO_B );
				TWrite<u32>( REG_DMA2CNT, DMA_ENABLE | DMA_START_ON_FIFO_EMPTY | DMA_32 | DMA_REPEAT );

#endif

			}

			// swap the audio buffers

			if ( g_Audio.L_Channel.pAudioBackBuffer == g_Audio.pAudioBufferA ) {

				// setup the back buffer address

				g_Audio.L_Channel.pAudioBackBuffer = g_Audio.pAudioBufferC;
				g_Audio.R_Channel.pAudioBackBuffer = g_Audio.pAudioBufferD;

			} else {

				g_Audio.L_Channel.pAudioBackBuffer = g_Audio.pAudioBufferA;
				g_Audio.R_Channel.pAudioBackBuffer = g_Audio.pAudioBufferB;

			}

			// reset the audio write offset

			g_Audio.L_Channel.dwBackBufferOffset = 0;
			g_Audio.R_Channel.dwBackBufferOffset = 0;

		}

#if defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ)

		if ( INTERUPT_VBLANK & request ) {

//			++g.interrupt;

			ServiceAudio( false );

		}

#endif // defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ)

#endif // defined(LOOPING_SAMPLE_TEST)

		TWrite<u16>( REG_IF, request );

	}

	// ------------------------------------------------------------------------

#define M_MIN( A, B ) (((A) <= (B)) ? (A) : (B))

	//
	//	AudioTest()
	//

	void 
	AudioTest()
	{
		if ( g_Audio.bAudioPlaying ) {

			StopAudio();

		} else {

			static int counter = 0;

			// ----------------------------------------------------------------

			int nSamples = 0;

			void * pLChannel = 0;

			void * pRChannel = 0;

			switch ( counter % 8 ) {

			default:
			case 0:

#if !defined(BIG_AUDIO_TEST)

#if 0
				pLChannel = (void *)bpt_funked_out_audio_channel;
				pRChannel = (void *)bpt_funked_out_audio_channel;
				nSamples = ((sizeof(bpt_funked_out_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)claytonk1_l_channel;
				pRChannel = (void *)claytonk1_l_channel;
				nSamples = ((sizeof(claytonk1_l_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)audio_whatiam_channel;
				pRChannel = (void *)audio_whatiam_channel;
				nSamples = ((sizeof(audio_whatiam_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)audio_devil_casual_channel;
				pRChannel = (void *)audio_devil_casual_channel;
				nSamples = ((sizeof(audio_devil_casual_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)hardest_button_audio_channel;
				pRChannel = (void *)hardest_button_audio_channel;
				nSamples = ((sizeof(hardest_button_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)audio_devil_casual_channel;
				pRChannel = (void *)audio_devil_casual_channel;
				nSamples = ((sizeof(audio_devil_casual_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 1
				pLChannel = (void *)audio_balloonman_channel;
				pRChannel = (void *)audio_balloonman_channel;
				nSamples = ((sizeof(audio_balloonman_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)seven_nation_army_audio_channel;
				pRChannel = (void *)seven_nation_army_audio_channel;
				nSamples = ((sizeof(seven_nation_army_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#else
				pLChannel = (void *)audio_sexdwarf_channel;
				pRChannel = (void *)audio_sexdwarf_channel;
				nSamples = ((sizeof(audio_sexdwarf_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#endif
				break;

#else
#if 1
				pLChannel = (void *)seven_nation_army_audio_channel;
				pRChannel = (void *)seven_nation_army_audio_channel;
				nSamples = ((sizeof(seven_nation_army_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#else
				pLChannel = (void *)audio_knockwood_channel;
				pRChannel = (void *)audio_knockwood_channel;
				nSamples = ((sizeof(audio_knockwood_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#endif
				break;

			case 1:
#if 0
				pLChannel = (void *)little_acorns_audio_channel;
				pRChannel = (void *)little_acorns_audio_channel;
				nSamples = ((sizeof(little_acorns_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 0
				pLChannel = (void *)hardest_button_audio_channel;
				pRChannel = (void *)hardest_button_audio_channel;
				nSamples = ((sizeof(hardest_button_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#elif 1
				pLChannel = (void *)audio_balloonman_channel;
				pRChannel = (void *)audio_balloonman_channel;
				nSamples = ((sizeof(audio_balloonman_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#else
				pLChannel = (void *)audio_knockwood_channel;
				pRChannel = (void *)audio_knockwood_channel;
				nSamples = ((sizeof(audio_knockwood_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
//				pLChannel = (void *)audio_truth_channel;
//				pRChannel = (void *)audio_truth_channel;
//				nSamples = ((sizeof(audio_truth_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#endif
				break;

#if 0
			case 2:
#if 1
				pLChannel = (void *)seven_nation_army_audio_channel;
				pRChannel = (void *)seven_nation_army_audio_channel;
				nSamples = ((sizeof(seven_nation_army_audio_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#else
				pLChannel = (void *)audio_soulwish_channel;
				pRChannel = (void *)audio_soulwish_channel;
				nSamples = ((sizeof(audio_soulwish_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
#endif
				break;

			case 3:
				pLChannel = (void *)audio_withoutme_channel;
				pRChannel = (void *)audio_withoutme_channel;
				nSamples = ((sizeof(audio_withoutme_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
				break;

			case 4:
				pLChannel = (void *)audio_balloonman_channel;
				pRChannel = (void *)audio_balloonman_channel;
				nSamples = ((sizeof(audio_balloonman_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
				break;

			case 5:
				pLChannel = (void *)audio_hiphop_channel;
				pRChannel = (void *)audio_hiphop_channel;
				nSamples = ((sizeof(audio_hiphop_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
				break;

			case 6:
				pLChannel = (void *)audio_whatiam_channel;
				pRChannel = (void *)audio_whatiam_channel;
				nSamples = ((sizeof(audio_whatiam_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK);
				break;

#if 1
			case 7:
				pLChannel = (void *)audio_withoutme_channel;
				pRChannel = (void *)audio_soulwish_channel;
				nSamples = 
					M_MIN(
						(((sizeof(audio_withoutme_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK)),
						(((sizeof(audio_soulwish_channel) * 2) + SERVICE_SAMPLE_GRANULARITY - 1) & (~SERVICE_GRANULARITY_MASK))
					);
				break;
#endif
#endif // 0

#endif // defined(BIG_AUDIO_TEST)

			}

			// ----------------------------------------------------------------

#if 1
			StartAudio(
					(void *)pLChannel
				,	(void *)pRChannel
				,	ADPCM_SAMPLE_FREQUENCY
				,	nSamples
			);

			++counter;
#else

			if ( ++counter & 1 ) {

				StartAudio(
						(void *)pLChannel
					,	(void *)pRChannel
					,	ADPCM_SAMPLE_FREQUENCY
					,	nSamples
				);

			} else {

#if defined(STEREO_TEST)

				if ( counter & 2 ) {

					StartAudio(
							(void *)pLChannel
						,	(void *)pLChannel
						,	ADPCM_SAMPLE_FREQUENCY
						,	nSamples
					);

				} else {

					StartAudio(
							(void *)pRChannel
						,	(void *)pRChannel
						,	ADPCM_SAMPLE_FREQUENCY
						,	nSamples
					);

				}

#else

				StartAudio(
						(void *)pLChannel
					,	(void *)pLChannel
					,	ADPCM_SAMPLE_FREQUENCY
					,	nSamples
				);

#endif

			}

#endif

		}

	}

#else

	void InterruptProcess(void) __attribute__ ((section(".iwram"))); 

	//
	//	InterruptProcess()
	//

	void 
	InterruptProcess(void) {

		u32 request = TRead<u16>( REG_IF );

		TWrite<u16>( REG_IF, request );

	}

#endif // defined(TEST_AUDIO)
// ============================================================================
// ============================================================================

#define SPRITE_OVERLAY_TEST

#if defined(SPRITE_OVERLAY_TEST) // SPRITE OVERLAY TEST

	// ========================================================================
	// ========================================================================

#include "./art/duck-rider-away.c"
#include "./art/duck-rider-toward.c"

#include "./art/big-teeth-away.c"
#include "./art/big-teeth-toward.c"

#include "./art/red-monster-16.c"

	// ------------------------------------------------------------------------

	typedef struct tag_OAM_ENTRY {

		u16 Attrib0;
		u16 Attrib1;
		u16 Attrib2;
		u16 RotateScale;

	} OAM_ENTRY;

	OAM_ENTRY g_OAM_echo[ 128 ];

	// ------------------------------------------------------------------------

	s16 meta_x;
	s16 meta_y;

	s16 g_FirstAvailableSpriteIndex;
	s16 g_FirstAvailableSpriteTile;

	//
	//	SetupHardwareSprite_16x16_8bpp()
	//

	void SetupHardwareSprite_16x16_8bpp(
		const int sprite
		,const int x
		,const int y
		,const int baseTile
	) __attribute__ ((section(".iwram"))); 

	void SetupHardwareSprite_16x16_8bpp(
		const int sprite
		,const int x
		,const int y
		,const int baseTile
	) {

		int px = (0 <= x) ? x : (x + 512);
		int py = (0 <= y) ? y : (y + 256);

		g_OAM_echo[ sprite ].Attrib0 = 0x2000 | ((py) & 0xff);
		g_OAM_echo[ sprite ].Attrib1 = 0x4000 | ((px) & 0x1ff);
		g_OAM_echo[ sprite ].Attrib2 = baseTile;
		g_OAM_echo[ sprite ].RotateScale = 0;
	}

	//
	//	PositionMetaSprite()
	//

	 void PositionMetaSprite( const int x, const int y ) __attribute__ ((section(".iwram"))); 

	void PositionMetaSprite( const int x, const int y ) {

		// This is bad form...

		g_FirstAvailableSpriteIndex = 0;
		g_FirstAvailableSpriteTile = 512;

		meta_x = x;
		meta_y = y;

		s32 corner_y = meta_y - 80/2;

		for ( int y = 0; y < 5; y++ ) {

			s32 corner_x = meta_x - 80/2;

			for ( int x = 0; x < 5; x++ ) {

				SetupHardwareSprite_16x16_8bpp(
					g_FirstAvailableSpriteIndex, corner_x, corner_y, g_FirstAvailableSpriteTile
				);

				corner_x += 16;

				++g_FirstAvailableSpriteIndex;

				g_FirstAvailableSpriteTile += 8;

			}

			corner_y += 16;

		}

	}

	//
	//	SetMetaSpriteFrame()
	//

	void SetMetaSpriteFrame( const u32 flags, const int speed ) __attribute__ ((section(".iwram"))); 

	void SetMetaSpriteFrame( const u32 flags, const int speed ) {

		int frame = 4;

		if ( !(P1_BUTTON_L & g.buttonsOnceAFrame ) ) {
			
			frame = 0; //6; //0;

		} else if ( !(P1_BUTTON_R & g.buttonsOnceAFrame ) ) {

			frame = 2; //8; //2;

		} else {

			if ( !(P1_DOWN & g.buttonsOnceAFrame ) ) frame -= 3;

			if ( !(P1_UP & g.buttonsOnceAFrame ) ) frame += 3;

			if ( !(P1_LEFT & g.buttonsOnceAFrame ) ) --frame;

			if ( !(P1_RIGHT & g.buttonsOnceAFrame ) ) ++frame;

		}

		pu8 pFrameData;

		if ( g.bSelectToggle ) {

			if ( 0 <= speed ) {

				pFrameData = (pu8)g_BigTeethAway;

			} else {

				pFrameData = (pu8)g_BigTeethToward;

			}

		} else {

			if ( 0 <= speed ) {

				pFrameData = (pu8)g_DuckRiderAway;

			} else {

				pFrameData = (pu8)g_DuckRiderToward;

			}

		}

		pFrameData += (frame * (80 * 80));

		DMA3Copy( (void *)(_VRAM_BASE + (80 * 1024)), (void *)pFrameData, (80 * 80) );

	}

	//
	//	SetupMetaSpriteTest()
	//

	//void SetupMetaSpriteTest() __attribute__ ((section(".iwram"))); 

	void SetupMetaSpriteTest() {

		// copy over object palette

		DMA3Copy( (void *)_OBJ_PAL, (void *)g_DuckRiderAway_Palette, sizeof(g_DuckRiderAway_Palette) );

		// secondary palette...

		DMA3Copy( (void *)(_OBJ_PAL + (240 * 2)), (void *)g_RedMonster16_Palette, sizeof(g_RedMonster16_Palette) );

		// copy over meta sprite tiles

		SetMetaSpriteFrame( ~0, 0 );

		// setup the meta sprite

		PositionMetaSprite( 120, 80 );

	}

	// ------------------------------------------------------------------------

	//
	//	SetupHardwareSprite_16x16_8bpp()
	//

	void SetupHardwareSprite_16x16_4bpp(
		const int sprite
		,const int x
		,const int y
		,const int baseTile
	) {

		int px = (0 <= x) ? x : (x + 512);
		int py = (0 <= y) ? y : (y + 256);

		g_OAM_echo[ sprite ].Attrib0 = /*0x2000 | */ ((py) & 0xff);
		g_OAM_echo[ sprite ].Attrib1 = 0x4000 | ((px) & 0x1ff);
		g_OAM_echo[ sprite ].Attrib2 = 0xf000 | baseTile;
		g_OAM_echo[ sprite ].RotateScale = 0;
	}

	void BPTLLC_Message()
	{
		if ( 1 == g.frame ) {

			DMA3Copy(
				(void *)(_VRAM_BASE + (64 * 1024) + (g_FirstAvailableSpriteTile * 32))
				,(void *)g_BPTLLC
				,sizeof(g_BPTLLC)
			);

		}

		int spriteIndex = g_FirstAvailableSpriteIndex;

		int baseTile = g_FirstAvailableSpriteTile;

		int xx = 0;

		for ( int x = 0; x < 14; x++ ) {

			SetupHardwareSprite_16x16_8bpp(
				spriteIndex++
				,xx
				,160 - 16
				,baseTile
			);

			xx += 16;

			baseTile += 8;

		}

	}

	void RunTheSpinners() {

		// DMA in the new tiles
		// --------------------------------------------------------------------

		int frame = g.frame & 15;

		pu8 pFrame = (pu8)g_RedMonster16 + (frame * ((32 * 4)*9));

		DMA3Copy(
			(void *)(_VRAM_BASE + (64 * 1024) + (g_FirstAvailableSpriteTile * 32))
			,(void *)pFrame
			, (4 * 32) * 9
		);

		// Setup the sprite
		// --------------------------------------------------------------------

		int spriteIndex = g_FirstAvailableSpriteIndex;

		int hackPhase = g.frame;

		int phaseStep = ((128 - g_FirstAvailableSpriteIndex)/9);

		if ( phaseStep ) {
		
			phaseStep = TRIG_ANGLE_360 / phaseStep;

		} else {

			phaseStep = 48;

		}

		while ( 128 > (spriteIndex + 9) ) {

			hackPhase &= TRIG_ANGLE_MASK;

			int xscale = 48 + ((48 * (*(g_SinAndCosTable + (((hackPhase * 9) + (g.frame * 7) + (spriteIndex*3))&TRIG_ANGLE_MASK)))) >> BPT_TRIG_SHIFT);

			int yscale = 32 + ((32 * (*(g_SinAndCosTable + ((hackPhase + (g.frame * 13) + (spriteIndex*5))&TRIG_ANGLE_MASK)))) >> BPT_TRIG_SHIFT);

			int px = 120 + ((xscale * (*(g_SinAndCosTable + hackPhase))) >> BPT_TRIG_SHIFT);
			int py = 80 + ((yscale * (*(g_SinAndCosTable + TRIG_ANGLE_90 + hackPhase))) >> BPT_TRIG_SHIFT);

			px -= 24;
			py -= 24;

			hackPhase += phaseStep;

			int baseTile = g_FirstAvailableSpriteTile;

			for ( int y = 0; y < 3; y++ ) {

				int xx = px;

				for ( int x = 0; x < 3; x++ ) {

					SetupHardwareSprite_16x16_4bpp(
						spriteIndex++
						,xx
						,py
						,baseTile
					);

					baseTile += 4;

					xx += 16;

				}

				py += 16;

			}

		}

	}

	// ------------------------------------------------------------------------

	void UpdateOAMFromEcho() {

		DMA3Copy( (void *)_OAM_BASE, (void *)g_OAM_echo, sizeof(g_OAM_echo) );

	}

	void MetaSpriteTest() {

		SetMetaSpriteFrame( g.buttonsOnceAFrame, g.HD.speed );

//		RunTheSpinners();
//		BPTLLC_Message();

		UpdateOAMFromEcho();

	}

#endif // SPRITE OVERLAY TEST
	
	// ========================================================================
	// ========================================================================


	// main code
	// ------------------------------------------------------------------------

	//
	//	ACTUAL_main()
	//

	int ACTUAL_main( int argc, char * argv[]  ) {

		// prepare for the video mode
		// --------------------------------------------------------------------

#if defined(HALF_RES_16BPP_MODE)
		u32 echo_DISPCNT = 5 | DISPCNT_BG2_ENABLE;
#else
		u32 echo_DISPCNT = 4 | DISPCNT_BG2_ENABLE;
#endif

#if defined(SPRITE_OVERLAY_TEST)

		echo_DISPCNT |= (DISPCNT_OBJ_1D_MAP | DISPCNT_OAM_ENABLE);

		SetupMetaSpriteTest();

#endif

		// fill in the heightmap/texture
		// --------------------------------------------------------------------

		g.tileset = 0;

		g.pTileset = (u8 *)0x2010000;

		DMA3Copy( (void *)0x2010000, (void *)g_Heightmap, sizeof(g_Heightmap) );
//		DMA3Copy( (void *)0x2010000, (void *)g_Heightmap2, sizeof(g_Heightmap2) );
//		DMA3Copy( (void *)0x2010000, (void *)g_Heightmap3, sizeof(g_Heightmap3) );
//		DMA3Copy( (void *)0x2010000, (void *)g_Heightmap4, sizeof(g_Heightmap4) );

//		DMA3Copy( (void *)0x2020000, (void *)g_Texture, sizeof(g_Texture) ); 
		DMA3Copy( (void *)0x2020000, (void *)g_Texture3, sizeof(g_Texture3) );

		// --------------------------------------------------------------------

#if defined(TEST_TILED_RENDERER) 

#if 1 

			g.pTilemap = (u8 *)0x2000000;
			DMA3Copy( (void *)0x2000000, (void *)tilemap, sizeof(tilemap) );
			DMA3Copy( (void *)0x2030000, (void *)g_Texture3_ShadeTable, sizeof(g_Texture3_ShadeTable) );

#else
		{
			int tw = 8;
			int th = 8;
			int mh = 128;
			int mw = 128;
			int rw = 256/tw;
			int rwm = rw - 1;
			int rh = 256/th;
			int rhm = rh - 1;

			g.pTilemap = (u8 *)0x2000000;

			u16 * pWriteA = (u16 *)g.pTilemap;

			for ( int y = 0; y < mh; y++ ) {

				int ty = y & rhm;

#if defined(SUPPORT_SPECIAL_TILE_BITS)

				int ys = y / rh;
				if ( ys & 1) ty ^= rhm;

#endif

				for ( int x = 0; x < mw; x++ ) {

					int tx = x & rwm;

#if defined(SUPPORT_SPECIAL_TILE_BITS)

					int xs = x / rw;

					if ( xs & 1) tx ^= rwm;

					int tile = ((ty * rw) + (tx));

					int flags = 0;

					if ( xs & 1) flags |= 1;
					if ( ys & 1) flags |= 2;

					if ( 3 == (flags & 3) ) flags |= 4;

					*pWriteA++ = flags | (tile << 3);
					*pWriteA++ = ((tile + 1024) << 2);

#else

					int tile = ((ty * rw) + (tx));

					*pWriteA++ = tile;
					*pWriteA++ = tile + 1024;

#endif

				}

			}

			// color shade table
			// ----------------------------------------------------------------

			DMA3Copy( (void *)0x2030000, (void *)g_Texture3_ShadeTable, sizeof(g_Texture3_ShadeTable) );

		}
#endif


#endif

		// --------------------------------------------------------------------

#if defined(OFFSCREEN_RENDERING)

		g.pBackBuffer = (pu8)g.pTextureTilemap + ((pu8)g.pTextureTilemap - (pu8)g.pHeightTilemap);

#endif

		// --------------------------------------------------------------------

		g.HD.altitude = 64; // 220; // 220; // 300; // 400; // 220;
		g.HD.pitch = DEFAULT_PITCH;
		g.HD.yaw = TRIG_ANGLE_90;
		g.HD.xOrg = 0; // ((UVMAP_CX*3)/2) << BPT_TRIG_SHIFT;
		g.HD.yOrg = 0; // ((UVMAP_CY*3)/2) << BPT_TRIG_SHIFT;

		// fake up VRAM and the palette
		// --------------------------------------------------------------------

		DMA3Copy( (void *)_BG_PAL, (void *)g_Texture3_Palette, sizeof(g_Texture3_Palette) );

		// DMA the background to the screen

		DMA3Copy( (void *)_VRAM_BASE, (void *)g_Background, sizeof(g_Background) );
		DMA3Copy( (void *)_VRAM_BASE2, (void *)g_Background, sizeof(g_Background) );

		// setup the display for rotated mode
		// --------------------------------------------------------------------

#if defined(ROTATED_DISPLAY_MODE)

#if defined(HALF_RES_16BPP_MODE)

		TWrite<u16>( REG_BG2PA, 0 );
		TWrite<u16>( REG_BG2PB, 256 ); // ((256 * REND_HEIGHT) / VRAM_ACTUAL_HEIGHT) );

	#if defined(TEST_50_50_BLEND)
		TWrite<u16>( REG_BG2PC, 64 ); // (256 * REND_WIDTH/HACK_DUP_COL_COUNT) / VRAM_ACTUAL_WIDTH );
	#else
		TWrite<u16>( REG_BG2PC, 128 ); // (256 * REND_WIDTH/HACK_DUP_COL_COUNT) / VRAM_ACTUAL_WIDTH );
	#endif

		TWrite<u16>( REG_BG2PD, 0 );

#else

		TWrite<u16>( REG_BG2PA, 0 );
		TWrite<u16>( REG_BG2PB, ((256 * REND_HEIGHT) / VRAM_ACTUAL_HEIGHT) );
		TWrite<u16>( REG_BG2PC, (256 * REND_WIDTH) / VRAM_PITCH );
		TWrite<u16>( REG_BG2PD, 0 );

#endif

#else

		TWrite<u16>( REG_BG2PA, (256 * REND_WIDTH) / VRAM_PITCH );
		TWrite<u16>( REG_BG2PB, 0 );
		TWrite<u16>( REG_BG2PC, 0 );
		TWrite<u16>( REG_BG2PD, ((256 * REND_HEIGHT) / VRAM_ACTUAL_HEIGHT) );

#endif

		TWrite<u16>( REG_DISPSTAT, 0x00000100 );
		TWrite<u16>( REG_DISPCNT, echo_DISPCNT );


		// start the audio test
		// --------------------------------------------------------------------

#if defined(TEST_AUDIO)

		InitAudioHardware();

		AudioTest();

#endif

		// the main loop
		// --------------------------------------------------------------------

		for ( ;; ) {

			// read the joypad
			// ----------------------------------------------------------------

			g.lastButtonsOnceAFrame = g.buttonsOnceAFrame;

			g.buttonsOnceAFrame = TRead<u16>( REG_P1 );

#if defined(TEST_AUDIO)

			if ( !(P1_START & g.buttonsOnceAFrame) ) {

				AudioTest();

				while ( !(P1_START & g.buttonsOnceAFrame) ) {

					g.buttonsOnceAFrame = TRead<u16>( REG_P1 );

				}

#if 0
		switch ( (g.tileset >> 2) & 3 ) {

		default:
		case 0:
//			DMA3Copy( (void *)0x2020000, (void *)g_Texture4, sizeof(g_Texture) );
//			DMA3Copy( (void *)0x2030000, (void *)g_Texture4_ShadeTable, sizeof(g_Texture2_ShadeTable) );
			DMA3Copy( (void *)0x2020000, (void *)g_Texture3, sizeof(g_Texture) );
			DMA3Copy( (void *)0x2030000, (void *)g_Texture3_ShadeTable, sizeof(g_Texture_ShadeTable) );
			break;

#if 0
		case 1:
			DMA3Copy( (void *)0x2020000, (void *)g_Texture2, sizeof(g_Texture) );
			DMA3Copy( (void *)0x2030000, (void *)g_Texture2_ShadeTable, sizeof(g_Texture_ShadeTable) );
			break;

		case 2:
			DMA3Copy( (void *)0x2020000, (void *)g_Texture, sizeof(g_Texture) );
			DMA3Copy( (void *)0x2030000, (void *)g_Texture_ShadeTable, sizeof(g_Texture_ShadeTable) );
			break;

		case 3:
			DMA3Copy( (void *)0x2020000, (void *)g_Texture3, sizeof(g_Texture) );
			DMA3Copy( (void *)0x2030000, (void *)g_Texture3_ShadeTable, sizeof(g_Texture_ShadeTable) );
			break;
#endif

		}
#endif

#if 0
		switch ( g.tileset++ % 4 ) {

		default:
		case 0:
			DMA3Copy( (void *)0x2010000, (void *)g_Heightmap3, sizeof(g_Heightmap3) );
			break;

		case 1:
			DMA3Copy( (void *)0x2010000, (void *)g_Heightmap2, sizeof(g_Heightmap2) );
			break;

		case 2:
			DMA3Copy( (void *)0x2010000, (void *)g_Heightmap, sizeof(g_Heightmap) );
			break;

		case 3:
			DMA3Copy( (void *)0x2010000, (void *)g_Heightmap4, sizeof(g_Heightmap4) );
			break;

		}
#endif

			}

#endif
			// control the angle
			// ----------------------------------------------------------------

			bool bSelectPressed = (0 == ((P1_SELECT & g.buttonsOnceAFrame )));

			if ( bSelectPressed ) {

			}

			if ( (P1_SELECT & g.buttonsOnceAFrame) != (P1_SELECT & g.lastButtonsOnceAFrame) ) {

				if ( bSelectPressed ) {

					g.bSelectToggle = !g.bSelectToggle;

				}

			}

#if 1
			bool bLeftPressed = (0 == ((P1_BUTTON_L & g.buttonsOnceAFrame )));
			bool bRightPressed = (0 == ((P1_BUTTON_R & g.buttonsOnceAFrame )));
			bool bLPressed = (0 == (P1_LEFT & g.buttonsOnceAFrame));
			bool bRPressed = (0 == (P1_RIGHT & g.buttonsOnceAFrame));
#else
			bool bLPressed = (0 == ((P1_BUTTON_L & g.buttonsOnceAFrame )));
			bool bRPressed = (0 == ((P1_BUTTON_R & g.buttonsOnceAFrame )));
			bool bLeftPressed = (0 == (P1_LEFT & g.buttonsOnceAFrame));
			bool bRightPressed = (0 == (P1_RIGHT & g.buttonsOnceAFrame));
#endif

			s32 originalYaw = g.HD.yaw;

			static s32 angleAdjustment = 16;

			s32 angleChange = 4; // - (angleAdjustment>>2);

			if ( (!bSelectPressed) && (bLeftPressed || bLPressed) ) {

				g.HD.yaw = (TRIG_ANGLE_360 + (g.HD.yaw - (angleChange + 0))) & TRIG_ANGLE_MASK;

				if ( 2 <= angleAdjustment ) angleAdjustment -= 2;

			}

			if ( (!bSelectPressed) && (bRightPressed || bRPressed) ) {

				g.HD.yaw = (g.HD.yaw + (angleChange + 0)) & TRIG_ANGLE_MASK;

				if ( 2 <= angleAdjustment ) angleAdjustment -= 2;

			}

			if ( 17 > angleAdjustment ) ++angleAdjustment;

			// control the speed
			// ----------------------------------------------------------------

			if ( !(P1_BUTTON_B & g.buttonsOnceAFrame) ) {

				if ( -(8 * 256 * SPEED_UP_MOVEMENT) < g.HD.speed ) {

					g.HD.speed -= 64;

				}

			}

			if ( !(P1_BUTTON_A & g.buttonsOnceAFrame) ) {

				if ( (8 * 256 * SPEED_UP_MOVEMENT) > g.HD.speed ) {

					g.HD.speed += 64;

				}

			}

			// handle altidude
			// ----------------------------------------------------------------

			bool bUPressed = (0 == ((P1_UP & g.buttonsOnceAFrame )));
			bool bDPressed = (0 == ((P1_DOWN & g.buttonsOnceAFrame )));

#if 1

			if ( bUPressed ) {

				if ( 16 < g.HD.altitude ) g.HD.altitude -= (8 * SPEED_UP_MOVEMENT);

			} else if ( bDPressed ) {

				if ( 1000 > g.HD.altitude ) g.HD.altitude += (8 * SPEED_UP_MOVEMENT);

			}

#if 1

			if ( bSelectPressed ) {

				if ( bLeftPressed || bRightPressed ) { // bLPressed || bRPressed ) {

					if ( bLeftPressed ) { // bLPressed ) {

						g.HD.pitch -= 8;

					}

					if ( bRightPressed ) { // bRPressed ) {

						g.HD.pitch += 8;

					}

				}

			} else {

				originalYaw = g.HD.yaw;

				int stepSpeed = ((g.HD.speed * 4) / 4);

				if ( 0 > stepSpeed ) {

					stepSpeed = 1024 + ((256 * stepSpeed) / (8 * 256 * SPEED_UP_MOVEMENT));

				} else {

					stepSpeed = 1024 - ((256 * stepSpeed) / (8 * 256 * SPEED_UP_MOVEMENT));

				}

				if ( bRightPressed ) {
//				if ( bLPressed || bRightPressed ) {

					int sideStep = (originalYaw + (TRIG_ANGLE_360 - 90)) & TRIG_ANGLE_MASK;

					g.HD.yOrg += (stepSpeed * (*(g_SinAndCosTable + sideStep))) >> 8;

					g.HD.xOrg += (stepSpeed * (*(g_SinAndCosTable + TRIG_ANGLE_90 + sideStep))) >> 8;

				} else if ( bLeftPressed ) {
//				} else if ( bRPressed || bLeftPressed ) {

					int sideStep = (originalYaw + (TRIG_ANGLE_360 + 90)) & TRIG_ANGLE_MASK;

					g.HD.yOrg += (stepSpeed * (*(g_SinAndCosTable + sideStep))) >> 8;

					g.HD.xOrg += (stepSpeed * (*(g_SinAndCosTable + TRIG_ANGLE_90 + sideStep))) >> 8;

				}

			}
#endif


#else


			if ( bUPressed || bDPressed ) {

				if ( bUPressed ) {

					g.HD.pitch -= (8 * SPEED_UP_MOVEMENT);

				}

				if ( bDPressed ) {

					g.HD.pitch += (8 * SPEED_UP_MOVEMENT);

				}

			} else {

#if 0
				if ( DEFAULT_PITCH > g.HD.pitch ) {

					g.HD.pitch += 4;

				} else if ( DEFAULT_PITCH < g.HD.pitch ) {

					g.HD.pitch -= 4;

				}
#endif

			}

			// handle altidude
			// ----------------------------------------------------------------

			g.HD.altitude += (g.HD.speed * (g.HD.pitch - DEFAULT_PITCH)) >> 14;

			if ( MAX_ALTITUDE < g.HD.altitude ) {

				g.HD.altitude = MAX_ALTITUDE;

			}

#endif


			// Handle movement constant speed
			// ----------------------------------------------------------------

			if ( g.HD.speed ) {

				g.HD.yOrg += (g.HD.speed * (*(g_SinAndCosTable + g.HD.yaw))) >> 8;

				g.HD.xOrg += (g.HD.speed * (*(g_SinAndCosTable + TRIG_ANGLE_90 + g.HD.yaw))) >> 8;

			}

			// check to see if the altitude needs to adjust for inside land
			// ----------------------------------------------------------------

#if 0 // this isn't quite right!

			{

				int rx = (g.HD.yOrg >> BPT_TRIG_SHIFT); // + ((g.HD.speed * (*(g_SinAndCosTable + g.HD.yaw))) >> 6);

				int ry = (g.HD.xOrg >> BPT_TRIG_SHIFT); // + ((g.HD.speed * (*(g_SinAndCosTable + TRIG_ANGLE_90 + g.HD.yaw))) >> 6);

				u16 * pTileMapEntry = 
					(u16 *)g.pTilemap + 
					(((((ry >> 3) & 127) * 128) + ((rx >> 3) & 127)) << 1);

				rx &= 7;
				ry &= 7;

				int whichTile = *(pTileMapEntry + 1);

				static int averageHeightValue = 0;

				int heightValue = 
					*((pu8)g.pTileset + (whichTile * 64) + (ry * 8) + rx);

				if ( (averageHeightValue - 32) > g.HD.altitude ) {

					g.HD.altitude = (averageHeightValue + g.HD.altitude) / 2;

				}

				averageHeightValue = (heightValue + averageHeightValue) / 2;

#if 0
				else {

					g.HD.altitude -= 16;

					if ( 16 > g.HD.altitude ) g.HD.altitude = 16;

				}
#endif

			}

#endif

			// get the backbuffer
			// ----------------------------------------------------------------

#if !defined(OFFSCREEN_RENDERING)

			if ( echo_DISPCNT & DISPCNT_BACKBUFFER ) {

				g.pBackBuffer = reinterpret_cast<pu8>( (void *)_VRAM_BASE );

			} else {

				g.pBackBuffer = reinterpret_cast<pu8>( (void *)_VRAM_BASE2 );

			}

#endif

			// reset the timer
			// ----------------------------------------------------------------

#if defined(REPORT_RENDER_TIME)

			TWrite<u16>( REG_TM3CNT, 0 );
			TWrite<u16>( REG_TM3D, 0 );
			TWrite<u16>( REG_TM3CNT, TIME_FREQUENCY_1024 | TIME_ENABLE );

			u32 start = TRead<u16>( REG_TM3D );

#endif

			// fill the background (HACK)
			// ----------------------------------------------------------------

#if defined(HALF_RES_16BPP_MODE)

	#if defined(TEST_50_50_BLEND) 

			if ( echo_DISPCNT & DISPCNT_BACKBUFFER ) {

				DMA3Copy( (void *)_VRAM_BASE, (void *)_VRAM_BASE2, (REND_WIDTH * VRAM_PITCH) );

			} else {

				DMA3Copy( (void *)_VRAM_BASE2, (void *)_VRAM_BASE, (REND_WIDTH * VRAM_PITCH) );

			}

	#else

#if 0 // the height field renderer - does a fake sky now so no need to clear...

			*(pu16)g.pBackBuffer = 0xffff; // 27 << 10; // prime the value

			TWrite<u32>( REG_DMA3SAD, (u32)g.pBackBuffer );
			TWrite<u32>( REG_DMA3DAD, (u32)(g.pBackBuffer + 2) );
			TWrite<u32>( REG_DMA3CNT, ((REND_WIDTH * VRAM_PITCH)/2 - 1) | (DMA_ENABLE | DMA_SOURCE_FIXED) );

#endif

	#endif
			
#elif defined(HACK_ODD_EVEN_RENDERER)

			if ( echo_DISPCNT & DISPCNT_BACKBUFFER ) {

				DMA3Copy( (void *)_VRAM_BASE, (void *)_VRAM_BASE2, (REND_HEIGHT * VRAM_PITCH) );

			} else {

				DMA3Copy( (void *)_VRAM_BASE2, (void *)_VRAM_BASE, (REND_HEIGHT * VRAM_PITCH) );

			}

#else

			DMA3Copy( (void *)g.pBackBuffer, (void *)g_Background, sizeof(g_Background) );

#endif

			// render the height field
			// ----------------------------------------------------------------

			RenderHeightfield();

			//RenderParticles();

#if defined(TEST_AUDIO) && defined(LOOPING_SAMPLE_TEST) && (!defined(HANDLE_SOUND_PROCESSING_IN_VBLANK_IRQ))

			u32 audioTime = TRead<u16>( REG_TM3D );

			ServiceAudio( false);

			audioTime = TRead<u16>( REG_TM3D ) - audioTime;

#endif

#if defined(REPORT_RENDER_TIME)
			if ( g.bSelectToggle ) {

				u32 took = TRead<u16>( REG_TM3D ) - start;

	//			if ( took ) DisplayDecimalNumber(  (pu8)g.pBackBuffer + 8 + ((REND_WIDTH - 10) * VRAM_PITCH), (16686/took) + ((g.interuptCounter&255) * 10000) );
				if ( took ) DisplayDecimalNumber(  (pu8)g.pBackBuffer + 8 + ((REND_WIDTH - 10) * VRAM_PITCH), 16686/took );
	//			if ( took ) DisplayDecimalNumber(  (pu8)g.pBackBuffer + 8 + ((REND_WIDTH - 10) * VRAM_PITCH), took + (audioTime * 10000) );
	//			if ( took ) DisplayDecimalNumber(  (pu8)g.pBackBuffer + (VRAM_PITCH * (REND_HEIGHT - 10)), 16686/took );

			}
#endif

			// DMA the offscreen
			// ----------------------------------------------------------------

#if defined(OFFSCREEN_RENDERING)

			if ( echo_DISPCNT & DISPCNT_BACKBUFFER ) {

				DMA3Copy( (void *)_VRAM_BASE, (void *)g.pBackBuffer, REND_WIDTH * REND_HEIGHT );

				if ( took ) DisplayDecimalNumber(  (pu8)_VRAM_BASE + 8 + ((REND_WIDTH - 10) * VRAM_PITCH), 16686/took );

			} else {

				DMA3Copy( (void *)_VRAM_BASE2, (void *)g.pBackBuffer, REND_WIDTH * REND_HEIGHT );

				if ( took ) DisplayDecimalNumber(  (pu8)_VRAM_BASE2 + 8 + ((REND_WIDTH - 10) * VRAM_PITCH), 16686/took );

			}

#endif

			// deal with video mode
			// ----------------------------------------------------------------

#if !defined(LOOPING_SAMPLE_TEST)
#if 1
			while ( 160 > TRead<u16>( REG_VCOUNT ) ) {

				// what about audio service?
			
			} // wait for vblank
#endif
#endif

#if defined(SPRITE_OVERLAY_TEST)

			MetaSpriteTest();

#endif

			echo_DISPCNT ^= DISPCNT_BACKBUFFER;

			TWrite<u16>( REG_DISPCNT, echo_DISPCNT );

			++g.frame;

		}

		return 0;

	}

	// ------------------------------------------------------------------------

}; // extern "C"

extern "C" {
	
	int main( int argc, char * argv[]  ) {

		return ACTUAL_main(argc,argv);
		
	}

};
