/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

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
#ifndef ___C3D_LOOP_PREREQUISITES_H___
#define ___C3D_LOOP_PREREQUISITES_H___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3DPrerequisites.hpp>

#include <Event/Frame/FrameListener.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Subdivider.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>

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
