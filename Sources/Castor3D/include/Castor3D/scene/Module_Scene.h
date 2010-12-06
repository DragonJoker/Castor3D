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
#ifndef ___C3D_Module_Scene___
#define ___C3D_Module_Scene___

#include "../Module_General.h"
#include "../main/Module_Main.h"
#include "../camera/Module_Camera.h"
#include "../material/Module_Material.h"
#include "../light/Module_Light.h"
#include "../importer/Module_Importer.h"
#include "../render_system/Module_Render.h"
#include "../geometry/Module_Geometry.h"

namespace Castor3D
{
	class Scene;
	class SceneManager;
	class NodeBase;
	template <typename T> class Node;
	class SceneFileContext;
	class SceneFileParser;

	typedef Node<Light>		LightNode;
	typedef Node<Camera>	CameraNode;
	typedef Node<Geometry>	GeometryNode;

	typedef Templates::SharedPtr<NodeBase>			NodePtr;
	typedef Templates::SharedPtr<LightNode>			LightNodePtr;
	typedef Templates::SharedPtr<CameraNode>		CameraNodePtr;
	typedef Templates::SharedPtr<GeometryNode>		GeometryNodePtr;
	typedef Templates::SharedPtr<Scene>				ScenePtr;
	typedef Templates::SharedPtr<SceneFileContext>	SceneFileContextPtr;
	typedef Templates::SharedPtr<SceneFileParser>	SceneFileParserPtr;

	typedef C3DMap(		String,	ScenePtr)			ScenePtrStrMap;			//!< Scene pointer map, sorted by name
	typedef C3DMap(		String,	NodePtr)			NodePtrStrMap;			//!< SceneNode pointer map, sorted by name
	typedef C3DMap(		String,	LightNodePtr)		LightNodePtrStrMap;		//!< LightNode pointer map, sorted by name
	typedef C3DMap(		String,	CameraNodePtr)		CameraNodePtrStrMap;	//!< CameraNode pointer map, sorted by name
	typedef C3DMap(		String,	GeometryNodePtr)	GeometryNodePtrStrMap;	//!< GeometryNode pointer map, sorted by name
	typedef C3DVector(	NodePtr)					NodePtrArray;
	
	typedef bool ( * ParserFunction)( String & p_strParams, SceneFileContextPtr p_pContext);
	typedef C3DMap(		String,	ParserFunction) AttributeParserMap;

	#define DECLARE_ATTRIBUTE_PARSER( X) bool C3D_API X( String & p_strParams, SceneFileContextPtr p_pContext);
	#define IMPLEMENT_ATTRIBUTE_PARSER( X) bool Castor3D :: X( String & p_strParams, SceneFileContextPtr p_pContext)
	#define PARSING_ERROR( p_strError) SceneFileParser::ParseError( p_strError, p_pContext)
	#define PARSING_WARNING( p_strWarning) SceneFileParser::ParseWarning( p_strWarning, p_pContext)
	#define PARSE_V2( T, V) SceneFileParser::ParseVector<T, 2>( p_strParams, V, p_pContext)
	#define PARSE_V3( T, V) SceneFileParser::ParseVector<T, 3>( p_strParams, V, p_pContext)
	#define PARSE_V4( T, V) SceneFileParser::ParseVector<T, 4>( p_strParams, V, p_pContext)
}

#endif
