/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_LOOP_PREREQUISITES_H___
#define ___C3D_LOOP_PREREQUISITES_H___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3DPrerequisites.hpp>

#include <Submesh.hpp>
#include <Vertex.hpp>
#include <Face.hpp>
#include <Version.hpp>
#include <Plugin.hpp>

#include <Subdivider.hpp>
#include <FrameListener.hpp>

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_Loop_API
#else
#	ifdef LoopDivider_EXPORTS
#		define C3D_Loop_API __declspec(dllexport)
#	else
#		define C3D_Loop_API __declspec(dllimport)
#	endif
#endif

namespace Loop
{
	class Subdivider;
	class Vertex;
	class Edge;
	class FaceEdges;

	DECLARE_SMART_PTR( Vertex );
	DECLARE_SMART_PTR( Edge );
	DECLARE_SMART_PTR( FaceEdges );

	DECLARE_VECTOR( FaceEdgesSPtr, FaceEdgesPtr );
	//! Map of edges, ordered by index
	DECLARE_MAP( uint32_t, EdgeSPtr, EdgePtrUInt );
	DECLARE_MAP( uint32_t, EdgePtrUIntMap, EdgePtrUIntMapUInt );
	DECLARE_MAP( uint32_t, VertexSPtr, VertexPtrUInt );
}

#endif
