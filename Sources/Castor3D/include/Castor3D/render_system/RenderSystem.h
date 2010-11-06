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
		static bool sm_useShaders;				//!< Tells whether or not we can use shaders
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
		static int sm_drawTypes[3];				//!< The link between Castor3D::eDRAW_TYPE and render system (OpenGL or Direct3D) draw types
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
		virtual void Delete()								= 0;
		/**
		 * Display an arc in a given display mode
		 */
		virtual void DisplayArc( eDRAW_TYPE p_displayMode, const VertexPtrList & p_vertex) = 0;
		/**
		 * Creates a renderer
		 */
		template <typename Ty> static Templates::SharedPtr<Ty> CreateRenderer()
		{
			return sm_singleton->_createRendererNS<Ty>();
		}
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateVertexShader		() = 0;
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateFragmentShader	() = 0;
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateGeometryShader	() = 0;
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		virtual ShaderProgram *	CreateShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
		/**
		 * Applies generic transformations (translation and rotation) from a position and an orientation
		 */
		virtual void ApplyTransformations( const Point3r & p_position, real * p_matrix) = 0;
		/**
		* Removes generic transformations (translation and rotation) from a position and an orientation
		*/
		virtual void RemoveTransformations()=0;
		/**
		 * Begins the overlay (2d elements) rendering
		 */
		virtual void BeginOverlaysRendering()=0;
		/**
		 * Ends the overlay (2d elements) rendering
		 */
		virtual void EndOverlaysRendering()=0;
		/**
		 * Renders the ambient lighting
		 */
		virtual void RenderAmbientLight( const Colour & p_clColour)=0;

	public:
		template <typename T>
		static inline T *	GetSingletonPtr		()	{ return (T *)sm_singleton; }
		static inline bool	UseMultiTexturing	()	{ return sm_useMultiTexturing; }
		static inline bool	UseShaders			()	{ return sm_useShaders; }
		static inline bool	IsInitialised		()	{ return sm_initialised; }

	private:
		template <typename Ty> Templates::SharedPtr<Ty> _createRendererNS()
		{
			Log::LogMessage( "Can't create a renderer of unknown type");
		}

		template <> Templates::SharedPtr<SubmeshRenderer> _createRendererNS()
		{
			return _createSubmeshRenderer();
		}
		template <> Templates::SharedPtr<TextureEnvironmentRenderer> _createRendererNS()
		{
			return _createTextureEnvironmentRenderer();
		}
		template <> Templates::SharedPtr<TextureRenderer> _createRendererNS()
		{
			return _createTextureRenderer();
		}
		template <> Templates::SharedPtr<PassRenderer> _createRendererNS()
		{
			return _createPassRenderer();
		}
		template <> Templates::SharedPtr<LightRenderer> _createRendererNS()
		{
			return _createLightRenderer();
		}
		template <> Templates::SharedPtr<CameraRenderer> _createRendererNS()
		{
			return _createCameraRenderer();
		}
		template <> Templates::SharedPtr<ViewportRenderer> _createRendererNS()
		{
			return _createViewportRenderer();
		}
		template <> Templates::SharedPtr<WindowRenderer> _createRendererNS()
		{
			return _createWindowRenderer();
		}
		template <> Templates::SharedPtr<SceneNodeRenderer> _createRendererNS()
		{
			return _createSceneNodeRenderer();
		}
		template <> Templates::SharedPtr<OverlayRenderer> _createRendererNS()
		{
			return _createOverlayRenderer();
		}

		virtual SubmeshRendererPtr				_createSubmeshRenderer				()=0;
		virtual TextureEnvironmentRendererPtr	_createTextureEnvironmentRenderer	()=0;
		virtual TextureRendererPtr				_createTextureRenderer				()=0;
		virtual PassRendererPtr					_createPassRenderer					()=0;
		virtual LightRendererPtr				_createLightRenderer				()=0;
		virtual CameraRendererPtr				_createCameraRenderer				()=0;
		virtual ViewportRendererPtr				_createViewportRenderer				()=0;
		virtual WindowRendererPtr				_createWindowRenderer				()=0;
		virtual SceneNodeRendererPtr			_createSceneNodeRenderer			()=0;
		virtual OverlayRendererPtr				_createOverlayRenderer				()=0;
	};
}

#endif
