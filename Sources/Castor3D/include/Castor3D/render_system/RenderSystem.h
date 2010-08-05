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
#ifndef ___C3D_RenderSystem___
#define ___C3D_RenderSystem___

#include "../shader/Module_Shader.h"
#include "../material/Module_Material.h"
#include "../render_system/Module_Render.h"

namespace Castor3D
{
	//! The render system representation
	/*!
	This is the class which is the link between Castor3D and the renderer driver (OpenGL or Direct3D)
	It is also the noly class which can create the renderers
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API RenderSystem
	{
	protected:
		static RenderSystem * sm_singleton;		//!< The singleton
		static bool sm_useMultiTexturing;		//!< Tells whether or not we can use multitexturing
		static bool sm_initialised;				//!< tells whether or not it is initialised
		SubmeshRendererPtrArray				m_submeshesRenderers;
		TextureEnvironmentRendererPtrArray	m_texEnvRenderers;
		TextureRendererPtrArray				m_textureRenderers;
		PassRendererPtrArray				m_passRenderers;
		LightRendererPtrArray				m_lightRenderers;
		WindowRendererPtrArray				m_windowRenderers;
		CameraRendererPtrArray				m_cameraRenderers;
		ViewportRendererPtrArray			m_viewportRenderers;
		SceneNodeRendererPtrArray			m_nodeRenderers;
		OverlayRendererPtrArray				m_overlayRenderers;

	public:
		static int sm_drawTypes[3];				//!< The link between Castor3D::DrawType and render system (OpenGL or Direct3D) draw types
		static int sm_environmentModes[5];		//!< The link between Castor3D::EnvironmentMode and render system (OpenGL or Direct3D) environment modes
		static int sm_RGBCombinations[8];		//!< The link between Castor3D::RGBCombination and render system (OpenGL or Direct3D) rgb combinations
		static int sm_RGBOperands[4];			//!< The link between Castor3D::RGBOperand and render system (OpenGL or Direct3D) rgb operands
		static int sm_alphaCombinations[6];		//!< The link between Castor3D::AlphaCombination and render system (OpenGL or Direct3D) alpha combinations
		static int sm_alphaOperands[2];			//!< The link between Castor3D::AlphaOperand and render system (OpenGL or Direct3D) alpha operands
		static int sm_combinationSources[5];	//!< The link between Castor3D::CombinationSource and render system (OpenGL or Direct3D) combination sources
		static int sm_textureDimensions[3];		//!< The link between Castor3D::TextureDimension and render system (OpenGL or Direct3D) texture dimensions
		static int sm_lightIndexes[8];			//!< The link between Castor3D::LightIndexes and render system (OpenGL or Direct3D) light indexes

	public:
		/**
		 * Constructor
		 */
		RenderSystem();
		/**
		 * Destructor
		 */
		virtual ~RenderSystem(){}
		/**
		 * Initialises the render system
		 */
		virtual void Initialise()							= 0;
		/**
		 * Display an arc in a given display mode
		 */
		virtual void DisplayArc( DrawType p_displayMode,
								 const Vector3fPtrList & p_vertex) = 0;
		/**
		 * Creates a SubmeshRenderer, only the render system can do that
		 */
		virtual SubmeshRenderer *				CreateSubmeshRenderer				() = 0;
		/**
		 * Creates a TextureEnvironmentRenderer, only the render system can do that
		 */
		virtual TextureEnvironmentRenderer *	CreateTextureEnvironmentRenderer	() = 0;
		/**
		 * Creates a TextureRenderer, only the render system can do that
		 */
		virtual TextureRenderer *				CreateTextureRenderer				() = 0;
		/**
		 * Creates a PassRenderer, only the render system can do that
		 */
		virtual PassRenderer *				CreatePassRenderer						() = 0;
		/**
		 * Creates a LightRenderer, only the render system can do that
		 */
		virtual LightRenderer *					CreateLightRenderer					() = 0;
		/**
		 * Creates a CameraRenderer, only the render system can do that
		 */
		virtual CameraRenderer *				CreateCameraRenderer				() = 0;
		/**
		 * Creates a ViewportRenderer, only the render system can do that
		 */
		virtual ViewportRenderer *				CreateViewportRenderer				() = 0;
		/**
		 * Creates a WindowRenderer, only the render system can do that
		 */
		virtual WindowRenderer *				CreateWindowRenderer				() = 0;
		/**
		 * Creates a SceneNodeRenderer, only the render system can do that
		 */
		virtual SceneNodeRenderer *				CreateSceneNodeRenderer				() = 0;
		/**
		 * Creates a OverlayRenderer, only the render system can do that
		 */
		virtual OverlayRenderer *				CreateOverlayRenderer				() = 0;
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		virtual ShaderObject *					CreateVertexShader					() = 0;
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		virtual ShaderObject *					CreateFragmentShader				() = 0;
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		virtual ShaderObject *					CreateGeometryShader				() = 0;
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		virtual ShaderProgram *					CreateShaderProgram					( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
		/**
		 * Applies generic transformations (translation and rotation) from a position and an orientation
		 */
		virtual void ApplyTransformations( const Vector3f & p_position, float * p_matrix) = 0;
		/**
		* Removes generic transformations (translation and rotation) from a position and an orientation
		*/
		virtual void RemoveTransformations() = 0;

		virtual void BeginOverlaysRendering()=0;
		virtual void EndOverlaysRendering()=0;

	public:
		static inline RenderSystem *	GetSingletonPtr		()	{ return sm_singleton; }
		static inline bool				UseMultiTexturing	()	{ return sm_useMultiTexturing; }
		static inline bool				IsInitialised		()	{ return sm_initialised; }
	};
}

#endif
