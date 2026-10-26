// imaadpcmencoder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <list>

// ----------------------------------------------------------------------------

static const char * s_pProgramName = _T( "imaadpcmencoder" );
static std::string s_srcFilename;
static std::string s_dstFilename;

static int s_nBitsPerElement = 8;
static int s_nSampleMax = 127;
static int s_nSampleMin = -128;

static bool s_bDecode = false;
static bool s_bBPT8ADPCMMode = false;
static bool s_bDumpStats = false;
static int s_nDumpStatsLevel = 0;
static int s_bIncreaseOutputFrequency = 0;
static bool s_bSmoothOutSamplesDuringDecode = false;

// ----------------------------------------------------------------------------

namespace TIM_KIENTZLE {

	/*

		NOTE:

			As soon as I can find another source of the _stepSizeTable
			and indexAdjustTable, I will be replacing this code.

		-BPT

	*/

	// ------------------------------------------------------------------------
	// The IMA ADPCM code is adapted from Tim Kientzle's example
	// from his book "A Programmer's GUIDE TO SOUND".
	// Portions Copyright 1998, Tim Kientzle & Jack Jansen, All rights reserved
	// see book for full copyright information.
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// constant tables
	// ------------------------------------------------------------------------

	static const int _stepSizeTable[ 89 ] = {

		7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34,
		37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
		157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
		544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
		1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
		4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
		11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
		27086, 29794, 32767

	};

//#define USE_2BIT_ADPCM_CODES
//#define USE_3BIT_ADPCM_CODES
//#define USE_CUSTOM_ADJUSTMENT_TABLE_FOR_3BIT_ADPCM

#if defined(USE_CUSTOM_ADJUSTMENT_TABLE_FOR_3BIT_ADPCM)

	static const int indexAdjustTable[ 8 ] = {

		-1, -2,
		2, 4,
		-1, -2,
		2, 4

	};

#else

	static const int indexAdjustTable[ 16 ] = {

		-1, -1, -1, -1,			// +0 - +3, decrease step size
		2, 4, 6, 8,				// +4 - +7, increase step size
		-1, -1, -1, -1,			// -0 - -3, decrease step size
		2, 4, 6, 8				// -4 - -7, increase step size

	};

#endif

	// ------------------------------------------------------------------------

	struct ImaState {

		int index;				// Index into step size table
		int previousValue;		// Most recent sample value

	};

	// ------------------------------------------------------------------------
	// encode/decode sample
	// ------------------------------------------------------------------------

	//
	//	ImaAdpcmDecode()
	//

	int ImaAdpcmDecode(
		const int deltaCode
		,ImaState & state
		,const int *pStepTable
		,const int nTableEntries
	) {

		// Get the current step size

		int step = pStepTable[ state.index ];

#if defined(USE_2BIT_ADPCM_CODES)

		step >>= 2; // quarter the range

		int difference = step >> 2;

		if ( 1 & deltaCode ) { difference += step >> 1; }
		if ( 2 & deltaCode ) { difference = -difference; }

#elif defined(USE_3BIT_ADPCM_CODES)

		step >>= 1; // half the range

		int difference = step >> 2;

		if ( 1 & deltaCode ) { difference += step >> 1; }
		if ( 2 & deltaCode ) { difference += step; }
		if ( 4 & deltaCode ) { difference = -difference; }

#else

		// Construct the difference by scaling the current step size
		// This is approximately: difference = (deltaCode+.5)*step/4

		int difference = step >> 3;

		if ( 1 & deltaCode ) { difference += step >> 2; }
		if ( 2 & deltaCode ) { difference += step >> 1; }
		if ( 4 & deltaCode ) { difference += step; }
		if ( 8 & deltaCode ) { difference = -difference; }

#endif

		// Build new sample

		if ( s_bSmoothOutSamplesDuringDecode ) {

			state.previousValue = 
				(((state.previousValue + difference) * 5) + (state.previousValue*3)) >> 3;

		} else {

			state.previousValue += difference;

		}

		// limit sample to valid sample range

		if ( s_nSampleMax < state.previousValue ) {
			
			state.previousValue = s_nSampleMax;

		} else if ( s_nSampleMin > state.previousValue ) {
			
			state.previousValue = s_nSampleMin;

		}

		// Update the state foe the next sample

#if defined(USE_2BIT_ADPCM_CODES)

		state.index += indexAdjustTable[ (deltaCode * 4) ];

#elif defined(USE_3BIT_ADPCM_CODES)

	#if defined(USE_CUSTOM_ADJUSTMENT_TABLE_FOR_3BIT_ADPCM)

		state.index += indexAdjustTable[ deltaCode ];

	#else

		state.index += indexAdjustTable[ (deltaCode * 2) ];

	#endif

#else

		state.index += indexAdjustTable[ deltaCode ];

#endif

		if ( 0 > state.index ) {

			state.index = 0;

		} else if ( nTableEntries <= state.index ) {

			state.index = (nTableEntries - 1);

		}

		return state.previousValue;

	}

	//
	//	ImaAdpcmEncode()
	//

	int ImaAdpcmEncode(
		const int sample
		,ImaState & state
		,const int *pStepTable
		,const int nTableEntries
	) {

		int diff = sample - state.previousValue;

		int step = pStepTable[ state.index ];

		int deltaCode = 0;

		// Set sign bit

#if defined(USE_2BIT_ADPCM_CODES)

		step >>= 2;

		if ( 0 > diff ) { deltaCode = 2; diff = -diff; }

		if ( diff >= step ) { deltaCode |= 1; }

#elif defined(USE_3BIT_ADPCM_CODES)

		step >>= 1;

		if ( 0 > diff ) { deltaCode = 4; diff = -diff; }

		if ( diff >= step ) { deltaCode |= 2; diff -= step; }

		step >>= 1;

		if ( diff >= step ) { deltaCode |= 1; }

#else

		// This is essentially deltaCode = (diff << 2) / step
		// expect the roundoff is handled differently.

		if ( 0 > diff ) { deltaCode = 8; diff = -diff; }

		if ( diff >= step ) { deltaCode |= 4; diff -= step; }

		step >>= 1;

		if ( diff >= step ) { deltaCode |= 2; diff -= step; }

		step >>= 1;

		if ( diff >= step ) { deltaCode |= 1; }

#endif

		ImaAdpcmDecode( deltaCode, state, pStepTable, nTableEntries ); // update state

		return deltaCode;

	}

}; // TIM_KIENTZLE

// ----------------------------------------------------------------------------

namespace BPT {

	// ------------------------------------------------------------------------

	static int _8bit_experimentalStepSizeTable[ 1024 ];

	// ------------------------------------------------------------------------

	//
	//	SPSD_COMMemoryPolicy
	//

	template< class T = void * >
	struct SPSD_COMMemoryPolicy {

		typedef T pointer_type;

		static pointer_type Allocate( const size_t nBytes ) {

			return reinterpret_cast<pointer_type>( ::CoTaskMemAlloc( nBytes ) );

		}

		static void Free( pointer_type ptr ) {

			::CoTaskMemFree( reinterpret_cast<void *>( ptr ) );

		}

	}; /* struct SPSD_COMMemoryPolicy */

	//
	//	SPSD_New
	//

	template< class T = void * >
	struct SPSD_New {

		typedef T pointer_type;

		static pointer_type Allocate( const size_t nBytes ) {

			return reinterpret_cast<pointer_type>( new BYTE [ nBytes ] );

		}

		static void Free( pointer_type ptr ) {

			if ( ptr ) delete [] ptr;

		}

	}; /* struct SPSD_New */

	// ------------------------------------------------------------------------

	//
	//	TFileIO<>
	//

	template< class MEM = SPSD_New<> >
   	class TFileIO {

	public:

		typedef MEM memory_policy_type;

   	private:

		MEM m_MemoryPolicy;

   		FILE * m_Handle;

   	public:

   		TFileIO() : m_Handle( 0 ) { /* Empty */ }

   		TFileIO( MEM memPolicy ) : 
			m_MemoryPolicy( memPolicy), m_Handle( 0 ) { /* Empty */ }

   		~TFileIO() {

   			if ( m_Handle ) {

   				Close();

   			}

   		}

   		bool Open( const char * filename, const char * access ) {

   			m_Handle = fopen( filename, access );
   
   			return (0 != m_Handle);
   
   		}
   
   		void Close() {

   			if ( m_Handle ) {

   				fclose( m_Handle );
   				m_Handle = 0;

   			}
   
   		}

		int FTell() {

   			if ( m_Handle ) {

				return ftell( m_Handle );

			}

			return 0;

		}
   
   		void Seek( const int pos, const int mode ) {
   
   			if ( m_Handle ) {

   				fseek( m_Handle, pos, mode );

   			}
   
   		}

		int FileSize() {

			int pos = FTell();

			Seek( 0, SEEK_END );

			int end = FTell();

			Seek( pos, SEEK_SET );

			return end;

		}
   
   		int Read_Byte() { 
   
   			if ( m_Handle ) {

   				return static_cast<unsigned>(fgetc( m_Handle )) & 255;

   			}

   			return 0;
   
   		}
   
   		int Read_m16() {
   
   			int hi = Read_Byte();
   			int lo = Read_Byte();
   
   			return ((hi << 8) | lo);
   
   		}
   
   		int Read_m32() {
   
   			int a = Read_Byte();
   			int b = Read_Byte();
   			int c = Read_Byte();
   			int d = Read_Byte();
   
   			return (int)((a << 24) | (b << 16) | (c << 8) | d);
   
   		}

   		BYTE * LoadData( const int nBytes ) {

   			if ( m_Handle ) {

				MEM::pointer_type pMemory = m_MemoryPolicy.Allocate( nBytes );
   
   				if ( 0 == pMemory ) {
   
   					return 0;
   
   				}
   
   				fread( reinterpret_cast<void *>( pMemory ), 1, nBytes, m_Handle );

   				return reinterpret_cast<BYTE *>( pMemory );

   			}

   			return static_cast<BYTE *>( 0 );

   		}

		BYTE * LoadEntireFile( int & size ) {

			size = FileSize();

			if ( 0 >= size ) return 0;

			int pos = FTell();

			BYTE * pData = LoadData( size );

			Seek( pos, SEEK_SET );

			return pData;

		}

   		void UnloadData( BYTE * ptr ) {

   			if ( ptr ) {

				m_MemoryPolicy.Free( reinterpret_cast<MEM::pointer_type>( ptr ) );

   			}

   		}

		// --------------------------------------------------------------------

		void Write_8( const BYTE value ) {

   			if ( m_Handle ) {

				fwrite( &value, sizeof(BYTE), 1, m_Handle );

			}

		}

		void Write_i16( const int value ) {

			Write_8( value & 0xff );
			Write_8( (value >> 8) & 0xff );

		}

		void Write_i32( const int value ) {

			Write_i16( value & 0xffff );
			Write_i16( (value >> 16) & 0xffff );

		}

		template< class T > void Write( T * pT ) {

   			if ( m_Handle ) {

				fwrite( static_cast<const void *>( pT ), sizeof(T), 1, m_Handle );

			}

		}

		template< class T > void Write( T & t ) {

   			if ( m_Handle ) {

				fwrite( static_cast<const void *>( &t ), sizeof(T), 1, m_Handle );

			}

		}

   	}; /* template TFileIO<> */

};

// ----------------------------------------------------------------------------

//
//	PrintBanner()
//

static void PrintBanner() {

	fprintf( stdout, "%s 1.0 -- IMA ADPCM like encoder.\n", s_pProgramName );
	fprintf( stdout, "Copyright (c) 2003, Brad P. Taylor, LLC\n" );

}

//
//	DumpArgs()
//

static void DumpArgs()
{
	fprintf( stdout, "usage %s source [output]\n", s_pProgramName );
	fprintf( stdout, "\t-b#\t: # bits per element.\n" );
	fprintf( stdout, "\t-d\t: decode mode\n" );
	fprintf( stdout, "\t-8\t: use custom 8 sample deltas.\n" );
	fprintf( stdout, "\t-s#\t: dump stats # indicates how deep\n" );
}

//
//	ProcessArgs()
//

static void ProcessArgs( const int argc, const _TCHAR * const argv[] )
{

	bool bDumpArgs = false;

	for ( int i = 1; i < argc; i++ ) {

		switch ( argv[ i ][ 0 ] ) {

		case '-':
		case '/':
			switch ( argv[ i ][ 1 ] ) {

			case '?':
				bDumpArgs = true;
				break;

			case 'b':
			case 'B':
				s_nBitsPerElement = atoi( argv[ i ] + 2 );

				if ( 0 >= s_nBitsPerElement ) {

					fprintf( stderr, "Enter valid element bit count >= 1 (you passed %d)", s_nBitsPerElement );

					bDumpArgs = true;

				}

				if ( 8 >= s_nBitsPerElement ) {

					s_nBitsPerElement = 8;
					s_nSampleMax = 127;
					s_nSampleMin = -128;

				} else {

					s_nBitsPerElement = 16;
					s_nSampleMax = 32767;
					s_nSampleMin = -32768;

				}

				fprintf( stdout, "s_nBitsPerElement = %d (%d to %d)\n", s_nBitsPerElement, s_nSampleMin, s_nSampleMax );

				break;

			case 'd':
			case 'D':
				s_bDecode = true;
				fprintf( stdout, "decode mode!\n" );
				switch ( argv[ i ][ 2] ) {
				default:
					break;
				case 'i':
				case 'I':
					s_bIncreaseOutputFrequency = atoi( argv[ i ] + 3 );
					fprintf( stdout, "Increase output frequency %d!\n", s_bIncreaseOutputFrequency );
					break;
				}
				break;

			case 's':
			case 'S':
				s_bDumpStats = true;
				s_nDumpStatsLevel = atoi( argv[ i ] + 2 );
				fprintf( stdout, "Dump stats mode!\n" );
				break;

			case '8':
				fprintf( stdout, "BPT8 ADPCM table selected.\n" );
				s_bBPT8ADPCMMode = true;
				break;

			default:
				fprintf( stderr, "Unexpected argument \"%s\"\n", argv[ i ] );
				break;

			}
			break;

		default:

			if ( 0 == s_srcFilename.size() ) {

				s_srcFilename = std::string( argv[ i ] );

				fprintf( stdout, "src = \"%s\"\n", s_srcFilename.c_str() );

			} else if ( 0 == s_dstFilename.size() ) {

				s_dstFilename = std::string( argv[ i ] );

				fprintf( stdout, "dst = \"%s\"\n", s_dstFilename.c_str() );

			} else {

				fprintf( stderr, "Unexpected argument \"%s\"\n", argv[ i ] );

			}

		}

	}

	if ( 0 == s_dstFilename.size() ) {

		s_dstFilename = s_srcFilename + ".output";


	}

	if ( s_bBPT8ADPCMMode && (8 != s_nBitsPerElement) ) {

		fprintf( stderr, "8 bit samples must be used with 8 bit quantizer table.\n" );

		bDumpArgs = true;

	}

	if ( bDumpArgs || (0 == s_srcFilename.size()) || (0 == s_dstFilename.size()) ) {

		DumpArgs();

		exit( -1 );

	}

}

// ----------------------------------------------------------------------------

//
//	TImaAdpcmDecode()
//

template< const int outputBits, class F >
void TImaAdpcmDecode(
	 F & out
	 ,DWORD * first
	 ,DWORD * last
	 ,const int *pStepTable
	,const int nTableEntries
)
{
	TIM_KIENTZLE::ImaState state;

	state.index = 0;

	state.previousValue = 0;

	// unpack in groups of 8

	if ( 8 == outputBits ) {

		if ( 1 < s_bIncreaseOutputFrequency ) {

			while ( first != last ) {

				DWORD dwCache = *first++;

				for ( int i = 0; i < 8; i++ ) {

					int code = dwCache & 0x0f;

					dwCache >>= 4;

					// what are the last two samples
					// --------------------------------------------------------

					int lastSample = state.previousValue;

					int sample = TIM_KIENTZLE::ImaAdpcmDecode( code, state, pStepTable, nTableEntries );

					// Increase the frequency 
					// --------------------------------------------------------

					for ( int j = 1; j < s_bIncreaseOutputFrequency; j++ ) {

						int output = 
							(((sample - lastSample) * j)/s_bIncreaseOutputFrequency) + lastSample;

						out.Write_8( output & 0xff );

					}

					out.Write_8( sample & 0xff );

				}

			}

		} else {

			while ( first != last ) {

				DWORD dwCache = *first++;

				for ( int i = 0; i < 8; i++ ) {

					int code = dwCache & 0x0f;

					dwCache >>= 4;

					out.Write_8( TIM_KIENTZLE::ImaAdpcmDecode( code, state, pStepTable, nTableEntries ) & 0xff );

				}

			}

		}

	} else {

		while ( first != last ) {

			DWORD dwCache = *first++;

			for ( int i = 0; i < 8; i++ ) {

				int code = dwCache & 0x0f;

				dwCache >>= 4;

				out.Write_i16( TIM_KIENTZLE::ImaAdpcmDecode( code, state, pStepTable, nTableEntries ) & 0xffff );

			}

		}

	}

}

// ----------------------------------------------------------------------------

static int s_gExactMatches = 0;
static int s_gTotalErrors = 0;
static int s_gTotalBigErrors = 0;

static int s_bErrorDifferenceTable[ 32768 ];

//
//	BitString()
//

char * 
BitString( const int value )
{
	static char string[ 8 ][ 256 ];

	static int counter = 0;

	return _itoa( value, &string[ (counter = ((counter + 1) & 7)) ][ 0 ], 2 );
}

//
//	BitsToEncode()
//

int
BitsToEncode( const int value ) {

	if ( 2 >= value ) return 1;
	if ( 4 >= value ) return 2;
	if ( 8 >= value ) return 3;
	if ( 16 >= value ) return 4;
	if ( 32 >= value ) return 5;
	if ( 64 >= value ) return 6;
	if ( 128 >= value ) return 7;
	if ( 256 >= value ) return 8;
	if ( 65536 >= value ) return 16;

	return 32;

}

//#define VARIABLE_BIT_OUTPUT_TEST

#if defined(VARIABLE_BIT_OUTPUT_TEST)

static int s_LogRunCountHistogram[ 33 ];
static int s_OutputCodeHistogramTable[ 16 ];

#endif // defined(VARIABLE_BIT_OUTPUT_TEST)

// ----------------------------------------------------------------------------

//#define COLLECT_SAMPLE_PAIRS_STATS

#if defined(COLLECT_SAMPLE_PAIRS_STATS) // delta pairs

// ----------------------------------------------------------------------------

static int s_DeltaPairCount = 0;

typedef std::map< int, int > bpt_pair_entry_type;
typedef std::map< int, bpt_pair_entry_type * > bpt_map_of_pairs_type;

static bpt_map_of_pairs_type s_DeltaPairs;

typedef std::map< std::pair<int,int>, int > bpt_unique_pairs_map_type;

static bpt_unique_pairs_map_type s_UniquePairs;

//
//	AddDeltaPair()
//

void AddDeltaPair( int current, int last ) {

	// ---------------------------------------------------------------

	std::pair<int,int> thePair( last, current );

	bpt_unique_pairs_map_type::iterator which = s_UniquePairs.find( thePair );

	if ( which != s_UniquePairs.end() ) {

		++(which->second);

	} else {

		s_UniquePairs[ thePair ] = 1;

	}

	// ---------------------------------------------------------------

	bpt_map_of_pairs_type::iterator found = s_DeltaPairs.find( last );

	if ( found != s_DeltaPairs.end() ) {

		bpt_pair_entry_type::iterator nested = found->second->find( current );

		if ( nested != found->second->end() ) {

			++(*found->second)[ current ];

		} else {

			(*found->second)[ current ] = 1;

		}

	} else {

		bpt_pair_entry_type * pNew = new bpt_pair_entry_type();

		if ( pNew ) {

			(*pNew)[ current ] = 1;

		}

		s_DeltaPairs[ last ] = pNew;

	}

}

void DumpDeltaPairs() {

	// ------------------------------------------------------------------------

	fprintf( stdout, "PAIRS LIST (%d)\n-----------------------------------\n\n", s_UniquePairs.size() );

	bpt_unique_pairs_map_type::iterator pIt = s_UniquePairs.begin();

	while ( pIt != s_UniquePairs.end() ) {

		fprintf( stdout, "%6d, %6d, %8d\n", pIt->first.first, pIt->first.second, pIt->second );

		++pIt;

	}

	fprintf( stdout, "\n-----------------------------------\n\n" );

	// ------------------------------------------------------------------------

	fprintf( stdout, "Pair count = %d\n", s_DeltaPairs.size() );

	bpt_map_of_pairs_type::iterator it = s_DeltaPairs.begin();

	while ( it != s_DeltaPairs.end() ) {

		bpt_pair_entry_type::iterator inner = it->second->begin();

		fprintf( stdout, "%5d (%5d)\n", it->first, it->second->size() );

		while ( inner != it->second->end() ) {

			fprintf( stdout, "\t%5d\t%5d\n", inner->first, inner->second );

			++inner;

		}

		++it;

	}

	fprintf( stdout, "\n" );

}

// ----------------------------------------------------------------------------


#endif

//
//	TImaAdpcmEncode()
//

template< class F, class I >
void TImaAdpcmEncode(
	F & out
	,I first
	,I last
	,const int *pStepTable
 	,const int nTableEntries
)
{
	// probably should store regular state info at some interval

	TIM_KIENTZLE::ImaState state;

	state.index = 0;

	state.previousValue = 0;

	int sample = 0;

	int sampleDelta = 0;

	// pack in groups of 8

	DWORD dwCache = 0;

	int cacheCount = 0;

	int shift = 0;

	int nibbleCode = 0;

#if defined(VARIABLE_BIT_OUTPUT_TEST)

	int sameCodeCount = 0;

	int outputCount = 0;

	int totalSamplesProcessed = 0;

#endif // defined(VARIABLE_BIT_OUTPUT_TEST)

	while ( first != last ) {

		// get fresh sample
		// --------------------------------------------------------------------

		int lastRawSample = sample;

		sample = *first++;

#if defined(COLLECT_SAMPLE_PAIRS_STATS) // dump difference stream

		int lastSampleDelta = sampleDelta;

		sampleDelta = sample - lastRawSample;

//		AddDeltaPair( sampleDelta, lastSampleDelta );
		AddDeltaPair( sample, lastRawSample );

		int deltaDiff = sampleDelta - lastSampleDelta;

#if 0
//		fprintf( stdout, "%4d (%32s)%c\n", sampleDelta, BitString( abs(sampleDelta) ), ((0 <= sampleDelta) ? '+' : '-') );
		fprintf(
			stdout, "%4d (%9s)%c vs. %4d (%9s)%c\n"
			,deltaDiff, BitString( abs(deltaDiff) ), ((0 <= deltaDiff) ? '+' : '-')
			,sampleDelta, BitString( abs(sampleDelta) ), ((0 <= sampleDelta) ? '+' : '-')
		);
#endif

#endif

		// encode 
		// --------------------------------------------------------------------

		int lastCode = nibbleCode;

		int outputCode = TIM_KIENTZLE::ImaAdpcmEncode( sample, state, pStepTable, nTableEntries ) & 0x0f;

#if defined(VARIABLE_BIT_OUTPUT_TEST)

		++totalSamplesProcessed;

		// reorder the bits

#if defined(USE_3BIT_ADPCM_CODES)

		nibbleCode = (((outputCode & 3) << 1) | ((outputCode & 0x4) >> 2)) & 0x7;

#else

		nibbleCode = (((outputCode & 7) << 1) | ((outputCode & 0x8) >> 3)) & 0xf;

#endif

		// -------------------------------------------

#if 0 // test entropy
		outputCode = nibbleCode;
#endif

		// -------------------------------------------

		++s_OutputCodeHistogramTable[ nibbleCode ];

		int testIndex = BitsToEncode( nibbleCode );

		++s_LogRunCountHistogram[ testIndex ];

		fprintf( stdout, "%4s (%d), ", BitString(nibbleCode), testIndex );

		if ( !(++outputCount % 1) ) {

			fprintf( stdout, "\n" );

		}

#endif // defined(VARIABLE_BIT_OUTPUT_TEST)

#if 1 // defined(COLLECT_STATS)

		int diff = sample - state.previousValue;

		if ( diff ) {

			++s_bErrorDifferenceTable[ abs(diff) ];

			++s_gTotalErrors;

			if ( 4 < abs(diff) ) {

				++s_gTotalBigErrors;

			}

		} else {

			++s_gExactMatches;

		}

#endif

		dwCache |= outputCode << shift;

		if ( 8 == ++cacheCount ) {

			out.Write( dwCache );

			dwCache = 0;

			cacheCount = 0;

			shift = 0;

		} else {

			shift += 4;

		}

	}

#if defined(VARIABLE_BIT_OUTPUT_TEST)

	int testIndex = BitsToEncode( nibbleCode );

	++s_LogRunCountHistogram[ testIndex ];

	fprintf( stdout, "%4s (%d)\nEND\n\n", BitString(nibbleCode), testIndex );

	// dump the code frequency
	// ------------------------------------------------------------------------

	fprintf( stdout, "Code Histogram.\n-----------------------\n" );

	for ( int histo = 0; histo < 16; histo++ ) {

		fprintf(
			stdout
			,"%4s : %6d, %7.4f%%\n"
			,BitString(histo)
			,s_OutputCodeHistogramTable[ histo ]
			,((float)s_OutputCodeHistogramTable[ histo ] * 100.0f) / (float)totalSamplesProcessed
		);

	}

	// now dump the run count log table
	// ------------------------------------------------------------------------

	fprintf( stdout, "\nCode length bit counts.\n-----------------------\n" );

	for ( int bits = 1; bits < 32; bits++ ) {

		fprintf(
			stdout
			,"%2d) %8d, %7.4f%%\n"
			,(bits)
			,s_LogRunCountHistogram[ bits ]
			,(float)(s_LogRunCountHistogram[ bits ]*100.0f)/(float)totalSamplesProcessed
		);

	}

	// okay let's guess the savings if any
	// ------------------------------------------------------------------------

	int variableEncodedBitCount = 0;

#if 1 // 2 length codes
	
	#if defined(USE_3BIT_ADPCM_CODES)

		// fixed 2 or 4 bits

		for ( int xxx = 0; xxx <= 1; xxx++ ) {

			variableEncodedBitCount += (s_LogRunCountHistogram[ xxx ] * 2);

		}

		for ( ; xxx < 32; xxx++ ) {

			variableEncodedBitCount += (s_LogRunCountHistogram[ xxx ] * 4);

		}

	#else

		// fixed 3 or 5 bits

		for ( int xxx = 0; xxx <= 2; xxx++ ) {

			variableEncodedBitCount += (s_LogRunCountHistogram[ xxx ] * 3);

		}

		for ( ; xxx < 32; xxx++ ) {

			variableEncodedBitCount += (s_LogRunCountHistogram[ xxx ] * 5);

		}

	#endif

#else

	// n bits

	for ( int xxx = 1; xxx < 32; xxx++ ) {

		variableEncodedBitCount += (s_LogRunCountHistogram[ xxx ] * (xxx * 2));

	}

#endif

	// ------------------------------------------------------------------------

	int variableOutputSize = (variableEncodedBitCount + 7)/8;

#if defined(USE_3BIT_ADPCM_CODES)

	int normalOutputSize = (totalSamplesProcessed*3+7)/8;

#else

	int normalOutputSize = (totalSamplesProcessed*4+7)/8;

#endif

	fprintf(
		stdout, "variable encoding %d vs %d (%7.4f)\n"
		,variableOutputSize
		,normalOutputSize
		,(float)(100.0f * variableOutputSize)/(float)normalOutputSize
	);

#endif // defined(VARIABLE_BIT_OUTPUT_TEST)

	if ( cacheCount ) {

		out.Write( dwCache );

	}

#if defined(COLLECT_SAMPLE_PAIRS_STATS)

	DumpDeltaPairs();

#endif

}

// ----------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
	// ------------------------------------------------------------------------

	PrintBanner();

	ProcessArgs( argc, argv );

	// load
	// ------------------------------------------------------------------------

	BPT::TFileIO<> loader;

	if ( !loader.Open( s_srcFilename.c_str(), "rb" ) ) {

		fprintf( stderr, "Unable to open \"%s\" for read.\n", s_srcFilename.c_str() );

		return -1;

	}

	int dataSize;

	BYTE * pLoadedData = loader.LoadEntireFile( dataSize );

	if ( !pLoadedData ) {

		fprintf( stderr, "Error during data load.\n" );

		return -1;

	}

	// backup the old file?
	// ------------------------------------------------------------------------

	std::string backupName = s_dstFilename + ".backup";

	if ( backupName.size() ) {

		unlink( backupName.c_str() );

		rename( s_dstFilename.c_str(), backupName.c_str() );

	}

	// select quantizer
	// ------------------------------------------------------------------------

	const int * pStepTable;

	int nTableEntries;

	if ( s_bBPT8ADPCMMode ) {

#if 1 // average the power curve result with the time sample

		nTableEntries = 128; // 256; // 89;
		float fScale = 256; // 37267; // 256
		float fPower = 7.0; // fPower = 3.141592f;

#else

		nTableEntries = 256; // 89;
		float fScale = 256; // 37267; // 256
		float fPower = 3.0; // fPower = 3.141592f;
//		float fPower = 2.0; // fPower = 3.141592f;

#endif

		{
			float t = 0.0f;

			float d = 1.0f / (float)(nTableEntries - 1);

			for ( int i = 0; i < nTableEntries; i++ ) {

#if 1 // average the power curve result with the time sample

				float fEntry = (int)(fScale * (powf( t, fPower ) + t)) / 2.0f;

				BPT::_8bit_experimentalStepSizeTable[ i ] = (int)(7 + fEntry);

#else

				BPT::_8bit_experimentalStepSizeTable[ i ] = 7 + (int)(fScale * powf( t, fPower ));

#endif

				t += d;

			}
		}

		pStepTable = BPT::_8bit_experimentalStepSizeTable;
	
	} else {

		pStepTable = TIM_KIENTZLE::_stepSizeTable;

		nTableEntries = 89;

	}

	// open output file
	// ------------------------------------------------------------------------

	BPT::TFileIO<> writer;

	if ( writer.Open( s_dstFilename.c_str(), "wb" ) ) {

		if ( s_bDecode ) {

			if ( 8 == s_nBitsPerElement ) {

				TImaAdpcmDecode<8>(
					writer, (DWORD *)pLoadedData
					,(DWORD *)pLoadedData + dataSize/4, pStepTable, nTableEntries
				);

			} else if ( 16 == s_nBitsPerElement ) {

				TImaAdpcmDecode<16>(
					writer, (DWORD *)pLoadedData
					,(DWORD *)pLoadedData + dataSize/4, pStepTable, nTableEntries
				);

			} else {

				fprintf( stderr, "Unsupported sample size %d\n", s_nBitsPerElement );

			}

		} else {

			if ( 8 == s_nBitsPerElement ) {

				TImaAdpcmEncode(
					writer, (signed char *)pLoadedData
					,(signed char *)pLoadedData + dataSize, pStepTable, nTableEntries
				);

			} else if ( 16 == s_nBitsPerElement ) {

				TImaAdpcmEncode(
					writer, (WORD *)pLoadedData,
					(WORD *)pLoadedData + (dataSize/2), pStepTable, nTableEntries
				);

			} else {

				fprintf( stderr, "Unsupported sample size %d\n", s_nBitsPerElement );

			}

		}

		writer.Close();

	} else {

		fprintf( stderr, "Unable to open \"%s\" for read.\n", s_dstFilename.c_str() );

		return -1;

	}

	// unload
	// ------------------------------------------------------------------------

	loader.UnloadData( pLoadedData );

	// ------------------------------------------------------------------------

	int totalSamples = ((dataSize*8)/s_nBitsPerElement);

	printf(
		"Samples = %6d, exact %6d (%3.2f%%), way off %6d (%3.2f%%), off %6d (%3.2f%%)\n",
		 totalSamples
		,s_gExactMatches
		,(float)((float)s_gExactMatches*100.0f)/(float)totalSamples
		,s_gTotalBigErrors
		,(float)((float)s_gTotalBigErrors*100.0f)/(float)totalSamples
		,s_gTotalErrors
		,(float)((float)s_gTotalErrors*100.0f)/(float)totalSamples
	);

	if ( s_bDumpStats ) {

		// dump quantizer step table
		// --------------------------------------------------------------------

		if ( 1 <= s_nDumpStatsLevel ) {

			fprintf( stdout, "Quantized step table (%d) entries\n", nTableEntries );

			for ( int i = 0; i < nTableEntries; i++ ) {

				fprintf( stdout, "\t%c%6d\n", ((i) ? ',' : ' '), *(pStepTable + i) );

			}

		}

		// dump the difference errors
		// --------------------------------------------------------------------

		printf( "\nError stats for difference magnitudes\n" );

		int errorRange = (1 << s_nBitsPerElement)/2;

		for ( int i = 0; i < errorRange; i++ ) {

			if ( (0 == s_bErrorDifferenceTable[ i ]) && (1 > s_nDumpStatsLevel) ) {

				continue;

			}

			fprintf( stdout, "%5d:\t%9d\n", i, s_bErrorDifferenceTable[ i ] );

		}

	}

	return 0;
}

