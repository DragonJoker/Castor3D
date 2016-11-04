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

	void ComputePreLightingMapContributions( GLSL::GlslWriter & p_writer
											 , GLSL::Vec3 & p_normal
											 , GLSL::Float & p_shininess
											 , uint16_t p_textureFlags
											 , uint16_t p_programFlags
											 , uint8_t p_sceneFlags )
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
			l_v3MapNormal = Float( &p_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &p_writer, 1 ), 1.0, 1.0 );
			p_normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eGloss ) )
		{
			auto c3d_mapGloss( p_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapGloss" ) ) );

			p_shininess = texture( c3d_mapGloss, vtx_texture.xy() ).r();
		}
	}

	void ComputePostLightingMapContributions( GLSL::GlslWriter & p_writer
											  , GLSL::Vec3 & p_ambient
											  , GLSL::Vec3 & p_diffuse
											  , GLSL::Vec3 & p_specular
											  , GLSL::Vec3 & p_emissive
											  , uint16_t p_textureFlags
											  , uint16_t p_programFlags
											  , uint8_t p_sceneFlags )
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
