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
#ifndef ___C3D_Module_Scene___
#define ___C3D_Module_Scene___

#include "../Module_General.h"
#include "../main/Module_Main.h"
#include "../camera/Module_Camera.h"
#include "../material/Module_Material.h"
#include "../light/Module_Light.h"
#include "../importer/Module_Importer.h"
#include "../render_system/Module_Render.h"

namespace Castor3D
{
	class Scene;
	class SceneManager;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;

	//! Scene pointer map, sorted by name
	typedef std::map	<String, Scene *>		SceneStrMap;
	//! SceneNode pointer map, sorted by name
	typedef std::map	<String, SceneNode *>	SceneNodeStrMap;
	typedef std::vector	<SceneNode *>			SceneNodePtrArray;
	
	typedef bool ( * ParserFunction)( String & p_strParams, SceneFileContext * p_pContext);
	typedef std::map <String, ParserFunction> AttributeParserMap;

	#define DECLARE_ATTRIBUTE_PARSER( X) bool CS3D_API X( String & p_strParams, SceneFileContext * p_pContext);
	#define IMPLEMENT_ATTRIBUTE_PARSER( X) bool Castor3D :: X( String & p_strParams, SceneFileContext * p_pContext)
	#define PARSING_ERROR( p_strError) SceneFileParser::ParseError( p_strError, p_pContext)
	#define PARSING_WARNING( p_strWarning) SceneFileParser::ParseWarning( p_strWarning, p_pContext)
	#define PARSE_V2( V) SceneFileParser::ParseVector2( p_strParams, V, p_pContext)
	#define PARSE_V3( V) SceneFileParser::ParseVector3( p_strParams, V, p_pContext)
	#define PARSE_V4( V) SceneFileParser::ParseVector4( p_strParams, V, p_pContext)
}

#endif
