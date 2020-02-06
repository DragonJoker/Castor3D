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
#include "PyCastor3DPch.hpp"

#include "PyCastor3DPrerequisites.hpp"

#include <Angle.hpp>
#include <Point.hpp>
#include <SquareMatrix.hpp>
#include <Quaternion.hpp>

#include <Animable.hpp>
#include <AnimatedObject.hpp>
#include <AnimatedObjectGroup.hpp>
#include <Engine.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <Mesh.hpp>
#include <Submesh.hpp>
#include <Light.hpp>
#include <DirectionalLight.hpp>
#include <SpotLight.hpp>
#include <PointLight.hpp>
#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <StaticTexture.hpp>
#include <DynamicTexture.hpp>
#include <ShaderProgram.hpp>
#include <Sampler.hpp>
#include <Geometry.hpp>
#include <RenderWindow.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <BorderPanelOverlay.hpp>
#include <TextOverlay.hpp>
#include <BlendState.hpp>
#include <DepthStencilState.hpp>
#include <RasteriserState.hpp>

using namespace Castor;
using namespace Castor3D;

namespace cpy
{
	struct PointAdder
	{
		void operator()( Submesh * p_submesh, Point3r const & p_point )
		{
			p_submesh->AddPoint( p_point );
		}
	};

	struct FaceAdder
	{
		void operator()( Submesh * p_submesh, uint32_t a, uint32_t b, uint32_t c )
		{
			p_submesh->AddFace( a, b, c );
		}
	};

	struct Texture3DResizer
	{
		void operator()( DynamicTexture * texture, Size const & size, uint32_t depth )
		{
			texture->Resize( Point3ui( size.width(), size.height(), depth ) );
		}
	};

	template< class Texture, typename Param >
	struct Texture3DImageSetter
	{
		void operator()( Texture * texture, Size const & size, uint32_t depth, Param param )
		{
			texture->SetImage( Point3ui( size.width(), size.height(), depth ), param );
		}
	};

	template< typename Blend >
	struct BlendSetter
	{
		typedef void ( BlendState::*Function )( Blend, uint8_t );
		Function m_function;
		uint8_t m_index;
		BlendSetter( Function function, uint8_t index )
			:	m_function( function )
			,	m_index( index )
		{
		}
		void operator()( BlendState * state, Blend blend )
		{
			( state->*( this->m_function ) )( blend, m_index );
		}
	};

	template< class Texture, typename Param >
	Texture3DImageSetter< Texture, Param >
	make_image_setter( void ( Texture::* )( Point3ui const & sizes, Param param ) )
	{
		return Texture3DImageSetter< Texture, Param >();
	}

	template< typename Blend >
	BlendSetter< Blend >
	make_blend_setter( void ( BlendState::*function )( Blend, uint8_t ), uint8_t index )
	{
		return BlendSetter< Blend >( function, index );
	}
}

namespace boost
{
	namespace python
	{
		namespace detail
		{
			inline boost::mpl::vector< PluginBaseSPtr, Engine *, Path const & >
			get_signature( PluginBaseSPtr( Engine::* )( Path const & ), void * = 0 )
			{
				return boost::mpl::vector< PluginBaseSPtr, Engine *, Path const & >();
			}
			inline boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const & >
			get_signature( MeshSPtr( Engine::* )( eMESH_TYPE, String const & ), void * = 0 )
			{
				return boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const & >();
			}
			inline boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const &, UIntArray const & >
			get_signature( MeshSPtr( Engine::* )( eMESH_TYPE, String const &, UIntArray const & ), void * = 0 )
			{
				return boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const &, UIntArray const & >();
			}
			inline boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const &, UIntArray const &, RealArray const & >
			get_signature( MeshSPtr( Engine::* )( eMESH_TYPE, String const &, UIntArray const &, RealArray const & ), void * = 0 )
			{
				return boost::mpl::vector< MeshSPtr, Engine *, eMESH_TYPE, String const &, UIntArray const &, RealArray const & >();
			}
			inline boost::mpl::vector< bool, Engine *, RenderWindowSPtr >
			get_signature( bool ( Engine::* )( RenderWindowSPtr ), void * = 0 )
			{
				return boost::mpl::vector< bool, Engine *, RenderWindowSPtr >();
			}
			inline boost::mpl::vector< OverlaySPtr, Engine *, eOVERLAY_TYPE, Castor::String const &, OverlaySPtr, SceneSPtr >
			get_signature( OverlaySPtr( Engine::* )( eOVERLAY_TYPE, Castor::String const &, OverlaySPtr, SceneSPtr ), void * = 0 )
			{
				return boost::mpl::vector< OverlaySPtr, Engine *, eOVERLAY_TYPE, Castor::String const &, OverlaySPtr, SceneSPtr >();
			}
			inline boost::mpl::vector< RenderWindowSPtr, Engine * >
			get_signature( RenderWindowSPtr( Engine::* )(), void * = 0 )
			{
				return boost::mpl::vector< RenderWindowSPtr, Engine * >();
			}
			inline boost::mpl::vector< SceneSPtr, Engine *, Castor::String const & >
			get_signature( SceneSPtr( Engine::* )( Castor::String const & ), void * = 0 )
			{
				return boost::mpl::vector< SceneSPtr, Engine *, Castor::String const & >();
			}
			inline boost::mpl::vector< void, Submesh *, Point3r const & >
			get_signature( cpy::PointAdder, void * = 0 )
			{
				return boost::mpl::vector< void, Submesh *, Point3r const & >();
			}
			inline boost::mpl::vector< void, Submesh *, uint32_t, uint32_t, uint32_t >
			get_signature( cpy::FaceAdder, void * = 0 )
			{
				return boost::mpl::vector< void, Submesh *, uint32_t, uint32_t, uint32_t >();
			}
			inline boost::mpl::vector< MaterialSPtr, Geometry *, SubmeshSPtr >
			get_signature( MaterialSPtr( Geometry::* )( SubmeshSPtr ), void * = 0 )
			{
				return boost::mpl::vector< MaterialSPtr, Geometry *, SubmeshSPtr >();
			}
			inline boost::mpl::vector< void, Geometry *, SubmeshSPtr, MaterialSPtr >
			get_signature( void ( Geometry::* )( SubmeshSPtr, MaterialSPtr ), void * = 0 )
			{
				return boost::mpl::vector< void, Geometry *, SubmeshSPtr, MaterialSPtr >();
			}
			inline boost::mpl::vector< void, DynamicTexture *, Size const &, uint32_t >
			get_signature( cpy::Texture3DResizer, void * = 0 )
			{
				return boost::mpl::vector< void, DynamicTexture *, Size const &, uint32_t >();
			}
			template< class Texture, typename Param >
			inline boost::mpl::vector< void, Texture *, Size const &, uint32_t, Param >
			get_signature( cpy::Texture3DImageSetter< Texture, Param >, void * = 0 )
			{
				return boost::mpl::vector< void, Texture *, Size const &, uint32_t, Param >();
			}
			template< typename Blend >
			inline boost::mpl::vector< void, BlendState *, Blend >
			get_signature( cpy::BlendSetter< Blend >, void * = 0 )
			{
				return boost::mpl::vector< void, BlendState *, Blend >();
			}
		}
	}
}

BOOST_PYTHON_MODULE( Castor3D )
{
	/**@group_name eRENDERER_TYPE	*/
	//@{
	py::enum_< eRENDERER_TYPE >( "RendererType" )
	.value( "OPENGL", eRENDERER_TYPE_OPENGL )
	.value( "DIRECT3D9", eRENDERER_TYPE_DIRECT3D9 )
	.value( "DIRECT3D10", eRENDERER_TYPE_DIRECT3D10 )
	.value( "DIRECT3D11", eRENDERER_TYPE_DIRECT3D11 );
	//@}
	/**@group_name eMESH_TYPE	*/
	//@{
	py::enum_< eMESH_TYPE >( "MeshType" )
	.value( "CUSTOM", eMESH_TYPE_CUSTOM )
	.value( "CONE", eMESH_TYPE_CONE )
	.value( "CYLINDER", eMESH_TYPE_CYLINDER )
	.value( "SPHERE", eMESH_TYPE_SPHERE )
	.value( "CUBE", eMESH_TYPE_CUBE )
	.value( "TORUS", eMESH_TYPE_TORUS )
	.value( "PLANE", eMESH_TYPE_PLANE )
	.value( "ICOSAHEDRON", eMESH_TYPE_ICOSAHEDRON )
	.value( "PROJECTION", eMESH_TYPE_PROJECTION );
	//@}
	/**@group_name eLIGHT_TYPE	*/
	//@{
	py::enum_< eLIGHT_TYPE >( "LightType" )
	.value( "DIRECTIONAL", eLIGHT_TYPE_DIRECTIONAL )
	.value( "POINT", eLIGHT_TYPE_POINT )
	.value( "SPOT", eLIGHT_TYPE_SPOT );
	//@}
	/**@group_name eMOVABLE_TYPE	*/
	//@{
	py::enum_< eMOVABLE_TYPE >( "MovableType" )
	.value( "CAMERA", eMOVABLE_TYPE_CAMERA )
	.value( "GEOMETRY", eMOVABLE_TYPE_GEOMETRY )
	.value( "LIGHT", eMOVABLE_TYPE_LIGHT )
	.value( "BILLBOARD", eMOVABLE_TYPE_BILLBOARD );
	//@}
	/**@group_name eOVERLAY_TYPE	*/
	//@{
	py::enum_< eOVERLAY_TYPE >( "OverlayType" )
	.value( "PANEL", eOVERLAY_TYPE_PANEL )
	.value( "BORDER_PANEL", eOVERLAY_TYPE_BORDER_PANEL )
	.value( "TEXT", eOVERLAY_TYPE_TEXT );
	//@}
	/**@group_name eVIEWPORT_TYPE	*/
	//@{
	py::enum_< eVIEWPORT_TYPE >( "ViewportType" )
	.value( "3D", eVIEWPORT_TYPE_3D )
	.value( "2D", eVIEWPORT_TYPE_2D );
	//@}
	/**@group_name eTOPOLOGY	*/
	//@{
	py::enum_< eTOPOLOGY >( "Topology" )
	.value( "POINTS", eTOPOLOGY_POINTS )
	.value( "LINES", eTOPOLOGY_LINES )
	.value( "LINE_LOOP", eTOPOLOGY_LINE_LOOP )
	.value( "LINE_STRIP", eTOPOLOGY_LINE_STRIP )
	.value( "TRIANGLES", eTOPOLOGY_TRIANGLES )
	.value( "TRIANGLE_STRIPS", eTOPOLOGY_TRIANGLE_STRIPS )
	.value( "TRIANGLE_FAN", eTOPOLOGY_TRIANGLE_FAN )
	.value( "QUADS", eTOPOLOGY_QUADS )
	.value( "QUAD_STRIPS", eTOPOLOGY_QUAD_STRIPS )
	.value( "POLYGON", eTOPOLOGY_POLYGON );
	//@}
	/**@group_name ePROJECTION_DIRECTION	*/
	//@{
	py::enum_< ePROJECTION_DIRECTION >( "ProjectionDirection" )
	.value( "FRONT", ePROJECTION_DIRECTION_FRONT )
	.value( "BACK", ePROJECTION_DIRECTION_BACK )
	.value( "LEFT", ePROJECTION_DIRECTION_LEFT )
	.value( "RIGHT", ePROJECTION_DIRECTION_RIGHT )
	.value( "TOP", ePROJECTION_DIRECTION_TOP )
	.value( "BOTTOM", ePROJECTION_DIRECTION_BOTTOM );
	//@}
	/**@group_name eTEXTURE_CHANNEL	*/
	//@{
	py::enum_< eTEXTURE_CHANNEL >( "TextureChannel" )
	.value( "COLOUR", eTEXTURE_CHANNEL_COLOUR )
	.value( "DIFFUSE", eTEXTURE_CHANNEL_DIFFUSE )
	.value( "NOTMAL", eTEXTURE_CHANNEL_NORMAL )
	.value( "OPACITY", eTEXTURE_CHANNEL_OPACITY )
	.value( "SPECULAR", eTEXTURE_CHANNEL_SPECULAR )
	.value( "HEIGHT", eTEXTURE_CHANNEL_HEIGHT )
	.value( "AMBIENT", eTEXTURE_CHANNEL_AMBIENT )
	.value( "GLOSS", eTEXTURE_CHANNEL_GLOSS )
	.value( "LGHTPASS", eTEXTURE_CHANNEL_LGHTPASS )
	.value( "ALL", eTEXTURE_CHANNEL_ALL );
	//@}
	/**@group_name eTEXTURE_MAP_MODE	*/
	//@{
	py::enum_< eTEXTURE_MAP_MODE >( "TextureMapMode" )
	.value( "NONE", eTEXTURE_MAP_MODE_NONE )
	.value( "REFLECTION", eTEXTURE_MAP_MODE_REFLECTION )
	.value( "SPHERE", eTEXTURE_MAP_MODE_SPHERE );
	//@}
	/**@group_name eRGB_BLEND_FUNC	*/
	//@{
	py::enum_< eRGB_BLEND_FUNC >( "RgbBlendFunc" )
	.value( "NONE", eRGB_BLEND_FUNC_NONE )
	.value( "FIRST_ARG", eRGB_BLEND_FUNC_FIRST_ARG )
	.value( "ADD", eRGB_BLEND_FUNC_ADD )
	.value( "ADD_SIGNED", eRGB_BLEND_FUNC_ADD_SIGNED )
	.value( "MODULATE", eRGB_BLEND_FUNC_MODULATE )
	.value( "INTERPOLATE", eRGB_BLEND_FUNC_INTERPOLATE )
	.value( "SUBTRACT", eRGB_BLEND_FUNC_SUBTRACT )
	.value( "DOT3_RGB", eRGB_BLEND_FUNC_DOT3_RGB )
	.value( "DOT3_RGBA", eRGB_BLEND_FUNC_DOT3_RGBA );
	//@}
	/**@group_name eALPHA_BLEND_FUNC	*/
	//@{
	py::enum_< eALPHA_BLEND_FUNC >( "AlphaBlendFunc" )
	.value( "NONE", eALPHA_BLEND_FUNC_NONE )
	.value( "FIRST_ARG", eALPHA_BLEND_FUNC_FIRST_ARG )
	.value( "ADD", eALPHA_BLEND_FUNC_ADD )
	.value( "ADD_SIGNED", eALPHA_BLEND_FUNC_ADD_SIGNED )
	.value( "MODULATE", eALPHA_BLEND_FUNC_MODULATE )
	.value( "INTERPOLATE", eALPHA_BLEND_FUNC_INTERPOLATE )
	.value( "SUBSTRACT", eALPHA_BLEND_FUNC_SUBSTRACT );
	//@}
	/**@group_name eBLEND_SOURCE	*/
	//@{
	py::enum_< eBLEND_SOURCE >( "BlendSource" )
	.value( "TEXTURE", eBLEND_SOURCE_TEXTURE )
	.value( "TEXTURE0", eBLEND_SOURCE_TEXTURE0 )
	.value( "TEXTURE1", eBLEND_SOURCE_TEXTURE1 )
	.value( "TEXTURE2,", eBLEND_SOURCE_TEXTURE2 )
	.value( "TEXTURE3", eBLEND_SOURCE_TEXTURE3 )
	.value( "CONSTANT", eBLEND_SOURCE_CONSTANT )
	.value( "DIFFUSE", eBLEND_SOURCE_DIFFUSE )
	.value( "PREVIOUS", eBLEND_SOURCE_PREVIOUS );
	//@}
	/**@group_name eALPHA_FUNC	*/
	//@{
	py::enum_< eALPHA_FUNC >( "AlphaFunc" )
	.value( "ALWAYS", eALPHA_FUNC_ALWAYS )
	.value( "LESS", eALPHA_FUNC_LESS )
	.value( "LESS_OR_EQUAL", eALPHA_FUNC_LESS_OR_EQUAL )
	.value( "EQUAL", eALPHA_FUNC_EQUAL )
	.value( "NOT_EQUAL", eALPHA_FUNC_NOT_EQUAL )
	.value( "GREATER_OR_EQUAL", eALPHA_FUNC_GREATER_OR_EQUAL )
	.value( "GREATER", eALPHA_FUNC_GREATER )
	.value( "NEVER", eALPHA_FUNC_NEVER );
	//@}
	/**@group_name eTEXTURE_TYPE	*/
	//@{
	py::enum_< eTEXTURE_TYPE >( "TextureType" )
	.value( "STATIC", eTEXTURE_TYPE_STATIC )
	.value( "DYNAMIC", eTEXTURE_TYPE_DYNAMIC );
	//@}
	/**@group_name eTEXTURE_DIMENSION	*/
	//@{
	py::enum_< eTEXTURE_DIMENSION >( "TextureDimension" )
	.value( "1D", eTEXTURE_DIMENSION_1D )
	.value( "2D", eTEXTURE_DIMENSION_2D )
	.value( "3D", eTEXTURE_DIMENSION_3D )
	.value( "2D_MULTISAMPLE", eTEXTURE_DIMENSION_2DMS )
	.value( "2D_ARRAY", eTEXTURE_DIMENSION_2DARRAY );
	//@}
	/**@group_name eINTERPOLATION_MODE	*/
	//@{
	py::enum_< eINTERPOLATION_MODE >( "InterpolationMode" )
	.value( "NEAREST", eINTERPOLATION_MODE_NEAREST )
	.value( "LINEAR", eINTERPOLATION_MODE_LINEAR )
	.value( "ANISOTROPIC", eINTERPOLATION_MODE_ANISOTROPIC );
	//@}
	/**@group_name eWRAP_MODE	*/
	//@{
	py::enum_< eWRAP_MODE >( "WrapMode" )
	.value( "REPEAT", eWRAP_MODE_REPEAT )
	.value( "MIRRORED_REPEAT", eWRAP_MODE_MIRRORED_REPEAT )
	.value( "CLAMP_TO_BORDER", eWRAP_MODE_CLAMP_TO_BORDER )
	.value( "CLAMP_TO_EDGE", eWRAP_MODE_CLAMP_TO_EDGE );
	//@}
	/**@group_name eBLEND_OP	*/
	//@{
	py::enum_< eBLEND_OP >( "BlendOp" )
	.value( "ADD", eBLEND_OP_ADD )
	.value( "SUBSTRACT", eBLEND_OP_SUBSTRACT )
	.value( "REV_SUBSTRACT", eBLEND_OP_REV_SUBSTRACT )
	.value( "MIN", eBLEND_OP_MIN )
	.value( "MAX", eBLEND_OP_MAX );
	//@}
	/**@group_name eBLEND	*/
	//@{
	py::enum_< eBLEND >( "Blend" )
	.value( "ZERO", eBLEND_ZERO )
	.value( "ONE", eBLEND_ONE )
	.value( "SRC_COLOUR", eBLEND_SRC_COLOUR )
	.value( "INV_SRC_COLOUR", eBLEND_INV_SRC_COLOUR )
	.value( "DST_COLOUR", eBLEND_DST_COLOUR )
	.value( "INV_DST_COLOUR", eBLEND_INV_DST_COLOUR )
	.value( "SRC_ALPHA", eBLEND_SRC_ALPHA )
	.value( "INV_SRC_ALPHA", eBLEND_INV_SRC_ALPHA )
	.value( "DST_ALPHA", eBLEND_DST_ALPHA )
	.value( "INV_DST_ALPHA", eBLEND_INV_DST_ALPHA )
	.value( "CONSTANT", eBLEND_CONSTANT )
	.value( "INV_CONSTANT", eBLEND_INV_CONSTANT )
	.value( "SRC_ALPHA_SATURATE", eBLEND_SRC_ALPHA_SATURATE )
	.value( "SRC1_COLOUR", eBLEND_SRC1_COLOUR )
	.value( "INV_SRC1_COLOUR", eBLEND_INV_SRC1_COLOUR )
	.value( "SRC1_ALPHA", eBLEND_SRC1_ALPHA )
	.value( "INV_SRC1_ALPHA", eBLEND_INV_SRC1_ALPHA );
	//@}
	/**@group_name eDEPTH_FUNC	*/
	//@{
	py::enum_< eDEPTH_FUNC >( "DepthFunc" )
	.value( "NEVER", eDEPTH_FUNC_NEVER )
	.value( "LESS", eDEPTH_FUNC_LESS )
	.value( "EQUAL", eDEPTH_FUNC_EQUAL )
	.value( "LEQUAL", eDEPTH_FUNC_LEQUAL )
	.value( "GREATER", eDEPTH_FUNC_GREATER )
	.value( "NOTEQUAL", eDEPTH_FUNC_NOTEQUAL )
	.value( "GEQUAL", eDEPTH_FUNC_GEQUAL )
	.value( "ALWAYS", eDEPTH_FUNC_ALWAYS );
	//@}
	/**@group_name eDEPTH_MASK	*/
	//@{
	py::enum_< eWRITING_MASK >( "WritingMask" )
	.value( "ZERO", eWRITING_MASK_ZERO )
	.value( "ALL", eWRITING_MASK_ALL );
	//@}
	/**@group_name eSTENCIL_FUNC	*/
	//@{
	py::enum_< eSTENCIL_FUNC >( "StencilFunc" )
	.value( "NEVER", eSTENCIL_FUNC_NEVER )
	.value( "LESS", eSTENCIL_FUNC_LESS )
	.value( "EQUAL", eSTENCIL_FUNC_EQUAL )
	.value( "LEQUAL", eSTENCIL_FUNC_LEQUAL )
	.value( "GREATER", eSTENCIL_FUNC_GREATER )
	.value( "NOTEQUAL", eSTENCIL_FUNC_NOTEQUAL )
	.value( "GEQUAL", eSTENCIL_FUNC_GEQUAL )
	.value( "ALWAYS", eSTENCIL_FUNC_ALWAYS );
	//@}
	/**@group_name eSTENCIL_OP	*/
	//@{
	py::enum_< eSTENCIL_OP >( "StencilOp" )
	.value( "KEEP", eSTENCIL_OP_KEEP )
	.value( "ZERO", eSTENCIL_OP_ZERO )
	.value( "REPLACE", eSTENCIL_OP_REPLACE )
	.value( "INCR", eSTENCIL_OP_INCR )
	.value( "INCR_WRAP", eSTENCIL_OP_INCR_WRAP )
	.value( "DECR", eSTENCIL_OP_DECR )
	.value( "DECR_WRAP", eSTENCIL_OP_DECR_WRAP )
	.value( "INVERT", eSTENCIL_OP_INVERT );
	//@}
	/**@group_name eFILL_MODE	*/
	//@{
	py::enum_< eFILL_MODE >( "FillMode" )
	.value( "POINT", eFILL_MODE_POINT )
	.value( "LINE", eFILL_MODE_LINE )
	.value( "SOLID", eFILL_MODE_SOLID );
	//@}
	/**@group_name eFACE	*/
	//@{
	py::enum_< Castor3D::eFACE >( "Face" )
	.value( "NONE", eFACE_NONE )
	.value( "FRONT", eFACE_FRONT )
	.value( "BACK", eFACE_BACK )
	.value( "FRONT_AND_BACK", eFACE_FRONT_AND_BACK );
	//@}
	/**@group_name eSHADER_TYPE	*/
	//@{
	py::enum_< Castor3D::eSHADER_TYPE >( "ShaderType" )
	.value( "NONE", eSHADER_TYPE_NONE )
	.value( "VERTEX", eSHADER_TYPE_VERTEX )
	.value( "HULL", eSHADER_TYPE_HULL )
	.value( "DOMAIN", eSHADER_TYPE_DOMAIN )
	.value( "GEOMETRY", eSHADER_TYPE_GEOMETRY )
	.value( "PIXEL", eSHADER_TYPE_PIXEL )
	.value( "COMPUTE", eSHADER_TYPE_COMPUTE );
	//@}
	/**@group_name eSHADER_MODEL	*/
	//@{
	py::enum_< Castor3D::eSHADER_MODEL >( "ShaderModel" )
	.value( "1", eSHADER_MODEL_1 )
	.value( "2", eSHADER_MODEL_2 )
	.value( "3", eSHADER_MODEL_3 )
	.value( "4", eSHADER_MODEL_4 )
	.value( "5", eSHADER_MODEL_5 );
	//@}
	/**@group_name BlendState	*/
	//@{
	py::class_< BlendState, boost::noncopyable >( "BlendState", py::no_init )
	.add_property( "alpha_to_coverage", &BlendState::IsAlphaToCoverageEnabled, &BlendState::EnableAlphaToCoverage, "The alpha to coverage enabled status" )
	.add_property( "independant_blend", &BlendState::IsIndependantBlendEnabled, &BlendState::EnableIndependantBlend, "The independant blend enabled status" )
	.add_property( "blend_factors", py::make_function( &BlendState::GetBlendFactors, py::return_value_policy< py::copy_const_reference >() ), &BlendState::SetBlendFactors, "The blend factors" )
	.add_property( "sample_coverage_mask", py::make_function( &BlendState::GetSampleCoverageMask ), &BlendState::SetSampleCoverageMask, "The sample coverage mask" )
	.add_property( "blend_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 0 ), cpy::make_blend_setter( &BlendState::EnableBlend, 0 ), "The blend enabled status" )
	.add_property( "rgb_src_blend", cpy::make_getter( &BlendState::GetRgbSrcBlend, 0 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 0 ), "The source source RGB blend value" )
	.add_property( "rgb_dst_blend", cpy::make_getter( &BlendState::GetRgbDstBlend, 0 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 0 ), "The source destination RGB blend value" )
	.add_property( "rgb_blend_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 0 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 0 ), "The RGB blend operator" )
	.add_property( "alpha_src_blend", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 0 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 0 ), "The source source Alpha blend value" )
	.add_property( "alpha_dst_blend", cpy::make_getter( &BlendState::GetAlphaDstBlend, 0 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 0 ), "The source destination Alpha blend value" )
	.add_property( "alpha_blend_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 0 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 0 ), "The Alpha blend operator" )
	.add_property( "write_mask", cpy::make_getter( &BlendState::GetWriteMask, 0 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 0 ), "The render target write mask" )
	.add_property( "blend_2_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 1 ), cpy::make_blend_setter( &BlendState::EnableBlend, 1 ), "The blend enabled status, for 2nd unit" )
	.add_property( "rgb_src_blend_2", cpy::make_getter( &BlendState::GetRgbSrcBlend, 1 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 1 ), "The source source RGB blend value, for 2nd unit" )
	.add_property( "rgb_dst_blend_2", cpy::make_getter( &BlendState::GetRgbDstBlend, 1 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 1 ), "The source destination RGB blend value, for 2nd unit" )
	.add_property( "rgb_blend_2_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 1 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 1 ), "The RGB blend operato, for 2nd unitr" )
	.add_property( "alpha_src_blend_2", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 1 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 1 ), "The source source Alpha blend value, for 2nd unit" )
	.add_property( "alpha_dst_blend_2", cpy::make_getter( &BlendState::GetAlphaDstBlend, 1 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 1 ), "The source destination Alpha blend value, for 2nd unit" )
	.add_property( "alpha_blend_2_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 1 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 1 ), "The Alpha blend operator, for 2nd unit" )
	.add_property( "write_mask_2", cpy::make_getter( &BlendState::GetWriteMask, 1 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 1 ), "The render target write mask, for 2nd unit" )
	.add_property( "blend_3_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 2 ), cpy::make_blend_setter( &BlendState::EnableBlend, 2 ), "The blend enabled status, for 3rd unit" )
	.add_property( "rgb_src_blend_3", cpy::make_getter( &BlendState::GetRgbSrcBlend, 2 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 2 ), "The source source RGB blend value, for 3rd unit" )
	.add_property( "rgb_dst_blend_3", cpy::make_getter( &BlendState::GetRgbDstBlend, 2 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 2 ), "The source destination RGB blend value, for 3rd unit" )
	.add_property( "rgb_blend_3_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 2 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 2 ), "The RGB blend operator, for 3rd unit" )
	.add_property( "alpha_src_blend_3", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 2 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 2 ), "The source source Alpha blend value, for 3rd unit" )
	.add_property( "alpha_dst_blend_3", cpy::make_getter( &BlendState::GetAlphaDstBlend, 2 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 2 ), "The source destination Alpha blend value, for 3rd unit" )
	.add_property( "alpha_blend_3_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 2 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 2 ), "The Alpha blend operator, for 3rd unit" )
	.add_property( "write_mask_3", cpy::make_getter( &BlendState::GetWriteMask, 2 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 2 ), "The render target write mask, for 3rd unit" )
	.add_property( "blend_4_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 3 ), cpy::make_blend_setter( &BlendState::EnableBlend, 3 ), "The blend enabled status, for 4th unit" )
	.add_property( "rgb_src_blend_4", cpy::make_getter( &BlendState::GetRgbSrcBlend, 3 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 3 ), "The source source RGB blend value, for 4th unit" )
	.add_property( "rgb_dst_blend_4", cpy::make_getter( &BlendState::GetRgbDstBlend, 3 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 3 ), "The source destination RGB blend value, for 4th unit" )
	.add_property( "rgb_blend_4_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 3 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 3 ), "The RGB blend operator, for 4th unit" )
	.add_property( "alpha_src_blend_4", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 3 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 3 ), "The source source Alpha blend value, for 4th unit" )
	.add_property( "alpha_dst_blend_4", cpy::make_getter( &BlendState::GetAlphaDstBlend, 3 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 3 ), "The source destination Alpha blend value, for 4th unit" )
	.add_property( "alpha_blend_4_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 3 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 3 ), "The Alpha blend operator, for 4th unit" )
	.add_property( "write_mask_4", cpy::make_getter( &BlendState::GetWriteMask, 3 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 3 ), "The render target write mask, for 4th unit" )
	.add_property( "blend_5_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 4 ), cpy::make_blend_setter( &BlendState::EnableBlend, 4 ), "The blend enabled status, for 5th unit" )
	.add_property( "rgb_src_blend_5", cpy::make_getter( &BlendState::GetRgbSrcBlend, 4 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 4 ), "The source source RGB blend value, for 5th unit" )
	.add_property( "rgb_dst_blend_5", cpy::make_getter( &BlendState::GetRgbDstBlend, 4 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 4 ), "The source destination RGB blend value, for 5th unit" )
	.add_property( "rgb_blend_5_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 4 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 4 ), "The RGB blend operator, for 5th unit" )
	.add_property( "alpha_src_blend_5", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 4 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 4 ), "The source source Alpha blend value, for 5th unit" )
	.add_property( "alpha_dst_blend_5", cpy::make_getter( &BlendState::GetAlphaDstBlend, 4 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 4 ), "The source destination Alpha blend value, for 5th unit" )
	.add_property( "alpha_blend_5_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 4 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 4 ), "The Alpha blend operator, for 5th unit" )
	.add_property( "write_mask_5", cpy::make_getter( &BlendState::GetWriteMask, 4 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 4 ), "The render target write mask, for 5th unit" )
	.add_property( "blend_6_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 5 ), cpy::make_blend_setter( &BlendState::EnableBlend, 5 ), "The blend enabled status, for 6th unit" )
	.add_property( "rgb_src_blend_6", cpy::make_getter( &BlendState::GetRgbSrcBlend, 5 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 5 ), "The source source RGB blend value, for 6th unit" )
	.add_property( "rgb_dst_blend_6", cpy::make_getter( &BlendState::GetRgbDstBlend, 5 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 5 ), "The source destination RGB blend value, for 6th unit" )
	.add_property( "rgb_blend_6_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 5 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 5 ), "The RGB blend operator, for 6th unit" )
	.add_property( "alpha_src_blend_6", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 5 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 5 ), "The source source Alpha blend value, for 6th unit" )
	.add_property( "alpha_dst_blend_6", cpy::make_getter( &BlendState::GetAlphaDstBlend, 5 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 5 ), "The source destination Alpha blend value, for 6th unit" )
	.add_property( "alpha_blend_6_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 5 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 5 ), "The Alpha blend operator, for 6th unit" )
	.add_property( "write_mask_6", cpy::make_getter( &BlendState::GetWriteMask, 5 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 5 ), "The render target write mask, for 6th unit" )
	.add_property( "blend_7_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 6 ), cpy::make_blend_setter( &BlendState::EnableBlend, 6 ), "The blend enabled status, for 7th unit" )
	.add_property( "rgb_src_blend_7", cpy::make_getter( &BlendState::GetRgbSrcBlend, 6 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 6 ), "The source source RGB blend value, for 7th unit" )
	.add_property( "rgb_dst_blend_7", cpy::make_getter( &BlendState::GetRgbDstBlend, 6 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 6 ), "The source destination RGB blend value, for 7th unit" )
	.add_property( "rgb_blend_7_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 6 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 6 ), "The RGB blend operator, for 7th unit" )
	.add_property( "alpha_src_blend_7", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 6 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 6 ), "The source source Alpha blend value, for 7th unit" )
	.add_property( "alpha_dst_blend_7", cpy::make_getter( &BlendState::GetAlphaDstBlend, 6 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 6 ), "The source destination Alpha blend value, for 7th unit" )
	.add_property( "alpha_blend_7_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 6 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 6 ), "The Alpha blend operator, for 7th unit" )
	.add_property( "write_mask_7", cpy::make_getter( &BlendState::GetWriteMask, 6 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 6 ), "The render target write mask, for 7th unit" )
	.add_property( "blend_8_enabled", cpy::make_getter( &BlendState::IsBlendEnabled, 7 ), cpy::make_blend_setter( &BlendState::EnableBlend, 7 ), "The blend enabled status, for 8th unit" )
	.add_property( "rgb_src_blend_8", cpy::make_getter( &BlendState::GetRgbSrcBlend, 7 ), cpy::make_blend_setter( &BlendState::SetRgbSrcBlend, 7 ), "The source source RGB blend value, for 8th unit" )
	.add_property( "rgb_dst_blend_8", cpy::make_getter( &BlendState::GetRgbDstBlend, 7 ), cpy::make_blend_setter( &BlendState::SetRgbDstBlend, 7 ), "The source destination RGB blend valu, for 8th unite" )
	.add_property( "rgb_blend_8_op", cpy::make_getter( &BlendState::GetRgbBlendOp, 7 ), cpy::make_blend_setter( &BlendState::SetRgbBlendOp, 7 ), "The RGB blend operator, for 8th unit" )
	.add_property( "alpha_src_blend_8", cpy::make_getter( &BlendState::GetAlphaSrcBlend, 7 ), cpy::make_blend_setter( &BlendState::SetAlphaSrcBlend, 7 ), "The source source Alpha blend value, for 8th unit" )
	.add_property( "alpha_dst_blend_8", cpy::make_getter( &BlendState::GetAlphaDstBlend, 7 ), cpy::make_blend_setter( &BlendState::SetAlphaDstBlend, 7 ), "The source destination Alpha blend value, for 8th unit" )
	.add_property( "alpha_blend_8_op", cpy::make_getter( &BlendState::GetAlphaBlendOp, 7 ), cpy::make_blend_setter( &BlendState::SetAlphaBlendOp, 7 ), "The Alpha blend operator, for 8th unit" )
	.add_property( "write_mask_8", cpy::make_getter( &BlendState::GetWriteMask, 7 ), cpy::make_blend_setter( &BlendState::SetWriteMask, 7 ), "The render target write mask, for 8th unit" )
	.def( "initialise", &BlendState::Initialise )
	.def( "cleanup", &BlendState::Cleanup );
	//@}
	/**@group_name DepthStencilState	*/
	//@{
	py::class_< DepthStencilState, boost::noncopyable >( "DepthStencilState", py::no_init )
	.add_property( "depth_test", &DepthStencilState::GetDepthTest, &DepthStencilState::SetDepthTest, "The depth test enabled status" )
	.add_property( "depth_func", &DepthStencilState::GetDepthFunc, &DepthStencilState::SetDepthFunc, "The depth test function" )
	.add_property( "depth_mask", &DepthStencilState::GetDepthMask, &DepthStencilState::SetDepthMask, "The depth test mask" )
	.add_property( "depth_near", &DepthStencilState::GetDepthNear, "The depth near distance" )
	.add_property( "depth_far", &DepthStencilState::GetDepthFar, "The depth far distance" )
	.add_property( "stencil_test", &DepthStencilState::GetStencilTest, &DepthStencilState::SetStencilTest, "The stencil test enabled status" )
	.add_property( "stencil_read_mask", &DepthStencilState::GetStencilReadMask, &DepthStencilState::SetStencilReadMask, "The stencil test read mask" )
	.add_property( "stencil_write_mask", &DepthStencilState::GetStencilWriteMask, &DepthStencilState::SetStencilWriteMask, "The stencil test write mask" )
	.add_property( "stencil_front_func", &DepthStencilState::GetStencilFrontFunc, &DepthStencilState::SetStencilFrontFunc, "The stencil test function, for the front faces" )
	.add_property( "stencil_front_fail_op", &DepthStencilState::GetStencilFrontFailOp, &DepthStencilState::SetStencilFrontFailOp, "The stencil test failed operation, for the front faces" )
	.add_property( "stencil_front_depth fail_op", &DepthStencilState::GetStencilFrontDepthFailOp, &DepthStencilState::SetStencilFrontDepthFailOp, "The stencil test, depth test failed operation, for the front faces" )
	.add_property( "stencil_front_pass_op", &DepthStencilState::GetStencilFrontPassOp, &DepthStencilState::SetStencilFrontPassOp, "The stencil test passed operation, for the front faces" )
	.add_property( "stencil_back_func", &DepthStencilState::GetStencilBackFunc, &DepthStencilState::SetStencilBackFunc, "The stencil test function, for the back faces" )
	.add_property( "stencil_back_fail_op", &DepthStencilState::GetStencilBackFailOp, &DepthStencilState::SetStencilBackFailOp, "The stencil test failed operation, for the back faces" )
	.add_property( "stencil_back_depth fail_op", &DepthStencilState::GetStencilBackDepthFailOp, &DepthStencilState::SetStencilBackDepthFailOp, "The stencil test, depth test failed operation, for the back faces" )
	.add_property( "stencil_back_pass_op", &DepthStencilState::GetStencilBackPassOp, &DepthStencilState::SetStencilBackPassOp, "The stencil test passed operation, for the back faces" )
	.def( "set_depth_range", &DepthStencilState::SetDepthRange )
	.def( "initialise", &DepthStencilState::Initialise )
	.def( "cleanup", &DepthStencilState::Cleanup );
	//@}
	/**@group_name RasteriserState	*/
	//@{
	py::class_< RasteriserState, boost::noncopyable >( "RasteriserState", py::no_init )
	.add_property( "fill_mode", &RasteriserState::GetFillMode, &RasteriserState::SetFillMode, "The fill mode" )
	.add_property( "cull_faces", &RasteriserState::GetCulledFaces, &RasteriserState::SetCulledFaces, "The faces culled by rasteriser" )
	.add_property( "ccw", &RasteriserState::GetFrontCCW, &RasteriserState::SetFrontCCW, "The faces which are considered front facing (true => CCW, false => CW)" )
	.add_property( "antialiased_lines", &RasteriserState::GetAntialiasedLines, &RasteriserState::SetAntialiasedLines, "The antialiased lines status" )
	.add_property( "depth_bias", &RasteriserState::GetDepthBias, &RasteriserState::SetDepthBias, "The depth bias" )
	.add_property( "depth_clipping", &RasteriserState::GetDepthClipping, &RasteriserState::SetDepthClipping, "The depth clipping enabled status" )
	.add_property( "multisample", &RasteriserState::GetMultisample, &RasteriserState::SetMultisample, "The multisample enabled status" )
	.add_property( "scissor_test", &RasteriserState::GetScissor, &RasteriserState::SetScissor, "The scissor test enabled status" )
	.def( "initialise", &RasteriserState::Initialise )
	.def( "cleanup", &RasteriserState::Cleanup );
	//@}
	/**@group_name Sampler	*/
	//@{
	py::class_< Sampler, boost::noncopyable >( "Sampler", py::no_init )
	.add_property( "name", py::make_function( &Sampler::GetName, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Sampler::SetName ), "The sampler name" )
	.add_property( "min_filter", cpy::make_getter( &Sampler::GetInterpolationMode, eINTERPOLATION_FILTER_MIN ), cpy::make_setter( &Sampler::SetInterpolationMode, eINTERPOLATION_FILTER_MIN ), "The sampler min filter" )
	.add_property( "mag_filter", cpy::make_getter( &Sampler::GetInterpolationMode, eINTERPOLATION_FILTER_MAG ), cpy::make_setter( &Sampler::SetInterpolationMode, eINTERPOLATION_FILTER_MAG ), "The sampler mag filter" )
	.add_property( "mip_filter", cpy::make_getter( &Sampler::GetInterpolationMode, eINTERPOLATION_FILTER_MIP ), cpy::make_setter( &Sampler::SetInterpolationMode, eINTERPOLATION_FILTER_MIP ), "The sampler mip filter" )
	.add_property( "u_wrap", cpy::make_getter( &Sampler::GetWrappingMode, eTEXTURE_UVW_U ), cpy::make_setter( &Sampler::SetWrappingMode, eTEXTURE_UVW_U ), "The sampler U wrap mode" )
	.add_property( "v_wrap", cpy::make_getter( &Sampler::GetWrappingMode, eTEXTURE_UVW_V ), cpy::make_setter( &Sampler::SetWrappingMode, eTEXTURE_UVW_V ), "The sampler V wrap mode" )
	.add_property( "w_wrap", cpy::make_getter( &Sampler::GetWrappingMode, eTEXTURE_UVW_W ), cpy::make_setter( &Sampler::SetWrappingMode, eTEXTURE_UVW_W ), "The sampler W wrap mode" )
	.add_property( "max_anisotropy", &Sampler::GetMaxAnisotropy, &Sampler::SetMaxAnisotropy, "The sampler max anisotropy" )
	.add_property( "min_lod", &Sampler::GetMinLod, &Sampler::SetMinLod, "The sampler min LOD" )
	.add_property( "max_lod", &Sampler::GetMaxLod, &Sampler::SetMaxLod, "The sampler max LOD" )
	.add_property( "lod_bias", &Sampler::GetLodBias, &Sampler::SetLodBias, "The sampler LOD bias" )
	.add_property( "border_colour", py::make_function( &Sampler::GetBorderColour, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Sampler::SetBorderColour ), "The sampler border colour" )
	.def( "initialise", &Sampler::Initialise )
	.def( "cleanup", &Sampler::Cleanup );
	//@}
	/**@group_name TextureBase	*/
	//@{
	py::class_< TextureBase, boost::noncopyable >( "Texture", py::no_init )
	.add_property( "type", &TextureBase::GetType, "The texture type" )
	.add_property( "dimension", &TextureBase::GetDimension, &TextureBase::SetDimension, "The texture dimension" )
	.add_property( "sampler", &TextureBase::GetSampler, &TextureBase::SetSampler, "The texture sampler" )
	.add_property( "mapping_mode", &TextureBase::GetMappingMode, &TextureBase::SetMappingMode, "The texture mapping mode" )
	.add_property( "image", &TextureBase::GetBuffer, &TextureBase::SetImage, "The texture image buffer" )
	.def( "initialise", &TextureBase::Initialise )
	.def( "cleanup", &TextureBase::Cleanup );
	//@}
	/**@group_name StaticTexture	*/
	//@{
	void ( StaticTexture::*staticTexture2DImageSetter )( PxBufferBaseSPtr ) = &StaticTexture::SetImage;
	py::class_< StaticTexture, py::bases< TextureBase >, boost::noncopyable >( "StaticTexture", py::no_init )
	.def( "set_image", staticTexture2DImageSetter, "Sets the texture 2D image" )
	.def( "set_image", cpy::make_image_setter( &StaticTexture::SetImage ), "Sets the texture 3D image" );
	//@}
	/**@group_name DynamicTexture	*/
	//@{
	void ( DynamicTexture::*texture2DImageSetter )( Size const &, ePIXEL_FORMAT ) = &DynamicTexture::SetImage;
	void ( DynamicTexture::*resizer2d )( Size const & ) = &DynamicTexture::Resize;
	py::class_< DynamicTexture, py::bases< TextureBase >, boost::noncopyable >( "DynamicTexture", py::no_init )
	.add_property( "render_target", &DynamicTexture::IsRenderTarget, &DynamicTexture::SetRenderTarget, "The render target status" )
	.add_property( "samples_count", &DynamicTexture::GetSamplesCount, &DynamicTexture::SetSamplesCount, "The samples count" )
	.def( "set_image", texture2DImageSetter, "Sets the texture 2D image" )
	.def( "set_image", cpy::make_image_setter( &DynamicTexture::SetImage ), "Sets the texture 3D image" )
	.def( "resize", resizer2d, "Resizes the texture image" )
	.def( "resize", cpy::Texture3DResizer(), "Resizes the texture image" );
	//@}
	/**@group_name TextureUnit	*/
	//@{
	py::class_< TextureUnit, boost::noncopyable >( "TextureUnit", py::no_init )
	.add_property( "texture", &TextureUnit::GetTexture, &TextureUnit::SetTexture, "The unit texture" )
	.add_property( "alpha_test_func", &TextureUnit::GetAlphaFunc, &TextureUnit::SetAlphaFunc, "The Alpha Test function" )
	.add_property( "alpha_test_value", &TextureUnit::GetAlphaValue, &TextureUnit::SetAlphaValue, "The Alpha Test reference value" )
	.add_property( "alpha_blend_func", &TextureUnit::GetAlpFunction, &TextureUnit::SetAlpFunction, "The Alpha Blending function" )
	.add_property( "alpha_blend_arg_0", cpy::make_getter( &TextureUnit::GetAlpArgument, eBLEND_SRC_INDEX_0 ), cpy::make_setter( &TextureUnit::SetAlpArgument, eBLEND_SRC_INDEX_0 ), "The Alpha Blending argument for index 0" )
	.add_property( "alpha_blend_arg_1", cpy::make_getter( &TextureUnit::GetAlpArgument, eBLEND_SRC_INDEX_1 ), cpy::make_setter( &TextureUnit::SetAlpArgument, eBLEND_SRC_INDEX_1 ), "The Alpha Blending argument for index 1" )
	.add_property( "alpha_blend_arg_2", cpy::make_getter( &TextureUnit::GetAlpArgument, eBLEND_SRC_INDEX_2 ), cpy::make_setter( &TextureUnit::SetAlpArgument, eBLEND_SRC_INDEX_2 ), "The Alpha Blending argument for index 2" )
	.add_property( "rgb_blend_func", &TextureUnit::GetRgbFunction, &TextureUnit::SetRgbFunction, "The RGB Blending function" )
	.add_property( "rgb_blend_arg_0", cpy::make_getter( &TextureUnit::GetRgbArgument, eBLEND_SRC_INDEX_0 ), cpy::make_setter( &TextureUnit::SetRgbArgument, eBLEND_SRC_INDEX_0 ), "The RGB Blending argument for index 0" )
	.add_property( "rgb_blend_arg_1", cpy::make_getter( &TextureUnit::GetRgbArgument, eBLEND_SRC_INDEX_1 ), cpy::make_setter( &TextureUnit::SetRgbArgument, eBLEND_SRC_INDEX_1 ), "The RGB Blending argument for index 1" )
	.add_property( "rgb_blend_arg_2", cpy::make_getter( &TextureUnit::GetRgbArgument, eBLEND_SRC_INDEX_2 ), cpy::make_setter( &TextureUnit::SetRgbArgument, eBLEND_SRC_INDEX_2 ), "The RGB Blending argument for index 2" )
	.add_property( "channel", &TextureUnit::GetChannel, &TextureUnit::SetChannel, "The texture channel" )
	.add_property( "blend_colour", py::make_function( &TextureUnit::GetBlendColour, py::return_value_policy< py::copy_const_reference >() ), &TextureUnit::SetBlendColour, "The texture blend colour" )
	.def( "load_texture", &TextureUnit::LoadTexture );
	//@}
	/**@group_name ShaderProgram	*/
	//@{
	void( ShaderProgramBase::*ShaderProgramFileSetter )( eSHADER_TYPE, eSHADER_MODEL, Path const & ) = &ShaderProgramBase::SetFile;
	py::class_< ShaderProgramBase, boost::noncopyable >( "ShaderProgram", py::no_init )
	.def( "get_file", &ShaderProgramBase::GetFile )
    .def( "set_file", ShaderProgramFileSetter )
	.def( "get_source", &ShaderProgramBase::GetSource )
	.def( "set_source", &ShaderProgramBase::SetSource )
	.def( "get_entry_point", &ShaderProgramBase::GetEntryPoint )
	.def( "set_entry_point", &ShaderProgramBase::SetEntryPoint )
	.def( "initialise", &ShaderProgramBase::Initialise )
	.def( "cleanup", &ShaderProgramBase::Cleanup )
	.def( "create_object", &ShaderProgramBase::CreateObject );
	//@}
	/**@group_name Pass	*/
	//@{
	TextureUnitSPtr( Pass::*passChannelTextureUnitGetter )( eTEXTURE_CHANNEL ) = &Pass::GetTextureUnit;
	typedef TextureUnitPtrArrayIt( Pass::*TextureUnitPtrArrayItFunc )();
	py::class_< Pass, boost::noncopyable >( "Pass", py::no_init )
	.add_property( "ambient", cpy::make_getter( &Pass::GetAmbient, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Pass::SetAmbient ), "The pass ambient colour" )
	.add_property( "diffuse", cpy::make_getter( &Pass::GetDiffuse, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Pass::SetDiffuse ), "The pass diffuse colour" )
	.add_property( "specular", cpy::make_getter( &Pass::GetSpecular, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Pass::SetSpecular ), "The pass specular colour" )
	.add_property( "emissive", cpy::make_getter( &Pass::GetEmissive, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Pass::SetEmissive ), "The pass emissive colour" )
	.add_property( "shininess", &Pass::GetShininess, &Pass::SetShininess, "The pass shininess value" )
	.add_property( "two_sided", &Pass::IsTwoSided, &Pass::SetTwoSided, "The pass two sided status" )
	.add_property( "alpha", &Pass::GetAlpha, &Pass::SetAlpha, "The pass global alpha value" )
	.add_property( "shader", &Pass::GetShader< ShaderProgramBase >, &Pass::SetShader, "The pass shader" )
	.def( "create_texture_unit", &Pass::AddTextureUnit )
	.def( "destroy_pass", &Pass::DestroyTextureUnit )
	.def( "get_texture_unit_at_channel", passChannelTextureUnitGetter )
	.def( "units", py::range< TextureUnitPtrArrayItFunc, TextureUnitPtrArrayItFunc >( &Pass::Begin, &Pass::End ) );
	//@}
	/**@group_name Material	*/
	//@{
	typedef PassPtrArrayIt( Material::*PassPtrArrayItFunc )();
	py::class_< Material >( "Material", py::init< Engine *, String const & >() )
	.add_property( "pass_count", &Material::GetPassCount, "The material's passes count" )
	.def( "initialise", &Material::Initialise )
	.def( "cleanup", &Material::Cleanup )
	.def( "create_pass", &Material::CreatePass )
	.def( "destroy_pass", &Material::DestroyPass )
	.def( "passes", py::range< PassPtrArrayItFunc, PassPtrArrayItFunc >( &Material::Begin, &Material::End ) );
	//@}
	/**@group_name Submesh	*/
	//@{
	py::class_< Submesh, boost::noncopyable >( "Submesh", py::no_init )
	.add_property( "faces_count", &Submesh::GetFaceCount, "The total submesh's faces count" )
	.add_property( "vertex_count", &Submesh::GetPointsCount, "The total submesh's vertices count" )
	.def( "add_point", cpy::PointAdder() )
	.def( "add_face", cpy::FaceAdder() );
	//@}
	/**@group_name Mesh	*/
	//@{
	typedef SubmeshPtrArrayIt( Mesh::*SubmeshPtrArrayItFunc )();
	py::class_< Mesh, boost::noncopyable >( "Mesh", py::no_init )
	.add_property( "type", &Mesh::GetMeshType, "The mesh type" )
	.add_property( "submesh_count", &Mesh::GetSubmeshCount, "The total mesh's submeshes count" )
	.add_property( "faces_count", &Mesh::GetFaceCount, "The total mesh's faces count" )
	.add_property( "vertex_count", &Mesh::GetVertexCount, "The total mesh's vertices count" )
	.add_property( "submeshes", py::range< SubmeshPtrArrayItFunc, SubmeshPtrArrayItFunc >( &Mesh::Begin, &Mesh::End ) )
	.def( "create_submesh", &Mesh::CreateSubmesh )
	.def( "delete_submesh", &Mesh::DeleteSubmesh );
	//@}
	/**@group_name Scene	*/
	//@{
	SceneNodeSPtr( Scene::*SceneNodeCreator )( Castor::String const &, SceneNodeSPtr ) = &Scene::CreateSceneNode;
	GeometrySPtr( Scene::*SceneGeometryCreator )( Castor::String const & ) = &Scene::CreateGeometry;
	CameraSPtr( Scene::*SceneCameraCreator )( Castor::String const &, int, int, SceneNodeSPtr, eVIEWPORT_TYPE ) = &Scene::CreateCamera;
	py::class_< Scene, boost::noncopyable >( "Scene", py::no_init )
	.add_property( "background_colour", py::make_function( &Scene::GetBackgroundColour, py::return_value_policy< py::copy_const_reference >() ), &Scene::SetBackgroundColour, "The background colour" )
    .add_property( "name", py::make_function( &Scene::GetName, py::return_value_policy< py::copy_const_reference >() ), &Scene::SetName, "The scene name" )
	.add_property( "ambient_light", py::make_function( &Scene::GetAmbientLight, py::return_value_policy< py::copy_const_reference >() ), &Scene::SetAmbientLight, "The ambient light colour" )
    .add_property( "nodes_count", &Scene::GetNodesCount, "The nodes count" )
	.add_property( "lights_count", &Scene::GetLightsCount, "The lights count" )
	.add_property( "geometries_count", &Scene::GetGeometriesCount, "The geometries count" )
	.add_property( "cameras_count", &Scene::GetCamerasCount, "The cameras count" )
    .add_property( "root_node", &Scene::GetRootNode, "The root scene node" )
	.add_property( "root_object_node", &Scene::GetObjectRootNode, "The objects root scene node" )
    .add_property( "root_camera_node", &Scene::GetCameraRootNode, "The cameras root scene node" )
	.def( "clear_scene", &Scene::ClearScene )
	.def( "create_node", SceneNodeCreator )
	.def( "create_geometry", SceneGeometryCreator )
	.def( "create_camera", SceneCameraCreator )
	.def( "create_light", &Scene::CreateLight )
	.def( "get_node", &Scene::GetNode )
	.def( "get_geometry", &Scene::GetGeometry )
	.def( "get_camera", &Scene::GetCamera )
	.def( "get_ligth", &Scene::GetLight )
	.def( "remove_node", &Scene::RemoveNode )
	.def( "remove_geometry", &Scene::RemoveGeometry )
	.def( "remove_camera", &Scene::RemoveCamera )
	.def( "remove_light", &Scene::RemoveLight )
	.def( "remove_all_nodes", &Scene::RemoveAllNodes )
	.def( "remove_all_geometries", &Scene::RemoveAllGeometries )
	.def( "remove_all_cameras", &Scene::RemoveAllCameras )
	.def( "remove_all_lights", &Scene::RemoveAllLights )
	.def( "get_background_image", &Scene::GetBackgroundImage )
	.def( "set_background_image", &Scene::SetBackgroundImage );
	//@}
	/**@group_name RenderWindow	*/
	//@{
	void( RenderWindow::*RenderWindowResizer )( Size const & ) = &RenderWindow::Resize;
	py::class_< RenderWindow, boost::noncopyable >( "RenderWindow", py::no_init )
	.add_property( "samples_count", &RenderWindow::GetSamplesCount, &RenderWindow::SetSamplesCount, "The samples count, if multisampling" )
	.add_property( "camera", &RenderWindow::GetCamera, &RenderWindow::SetCamera, "The window camera" )
	.add_property( "topology", &RenderWindow::GetPrimitiveType, &RenderWindow::SetPrimitiveType, "The rendering primitive type" )
	.add_property( "viewport", &RenderWindow::GetViewportType, &RenderWindow::SetViewportType, "The viewport type" )
	.add_property( "scene", &RenderWindow::GetScene, &RenderWindow::SetScene, "The rendered scene" )
	.add_property( "pixel_format", &RenderWindow::GetPixelFormat, &RenderWindow::SetPixelFormat, "The window pixel format" )
	.add_property( "size", &RenderWindow::GetSize, "The window size" )
	.add_property( "name", py::make_function( &RenderWindow::GetName, py::return_value_policy< py::copy_const_reference >() ), "The window name" )
	.def( "initialise", &RenderWindow::Initialise )
	.def( "cleanup", &RenderWindow::Cleanup )
	.def( "resize", RenderWindowResizer );
	//@}
	/**@group_name MovableObject	*/
	//@{
	py::class_< MovableObject, boost::noncopyable >( "MovableObject", py::no_init )
	.add_property( "name", py::make_function( &MovableObject::GetName, py::return_value_policy< py::copy_const_reference >() ), "The object name" )
	.add_property( "type", &MovableObject::GetType, "The movable type" )
	.add_property( "scene", py::make_function( &MovableObject::GetScene, py::return_value_policy< py::reference_existing_object >() ), "The object's parent scene" )
	.def( "attach", &MovableObject::AttachTo )
	.def( "detach", &MovableObject::Detach );
	//@}
	/**@group_name LightCategory	*/
	//@{
	py::class_< LightCategory, boost::noncopyable >( "LightCategory", py::no_init )
	.add_property( "ambient", cpy::make_getter( &LightCategory::GetAmbient, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LightCategory::SetAmbient ), "The light ambient value" )
	.add_property( "diffuse", cpy::make_getter( &LightCategory::GetDiffuse, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LightCategory::SetDiffuse ), "The light diffuse value" )
	.add_property( "specular", cpy::make_getter( &LightCategory::GetSpecular, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LightCategory::SetSpecular ), "The light specular value" );
	//@}
	/**@group_name DirectionalLight	*/
	//@{
	py::class_< DirectionalLight, py::bases< LightCategory >, boost::noncopyable >( "DirectionalLight", py::no_init )
	.add_property( "direction", &DirectionalLight::GetDirection, cpy::make_setter< Point3f, DirectionalLight >( &DirectionalLight::SetDirection ), "The light direction" );
	//@}
	/**@group_name PointLight	*/
	//@{
	py::class_< PointLight, py::bases< LightCategory >, boost::noncopyable >( "PointLight", py::no_init )
	.add_property( "attenuation", cpy::make_getter( &PointLight::GetAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::SetAttenuation ), "The light attenuation values (constant, linear and quadratic)" );
	//@}
	/**@group_name SpotLight	*/
	//@{
	py::class_< SpotLight, py::bases< LightCategory >, boost::noncopyable >( "SpotLight", py::no_init )
	.add_property( "attenuation", cpy::make_getter( &PointLight::GetAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::SetAttenuation ), "The light attenuation values (constant, linear and quadratic)" )
	.add_property( "exponent", &SpotLight::GetExponent, &SpotLight::SetExponent, "The light exponent value" )
	.add_property( "cut_off", &SpotLight::GetCutOff, &SpotLight::SetCutOff, "The light cut off value" );
	//@}
	/**@group_name Camera	*/
	//@{
	void ( Camera::*cameraResizer )( Size const & ) = &Camera::Resize;
	py::class_< Camera, py::bases< MovableObject >, boost::noncopyable >( "Camera", py::no_init )
	.add_property( "primitive_type", &Camera::GetPrimitiveType, &Camera::SetPrimitiveType, "The camera rendering primitive type" )
	.add_property( "viewport_type", &Camera::GetViewportType, &Camera::SetViewportType, "The camera viewport type" )
	.add_property( "width", &Camera::GetWidth, "The camera horizontal resolution" )
	.add_property( "height", &Camera::GetHeight, "The camera vertical resolution" )
	.def( "resize", cameraResizer );
	//@}
	/**@group_name Light	*/
	//@{
	py::class_< Light, py::bases< MovableObject >, boost::noncopyable >( "Light", py::no_init )
	.add_property( "light_type", &Light::GetLightType, "The light type" )
	.add_property( "category", &Light::GetLightCategory, "The light category" );
	//@}
	/**@group_name Geometry	*/
	//@{
	py::class_< Geometry, py::bases< MovableObject >, boost::noncopyable >( "Geometry", py::no_init )
	.add_property( "mesh", &Geometry::GetMesh, &Geometry::SetMesh, "The geometry's mesh" )
	.def( "get_material", &Geometry::GetMaterial )
	.def( "set_material", &Geometry::SetMaterial );
	//@}
	/**@group_name SceneNode	*/
	//@{
	py::class_< SceneNode >( "SceneNode", py::init< Scene *, String const & >() )
	.add_property( "position", cpy::make_getter( &SceneNode::GetPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::SetPosition ), "The node position" )
	.add_property( "orientation", cpy::make_getter( &SceneNode::GetOrientation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::SetOrientation ), "The node orientation" )
	.add_property( "scale", cpy::make_getter( &SceneNode::GetScale, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::SetScale ), "The node scale" )
	.def( "attach_object", &SceneNode::AttachObject )
	.def( "detach_object", &SceneNode::DetachObject )
	.def( "attach_to", &SceneNode::AttachTo )
	.def( "detach_from", &SceneNode::Detach )
	.def( "yaw", &SceneNode::Yaw )
	.def( "pitch", &SceneNode::Pitch )
	.def( "roll", &SceneNode::Roll )
	.def( "rotate", cpy::make_setter( &SceneNode::Rotate ) )
	.def( "translate", cpy::make_setter( &SceneNode::Translate ) )
	.def( "scale", cpy::make_setter( &SceneNode::Scale ) );
	//@}
	/**@group_name OverlayCategory	*/
	//@{
	py::class_< OverlayCategory, boost::noncopyable >( "OverlayCategory", py::no_init )
	.add_property( "type", &OverlayCategory::GetType, "The overlay type" )
	.add_property( "position", cpy::make_getter( &OverlayCategory::GetPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::SetPosition ), "The overlay position, relative to its parent" )
	.add_property( "size", cpy::make_getter( &OverlayCategory::GetSize, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::SetSize ), "The overlay size, relative to its parent" )
	.add_property( "visible", &Overlay::IsVisible, &Overlay::SetVisible, "The overlay visibility" )
	.add_property( "z_index", &Overlay::GetZIndex, &Overlay::SetZIndex, "The overlay z-index" )
	.add_property( "material", &Overlay::GetMaterial, &Overlay::SetMaterial, "The overlay material" );
	//@}
	/**@group_name PanelOverlay	*/
	//@{
	py::class_< PanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "PanelOverlay", py::no_init );
	//@}
	/**@group_name BorderPanelOverlay	*/
	//@{
	py::class_< BorderPanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "BorderPanelOverlay", py::no_init )
	.add_property( "border_left", &BorderPanelOverlay::GetLeftBorderSize, &BorderPanelOverlay::SetLeftBorderSize, "The bottom border width" )
	.add_property( "border_right", &BorderPanelOverlay::GetRightBorderSize, &BorderPanelOverlay::SetRightBorderSize, "The right border width" )
	.add_property( "border_top", &BorderPanelOverlay::GetTopBorderSize, &BorderPanelOverlay::SetTopBorderSize, "The top border width" )
	.add_property( "border_bottom", &BorderPanelOverlay::GetBottomBorderSize, &BorderPanelOverlay::SetBottomBorderSize, "The bottom border width" )
	.add_property( "border_material", &BorderPanelOverlay::GetBorderMaterial, &BorderPanelOverlay::SetBorderMaterial, "The border material" );
	//@}
	/**@group_name TextOverlay	*/
	//@{
	py::class_< TextOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "TextOverlay", py::no_init )
	.add_property( "font", py::make_function( &TextOverlay::GetFontName, py::return_value_policy< py::copy_const_reference >() ), &TextOverlay::SetFont, "The text font" )
	.add_property( "caption", &TextOverlay::GetCaption, &TextOverlay::SetCaption, "The text caption" );
	//@}
	/**@group_name Overlay	*/
	//@{
	typedef OverlayPtrIntMapIt( Overlay::*OverlayPtrIntMapItFunc )();
	py::class_< Overlay, boost::noncopyable >( "Overlay", py::no_init )
	.add_property( "category", &Overlay::GetOverlayCategory, "The overlay category" )
	.add_property( "name", py::make_function( &Overlay::GetName, py::return_value_policy< py::copy_const_reference >() ), &Overlay::SetName, "The overlay name" )
	.add_property( "childs", py::range< OverlayPtrIntMapItFunc, OverlayPtrIntMapItFunc >( &Overlay::Begin, &Overlay::End ) )
	.def( "childs_count", &Overlay::GetChildsCount )
	.def( "add_child", &Overlay::AddChild );
	//@}
	/**@group_name Animable	*/
	//@{
	typedef AnimationPtrStrMapIt( Animable::*AnimableAnimationsItFunc )();
	py::class_< Animable >( "Animable" )
	.add_property( "animations", py::range< AnimableAnimationsItFunc, AnimableAnimationsItFunc >( &Animable::AnimationsBegin, &Animable::AnimationsEnd ) )
	.def( "create_animation", &Animable::CreateAnimation )
	.def( "get_animation", &Animable::GetAnimation );
	//@}
	/**@group_name AnimatedObject	*/
	//@{
	typedef AnimationPtrStrMapIt( AnimatedObject::*AnimatedObjectAnimationsItFunc )();
	py::class_< AnimatedObject >( "AnimatedObject", py::init< Castor::String >() )
	.add_property( "geometry", &AnimatedObject::GetGeometry, &AnimatedObject::SetGeometry )
	.add_property( "mesh", &AnimatedObject::GetMesh, &AnimatedObject::SetMesh )
	.add_property( "skeleton", &AnimatedObject::GetSkeleton, &AnimatedObject::SetSkeleton )
	.add_property( "animations", py::range< AnimatedObjectAnimationsItFunc, AnimatedObjectAnimationsItFunc >( &AnimatedObject::AnimationsBegin, &AnimatedObject::AnimationsEnd ) )
	.def( "start_all_animations", &AnimatedObject::StartAllAnimations )
	.def( "stop_all_animations", &AnimatedObject::StopAllAnimations )
	.def( "pause_all_animations", &AnimatedObject::PauseAllAnimations )
	.def( "start_animation", &AnimatedObject::StartAnimation )
	.def( "stop_animation", &AnimatedObject::StopAnimation )
	.def( "pause_animation", &AnimatedObject::StopAnimation )
	.def( "get_animation", &AnimatedObject::GetAnimation );
	//@}
	/**@group_name AnimatedObjectGroup	*/
	//@{
	typedef Castor::StrSetIt( AnimatedObjectGroup::*AnimatedObjectGroupAnimationsItFunc )();
	typedef AnimatedObjectPtrStrMapIt( AnimatedObjectGroup::*AnimatedObjectGroupObjectsItFunc )();
	py::class_< AnimatedObjectGroup >( "AnimatedObjectGroup", py::init< Scene *, Castor::String >() )
	.add_property( "animation_count", &AnimatedObjectGroup::GetAnimationCount )
	.add_property( "object_count", &AnimatedObjectGroup::GetObjectCount )
	.add_property( "animations", py::range< AnimatedObjectGroupAnimationsItFunc, AnimatedObjectGroupAnimationsItFunc >( &AnimatedObjectGroup::AnimationsBegin, &AnimatedObjectGroup::AnimationsEnd ) )
	.add_property( "objects", py::range< AnimatedObjectGroupObjectsItFunc, AnimatedObjectGroupObjectsItFunc >( &AnimatedObjectGroup::ObjectsBegin, &AnimatedObjectGroup::ObjectsEnd ) )
	.add_property( "scene", py::make_function( &AnimatedObjectGroup::GetScene, py::return_value_policy< py::reference_existing_object >() ) )
	.def( "start_all_animations", &AnimatedObjectGroup::StartAllAnimations )
	.def( "stop_all_animations", &AnimatedObjectGroup::StopAllAnimations )
	.def( "pause_all_animations", &AnimatedObjectGroup::PauseAllAnimations )
	.def( "start_animation", &AnimatedObjectGroup::StartAnimation )
	.def( "stop_animation", &AnimatedObjectGroup::StopAnimation )
	.def( "pause_animation", &AnimatedObjectGroup::StopAnimation )
	.def( "create_object", &AnimatedObjectGroup::CreateObject )
	.def( "add_object", &AnimatedObjectGroup::AddObject )
	.def( "add_animation", &AnimatedObjectGroup::AddAnimation )
	.def( "set_animation_looped", &AnimatedObjectGroup::SetAnimationLooped );
	/**@group_name Engine	*/
	//@{
	MeshSPtr( Engine::*meshCreator1 )( eMESH_TYPE, Castor::String const & ) = &Engine::CreateMesh;
	MeshSPtr( Engine::*meshCreator2 )( eMESH_TYPE, Castor::String const &, UIntArray const & ) = &Engine::CreateMesh;
	MeshSPtr( Engine::*meshCreator3 )( eMESH_TYPE, Castor::String const &, UIntArray const &, RealArray const & ) = &Engine::CreateMesh;
	bool ( Engine::*windowRemover )( RenderWindowSPtr ) = &Engine::RemoveRenderWindow;
	PluginBaseSPtr( Engine::*pluginLoader )( Castor::Path const & ) = &Engine::LoadPlugin;
	py::class_< Engine, boost::noncopyable >( "Engine", py::init< Logger * >() )
	.def( "initialise", &Engine::Initialise )
	.def( "cleanup", &Engine::Cleanup )
	.def( "create_scene", &Engine::CreateScene )
	.def( "clear_scenes", &Engine::ClearScenes )
	.def( "load_renderer", &Engine::LoadRenderer )
	.def( "render_one_frame", &Engine::RenderOneFrame )
	.def( "load_plugin", pluginLoader )
	.def( "create_mesh", meshCreator1 )
	.def( "create_mesh", meshCreator2 )
	.def( "create_mesh", meshCreator3 )
	.def( "create_overlay", &Engine::CreateOverlay )
	.def( "create_window", &Engine::CreateRenderWindow )
	.def( "remove_window", windowRemover );
	//@}
}
