/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_SCENE_FILE_PARSER_H___
#define ___C3D_SCENE_FILE_PARSER_H___

#include "Castor3DPrerequisites.hpp"

#include <FileParser.hpp>
#include <FileParserContext.hpp>
#include "Submesh.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	//! Scene file sections Enum
	/*!
	The enumeration which defines all the sections and subsections of a scene file
	*/
	typedef enum eSECTION CASTOR_TYPE( uint32_t )
	{
		eSECTION_ROOT,					//!< No section id est root
		eSECTION_SCENE,					//!< Scene section
		eSECTION_WINDOW,				//!< Window section
		eSECTION_SAMPLER,				//!< Sampler state section
		eSECTION_CAMERA,				//!< Camera section
		eSECTION_VIEWPORT,				//!< Viewport section
		eSECTION_LIGHT,					//!< Light section
		eSECTION_NODE,					//!< SceneNode section
		eSECTION_OBJECT,				//!< Geometry section
		eSECTION_OBJECT_MATERIALS,		//!< Geometry materials section
		eSECTION_FONT,					//!< Font section
		eSECTION_PANEL_OVERLAY,			//!< Panel Overlay sections
		eSECTION_BORDER_PANEL_OVERLAY,	//!< Border Panel Overlay sections
		eSECTION_TEXT_OVERLAY,			//!< Text Overlay sections
		eSECTION_MESH,					//!< Mesh subsection of a geometry section
		eSECTION_SUBMESH,				//!< Submesh subsection of a Mesh subsection
		eSECTION_MATERIAL,				//!< Material section
		eSECTION_PASS,					//!< Pass subsection of a material section
		eSECTION_TEXTURE_UNIT,			//!< TextureUnit subsection of a pass subsection
		eSECTION_RENDER_TARGET,			//!< RenderTarget subsection of a texture unit or window subsection
		eSECTION_GLSL_SHADER,			//!< GLSL Shader subsection of a pass subsection
		eSECTION_HLSL_SHADER,			//!< HLSL Shader subsection of a pass subsection
		eSECTION_SHADER_PROGRAM,		//!< Shader Program subsection of a shader subsection
		eSECTION_SHADER_SAMPLER,		//!< Sampler shader variable subsection of a shader program subsection
		eSECTION_SHADER_UBO,			//!< UBO subsection of a shader subsection
		eSECTION_SHADER_UBO_VARIABLE,	//!< Variable subsection of a UBO subsection
		eSECTION_BILLBOARD,				//!< Billboards list section
		eSECTION_BILLBOARD_LIST,		//!< Billboards points list subsection
		eSECTION_ANIMGROUP,				//!< Animated Objects Groups section
		eSECTION_COUNT
	}	eSECTION;
	//! The context used into parsing functions
	/*!
	While parsing a scene file, the context holds the important data retrieved
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API SceneFileContext
		: public Castor::FileParserContext
	{
	public:
		SceneSPtr pScene;
		RenderWindowSPtr pWindow;
		SceneNodeSPtr pSceneNode;
		GeometrySPtr pGeometry;
		MeshSPtr pMesh;
		SubmeshSPtr pSubmesh;
		LightSPtr pLight;
		CameraSPtr pCamera;
		MaterialSPtr pMaterial;
		SamplerSPtr pSampler;
		RenderTargetSPtr pRenderTarget;
		uint32_t uiPass;
		TextureUnitSPtr pTextureUnit;
		ShaderProgramBaseSPtr pShaderProgram;
		eSHADER_TYPE eShaderObject;
		FrameVariableBufferSPtr pFrameVariableBuffer;
		FrameVariableSPtr pFrameVariable;
		OneTextureFrameVariableSPtr pSamplerFrameVariable;
		OverlaySPtr pOverlay;
		BillboardListSPtr pBillboards;
		int iFace1;
		int iFace2;
		eLIGHT_TYPE eLightType;
		eMESH_TYPE eMeshType;
		eTOPOLOGY ePrimitiveType;
		ViewportSPtr pViewport;
		Castor::String strName;
		Castor::String strName2;
		Castor::Path path;
		uint8_t uiUInt8;
		uint16_t uiUInt16;
		uint32_t uiUInt32;
		uint64_t uiUInt64;
		int8_t iInt8;
		int16_t iInt16;
		int32_t iInt32;
		int64_t iInt64;
		bool bBool1;
		bool bBool2;
		SceneNodeSPtr m_pGeneralParentMaterial;
		ScenePtrStrMap mapScenes;
		SceneFileParser * m_pParser;
		eRENDERER_TYPE eRendererType;
		RealArray vertexPos;
		RealArray vertexNml;
		RealArray vertexTan;
		RealArray vertexTex;
		UIntArray faces;
		AnimatedObjectGroupSPtr pGroup;

		UIntStrMap m_mapBlendFactors;
		UIntStrMap m_mapTypes;
		UIntStrMap m_mapMapModes;
		UIntStrMap m_mapAlphaFuncs;
		UIntStrMap m_mapTextureArguments;
		UIntStrMap m_mapTextureRgbFunctions;
		UIntStrMap m_mapTextureAlphaFunctions;
		UIntStrMap m_mapTextureChannels;
		UIntStrMap m_mapNormalModes;
		UIntStrMap m_mapLightTypes;
		UIntStrMap m_mapPrimitiveTypes;
		UIntStrMap m_mapPrimitiveOutputTypes;
		UIntStrMap m_mapModels;
		UIntStrMap m_mapViewportModes;
		UIntStrMap m_mapInterpolationModes;
		UIntStrMap m_mapWrappingModes;
		UIntStrMap m_mapShaderTypes;
		UIntStrMap m_mapVariableTypes;
		UIntStrMap m_mapMovables;
		UIntStrMap m_mapTextWrappingModes;
		UIntStrMap m_mapBorderPositions;
		UIntStrMap m_mapBlendModes;

	public:
		/**
		 * Constructor
		 */
		SceneFileContext( SceneFileParser * p_pParser, Castor::TextFile * p_pFile );
		/**
		 * Initialises all variables
		 */
		void Initialise();
	};
	//! ESCN file parser
	/*!
	Reads ESCN files and extracts all 3D data from it
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API SceneFileParser : public Castor::FileParser
	{
	private:
		Castor::String	m_strSceneFilePath;
		ScenePtrStrMap	m_mapScenes;
		Engine 	*	m_pEngine;

	public:
		/**
		 * Constructor
		 */
		SceneFileParser( Engine * p_pEngine );
		/**
		 * Destructor
		 */
		~SceneFileParser();
		/**
		 * Retrieves the render window defined by the scene
		 */
		RenderWindowSPtr GetRenderWindow();
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in,out]	p_file	The file
		 *\return	the parsed scene
		 */
		bool ParseFile( Castor::TextFile & p_file );
		/**
		 * Parses the given file (expecting it to be in ESCN file format)
		 *\param[in]	p_pathFile	The file path
		 *\return	true if successful, false if not
		 */
		bool ParseFile( Castor::Path const & p_pathFile );

		inline Engine 			*				GetEngine()const
		{
			return m_pEngine;
		}
		inline ScenePtrStrMap::iterator			ScenesBegin()
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator	ScenesBegin()const
		{
			return m_mapScenes.begin();
		}
		inline ScenePtrStrMap::const_iterator	ScenesEnd()const
		{
			return m_mapScenes.end();
		}

	private:
		virtual void DoInitialiseParser( Castor::TextFile & p_file );
		virtual void DoCleanupParser();
		virtual bool DoDelegateParser( Castor::String const & CU_PARAM_UNUSED( p_strLine ) )
		{
			return false;
		}
		virtual void DoDiscardParser( Castor::String const & p_strLine );
		virtual void DoValidate();
	};
}

#pragma warning( pop )

#endif
