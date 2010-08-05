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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___3DS___
#define ___3DS___

#include "ExternalImporter.h"

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
#define SMAX_MATDIFFUSE			0xA020
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
	struct CS3D_API SMaxChunk
	{
		unsigned short int m_id;
		unsigned int m_length;
		unsigned int m_bytesRead;
	};

	class CS3D_API SMaxImporter : public ExternalImporter
	{
	private:
		File * m_pFile;

	public:
		SMaxImporter();

	private:
		virtual bool _import();
		int _getString( Char * p_pBuffer);
		void _readChunk( SMaxChunk * p_chunk);
		void _processNextChunk( Imported3DModel * p_model, SMaxChunk * p_chunk);
		void _processNextObjectChunk( Imported3DModel * p_model, Imported3DObject * p_object, SMaxChunk * p_chunk);
		void _processNextMaterialChunk( Imported3DModel * p_model, SMaxChunk * p_chunk);
		void _readColorChunk( ImportedMaterialInfo *p_material, SMaxChunk * p_chunk);
		void _readVertices( Imported3DObject * p_object, SMaxChunk * p_chunk);
		void _readVertexIndices( Imported3DObject * p_object, SMaxChunk * p_chunk);
		void _readUVCoordinates( Imported3DObject * p_object, SMaxChunk * p_chunk);
		void _readObjectMaterial( Imported3DModel * p_model, Imported3DObject * p_object, SMaxChunk * p_chunk);
		void _cleanUp();
	};
}

#endif