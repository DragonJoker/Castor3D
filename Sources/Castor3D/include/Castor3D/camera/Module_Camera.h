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
#ifndef ___C3D_Module_Camera___
#define ___C3D_Module_Camera___

#include "../Module_General.h"

namespace Castor3D
{
	class Camera;
	class Viewport;
	class Ray;

	//! The  viewport projection types enumerator
	/*!
	There are 2 projection types : 2D and 3D
	*/
	enum ProjectionType
	{
		pt3DView,	//!< 3 Dimensions projection type
		pt2DView	//!< 2 Dimensions projection type
	};
	//! The selection mode enumerator
	/*!
	There are four modes of selection when you click over an object
	*/
	typedef enum SelectionMode
	{
		smVertex,		//!< Vertex selection mode
		smEdge,			//!< Edge selection mode
		smFace,			//!< Face selection mode
		smSubmesh,		//!< Submesh selection mode
		smObject		//!< Geometry selection mode
	} SelectionMode;

	//! Camera pointer array
	typedef std::vector <Camera *>			CameraPtrArray;
	//! Camera pointer map, sorted by name
	typedef std::map	<String, Camera *>	CameraStrMap;
}

#endif

