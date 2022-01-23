@@ ============================================================================
@@
@@	armhmap.s
@@
@@ The MIT License (MIT)
@@
@@ Copyright (c) 2002- Brad P. Taylor
@@
@@ Permission is hereby granted, free of charge, to any person obtaining a copy
@@ of this software and associated documentation files (the "Software"), to deal
@@ in the Software without restriction, including without limitation the rights
@@ to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
@@ copies of the Software, and to permit persons to whom the Software is
@@ furnished to do so, subject to the following conditions:
@@
@@ The above copyright notice and this permission notice shall be included in all
@@ copies or substantial portions of the Software.
@@
@@ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@@ IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@@ FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@@ AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@@ LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@@ OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@@ SOFTWARE.
@@
@@	-- FILE NOTES --
@@
@@	the current protype build has the following memory map assumptions
@@  (see folder memory map document)
@@
@@  -- TODOS --
@@
@@ TODO need a concept instead of shade tables!!! (like a palette/tileset per scanline based on angle or something)
@@ the shade table is 32 entries for each 256 colors might be better spent as a tiled screen based detail texture
@@ or a dithering pattern of some kind?
@@
@@ TODO - need to think about instead of duplicating columns using
@@ 8bpp mode and 4x2 wide textures (with do a fake swap dithering of some kind)
@@
@@ ============================================================================

@ -----------------------------------------------------------------------------
@ FUNCTION GENERATION CONTROL
@ -----------------------------------------------------------------------------

.SET COMPILE_HEIGHTMAP_RENDERER, 1

@ -----------------------------------------------------------------------------
@ CODE GENERATION OPTIONS
@ -----------------------------------------------------------------------------

@.SET TEST_50_50_BLEND, 1

@.SET SPECIAL_CAST_HALVED_SAMPLE_MODE, 1
@.SET WIDE_RENDERER_MODE, 1
@.SET WIDE_DUP_COLOR_MODE, 1
.SET PROJECTED_PIXEL_HEIGHT_BIAS, -32
@.SET PROJECTED_PIXEL_HEIGHT_BIAS, 0

.SET ROTATED_DISPLAY_MODE, 1 

.SET EMBED_RAY_HIT_WRITE, 1
.IFDEF EMBED_RAY_HIT_WRITE
	.SET UNROLL_RAY_SEARCH_COUNT, 1
	.SET UNROLL_RAY_HIT_COUNT, 4
.ELSE
	.SET UNROLL_RAY_SEARCH_COUNT, 4
	.SET UNROLL_RAY_HIT_COUNT, 1
.ENDIF

.SET USE_TILED_HEIGHT_MAP, 1
.SET USE_TILED_TEXTURE_MAP, 1

@.IFDEF SPECIAL_CAST_HALVED_SAMPLE_MODE
@	.SET SCALE_DXDYDZPH_STEPS, 1
@.ENDIF

@.SET USE_DEPTH_TEXTURE_SET, 1

.SET RENDER_FAKE_SKY, 1
@.SET USE_4X_WIDE_TEXTURES, 1
@.SET ROR_1X4_TEXEL_MODE, 1

@ -----------------------------------------------------------------------------

.SET DUP_RENDER_COLS, 1

@ -----------------------------------------------------------------------------
@ TODO - instead of duplicating add another mode for writing the uv as the dup (16 bit X5.Z6.Y5)
@.SET DUP_WRITE_MODULO_XYZ_MODE
@ -----------------------------------------------------------------------------

.SET FLIP_VERTICALLY, 1
@.SET USE_CONSTANT_FOR_HEIGHT, 1
@.SET INVERT_HEIGHT, 1
@.SET USE_HEIGHT_FOR_TEXTURE_VALUE, 1
@.SET FAKE_WIDE_TEXTURES, 1
@.SET HALFWORD_FAKE_WIDE_TEXTURES, 1
.SET USE_HALFWORD_WRITES, 1

@.SET DBUG_SHOW_DUPLICATE_WRITES, 1
.SET SUPPORT_FLEXIBLE_RENDER_HEIGHT, 1
.SET USE_ONE_TILESET_FOR_BOTH_TILEMAPS, 1

@ -----------------------------------------------------------------------------

@.SET RENDER_DEPTH_INSTEAD_OF_COLORS, 1
.SET USE_COLOR_SHADE_TABLE, 1

.IFDEF USE_COLOR_SHADE_TABLE
	.SET COLOR_SHADES_SHIFT, 5
	.SET DEPTH_TO_SHADE_SHIFT, 3
	.SET USE_16BPP_SHADE_TABLE_ENTRIES, 1
.ENDIF

@ -----------------------------------------------------------------------------
@ CONSTANTS
@ -----------------------------------------------------------------------------

.IFNDEF SUPPORT_FLEXIBLE_RENDER_HEIGHT
	.SET REND_HEIGHT, 160
.ENDIF

@ -----------------------------------------------------------------------------

.SET V_SCALE_LOG2, 11
.SET V_SCALE, (1 << V_SCALE_LOG2)
.SET DEPTH_CLIP, ((((V_SCALE)*256)*3)/4)
.SET HSCALE_SHIFT, 16
.SET DBUG_CONSTANT_HEIGHT, 1

@ -----------------------------------------------------------------------------
@ Height texture step scale factor
@ -----------------------------------------------------------------------------

.IFDEF SCALE_DXDYDZPH_STEPS
	.IFDEF TEST_50_50_BLEND
		.SET DXDYDZPH_SCALE_SHIFT, 1
	.ELSE
		.SET DXDYDZPH_SCALE_SHIFT, 1
	.ENDIF
	.SET DXDYDZPH_SCALE, (1 << DXDYDZPH_SCALE_SHIFT)
.ENDIF

@ -----------------------------------------------------------------------------

.IFDEF SPECIAL_CAST_HALVED_SAMPLE_MODE
	.SET BPT_TRIG_SHIFT, (14 + 1)
.ELSE
	.SET BPT_TRIG_SHIFT, (14)
.ENDIF

@ -----------------------------------------------------------------------------
@ PITCH & PIXEL INFO
@ -----------------------------------------------------------------------------

@.SET BYTES_PER_PIXEL, 1
.SET BYTES_PER_PIXEL, 2

.IFDEF ROTATED_DISPLAY_MODE

	.IF (2 == BYTES_PER_PIXEL)
		.SET RENDER_EVERY_ROW_MODE, 1
		.SET COL_STRIDE, 320
	.ELSE
		.SET COL_STRIDE, 240
	.ENDIF

	.IFDEF RENDER_EVERY_ROW_MODE
		.SET ROW_STRIDE, BYTES_PER_PIXEL
	.ELSE
		.SET ROW_STRIDE, (BYTES_PER_PIXEL * 2)
	.ENDIF
	
.ELSE

	.SET COL_STRIDE, 2
	.SET ROW_STRIDE, 240
	
.ENDIF

@ -----------------------------------------------------------------------------
@ TILED HEIGHTMAP CONSTANTS
@ -----------------------------------------------------------------------------

.IFDEF USE_TILED_HEIGHT_MAP

	.SET TILE_MAP_HSHIFT, 7
	.SET TILE_MAP_HMASK, ((1 << TILE_MAP_HSHIFT) - 1)
	.SET TILE_MAP_WSHIFT, 7
	.SET TILE_MAP_WMASK, ((1 << TILE_MAP_WSHIFT) - 1)

	.SET TEXTURE_HSHIFT, 3
	.SET TEXTURE_HMASK, ((1 << TEXTURE_HSHIFT) - 1)
	.SET TEXTURE_WSHIFT, 3
	.SET TEXTURE_WMASK, ((1 << TEXTURE_WSHIFT) - 1)
	
	.SET TEXTURE_INPLACE_HMASK, (TEXTURE_HMASK << TEXTURE_WSHIFT)
	.SET TEXTURE_INPLACE_WMASK, TEXTURE_WMASK
	
	.SET TEXTURE_SIZE_SHIFT, (TEXTURE_HSHIFT + TEXTURE_WSHIFT)
	
	@@ ------------------------------------------------------------------------
	@@ TILE DATA INFO
	@@ ------------------------------------------------------------------------
	
	.SET TILEMAP_ENTRY_SHIFT, 2
	.SET TILEMAP_BITS_PER_ENTRY, (((1 << TILEMAP_ENTRY_SHIFT) * 8) / 2)
	
	.SET SUPPORT_TILE_SPECIAL_BITS, 1
	@.SET SUPPORT_ALTERNATE_PALETTE_SPECIAL_BIT, 1

.ENDIF @ USE_TILED_DATA

@ -----------------------------------------------------------------------------
@ TEXTURE CONSTANTS
@ -----------------------------------------------------------------------------

.IFDEF USE_DEPTH_TEXTURE_SET

	.SET TEXEL_COORD_BITS, 6
	.SET H2TEXTURE_CONVERSION_SHIFT, 4
	
	.SET U_TEXEL_COORD_BITS, TEXEL_COORD_BITS
	.SET V_TEXEL_COORD_BITS, TEXEL_COORD_BITS
	.SET U_TEXEL_COORD_SIZE, (1 << U_TEXEL_COORD_BITS)
	.SET V_TEXEL_COORD_SIZE, (1 << V_TEXEL_COORD_BITS)
	.SET U_TEXEL_COORD_MASK, ((U_TEXEL_COORD_SIZE - 1) << BPT_TRIG_SHIFT)
	.SET V_TEXEL_COORD_MASK, ((V_TEXEL_COORD_SIZE - 1) << BPT_TRIG_SHIFT)
	
.ENDIF @ USE_DEPTH_TEXTURE_SET

@@ ----------------------------------------------------------------------------
@@	ARM_RenderHeightmapCommandStream(
@@		const u32 * pCommandStream_PDST_DX_DY
@@		,const u8 * pHeightMap
@@		,const u8 * pTextureMap
@@		,const int dz
@@		,const int x
@@		,const int y
@@		,const int z
@@	)
@@
@@	r0			== pDst/pCommandStream_PDST_DX_DY
@@	r1			== pHeightMap
@@	r2			== pTextureMap
@@	r3			== dz
@@	r4: [sp]	== x
@@	r5: [sp,4]	== y
@@	r6: [sp,8]	== z
@@	r7			== dx
@@	r8			== dy
@@	r9			== h
@@	r10			== ph
@@	r11			== color
@@	r12			== pEnd
@@	lr			== temp
@@
@@	-- THIS ASSUMES THAT THE ARGUMENTS ARE VALID --
@@ ----------------------------------------------------------------------------

@ >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
.IFDEF COMPILE_HEIGHTMAP_RENDERER
@ <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	.TEXT
	.CODE 32
	.ALIGN 2
	.ARM
	.SECTION .iwram,"ax",%progbits
	
@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@ GENERATE THE FUNCTION NAME BASED ON COMPILE OPTIONS
@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	.GLOBAL ARM_RenderHeightmapCommandStream
	.TYPE ARM_RenderHeightmapCommandStream, function
	ARM_RenderHeightmapCommandStream:

@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	str		r3, [ sp, #-4 ]!						@@ save dz on the stack for reload
	stmfd	sp!, { r4 - r12, lr }					@@ push used regs on the stack
	str		r0, [ sp, #-4 ]!						@@ store command stream on the stack
	b		.LRH8_START_COL							@@ skip the sky renderer for now :)
	
@@ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.IFNDEF DONT_EMBED_COPYRIGHT_MESSAGE
.ASCII "BPT>"
.ENDIF
@@ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.LRH8_DRAW_SKY:

.IFDEF RENDER_FAKE_SKY
@@ TODO - if available use the 8 unused scanlines for a repeating pattern for a or maybe a gradient?
	.IF 1
		.IFDEF TEST_50_50_BLEND
			mov		r11, #0x00
		.ELSE
			mov		r11, #0xff
			orr		r11, r11, #0xff00
		.ENDIF
	.ELSE
		mov		r11, #(27 * 1024)					@@ clear color
	.ENDIF
	add		lr, r0, #COL_STRIDE						@@ fun
.LRH8_DRAW_SKY_LOOP:
	.IFDEF DUP_RENDER_COLS
		strh	r11, [ lr ], #2						@@ write sky color
	.ENDIF
	strh	r11, [ r0 ], #2							@@ write sky color
	cmp		r0, r12									@@ pDst ? pEnd
	blt		.LRH8_DRAW_SKY_LOOP						@@ wee doggies
.ENDIF @ RENDER_FAKE_SKY

.LRH8_START_COL:

	ldr		r4, [ sp ]								@@ read the command stream from the stack
	
.IFDEF SUPPORT_FLEXIBLE_RENDER_HEIGHT
	ldmia	r4!, { r0, r7, r8, r12 }				@@ load pDst, dx, dy, pEnd from command stream
.ELSE
	ldmia	r4!, { r0, r7, r8 }						@@ load pDst, dx, dy from command stream
.ENDIF

	str		r4, [ sp ]								@@ update command stream pointer
	teq		r0, #0									@@ check for end condition
	beq		.LRH8_DONE								@@ done if pDst is 0!

.IFNDEF SUPPORT_FLEXIBLE_RENDER_HEIGHT
	add		r12, r0, #REND_HEIGHT					@@ pEnd = pDst + REND_HEIGHT
.ENDIF

	add		r10, sp, #44							@@ how many bytes before x,y & z?
	ldmia	r10, { r3, r4, r5, r6 }					@@ load dz, x, y, z from stack
	
.IFDEF PROJECTED_PIXEL_HEIGHT_BIAS
	mov		r10, #PROJECTED_PIXEL_HEIGHT_BIAS		@@ ph = PROJECTED_PIXEL_HEIGHT_BIAS
.ELSE
	mov		r10, #0									@@ ph = 0
.ENDIF
	
.IFDEF EMBED_RAY_HIT_WRITE
.LRH8_SKIP_RAY_HIT:
	cmp		r10, #DEPTH_CLIP						@@ ph < DEPTH_CLIP
	bge		.LRH8_DRAW_SKY							@@ next col if ph >= DEPTH_CLIP
.ENDIF

.LRH8_LOOP_TOP:

@@ ----------------------------------------------------------------------------

.SET UNROLL_COUNTER,1
.REPT (UNROLL_RAY_SEARCH_COUNT)
.IF (1 == UNROLL_COUNTER)
.ELSE
	cmp		r10, #DEPTH_CLIP						@@ ph < DEPTH_CLIP
	bge		.LRH8_DRAW_SKY							@@ next col if ph >= DEPTH_CLIP
.ENDIF

.IFDEF SCALE_DXDYDZPH_STEPS
	add		r4, r4, r7, asl #DXDYDZPH_SCALE_SHIFT	@@ x += (dx * scale)
	add		r5, r5, r8, asl #DXDYDZPH_SCALE_SHIFT	@@ y += (dy * scale)
	add		r6, r6, r3, asl #DXDYDZPH_SCALE_SHIFT	@@ z += (dz * scale)
	add		r10, r10, #(V_SCALE * DXDYDZPH_SCALE)	@@ ph += (V_SCALE * scale)
.ELSE
	add		r4, r4, r7								@@ x += dx
	add		r5, r5, r8								@@ y += dy
	add		r6, r6, r3								@@ z += dz
	add		r10, r10, #V_SCALE						@@ ph += V_SCALE
.ENDIF
	
.IFDEF USE_TILED_HEIGHT_MAP

	mov		r9, r5, asr #(BPT_TRIG_SHIFT + TEXTURE_HSHIFT)	@@ ty = y / TILE_HEIGHT
	and		r9, r9, #TILE_MAP_HMASK					@@ ty %= TILE_MAP_HMASK
	mov		r11, r4, asr #(BPT_TRIG_SHIFT + TEXTURE_WSHIFT) @@ tx = x / TILE_WIDTH
	and		r11, r11, #TILE_MAP_WMASK				@@ tx %= TILE_MAP_WMASK
	orr		r11, r11, r9, lsl #TILE_MAP_WSHIFT		@@ tileIndex
	add		r11, r1, r11, lsl #(TILEMAP_ENTRY_SHIFT) @@ get interleaved tilemap address
	 
	mov		r9, r5, asr #BPT_TRIG_SHIFT				@@ py = y
	and		r9, r9, #TEXTURE_HMASK					@@ py %= TEXTURE_HEIGHT
	mov		lr, r4, asr #BPT_TRIG_SHIFT				@@ px = x
	and		lr, lr, #TEXTURE_WMASK					@@ px %= TEXTURE_WIDTH
	orr		lr, lr, r9, lsl #TEXTURE_WSHIFT			@@ sampleOffset

	@@ ---------------------------------------------------------------------------
	@@ TILE LOOKUP
	@@ ---------------------------------------------------------------------------
	.IF (8 == TILEMAP_BITS_PER_ENTRY)
		ldrb	r9, [ r11 ], #1					@@ get height tile
	.ELSE @ 8 BIT TILES
		.IF (16 == TILEMAP_BITS_PER_ENTRY)
			ldrh	r9, [ r11 ], #2				@@ get height tile
		.ELSE
			ldr		r9, [ r11 ], #4				@@ get height tile
		.ENDIF
	.ENDIF
	@@ ---------------------------------------------------------------------------
	@@ CHECK SPECIAL BITS
	@@ ---------------------------------------------------------------------------
	.IFDEF SUPPORT_TILE_SPECIAL_BITS
		movs	r9, r9, lsr #1						@@ extract horizontal flip bit
		eorcs	lr, lr, #TEXTURE_INPLACE_WMASK		@@ flip the x texture corrdinate
		movs	r9, r9, lsr #1						@@ extract vertical flip bit
		eorcs	lr, lr, #TEXTURE_INPLACE_HMASK		@@ flip the y texture corrdinate
		movs	r9, r9, lsr #1						@@ extract halve value bit
	.ENDIF @ SUPPORT_TILE_SPECIAL_BITS
	@@ ---------------------------------------------------------------------------
	
	add		lr, lr, r2								@@ add height tiles base address
	ldrb	r9, [ lr, r9, lsl #TEXTURE_SIZE_SHIFT ]	@@ lookup height texel

	.IFDEF SUPPORT_TILE_SPECIAL_BITS
		movcs	r9, r9, lsr #1						@@ halve the height value
	.ENDIF
	
.ELSE @ USE_TILED_HEIGHT_MAP

	mov		r9, r5, asr #(BPT_TRIG_SHIFT - 8)		@@ extract y texel value
	and		r9, r9, #0x0000ff00						@@ limit to range
	mov		lr, r4, asr #BPT_TRIG_SHIFT				@@ extract x texel value
	and		lr, lr, #0x000000ff						@@ limit to range
	orr		lr, lr, r9								@@ texel offset yyyyyyyyxxxxxxxx
	ldrb	r9, [ r1, lr ]							@@ h = heightmap( x, y )
	
.ENDIF @ USE_TILED_HEIGHT_MAP

.IFDEF INVERT_HEIGHT
	rsb		r9, r9, #255							@@ h = 255 - h
.ENDIF

.IFDEF USE_CONSTANT_FOR_HEIGHT
	mov		r9, #DBUG_CONSTANT_HEIGHT
.ENDIF

	cmp		r6, r9, asl #HSCALE_SHIFT				@@ flags( h - z )
	
.IFDEF EMBED_RAY_HIT_WRITE
	bge		.LRH8_SKIP_RAY_HIT						@@ hit something?
.ELSE
	blt		.LRH8_RAY_HIT							@@ hit something?
.ENDIF

.SET UNROLL_COUNTER,(UNROLL_COUNTER + 1)
.ENDR @ UNROLL_RAY_SEARCH_COUNT

@@ ----------------------------------------------------------------------------

.LRH8_LOOP_END:
	
.IFNDEF EMBED_RAY_HIT_WRITE
	cmp		r10, #DEPTH_CLIP						@@ ph < DEPTH_CLIP
	bge		.LRH8_START_COL							@@ next col if ph >= DEPTH_CLIP
	b		.LRH8_LOOP_TOP							@@ keep searching for hit
.ENDIF

.LRH8_RAY_HIT:

@@ ----------------------------------------------------------------------------
@@ LOOKUP TEXEL
@@ ----------------------------------------------------------------------------
.IFDEF USE_TILED_TEXTURE_MAP
	@@ ---------------------------------------------------------------------------
	@@ TILE LOOKUP
	@@ ---------------------------------------------------------------------------
	.IF (8 == TILEMAP_BITS_PER_ENTRY)
		ldrb	r11, [ r11 ]						@@ get height tile
	.ELSE @ 8 BIT TILES
		.IF (16 == TILEMAP_BITS_PER_ENTRY)
			ldrh	r11, [ r11 ]					@@ get height tile
		.ELSE
			ldr		r11, [ r11 ]					@@ get height tile
		.ENDIF
	.ENDIF @ 8 BIT TILES
	@@ ---------------------------------------------------------------------------
	@@ CHECK SPECIAL BITS
	@@ ---------------------------------------------------------------------------
	.IFDEF SUPPORT_TILE_SPECIAL_BITS
		movs	r11, r11, lsr #1					@@ extract horizontal flip bit
		eorcs	lr, lr, #TEXTURE_INPLACE_WMASK		@@ flip the x texture corrdinate
		movs	r11, r11, lsr #1					@@ extract vertical flip bit
		eorcs	lr, lr, #TEXTURE_INPLACE_HMASK		@@ flip the y texture corrdinate
		.IFDEF SUPPORT_ALTERNATE_PALETTE_SPECIAL_BIT
			movs	r11, r11, lsr #1				@@ extract alternate palette bit
		.ENDIF
	.ENDIF @ SUPPORT_TILE_SPECIAL_BITS
	@@ ---------------------------------------------------------------------------
	.IFNDEF USE_ONE_TILESET_FOR_BOTH_TILEMAPS
		add		lr, lr, #0x0010000					@@ add texture tiles base address
	.ENDIF
	.IFDEF FAKE_WIDE_TEXTURES
		.IFDEF HALFWORD_FAKE_WIDE_TEXTURES
			bic		lr, lr, #1						@@ half-word align
			add		r11, lr, r11, lsl #TEXTURE_SIZE_SHIFT
			ldrh	r11, [ r11 ]					@@ lookup texture texel
			orr		r11, r11, r11, lsl #16			@@ duplicate
		.ELSE
			bic		lr, lr, #3						@@ word align
			ldr		r11, [ lr, r11, lsl #TEXTURE_SIZE_SHIFT ]	@@ lookup texture texel
		.ENDIF
	.ELSE
		.IFDEF USE_COLOR_SHADE_TABLE @ color shade table
			ldrb	r11, [ lr, r11, lsl #TEXTURE_SIZE_SHIFT ]	@@ lookup texture texel
			add		lr, r2, #0x0020000							@@ add shade table base address
			.IFDEF USE_16BPP_SHADE_TABLE_ENTRIES
				add		lr, lr, r11, lsl #(COLOR_SHADES_SHIFT + 1)	@@ number of shades shift
			.ELSE
				add		lr, lr, r11, lsl #(COLOR_SHADES_SHIFT)		@@ number of shades shift
			.ENDIF
			@ ----------------------------------------------------
			@ FUTURE this could look up a color pair to write or
			@ in 16bpp mode could be used to represet the palette
			@ base 256 colors with x shades of each...
			@ other palettes could be handled by using the
			@ extra 'tile' bits...
			@ also rethink other uses of 64 bytes per palette entry (8x4 texture...)
			@ ----------------------------------------------------
			.IFDEF USE_16BPP_SHADE_TABLE_ENTRIES

				mov		r11, r10, lsr #(V_SCALE_LOG2 + DEPTH_TO_SHADE_SHIFT)
				mov		r11, r11, lsl #1
				ldrh	r11, [ lr, r11 ]	@@ look up color shade
				
			.ELSE			
				ldrb	r11, [ lr, r10, lsr #(V_SCALE_LOG2 + DEPTH_TO_SHADE_SHIFT) ]	@@ look up color shade
			.ENDIF
		.ELSE
			.IFDEF RENDER_DEPTH_INSTEAD_OF_COLORS
				mov		r11, r10, lsr #V_SCALE_LOG2					@@ 0 to DEPTH_CLIP/V_SCALE
			.ELSE
				ldrb	r11, [ lr, r11, lsl #TEXTURE_SIZE_SHIFT ]	@@ lookup texture texel
			.ENDIF
		.ENDIF
		.IFDEF WIDE_DUP_COLOR_MODE
			.IFDEF USE_16BPP_SHADE_TABLE_ENTRIES
				orr		r11, r11, r11, lsl #16
			.ELSE
				orr		r11, r11, r11, lsl #8
				orr		r11, r11, r11, lsl #16
			.ENDIF
		.ENDIF
	.ENDIF
	.IFDEF SUPPORT_ALTERNATE_PALETTE_SPECIAL_BIT
		eorcs	r11, r11, #0x80						@@ invert the high palette index
	.ENDIF
.ELSE @ USE_TILED_TEXTURE_MAP
	.IFDEF USE_DEPTH_TEXTURE_SET
		and		r11, r4, #U_TEXEL_COORD_MASK				@@ u texel value
		mov		r11, r11, lsr #(BPT_TRIG_SHIFT - V_TEXEL_COORD_BITS)
		and		lr, r5, #V_TEXEL_COORD_MASK					@@ v texel value
		orr		r11, r11, lr, lsr #BPT_TRIG_SHIFT			@@ texel offset u*.v*
		mov		lr, r9, lsr #H2TEXTURE_CONVERSION_SHIFT		@@ limit to N textures
		add		lr, r11, lr, lsl #(U_TEXEL_COORD_BITS + V_TEXEL_COORD_BITS)
		.IFDEF USE_4X_WIDE_TEXTURES
			ldr		r11, [ r2, lr, lsl #2 ]					@@ lookup texel
		.ELSE
			.IFDEF FAKE_WIDE_TEXTURES
				.IFDEF HALFWORD_FAKE_WIDE_TEXTURES
					bic		lr, lr, #1						@@ half-word align
					ldrh	r11, [ r2, lr ]					@@ lookup texel
					orr		r11, r11, r11, lsl #16			@@ duplicate
				.ELSE
					bic		lr, lr, #3						@@ word align
					ldr		r11, [ r2, lr ]					@@ lookup texel
				.ENDIF
			.ELSE
				ldrb	r11, [ r2, lr ]						@@ lookup texel
			.ENDIF
		.ENDIF
		@@ SHADE...
	.ELSE @ USE_DEPTH_TEXTURE_SET
		.IFDEF USE_HEIGHT_FOR_TEXTURE_VALUE
			mov		r11, r9									@@ color = h
		.ELSE @ USE_HEIGHT_FOR_TEXTURE_VALUE
			@@ HANDLE 1:1 mapping mode
			.IFDEF USE_4X_WIDE_TEXTURES
				bic		lr, lr, #3							@@ dword align
				ldr		r11, [ r2, lr ]						@@ color = texture( x, y )
			.ELSE @ USE_4X_WIDE_TEXTURES
				.IFDEF FAKE_WIDE_TEXTURES
					.IFDEF HALFWORD_FAKE_WIDE_TEXTURES
						bic		lr, lr, #1					@@ half-word align
						ldrh	r11, [ r2, lr ]				@@ lookup texel
						orr		r11, r11, r11, lsl #16		@@ duplicate
					.ELSE
						bic		lr, lr, #3					@@ dword align
						ldr		r11, [ r2, lr ]				@@ lookup texel
					.ENDIF
				.ELSE
					ldrb	r11, [ r2, lr ]					@@ color = texture( x, y )
				.ENDIF
			.ENDIF @ USE_4X_WIDE_TEXTURES
		.ENDIF @ USE_HEIGHT_FOR_TEXTURE_VALUE
	.ENDIF @ USE_DEPTH_TEXTURE_SET
.ENDIF @ USE_TILED_TEXTURE_MAP
@@ ----------------------------------------------------------------------------

@@ ----------------------------------------------------------------------------
@@ SETUP FOR WRITE
@@ ----------------------------------------------------------------------------

.IFDEF DUP_RENDER_COLS
	.IF (256 <= COL_STRIDE)
		.SET USE_LR_FOR_COL_DUP_ADDRESS, 1
	.ENDIF
.ENDIF

.IFDEF USE_LR_FOR_COL_DUP_ADDRESS
	.IFDEF FLIP_VERTICALLY
		add		lr, r12, #COL_STRIDE
	.ELSE
		add		lr, r0, #COL_STRIDE
	.ENDIF
.ENDIF

.IFDEF TEST_50_50_BLEND
	mov		r11, r11, lsr #1
	bic		r11, r11, #0x0210
.ENDIF

@@ ----------------------------------------------------------------------------
	
.LRH8_WRITE_LOOP:

.SET UNROLL_COUNTER,1
.REPT (UNROLL_RAY_HIT_COUNT)
.IF (1 == UNROLL_COUNTER)
.ELSE
	cmp		r6, r9, asl #HSCALE_SHIFT				@@ flags( h - z )
	bge		.LRH8_END_WRITE_LOOP					@@ write while still hitting	
.ENDIF

@@ ----------------------------------------------------------------------------

.IFDEF ROR_1X4_TEXEL_MODE
	.IFDEF FLIP_VERTICALLY
		.IFDEF DUP_RENDER_COLS
			mov		r11, r11, ror #8				@@ reorder texels
			strh	r11, [ r12, #(COL_STRIDE - ROW_STRIDE) ]	@@ dup
			mov		r11, r11, ror #8				@@ reorder texels
		.ELSE
			mov		r11, r11, ror #16				@@ reorder texels
		.ENDIF
		strh	r11, [ r12, #-ROW_STRIDE ]!				@@ store 
	.ELSE
		.IFDEF DUP_RENDER_COLS
			mov		r11, r11, ror #8				@@ reorder texels
			strh	r11, [ r10, #COL_STRIDE ]		@@ dup 
			mov		r11, r11, ror #8				@@ reorder texels
		.ELSE
			mov		r11, r11, ror #16				@@ reorder texels
		.ENDIF
		strh	r11, [ r0 ], #ROW_STRIDE			@@ store 
	.ENDIF
.ELSE @ ROR_1X4_TEXEL_MODE
	.IF (1 == ROW_STRIDE)
		.IFDEF FLIP_VERTICALLY
			.IFDEF DUP_RENDER_COLS
				strb	r11, [ r12, #(COL_STRIDE - 1) ]	@@ dup
			.ENDIF
			strb	r11, [ r12, #-ROW_STRIDE ]!		@@ just write
		.ELSE
			.IFDEF DUP_RENDER_COLS
				strb	r11, [ r0, #COL_STRIDE ]	@@ dup
			.ENDIF
			strb	r11, [ r0 ], #ROW_STRIDE		@@ just write
		.ENDIF
	.ELSE
		.IFDEF FLIP_VERTICALLY
			.IFDEF WIDE_RENDERER_MODE @ test wide renderer
				str		r11, [ r12, #-ROW_STRIDE ]!		@@ store
				.IFNDEF WIDE_DUP_COLOR_MODE
					.IFDEF HALFWORD_FAKE_WIDE_TEXTURES
						mov		r11, r11, ror #8			@@ reorder
					.ELSE
						mov		r11, r11, ror #16			@@ reorder
					.ENDIF
				.ENDIF
			.ELSE
				.IFDEF USE_HALFWORD_WRITES
					.IFDEF TEST_50_50_BLEND @ TEST 50:50 blend
						ldrh	lr, [ r12, #-ROW_STRIDE ]
						mov		lr, lr, lsr #1
						bic		lr, lr, #0x0210
						add		lr, lr, r11
						strh	lr, [ r12, #-ROW_STRIDE ]!
					.ELSE
						.IFDEF DUP_RENDER_COLS
							.IFDEF USE_LR_FOR_COL_DUP_ADDRESS
								strh	r11, [ lr, #-ROW_STRIDE ]!		@@ store
							.ELSE
								strh	r11, [ r12, #(COL_STRIDE - ROW_STRIDE) ]	@@ dup 
							.ENDIF
						.ENDIF
						strh	r11, [ r12, #-ROW_STRIDE ]!		@@ store
					.ENDIF
				.ELSE
					.IFDEF DUP_RENDER_COLS
						.IFDEF USE_LR_FOR_COL_DUP_ADDRESS
							strb	r11, [ lr, #-ROW_STRIDE ]!		@@ dup
						.ELSE
							strb	r11, [ r12, #(COL_STRIDE - ROW_STRIDE) ]	@@ dup 
						.ENDIF
					.ENDIF
					strb	r11, [ r12, #-ROW_STRIDE ]!		@@ store
				.ENDIF
			.ENDIF
		.ELSE
			.IFDEF WIDE_RENDERER_MODE @ test wide renderer
				str		r11, [ r0, #ROW_STRIDE ]!		@@ store
				.IFNDEF WIDE_DUP_COLOR_MODE
					.IFDEF HALFWORD_FAKE_WIDE_TEXTURES
						mov		r11, r11, ror #8			@@ reorder
					.ELSE
						mov		r11, r11, ror #16			@@ reorder
					.ENDIF
				.ENDIF
			.ELSE
				.IFDEF USE_HALFWORD_WRITES
					.IFDEF DUP_RENDER_COLS
						strh	r11, [ r0, #COL_STRIDE ]	@@ dup
					.ENDIF
					strh	r11, [ r0 ], #ROW_STRIDE		@@ store
				.ELSE
					.IFDEF DUP_RENDER_COLS
						strb	r11, [ r0, #COL_STRIDE ]	@@ dup
					.ENDIF
					strb	r11, [ r0 ], #ROW_STRIDE		@@ store
				.ENDIF
			.ENDIF
		.ENDIF
	.ENDIF
.ENDIF @ USE_TILED_DATA
@@ ----------------------------------------------------------------------------

.IFDEF DBUG_SHOW_DUPLICATE_WRITES
	mov		r11, #0									@@ [DEBUG] show duplicate texels
.ENDIF @ DBUG_SHOW_DUPLICATE_WRITES

	cmp		r0, r12									@@ pDst ? pEnd
	bge		.LRH8_START_COL							@@ next col
	
.IFDEF RENDER_EVERY_ROW_MODE
	add		r3, r3, #V_SCALE						@@ dz += V_SCALE
	add		r6, r6, r10								@@ z += ph
.ELSE
	.IFDEF ROTATED_DISPLAY_MODE
		add		r3, r3, #(V_SCALE * 2)				@@ dz += V_SCALE * 2
		add		r6, r6, r10, asl #1					@@ z += ph * 2
	.ELSE
		add		r3, r3, #V_SCALE					@@ dz += V_SCALE
		add		r6, r6, r10							@@ z += ph
	.ENDIF
.ENDIF

.SET UNROLL_COUNTER,(UNROLL_COUNTER + 1)
.ENDR @ UNROLL_RAY_HIT_COUNT

	cmp		r6, r9, asl #HSCALE_SHIFT				@@ flags( h - z )
	blt		.LRH8_WRITE_LOOP						@@ write while still hitting	
	
.LRH8_END_WRITE_LOOP:
	
	cmp		r10, #DEPTH_CLIP						@@ ph < DEPTH_CLIP
	bge		.LRH8_DRAW_SKY							@@ next col if ph >= DEPTH_CLIP
	b		.LRH8_LOOP_TOP							@@ keep searching for hit

.LRH8_DONE:
		
	add		sp, sp, #4								@@ remove command stream pointer from stack
	ldmfd	sp!, { r4 - r12, lr }					@@ pop used regs off the stack
	add		sp, sp, #4								@@ remove command stream pointer from stack
	bx		lr										@@ return to caller
	
@@ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.IFNDEF DONT_EMBED_COPYRIGHT_MESSAGE
.ASCII "<BPT"
.ENDIF
@@ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@ DEAL WITH PROLOG 
@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	end_ARM_RenderHeightmapCommandStream:
	.SIZE ARM_RenderHeightmapCommandStream, end_ARM_RenderHeightmapCommandStream - ARM_RenderHeightmapCommandStream

@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	.POOL

@ >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
.ENDIF @ .IFDEF COMPILE_HEIGHTMAP_RENDERER
@ <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

@@ =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
@@ END END END END END END END END END END END END END END END END END END END 
@@ =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	.END
	