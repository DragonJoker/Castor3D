/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___3DS___
#define ___3DS___

#include <Castor3D/importer/Importer.h>

#	ifdef SMaxImporter_EXPORTS
#		define C3D_SMax_API __declspec(dllexport)
#	else
#		define C3D_SMax_API __declspec(dllimport)
#	endif

// Primary Chunk, at the beginning of each file
#define SMAX_PRIMARY			0x4D4D

// Main Chunks
#define SMAX_OBJECTINFO			0x3D3D
#define SMAX_VERSION			0x0002
#define SMAX_EDITKEYFRAME		0xB000

// sub defines of OBJECTINFO
#define SMAX_MATERIAL			0xAFFF
#define SMAX_OBJECT				0x4000
#define SMAX_AMBIENT			0x2100

// sub defines of MATERIAL
#define SMAX_MATNAME			0xA000
#define SMAX_MATAMBIENT			0xA010
#define SMAX_MATDIFFUSE			0xA020
#define SMAX_MATSPECULAR		0xA030
#define SMAX_MATSHININESS		0xA040
#define SMAX_MATALPHA			0xA050
#define SMAX_MATTWOSIDED		0xA081
#define SMAX_MATMAP				0xA200
#define SMAX_MATMAPFILE			0xA300

// sub defines of OBJECT
#define SMAX_OBJECT_MESH		0x4100
#define SMAX_OBJECT_LIGHT		0x4600
#define SMAX_OBJECT_CAMERA		0x4700

// sub defines of OBJECT_MESH
#define SMAX_OBJECT_VERTICES    0x4110
#define SMAX_OBJECT_FACES		0x4120
#define SMAX_OBJECT_MATERIAL	0x4130
#define SMAX_OBJECT_UV			0x4140

namespace Castor3D
{
	//! 3DS file importer
	/*!
	Imports data from 3DS (3D Studio Max) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_SMax_API SMaxImporter : public Importer, public MemoryTraced<SMaxImporter>
	{
	private:
		struct SMaxChunk
		{
			unsigned short int m_id;
			unsigned int m_length;
			unsigned int m_bytesRead;
		};

	private:
		File * m_pFile;
		Point2r * m_texVerts;
		int m_iNumOfMaterials;

	public:
		/**
		 * Constructor
		 */
		SMaxImporter( SceneManager * p_pManager);

	private:
		virtual bool _import();
		int _getString( Char * p_pBuffer);
		int _getString( String & p_strString);
		void _readChunk( SMaxChunk * p_chunk);
		void _processNextChunk( MeshPtr p_pMesh, SMaxChunk * p_chunk);
		void _processNextObjectChunk( MeshPtr p_pMesh, SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk);
		void _processNextMaterialChunk( SMaxChunk * p_chunk);
		void _processMaterialMapChunk( String & p_strName, SMaxChunk * p_chunk);
		void _readColorChunk( Colour & p_colour, SMaxChunk * p_chunk);
		void _readVertices( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk);
		void _readVertexIndices( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk);
		void _readUVCoordinates( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk);
		void _readObjectMaterial( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk);
		void _cleanUp();
	};
}

#endif