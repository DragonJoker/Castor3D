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
#ifndef ___GL_RENDER_SYSTEM_PREREQUISITES_H___
#define ___GL_RENDER_SYSTEM_PREREQUISITES_H___

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <Windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#elif defined( __linux__ )
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#endif
#include <GL/gl.h>

#include <CastorUtils.hpp>
#include <Castor3DPrerequisites.hpp>

#ifdef _WIN32
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

#define BUFFER_OFFSET( n) ((uint8_t *)NULL + ( n))
#define C3DGL_LIMIT_TO_2_1	0
#define C3DGL_USE_TBO		0

using Castor::real;

namespace GlRender
{
	class GlAttributeBase;
	template< typename T, uint32_t Count > class GlAttribute;
	template< typename T > class GlBufferBase;
	template< typename T > class GlBuffer;
	class GlVertexBufferObject;
	class Gl3VertexBufferObject;
	class GlGeometryBuffers;
	class GlIndexArray;
	class GlVertexArray;
	class GlIndexBufferObject;
	class GlMatrixBufferObject;
	class GlTextureBufferObject;
	typedef GlAttribute< real, 4 > GlAttribute4r;
	typedef GlAttribute< real, 3 > GlAttribute3r;
	typedef GlAttribute< real, 2 > GlAttribute2r;
	typedef GlAttribute< real, 1 > GlAttribute1r;
	typedef GlAttribute< int, 4 > GlAttribute4i;
	typedef GlAttribute< int, 3 > GlAttribute3i;
	typedef GlAttribute< int, 2 > GlAttribute2i;
	typedef GlAttribute< int, 1 > GlAttribute1i;
	typedef GlAttribute< uint32_t, 1 > GlAttribute1ui;
	DECLARE_SMART_PTR( GlAttributeBase );
	DECLARE_SMART_PTR( GlIndexArray );
	DECLARE_SMART_PTR( GlVertexArray );
	DECLARE_SMART_PTR( GlIndexBufferObject );
	DECLARE_SMART_PTR( GlVertexBufferObject );
	DECLARE_SMART_PTR( GlTextureBufferObject );
	DECLARE_VECTOR(	GlAttributeBaseSPtr, GlAttributePtr );
	DECLARE_VECTOR(	GlTextureBufferObjectSPtr, GlTextureBufferObjectPtr );

	class GlShaderObject;
	class GlShaderProgram;
	template< typename Type > struct OneVariableBinder;
	template< typename Type, uint32_t Count > struct PointVariableBinder;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder;
	class GlFrameVariableBase;
	struct GlVariableApplyerBase;
	class UboVariableInfos;
	class GlFrameVariableBuffer;
	template< typename T > class GlOneFrameVariable;
	template< typename T, uint32_t Count > class GlPointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns > class GlMatrixFrameVariable;
	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_SMART_PTR( GlVariableApplyerBase );
	DECLARE_SMART_PTR( GlFrameVariableBase );
	DECLARE_SMART_PTR( GlShaderObject );
	DECLARE_SMART_PTR( GlShaderProgram );
	DECLARE_VECTOR( GlShaderProgramSPtr, GlShaderProgramPtr );
	DECLARE_VECTOR( GlShaderObjectSPtr, GlShaderObjectPtr );
	DECLARE_MAP( Castor::String, GlFrameVariableBaseSPtr, GlFrameVariablePtrStr );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, GlVariableApplyerBaseSPtr, VariableApplyer );

	class GlRenderBuffer;
	class GlColourRenderBuffer;
	class GlDepthRenderBuffer;
	class GlStencilRenderBuffer;
	class GlRenderBufferAttachment;
	class GlTextureAttachment;
	class GlFrameBuffer;
	DECLARE_SMART_PTR( GlFrameBuffer );
	DECLARE_SMART_PTR( GlRenderBuffer );
	DECLARE_SMART_PTR( GlColourRenderBuffer );
	DECLARE_SMART_PTR( GlDepthRenderBuffer );
	DECLARE_SMART_PTR( GlTextureAttachment );
	DECLARE_SMART_PTR( GlRenderBufferAttachment );
	DECLARE_VECTOR(	GlDepthRenderBufferSPtr, GlDepthRenderBufferPtr );
	DECLARE_VECTOR(	GlTextureAttachmentSPtr, GlTextureAttachmentPtr );
	DECLARE_VECTOR(	GlRenderBufferAttachmentSPtr, GlRenderBufferAttachmentPtr );

	class GlBillboardList;
	class GlBlendState;
	class GlDepthStencilState;
	class GlRasteriserState;
	class GlPipelineImplNoShader;
	class GlPipelineImplGlsl;
	class GlPipeline;
	class GlContext;
	class GlContextImpl;
	class GlRenderSystem;
	DECLARE_SMART_PTR( GlContext );

	class GlDynamicTexture;
	class GlStaticTexture;
	class GlPixelBuffer;
	class GlPackPixelBuffer;
	class GlUnpackPixelBuffer;
	DECLARE_SMART_PTR( GlDynamicTexture );
	DECLARE_SMART_PTR( GlStaticTexture );
	DECLARE_SMART_PTR( GlPixelBuffer );
	DECLARE_SMART_PTR( GlPackPixelBuffer );
	DECLARE_SMART_PTR( GlUnpackPixelBuffer );

	class GlOverlayRenderer;
	class GlRenderTarget;
	class GlRenderWindow;
	class OpenGl;

	namespace GLSL
	{
		class KeywordsBase
		{
		protected:
			Castor::String m_strStdLayout;
			Castor::String m_strVersion;
			Castor::String m_strAttribute;
			Castor::String m_strIn;
			Castor::String m_strOut;
			Castor::String m_strTexture1D;
			Castor::String m_strTexture2D;
			Castor::String m_strTexture3D;
			Castor::String m_strPixelOut;
			Castor::String m_strPixelOutputName;
			Castor::String m_strGSOutPositionName;
			Castor::String m_strGSOutNormalName;
			Castor::String m_strGSOutTangentName;
			Castor::String m_strGSOutBitangentName;
			Castor::String m_strGSOutDiffuseName;
			Castor::String m_strGSOutSpecularName;
			Castor::String m_strGSOutEmissiveName;
			Castor::String m_strGSOutPositionDecl;
			Castor::String m_strGSOutNormalDecl;
			Castor::String m_strGSOutTangentDecl;
			Castor::String m_strGSOutBitangentDecl;
			Castor::String m_strGSOutDiffuseDecl;
			Castor::String m_strGSOutSpecularDecl;
			Castor::String m_strGSOutEmissiveDecl;

		public:
			inline Castor::String GetStdLayout( int p_index )const
			{
				return m_strStdLayout + cuT( "( std" ) + Castor::str_utils::to_string( p_index ) + cuT( " ) " );
			}
			inline Castor::String const & GetVersion()const
			{
				return m_strVersion;
			}
			inline Castor::String const & GetIn()const
			{
				return m_strIn;
			}
			inline Castor::String const & GetOut()const
			{
				return m_strOut;
			}
			inline Castor::String const & GetTexture1D()const
			{
				return m_strTexture1D;
			}
			inline Castor::String const & GetTexture2D()const
			{
				return m_strTexture2D;
			}
			inline Castor::String const & GetTexture3D()const
			{
				return m_strTexture3D;
			}
			inline Castor::String const & GetPixelOut()const
			{
				return m_strPixelOut;
			}
			inline Castor::String const & GetPixelOutputName()const
			{
				return m_strPixelOutputName;
			}
			inline Castor::String const & GetGSOutPositionName()const
			{
				return m_strGSOutPositionName;
			}
			inline Castor::String const & GetGSOutNormalName()const
			{
				return m_strGSOutNormalName;
			}
			inline Castor::String const & GetGSOutTangentName()const
			{
				return m_strGSOutTangentName;
			}
			inline Castor::String const & GetGSOutBitangentName()const
			{
				return m_strGSOutBitangentName;
			}
			inline Castor::String const & GetGSOutDiffuseName()const
			{
				return m_strGSOutDiffuseName;
			}
			inline Castor::String const & GetGSOutSpecularName()const
			{
				return m_strGSOutSpecularName;
			}
			inline Castor::String const & GetGSOutEmissiveName()const
			{
				return m_strGSOutEmissiveName;
			}
			inline Castor::String const & GetGSOutPositionDecl()const
			{
				return m_strGSOutPositionDecl;
			}
			inline Castor::String const & GetGSOutNormalDecl()const
			{
				return m_strGSOutNormalDecl;
			}
			inline Castor::String const & GetGSOutTangentDecl()const
			{
				return m_strGSOutTangentDecl;
			}
			inline Castor::String const & GetGSOutBitangentDecl()const
			{
				return m_strGSOutBitangentDecl;
			}
			inline Castor::String const & GetGSOutDiffuseDecl()const
			{
				return m_strGSOutDiffuseDecl;
			}
			inline Castor::String const & GetGSOutSpecularDecl()const
			{
				return m_strGSOutSpecularDecl;
			}
			inline Castor::String const & GetGSOutEmissiveDecl()const
			{
				return m_strGSOutEmissiveDecl;
			}
			virtual Castor::String GetLayout( uint32_t p_uiIndex )const = 0;
			virtual Castor::String GetAttribute( uint32_t p_uiIndex )const
			{
				return GetLayout( p_uiIndex ) + m_strAttribute;
			}
		};

		template< int Version, class Enable = void > class Keywords;

		template< int Version > class Keywords < Version, typename std::enable_if< ( Version <= 120 ) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strAttribute = cuT( "attribute" );
				m_strIn = cuT( "varying" );
				m_strOut = cuT( "varying" );
				m_strTexture1D = cuT( "texture1D" );
				m_strTexture2D = cuT( "texture2D" );
				m_strTexture3D = cuT( "texture3D" );
				m_strPixelOutputName = cuT( "gl_FragColor" );
				m_strGSOutPositionName = cuT( "gl_FragData[0]" );
				m_strGSOutNormalName = cuT( "gl_FragData[1]" );
				m_strGSOutTangentName = cuT( "gl_FragData[2]" );
				m_strGSOutBitangentName = cuT( "gl_FragData[3]" );
				m_strGSOutDiffuseName = cuT( "gl_FragData[4]" );
				m_strGSOutSpecularName = cuT( "gl_FragData[5]" );
				m_strGSOutEmissiveName = cuT( "gl_FragData[6]" );
			}

			virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_uiIndex ) )const
			{
				return cuT( "" );
			}
		};

		template< int Version > class Keywords < Version, typename std::enable_if < ( Version > 120 ) && ( Version < 140 ) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strVersion = cuT( "#version " ) + Castor::str_utils::to_string( Version );
				m_strAttribute = cuT( "in" );
				m_strIn = cuT( "in" );
				m_strOut = cuT( "out" );
				m_strTexture1D = cuT( "texture1D" );
				m_strTexture2D = cuT( "texture2D" );
				m_strTexture3D = cuT( "texture3D" );
				m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
				m_strPixelOutputName = cuT( "pxl_v4FragColor" );
				m_strGSOutPositionName = cuT( "gl_FragData[0]" );
				m_strGSOutDiffuseName = cuT( "gl_FragData[1]" );
				m_strGSOutNormalName = cuT( "gl_FragData[2]" );
				m_strGSOutTangentName = cuT( "gl_FragData[3]" );
				m_strGSOutBitangentName = cuT( "gl_FragData[4]" );
				m_strGSOutSpecularName = cuT( "gl_FragData[5]" );
				m_strGSOutEmissiveName = cuT( "gl_FragData[6]" );
			}

			virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_uiIndex ) )const
			{
				return cuT( "" );
			}
		};

		template< int Version > class Keywords < Version, typename std::enable_if < ( Version >= 140 ) && ( Version < 330 ) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strStdLayout  = cuT( "layout" );
				m_strVersion = cuT( "#version " ) + Castor::str_utils::to_string( Version );
				m_strAttribute = cuT( "in" );
				m_strIn = cuT( "in" );
				m_strOut = cuT( "out" );
				m_strTexture1D = cuT( "texture" );
				m_strTexture2D = cuT( "texture" );
				m_strTexture3D = cuT( "texture" );
				m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
				m_strPixelOutputName = cuT( "pxl_v4FragColor" );
				m_strGSOutPositionName = cuT( "out_c3dPosition" );
				m_strGSOutNormalName = cuT( "out_c3dNormals" );
				m_strGSOutTangentName = cuT( "out_c3dTangent" );
				m_strGSOutBitangentName = cuT( "out_c3dBitangent" );
				m_strGSOutDiffuseName = cuT( "out_c3dDiffuse" );
				m_strGSOutSpecularName = cuT( "out_c3dSpecular" );
				m_strGSOutEmissiveName = cuT( "out_c3dEmissive" );
			}

			virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_uiIndex ) )const
			{
				return cuT( "" );
			}
		};

		template< int Version > class Keywords< Version, typename std::enable_if< ( Version >= 330 ) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strStdLayout  = cuT( "layout" );
				m_strVersion = cuT( "#version " ) + Castor::str_utils::to_string( Version );
				m_strAttribute = cuT( "in" );
				m_strIn = cuT( "in" );
				m_strOut = cuT( "out" );
				m_strTexture1D = cuT( "texture" );
				m_strTexture2D = cuT( "texture" );
				m_strTexture3D = cuT( "texture" );
				m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
				m_strPixelOutputName = cuT( "pxl_v4FragColor" );
				m_strGSOutPositionName = cuT( "out_c3dPosition" );
				m_strGSOutNormalName = cuT( "out_c3dNormals" );
				m_strGSOutTangentName = cuT( "out_c3dTangent" );
				m_strGSOutBitangentName = cuT( "out_c3dBitangent" );
				m_strGSOutDiffuseName = cuT( "out_c3dDiffuse" );
				m_strGSOutSpecularName = cuT( "out_c3dSpecular" );
				m_strGSOutEmissiveName = cuT( "out_c3dEmissive" );
				m_strGSOutPositionDecl = GetLayout( 0 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutPositionName + cuT( ";" );
				m_strGSOutDiffuseDecl = GetLayout( 1 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutDiffuseName + cuT( ";" );
				m_strGSOutNormalDecl = GetLayout( 2 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutNormalName + cuT( ";" );
				m_strGSOutTangentDecl = GetLayout( 3 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutTangentName + cuT( ";" );
				m_strGSOutBitangentDecl = GetLayout( 4 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutBitangentName + cuT( ";" );
				m_strGSOutSpecularDecl = GetLayout( 5 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutSpecularName + cuT( ";" );
				m_strGSOutEmissiveDecl = GetLayout( 6 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutEmissiveName + cuT( ";" );
			}

			virtual Castor::String GetLayout( uint32_t p_uiIndex )const
			{
				return cuT( "layout( location=" ) + Castor::str_utils::to_string( p_uiIndex ) + cuT( " ) " );
			}
		};

		class ConstantsBase
		{
		public:
			virtual Castor::String Matrices() = 0;
			virtual Castor::String Scene() = 0;
			virtual Castor::String Pass() = 0;
			virtual Castor::String Billboard() = 0;
		};

		class ConstantsStd : public ConstantsBase
		{
		public:
			virtual Castor::String Matrices();
			virtual Castor::String Scene();
			virtual Castor::String Pass();
			virtual Castor::String Billboard();
		};

		class ConstantsUbo : public ConstantsBase
		{
		public:
			virtual Castor::String Matrices();
			virtual Castor::String Scene();
			virtual Castor::String Pass();
			virtual Castor::String Billboard();
		};

		static ConstantsStd	constantsStd;
		static ConstantsUbo	constantsUbo;

		class VariablesBase
		{
		public:
			virtual Castor::String GetVertexOutMatrices()const = 0;
			virtual Castor::String GetVertexMatrixCopy()const = 0;
			virtual Castor::String GetPixelInMatrices()const = 0;
			virtual Castor::String GetPixelMtxModelView()const = 0;
		};

		class VariablesUbo
			: public VariablesBase
		{
		public:
			virtual Castor::String GetVertexOutMatrices()const
			{
				return cuT( "" );
			}

			virtual Castor::String GetVertexMatrixCopy()const
			{
				return cuT( "" );
			}

			virtual Castor::String GetPixelInMatrices()const
			{
				return GlRender::GLSL::constantsUbo.Matrices();
			}

			virtual Castor::String GetPixelMtxModelView()const
			{
				return cuT( "c3d_mtxModelView" );
			}
		};

		class VariablesStd
			: public VariablesBase
		{
		public:
			virtual Castor::String GetVertexOutMatrices()const
			{
				return
					cuT( "out mat4 vtx_mtxModelView;\n" )
					cuT( "out mat4 vtx_mtxView;" );
			}

			virtual Castor::String GetVertexMatrixCopy()const
			{
				return
					cuT( "	vtx_mtxModelView = c3d_mtxModelView;\n" )
					cuT( "	vtx_mtxView = c3d_mtxView;" );
			}

			virtual Castor::String GetPixelInMatrices()const
			{
				return
					cuT( "in mat4 vtx_mtxModelView;\n" )
					cuT( "in mat4 vtx_mtxView;" );
			}

			virtual Castor::String GetPixelMtxModelView()const
			{
				return cuT( "vtx_mtxModelView" );
			}
		};

		static VariablesUbo variablesUbo;
		static VariablesStd variablesStd;

		GLSL::VariablesBase * GetVariables( OpenGl const & p_gl );
		GLSL::ConstantsBase * GetConstants( OpenGl const & p_gl );
		std::unique_ptr< GLSL::KeywordsBase > GetKeywords( OpenGl const & p_gl );
	}
}

#include "OpenGl.hpp"
#endif
