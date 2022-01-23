// GBARegs.H: memory mapped hardware register addresses
//
//	-- FILE NOTES --
//

#if !defined(__GBAREGS_H__)
#define __GBAREGS_H__

// ----------------------------------------------------------------------------
// Trying to force the compiler to always deal with reading the registers!
// ----------------------------------------------------------------------------

#if defined(__cplusplus)

//
//	TRead
//

template< class T > inline T TRead( const unsigned offset ) {

	volatile T * p = reinterpret_cast< T * >( offset );

	return *p;

}

//
//	TWrite
//

template< class T > inline void TWrite( const unsigned offset, const T value ) {

	*(reinterpret_cast< T * >( offset )) = value;

}

#endif /* defined(__cplusplus) */

// ----------------------------------------------------------------------------
// GBA Graphics registers
// ----------------------------------------------------------------------------

#define _VIDC_BASE	0x4000000						// Video Controller Register Base Address
#define _VRAM_BASE	0x6000000						// VRAM page 0
#define _VRAM_BASE2	0x600A000						// VRAM page 1
#define _BG_PAL		0x5000000						// background palette (256 * RGB15)
#define _OBJ_PAL	0x5000200						// sprite palette (256 * RGB15)
#define _OAM_BASE	0x7000000						// sprite OAM memory

// (DISPCNT) -------------------------------------------------------------------

#define REG_DISPCNT				(0x4000000)			// display control register

#define	DISPCNT_BACKBUFFER		0x0010				// backbuffer bit
#define DISPCNT_MODE_BITS		0x0007				// mode bits
#define DISPCNT_BG0_ENABLE		0x0100				// BG0 ENABLE
#define DISPCNT_BG1_ENABLE		0x0200				// BG1 ENABLE
#define DISPCNT_BG2_ENABLE		0x0400				// BG2 ENABLE
#define DISPCNT_BG3_ENABLE		0x0800				// BG3 ENABLE
#define DISPCNT_OBJ_1D_MAP		0x0040				// 1d object mode
#define DISPCNT_OAM_ENABLE		0x1000				// enable sprites (OAM)

// -----------------------------------------------------------------------------

#define REG_DISPSTAT	(0x4000004)					// display status
#define REG_VCOUNT		(0x4000006)					// LCD y scanline

#define REG_BG0CNT		(0x4000008)					// BG0 control register
#define REG_BG1CNT		(0x400000A)					// BG1 control register
#define REG_BG2CNT		(0x400000C)					// BG2 control register
#define REG_BG3CNT		(0x400000E)					// BG3 control register

#define REG_BG0HOFS		(0x4000010)					// BG0 Horizontal offset
#define REG_BG0VOFS		(0x4000012)					// BG0 Vertical offset
#define REG_BG1HOFS		(0x4000014)					// BG1 Horizontal offset
#define REG_BG1VOFS		(0x4000016)					// BG1 Vertical offset
#define REG_BG2HOFS		(0x4000018)					// BG2 Horizontal offset
#define REG_BG2VOFS		(0x400001A)					// BG2 Vertical offset
#define REG_BG3HOFS		(0x400001C)					// BG3 Horizontal offset
#define REG_BG3VOFS		(0x400001E)					// BG3 Vertical offset

#define REG_BG2PA		(0x4000020)					// BG2 same line delta x
#define REG_BG2PB		(0x4000022)					// BG2 next line delta x
#define REG_BG2PC		(0x4000024)					// BG2 same line delta y
#define REG_BG2PD		(0x4000026)					// BG2 next line delta y

#define REG_BG2X		(0x4000028)					// BG2 x start
#define REG_BG2X_L		(0x4000028)					// BG2 x start (LO)
#define REG_BG2X_H		(0x400002A)					// BG2 x start (HI)
#define REG_BG2Y		(0x400002C)					// BG2 y start
#define REG_BG2Y_L		(0x400002C)					// BG2 y start (LO)
#define REG_BG2Y_H		(0x400002E)					// BG2 y start (HI)

#define REG_BG3PA		(0x4000030)					// BG3 same line delta x
#define REG_BG3PB		(0x4000032)					// BG3 next line delta x
#define REG_BG3PC		(0x4000034)					// BG3 same line delta y
#define REG_BG3PD		(0x4000036)					// BG3 next line delta y

#define REG_BG3X		(0x4000038)					// BG3 x start
#define REG_BG3X_L		(0x4000038)					// BG3 x start (LO)
#define REG_BG3X_H		(0x400003A)					// BG3 x start (HI)
#define REG_BG3Y		(0x400003C)					// BG3 y start
#define REG_BG3Y_L		(0x400003C)					// BG3 y start (LO)
#define REG_BG3Y_H		(0x400003E)					// BG3 y start (HI)

#define REG_WIN0H		(0x4000040)					// Window 0 upper left & lower right x coordinate
#define REG_WIN1H		(0x4000042)					// Window 1 upper left & lower right x coordinate
#define REG_WIN0V		(0x4000044)					// Window 0 upper left & lower right y coordinate
#define REG_WIN1V		(0x4000046)					// Window 1 upper left & lower right y coordinate
#define REG_WININ		(0x4000048)					// Inside Window 1 / Window 0 control settings
#define REG_WINOUT		(0x400004A)					// Outside Window 1 / Window 0 control settings

#define REG_MOSAIC		(0x400004C)					// Mosaic control register
#define REG_BLDMOD		(0x4000050)					// Blend mode control register

// ----------------------------------------------------------------------------
// GBA Sound registers
// ----------------------------------------------------------------------------

#define REG_SOUND1CNT_L (0x04000060)       			// sound 1 sweep control  
#define REG_SOUND1CNT_H (0x04000062)       			// sound 1 length, wave duty and envelope control 
#define REG_SOUND1CNT_X (0x04000064)       			// sound 1 frequency, reset and loop control 
#define REG_SOUND2CNT_L (0x04000068)       			// sound 2 length, wave duty and envelope control 
#define REG_SOUND2CNT_H (0x0400006C)       			// sound 2 frequency, reset and loop control 
#define REG_SOUND3CNT_L (0x04000070)       			// sound 3 enable and wave ram bank control 
#define REG_SOUND3CNT_H (0x04000072)       			// sound 3 sound length and output level control 
#define REG_SOUND3CNT_X (0x04000074)       			// sound 3 frequency, reset and loop control 
#define REG_SOUND4CNT_L (0x04000078)       			// sound 4 length, output level and envelope control 
#define REG_SOUND4CNT_H (0x0400007C)       			// sound 4 noise parameters, reset and loop control 
#define REG_SOUNDCNT_L  (0x04000080)       			// sound 1-4 output level and stereo control 
#define REG_SOUNDCNT_H  (0x04000082)       			// direct sound control and sound 1-4 output ratio 
#define REG_SOUNDCNT_X  (0x04000084)       			// master sound enable and sound 1-4 play status 
#define REG_SOUNDBIAS   (0x04000088)       			// sound bias and amplitude resolution control 
#define REG_WAVE_RAM0_L (0x04000090)       			// sound 3 samples 0-3 
#define REG_WAVE_RAM0_H (0x04000092)       			// sound 3 samples 4-7 
#define REG_WAVE_RAM1_L (0x04000094)       			// sound 3 samples 8-11 
#define REG_WAVE_RAM1_H (0x04000096)       			// sound 3 samples 12-15 
#define REG_WAVE_RAM2_L (0x04000098)       			// sound 3 samples 16-19 
#define REG_WAVE_RAM2_H (0x0400009A)       			// sound 3 samples 20-23 
#define REG_WAVE_RAM3_L (0x0400009C)       			// sound 3 samples 23-27 
#define REG_WAVE_RAM3_H (0x0400009E)       			// sound 3 samples 28-31 
#define REG_FIFO_A      0x040000A0              	// direct sound channel A samples 0-3 
#define REG_FIFO_A_L    0x040000A0              	// direct sound channel A samples 0-1 
#define REG_FIFO_A_H    0x040000A2              	// direct sound channel A samples 2-3 
#define REG_FIFO_B      0x040000A4              	// direct sound channel B samples 0-3 
#define REG_FIFO_B_L    0x040000A4              	// direct sound channel B samples 0-1 
#define REG_FIFO_B_H    0x040000A6              	// direct sound channel B samples 2-3 

// ----------------------------------------------------------------------------
// GBA DMA registers
// ----------------------------------------------------------------------------

#define REG_DMA0SAD		(0x40000B0)
#define REG_DMA0SAD_L	(0x40000B0)
#define REG_DMA0SAD_H	(0x40000B2)
#define REG_DMA0DAD		(0x40000B4)
#define REG_DMA0DAD_L	(0x40000B4)
#define REG_DMA0DAD_H	(0x40000B6)
#define REG_DMA0CNT		(0x40000B8)
#define REG_DMA0CNT_L	(0x40000B8)
#define REG_DMA0CNT_H	(0x40000BA)
#define REG_DMA1SAD		(0x40000BC)
#define REG_DMA1SAD_L	(0x40000BC)
#define REG_DMA1SAD_H	(0x40000BE)
#define REG_DMA1DAD		(0x40000C0)
#define REG_DMA1DAD_L	(0x40000C0)
#define REG_DMA1DAD_H	(0x40000C2)
#define REG_DMA1CNT		(0x40000C4)
#define REG_DMA1CNT_L	(0x40000C4)
#define REG_DMA1CNT_H	(0x40000C6)
#define REG_DMA2SAD		(0x40000C8)
#define REG_DMA2SAD_L	(0x40000C8)
#define REG_DMA2SAD_H	(0x40000CA)
#define REG_DMA2DAD		(0x40000CC)
#define REG_DMA2DAD_L	(0x40000CC)
#define REG_DMA2DAD_H	(0x40000CE)
#define REG_DMA2CNT		(0x40000D0)
#define REG_DMA2CNT_L	(0x40000D0)
#define REG_DMA2CNT_H	(0x40000D2)
#define REG_DMA3SAD		(0x40000D4)
#define REG_DMA3SAD_L	(0x40000D4)
#define REG_DMA3SAD_H	(0x40000D6)
#define REG_DMA3DAD		(0x40000D8)
#define REG_DMA3DAD_L	(0x40000D8)
#define REG_DMA3DAD_H	(0x40000DA)
#define REG_DMA3CNT		(0x40000DC)
#define REG_DMA3CNT_L	(0x40000DC)
#define REG_DMA3CNT_H	(0x40000DE)

#define DMA_ENABLE							0x80000000
#define DMA_INTERUPT_ENABLE					0x40000000
#define DMA_START_ON_FIFO_EMPTY				0x30000000
#define DMA_TIMEING_IMMEDIATE				0x00000000
#define DMA_TIMEING_VBLANK					0x10000000
#define DMA_TIMEING_HBLANK					0x20000000
#define DMA_TIMEING_SYNC_TO_DISPLAY			0x30000000
#define DMA_16								0x00000000
#define DMA_32								0x04000000
#define DMA_REPEAT							0x02000000
#define DMA_SOURCE_INCREMENT				0x00000000
#define DMA_SOURCE_DECREMENT				0x00800000
#define DMA_SOURCE_FIXED					0x01000000
#define DMA_DEST_INCREMENT					0x00000000
#define DMA_DEST_DECREMENT					0x00200000
#define DMA_DEST_FIXED						0x00400000
#define DMA_DEST_RELOAD						0x00600000

// ----------------------------------------------------------------------------
// GBA Timer registers
// ----------------------------------------------------------------------------

#define REG_TM0D       						(0x4000100)
#define REG_TM0CNT     						(0x4000102)
#define REG_TM1D       						(0x4000104)
#define REG_TM1CNT     						(0x4000106)
#define REG_TM2D       						(0x4000108)
#define REG_TM2CNT     						(0x400010A)
#define REG_TM3D       						(0x400010C)
#define REG_TM3CNT     						(0x400010E)

#define TIME_FREQUENCY_SYSTEM				0x00
#define TIME_FREQUENCY_64					0x01
#define TIME_FREQUENCY_256					0x02
#define TIME_FREQUENCY_1024					0x03
#define TIME_OVERFLOW						0x04
#define TIME_IRQ_ENABLE						0x40
#define TIME_ENABLE							0x80

// ----------------------------------------------------------------------------
// GBA Interupt registers
// ----------------------------------------------------------------------------

#define REG_IE								(0x4000200)
#define REG_IF								(0x4000202)
#define REG_IME								(0x4000208)

#define REG_INTR_HANDLER_ADDRESS			0x3007FFC

#define INTERUPT_VBLANK						(0x0001)
#define INTERUPT_TIMER_0					(0x0008)
#define INTERUPT_TIMER_1					(0x0010)
#define INTERUPT_TIMER_2					(0x0020)
#define INTERUPT_TIMER_3					(0x0040)

// ----------------------------------------------------------------------------
// GBA Input registers
// ----------------------------------------------------------------------------

#define REG_P1								(0x4000130)

#define P1_BUTTON_A							0x0001
#define P1_BUTTON_B							0x0002
#define P1_SELECT							0x0004
#define P1_START							0x0008
#define P1_RIGHT							0x0010
#define P1_LEFT								0x0020
#define P1_UP								0x0040
#define P1_DOWN								0x0080
#define P1_BUTTON_R							0x0100
#define P1_BUTTON_L							0x0200
#define P1_DPAD								(P1_RIGHT|P1_LEFT|P1_UP|P1_DOWN)

// ----------------------------------------------------------------------------

#endif /* !defined(__GBAREGS_H__) */

