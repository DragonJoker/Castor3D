// cyCodeBase by Cem Yuksel
// [www.cemyuksel.com]
//-------------------------------------------------------------------------------
//! \file   cyHairFile.h 
//! \author Cem Yuksel
//! 
//! \brief  A class for the HAIR file type
//! 
//-------------------------------------------------------------------------------
//
// Copyright (c) 2016, Cem Yuksel <cem@cemyuksel.com>
// All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
// 
//-------------------------------------------------------------------------------

#ifndef _CY_HAIR_FILE_H_INCLUDED_
#define _CY_HAIR_FILE_H_INCLUDED_

//-------------------------------------------------------------------------------

#include "cyCore.h"
#include <stdio.h>

//-------------------------------------------------------------------------------

_CY_CRT_SECURE_NO_WARNINGS

//-------------------------------------------------------------------------------
namespace cy {
//-------------------------------------------------------------------------------

#define _CY_HAIR_FILE_SEGMENTS_BIT		1
#define _CY_HAIR_FILE_POINTS_BIT		2
#define _CY_HAIR_FILE_THICKNESS_BIT		4
#define _CY_HAIR_FILE_TRANSPARENCY_BIT	8
#define _CY_HAIR_FILE_COLORS_BIT		16

#define _CY_HAIR_FILE_INFO_SIZE			88

// File read errors
#define CY_HAIR_FILE_ERROR_CANT_OPEN_FILE		-1	//!< Error code cannot open file
#define CY_HAIR_FILE_ERROR_CANT_READ_HEADER		-2	//!< Error code cannot read header
#define	CY_HAIR_FILE_ERROR_WRONG_SIGNATURE		-3	//!< Error code wrong signature
#define	CY_HAIR_FILE_ERROR_READING_SEGMENTS		-4	//!< Error code failed reading segments
#define	CY_HAIR_FILE_ERROR_READING_POINTS		-5	//!< Error code failed reading points
#define	CY_HAIR_FILE_ERROR_READING_THICKNESS	-6	//!< Error code failed reading thickness
#define	CY_HAIR_FILE_ERROR_READING_TRANSPARENCY	-7	//!< Error code failed reading transparency
#define	CY_HAIR_FILE_ERROR_READING_COLORS		-8	//!< Error code failed reading colors

//-------------------------------------------------------------------------------

//! HAIR file class

class HairFile
{
public:
	HairFile() : segments(nullptr), points(nullptr), thickness(nullptr), transparency(nullptr), colors(nullptr) { Initialize(); }
	~HairFile() { Initialize(); }

	//! Hair file header
	struct Header
	{
		char			signature[4];	//!< This should be "HAIR"
		unsigned int	hair_count;		//!< number of hair strands
		unsigned int	point_count;	//!< total number of points of all strands
		unsigned int	arrays;			//!< bit array of data in the file

		unsigned int	d_segments;		//!< default number of segments of each strand
		float			d_thickness;	//!< default thickness of hair strands
		float			d_transparency;	//!< default transparency of hair strands
		float			d_color[3];		//!< default color of hair strands

		char			info[_CY_HAIR_FILE_INFO_SIZE];	//!< information about the file
	};

	//////////////////////////////////////////////////////////////////////////
	//!@name Constant Data Access Methods
	
	Header         const & GetHeader           () const { return header; }			//!< Use this method to access header data.
	unsigned short const * GetSegmentsArray    () const { return segments; }		//!< Returns segments array (segment count for each hair strand).
	float          const * GetPointsArray      () const { return points; }			//!< Returns points array (xyz coordinates of each hair point).
	float          const * GetThicknessArray   () const { return thickness; }		//!< Returns thickness array (thickness at each hair point}.
	float          const * GetTransparencyArray() const { return transparency; }	//!< Returns transparency array (transparency at each hair point).
	float          const * GetColorsArray      () const { return colors; }			//!< Returns colors array (rgb color at each hair point).


	//////////////////////////////////////////////////////////////////////////
	//!@name Data Access Methods

	unsigned short* GetSegmentsArray    () { return segments; }		//!< Returns segments array (segment count for each hair strand).
	float*          GetPointsArray      () { return points; }		//!< Returns points array (xyz coordinates of each hair point).
	float*          GetThicknessArray   () { return thickness; }	//!< Returns thickness array (thickness at each hair point}.
	float*          GetTransparencyArray() { return transparency; }	//!< Returns transparency array (transparency at each hair point).
	float*          GetColorsArray      () { return colors; }		//!< Returns colors array (rgb color at each hair point).


	//////////////////////////////////////////////////////////////////////////
	//!@name Methods for Setting Array Sizes
	
	//! Deletes all arrays and initializes the header data.
	void Initialize()
	{
		if ( segments ) delete [] segments;
		if ( points ) delete [] points;
		if ( colors ) delete [] colors;
		if ( thickness ) delete [] thickness;
		if ( transparency ) delete [] transparency;
		header.signature[0] = 'H';
		header.signature[1] = 'A';
		header.signature[2] = 'I';
		header.signature[3] = 'R';
		header.hair_count = 0;
		header.point_count = 0;
		header.arrays = 0;	// no arrays
		header.d_segments = 0;
		header.d_thickness = 1.0f;
		header.d_transparency = 0.0f;
		header.d_color[0] = 1.0f;
		header.d_color[1] = 1.0f;
		header.d_color[2] = 1.0f;
		memset( header.info, '\0', _CY_HAIR_FILE_INFO_SIZE );
	}

	//! Sets the hair count, re-allocates segments array if necessary.
	void SetHairCount( int count )
	{
		header.hair_count = count;
		if ( segments ) {
			delete [] segments;
			segments = new unsigned short[ header.hair_count ];
		}
	}

	// Sets the point count, re-allocates points, thickness, transparency, and colors arrays if necessary.
	void SetPointCount( int count )
	{
		header.point_count = count;
		if ( points ) {
			delete [] points;
			points = new float[ header.point_count*3 ];
		}
		if ( thickness ) {
			delete [] thickness;
			thickness = new float[ header.point_count ];
		}
		if ( transparency ) {
			delete [] transparency;
			transparency = new float[ header.point_count ];
		}
		if ( colors ) {
			delete [] colors;
			colors = new float[ header.point_count*3 ];
		}
	}

	//! Use this function to allocate/delete arrays.
	//! Before you call this method set hair count and point count.
	//! Note that a valid HAIR file should always have points array.
	void SetArrays( int array_types )
	{
		header.arrays = array_types;
		if (  (header.arrays & _CY_HAIR_FILE_SEGMENTS_BIT    ) && !segments     ) { segments = new unsigned short[header.hair_count]; }
		if ( !(header.arrays & _CY_HAIR_FILE_SEGMENTS_BIT    ) &&  segments     ) { delete [] segments; segments=nullptr; }
		if (  (header.arrays & _CY_HAIR_FILE_POINTS_BIT      ) && !points       ) { points = new float[header.point_count*3]; }
		if ( !(header.arrays & _CY_HAIR_FILE_POINTS_BIT      ) &&  points       ) { delete [] points; points=nullptr; }
		if (  (header.arrays & _CY_HAIR_FILE_THICKNESS_BIT   ) && !thickness    ) { thickness = new float[header.point_count]; }
		if ( !(header.arrays & _CY_HAIR_FILE_THICKNESS_BIT   ) &&  thickness    ) { delete [] thickness; thickness=nullptr; }
		if (  (header.arrays & _CY_HAIR_FILE_TRANSPARENCY_BIT) && !transparency ) { transparency = new float[header.point_count]; }
		if ( !(header.arrays & _CY_HAIR_FILE_TRANSPARENCY_BIT) &&  transparency ) { delete [] transparency; transparency=nullptr; }
		if (  (header.arrays & _CY_HAIR_FILE_COLORS_BIT      ) && !colors       ) { colors = new float[header.point_count*3]; }
		if ( !(header.arrays & _CY_HAIR_FILE_COLORS_BIT      ) &&  colors       ) { delete [] colors; colors=nullptr; }
	}

	//! Sets default number of segments for all hair strands, which is used if segments array does not exist.
	void SetDefaultSegmentCount( int s ) { header.d_segments = s; }

	//! Sets default hair strand thickness, which is used if thickness array does not exist.
	void SetDefaultThickness( float t ) { header.d_thickness = t; }

	//! Sets default hair strand transparency, which is used if transparency array does not exist.
	void SetDefaultTransparency( float t ) { header.d_transparency = t; }

	//! Sets default hair color, which is used if color array does not exist.
	void SetDefaultColor( float r, float g, float b ) { header.d_color[0]=r; header.d_color[1]=g; header.d_color[2]=b; }


	//////////////////////////////////////////////////////////////////////////
	//!@name Load and Save Methods

	//! Loads hair data from the given HAIR file.
	int LoadFromFile( char const *filename )
	{
		Initialize();

		FILE *fp;
		fp = fopen( filename, "rb" );
		if ( fp == nullptr ) return CY_HAIR_FILE_ERROR_CANT_OPEN_FILE;

		// read the header
		size_t headread = fread( &header, sizeof(Header), 1, fp );

		#define _CY_FAILED_RETURN(errorno) { Initialize(); fclose( fp ); return errorno; }


		// Check if header is correctly read
		if ( headread < 1 ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_CANT_READ_HEADER);

		// Check if this is a hair file
		if ( strncmp( header.signature, "HAIR", 4) != 0 ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_WRONG_SIGNATURE);

		// Read segments array
		if ( header.arrays & _CY_HAIR_FILE_SEGMENTS_BIT ) {
			segments = new unsigned short[ header.hair_count ];
			size_t readcount = fread( segments, sizeof(unsigned short), header.hair_count, fp );
			if ( readcount < header.hair_count ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_READING_SEGMENTS);
		}

		// Read points array
		if ( header.arrays & _CY_HAIR_FILE_POINTS_BIT ) {
			points = new float[ header.point_count*3 ];
			size_t readcount = fread( points, sizeof(float), header.point_count*3, fp );
			if ( readcount < header.point_count*3 ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_READING_POINTS);
		}

		// Read thickness array
		if ( header.arrays & _CY_HAIR_FILE_THICKNESS_BIT ) {
			thickness = new float[ header.point_count ];
			size_t readcount = fread( thickness, sizeof(float), header.point_count, fp );
			if ( readcount < header.point_count ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_READING_THICKNESS);
		}

		// Read thickness array
		if ( header.arrays & _CY_HAIR_FILE_TRANSPARENCY_BIT ) {
			transparency = new float[ header.point_count ];
			size_t readcount = fread( transparency, sizeof(float), header.point_count, fp );
			if ( readcount < header.point_count ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_READING_TRANSPARENCY);
		}

		// Read colors array
		if ( header.arrays & _CY_HAIR_FILE_COLORS_BIT ) {
			colors = new float[ header.point_count*3 ];
			size_t readcount = fread( colors, sizeof(float), header.point_count*3, fp );
			if ( readcount < header.point_count*3 ) _CY_FAILED_RETURN(CY_HAIR_FILE_ERROR_READING_COLORS);
		}

		fclose( fp );

		return header.hair_count;
	}

	//! Saves hair data to the given HAIR file.
	int SaveToFile( char const *filename ) const
	{
		FILE *fp;
		fp = fopen( filename, "wb" );
		if ( fp == nullptr ) return -1;

		// Write header
		fwrite( &header, sizeof(Header), 1, fp );

		// Write arrays
		if ( header.arrays & _CY_HAIR_FILE_SEGMENTS_BIT     ) fwrite( segments,     sizeof(unsigned short), header.hair_count,    fp );
		if ( header.arrays & _CY_HAIR_FILE_POINTS_BIT       ) fwrite( points,       sizeof(float),          header.point_count*3, fp );
		if ( header.arrays & _CY_HAIR_FILE_THICKNESS_BIT    ) fwrite( thickness,    sizeof(float),          header.point_count,   fp );
		if ( header.arrays & _CY_HAIR_FILE_TRANSPARENCY_BIT ) fwrite( transparency, sizeof(float),          header.point_count,   fp );
		if ( header.arrays & _CY_HAIR_FILE_COLORS_BIT       ) fwrite( colors,       sizeof(float),          header.point_count*3, fp );

		fclose( fp );

		return header.hair_count;
	}


	//////////////////////////////////////////////////////////////////////////
	//!@name Other Methods

	//! Fills the given direction array with normalized directions using the points array.
	//! Call this function if you need strand directions for shading.
	//! The given array dir should be allocated as an array of size 3 times point count.
	//! Returns point count, returns zero if fails.
	int FillDirectionArray( float *dir )
	{
		if ( dir==nullptr || header.point_count<=0 || points==nullptr ) return 0;

		int p = 0;	// point index
		for ( unsigned int i=0; i<header.hair_count; i++ ) {
			int s = (segments) ? segments[i] : header.d_segments;
			if ( s > 1 ) {
				// direction at point1
				float len0, len1;
				ComputeDirection( &dir[(p+1)*3], len0, len1, &points[p*3], &points[(p+1)*3], &points[(p+2)*3] );

				// direction at point0
				float d0[3];
				d0[0] = points[(p+1)*3]   - dir[(p+1)*3]  *len0*0.3333f - points[p*3];
				d0[1] = points[(p+1)*3+1] - dir[(p+1)*3+1]*len0*0.3333f - points[p*3+1];
				d0[2] = points[(p+1)*3+2] - dir[(p+1)*3+2]*len0*0.3333f - points[p*3+2];
				float d0lensq = d0[0]*d0[0] + d0[1]*d0[1] + d0[2]*d0[2];
				float d0len = ( d0lensq > 0 ) ? (float) sqrt(d0lensq) : 1.0f;
				dir[p*3]   = d0[0] / d0len;
				dir[p*3+1] = d0[1] / d0len;
				dir[p*3+2] = d0[2] / d0len;

				// We computed the first 2 points
				p += 2;

				// Compute the direction for the rest
				for ( int t=2; t<s; t++, p++ ) {
					ComputeDirection( &dir[p*3], len0, len1, &points[(p-1)*3], &points[p*3], &points[(p+1)*3] );
				}

				// direction at the last point
				d0[0] = - points[(p-1)*3]   + dir[(p-1)*3]  *len1*0.3333f + points[p*3];
				d0[1] = - points[(p-1)*3+1] + dir[(p-1)*3+1]*len1*0.3333f + points[p*3+1];
				d0[2] = - points[(p-1)*3+2] + dir[(p-1)*3+2]*len1*0.3333f + points[p*3+2];
				d0lensq = d0[0]*d0[0] + d0[1]*d0[1] + d0[2]*d0[2];
				d0len = ( d0lensq > 0 ) ? (float) sqrt(d0lensq) : 1.0f;
				dir[p*3]   = d0[0] / d0len;
				dir[p*3+1] = d0[1] / d0len;
				dir[p*3+2] = d0[2] / d0len;
				p++;

			} else if ( s > 0 ) {
				// if it has a single segment
				float d0[3];
				d0[0] = points[(p+1)*3]   - points[p*3];
				d0[1] = points[(p+1)*3+1] - points[p*3+1];
				d0[2] = points[(p+1)*3+2] - points[p*3+2];
				float d0lensq = d0[0]*d0[0] + d0[1]*d0[1] + d0[2]*d0[2];
				float d0len = ( d0lensq > 0 ) ? (float) sqrt(d0lensq) : 1.0f;
				dir[p*3]   = d0[0] / d0len;
				dir[p*3+1] = d0[1] / d0len;
				dir[p*3+2] = d0[2] / d0len;
				dir[(p+1)*3]   = dir[p*3];
				dir[(p+1)*3+1] = dir[p*3+1];
				dir[(p+1)*3+2] = dir[p*3+2];
				p += 2;
			}
			//*/
		}
		return p;
	}


private:
	//////////////////////////////////////////////////////////////////////////
	//!@name Private Variables and Methods

	Header header;
	unsigned short	*segments;
	float			*points;
	float			*thickness;
	float			*transparency;
	float			*colors;

	// Given point before (p0) and after (p2), computes the direction (d) at p1.
	float ComputeDirection( float *d, float &d0len, float &d1len, float const *p0, float const *p1, float const *p2 )
	{
		// line from p0 to p1
		float d0[3];
		d0[0] = p1[0] - p0[0];
		d0[1] = p1[1] - p0[1];
		d0[2] = p1[2] - p0[2];
		float d0lensq = d0[0]*d0[0] + d0[1]*d0[1] + d0[2]*d0[2];
		d0len = ( d0lensq > 0 ) ? (float) sqrt(d0lensq) : 1.0f;

		// line from p1 to p2
		float d1[3];
		d1[0] = p2[0] - p1[0];
		d1[1] = p2[1] - p1[1];
		d1[2] = p2[2] - p1[2];
		float d1lensq = d1[0]*d1[0] + d1[1]*d1[1] + d1[2]*d1[2];
		d1len = ( d1lensq > 0 ) ? (float) sqrt(d1lensq) : 1.0f;

		// make sure that d0 and d1 has the same length
		d0[0] *= d1len / d0len;
		d0[1] *= d1len / d0len;
		d0[2] *= d1len / d0len;

		// direction at p1
		d[0] = d0[0] + d1[0];
		d[1] = d0[1] + d1[1];
		d[2] = d0[2] + d1[2];
		float dlensq = d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
		float dlen = ( dlensq > 0 ) ? (float) sqrt(dlensq) : 1.0f;
		d[0] /= dlen;
		d[1] /= dlen;
		d[2] /= dlen;

		return d0len;
	}
};

//-------------------------------------------------------------------------------
} // namespace cy
//-------------------------------------------------------------------------------

typedef cy::HairFile cyHairFile;	//!< HAIR file class

//-------------------------------------------------------------------------------

_CY_CRT_SECURE_RESUME_WARNINGS
#endif