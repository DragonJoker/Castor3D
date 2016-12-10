#include "Castor3DPrerequisites.hpp"

#include <Engine.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>

#include <GlslSource.hpp>

IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::Engine, Engine )
IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::RenderSystem, RenderSystem )
IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::Scene, Scene )

using namespace Castor;

namespace Castor3D
{
	template<> String const TopologyNamer< Topology::ePoints >::Name = cuT( "points" );
	template<> String const TopologyNamer< Topology::eLines >::Name = cuT( "lines" );
	template<> String const TopologyNamer< Topology::eLineLoop >::Name = cuT( "line_loop" );
	template<> String const TopologyNamer< Topology::eLineStrip >::Name = cuT( "line_strip" );
	template<> String const TopologyNamer< Topology::eTriangles >::Name = cuT( "triangles" );
	template<> String const TopologyNamer< Topology::eTriangleStrips >::Name = cuT( "triangle_strip" );
	template<> String const TopologyNamer< Topology::eTriangleFan >::Name = cuT( "triangle_fan" );
	template<> String const TopologyNamer< Topology::eQuads >::Name = cuT( "quads" );
	template<> String const TopologyNamer< Topology::eQuadStrips >::Name = cuT( "quad_strip" );
	template<> String const TopologyNamer< Topology::ePolygon >::Name = cuT( "polygon" );

	String GetName( ElementType p_type )
	{
		switch ( p_type )
		{
		case ElementType::eFloat:
			return cuT( "float" );
			break;

		case ElementType::eVec2:
			return cuT( "vec2f" );
			break;

		case ElementType::eVec3:
			return cuT( "vec3f" );
			break;

		case ElementType::eVec4:
			return cuT( "vec4f" );
			break;

		case ElementType::eColour:
			return cuT( "colour" );
			break;

		case ElementType::eInt:
			return cuT( "int" );
			break;

		case ElementType::eIVec2:
			return cuT( "vec2i" );
			break;

		case ElementType::eIVec3:
			return cuT( "vec3i" );
			break;

		case ElementType::eIVec4:
			return cuT( "vec4i" );
			break;

		case ElementType::eUInt:
			return cuT( "uint" );
			break;

		case ElementType::eUIVec2:
			return cuT( "vec2ui" );
			break;

		case ElementType::eUIVec3:
			return cuT( "vec3ui" );
			break;

		case ElementType::eUIVec4:
			return cuT( "vec4ui" );
			break;

		case ElementType::eMat2:
			return cuT( "mat2" );
			break;

		case ElementType::eMat3:
			return cuT( "mat3" );
			break;

		case ElementType::eMat4:
			return cuT( "mat4" );
			break;

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}

	void ComputePreLightingMapContributions(
		GLSL::GlslWriter & p_writer,
		GLSL::Vec3 & p_normal,
		GLSL::Float & p_shininess,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags )
	{
		using namespace GLSL;
		auto vtx_texture( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_texture" ) ) );

		if ( CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			auto vtx_normal( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
			auto vtx_tangent( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
			auto vtx_bitangent( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
			auto c3d_mapNormal( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

			auto l_v3MapNormal = p_writer.GetLocale( cuT( "l_v3MapNormal" ), texture( c3d_mapNormal, vtx_texture.xy() ).xyz() );
			l_v3MapNormal = l_v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 );
			p_normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eGloss ) )
		{
			auto c3d_mapGloss( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapGloss" ) ) );

			p_shininess = texture( c3d_mapGloss, vtx_texture.xy() ).r();
		}
	}

	void ComputePostLightingMapContributions(
		GLSL::GlslWriter & p_writer,
		GLSL::Vec3 & p_ambient,
		GLSL::Vec3 & p_diffuse,
		GLSL::Vec3 & p_specular,
		GLSL::Vec3 & p_emissive,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		uint8_t p_sceneFlags )
	{
		using namespace GLSL;
		auto vtx_texture( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_texture" ) ) );

		if ( CheckFlag( p_textureFlags, TextureChannel::eColour ) )
		{
			auto c3d_mapColour( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapColour" ) ) );

			p_ambient += texture( c3d_mapColour, vtx_texture.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eAmbient ) )
		{
			auto c3d_mapAmbient( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapAmbient" ) ) );

			p_ambient += texture( c3d_mapAmbient, vtx_texture.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) )
		{
			auto c3d_mapDiffuse( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDiffuse" ) ) );

			p_diffuse *= texture( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eSpecular ) )
		{
			auto c3d_mapSpecular( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapSpecular" ) ) );

			p_specular *= texture( c3d_mapSpecular, vtx_texture.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eEmissive ) )
		{
			auto c3d_mapEmissive( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapEmissive" ) ) );

			p_emissive = texture( c3d_mapEmissive, vtx_texture.xy() ).xyz();
		}
	}
}
