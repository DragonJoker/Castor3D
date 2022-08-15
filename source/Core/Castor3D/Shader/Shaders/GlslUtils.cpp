#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	Utils::Utils( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	LightingModelPtr Utils::createLightingModel( Engine const & engine
		, castor::String const & name
		, ShadowOptions shadowsOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return engine.getLightingModelFactory().create( name
			, m_writer
			, *this
			, std::move( shadowsOptions )
			, sssProfiles
			, enableVolumetric );
	}
	
	sdw::Float Utils::saturate( sdw::Float const v )
	{
		return clamp( v, 0.0_f, 1.0_f );
	}

	sdw::Vec3 Utils::saturate( sdw::Vec3 const v )
	{
		return clamp( v, vec3( 0.0_f ), vec3( 1.0_f ) );
	}
	
	sdw::Vec2 Utils::topDownToBottomUp( sdw::Vec2 const v )
	{
		return vec2( v.x(), 1.0_f - v.y() );
	}

	sdw::Vec3 Utils::topDownToBottomUp( sdw::Vec3 const v )
	{
		return vec3( v.x(), 1.0_f - v.y(), v.z() );
	}

	sdw::Vec4 Utils::topDownToBottomUp( sdw::Vec4 const v )
	{
		return vec4( v.x(), 1.0_f - v.y(), v.z(), v.w() );
	}

	sdw::Vec2 Utils::negateTopDownToBottomUp( sdw::Vec2 const v )
	{
		return vec2( v.x(), -v.y() );
	}

	sdw::Vec3 Utils::negateTopDownToBottomUp( sdw::Vec3 const v )
	{
		return vec3( v.x(), -v.y(), v.z() );
	}

	sdw::Vec4 Utils::negateTopDownToBottomUp( sdw::Vec4 const v )
	{
		return vec4( v.x(), -v.y(), v.z(), v.w() );
	}

	sdw::Vec2 Utils::calcTexCoord( sdw::Vec2 const renderPos
			, sdw::Vec2 const renderSize )
	{
		return renderPos / renderSize;
	}
	
	sdw::Vec3 Utils::applyGamma( sdw::Float const gamma
		, sdw::Vec3 const hdr )
	{
		return pow( max( hdr, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( 1.0_f / gamma ) );
	}

	sdw::Vec3 Utils::removeGamma( sdw::Float const gamma
		, sdw::Vec3 const srgb )
	{
		return pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) );
	}

	void Utils::compute2DMapsContributions( FilteredTextureFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const maps
		, sdw::Vec3 const texCoords
		, sdw::Vec3 & colour
		, sdw::Float & opacity )
	{
		if ( !textureConfigs.isEnabled() )
		{
			return;
		}

		for ( auto & textureIt : flags )
		{
			if ( checkFlag( textureIt.second.flags, TextureFlag::eColour )
				|| checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
			{
				auto i = textureIt.first;
				auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
				CU_Require( textureIt.second.id > 0u );
				auto config = m_writer.declLocale( "c3d_config" + name
					, textureConfigs.getTextureConfiguration( sdw::UInt( textureIt.second.id ) ) );
				auto anim = m_writer.declLocale( "c3d_anim" + name
					, textureAnims.getTextureAnimation( sdw::UInt( textureIt.second.id ) ) );
				auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
					, texCoords.xy() );
				texCoord = transformUV( config, anim, texCoord );
				auto sampled = m_writer.declLocale< sdw::Vec4 >( "c3d_sampled" + name
					, maps[i].sample( texCoord ) );

				if ( checkFlag( textureIt.second.flags, TextureFlag::eColour ) )
				{
					colour = config.getColour( sampled, colour );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
				{
					opacity = config.getOpacity( sampled, opacity );
				}
			}
		}
	}

	sdw::Float Utils::remap( sdw::Float const originalValue
		, sdw::Float const originalMin
		, sdw::Float const originalMax
		, sdw::Float const newMin
		, sdw::Float const newMax )
	{
		return newMin + ( ( ( originalValue - originalMin ) / ( originalMax - originalMin ) ) * ( newMax - newMin ) );
	}

	sdw::Float Utils::threshold( sdw::Float const v
		, sdw::Float const t )
	{
		return m_writer.ternary( v > t, v, 0.0_f );
	}

	sdw::Float Utils::beer( sdw::Float const d )
	{
		return exp( -d );
	}

	sdw::Float Utils::powder( sdw::Float const d )
	{
		return 1.0_f - exp( -2.0_f * d );
	}

	sdw::Float Utils::powder( sdw::Float const d
		, sdw::Float const cosTheta )
	{
		return mix( 1.0_f
			, powder( d )
			, clamp( fma( cosTheta, 0.5_f, 0.5_f ), 0.0_f, 1.0_f ) );
	}

	sdw::Vec4 Utils::sampleMap( PassFlags const & passFlags
		, sdw::CombinedImage2DRgba32 const map
		, sdw::Vec2 const texCoords
		, sdw::Float const * lod )
	{
		return ( checkFlag( passFlags, PassFlag::eUntile )
			? sampleUntiled( map, texCoords )
			: ( lod
				? map.sample( texCoords, *lod )
				: map.sample( texCoords ) ) );
	}

	sdw::Vec4 Utils::sampleMap( PassFlags const & passFlags
		, sdw::CombinedImage2DRgba32 const map
		, DerivTex const texCoords
		, sdw::Float const * lod )
	{
		return ( checkFlag( passFlags, PassFlag::eUntile )
			? sampleUntiled( map, texCoords )
			: map.grad( texCoords.uv(), texCoords.dx(), texCoords.dy() ) );
	}

	sdw::RetVec2 Utils::transformUV( TextureConfigData const & pconfig
		, TextureAnimData const & panim
		, sdw::Vec2 const puv )
	{
		if ( !m_transformUV )
		{
			m_transformUV = m_writer.implementFunction< sdw::Vec2 >( "transformUV"
				, [&]( TextureConfigData const & config
					, TextureAnimData const & anim
					, sdw::Vec2 uv )
				{
					uv = vec2( uv.x()
						, mix( uv.y(), 1.0_f - uv.y(), config.fneedYI ) );
					uv = scaleUV( config.texScl.xy(), uv );
					uv = rotateUV( config.texRot.xy(), uv );
					uv = translateUV( config.texTrn.xy(), uv );

					IF( m_writer, config.isTrnfAnim )
					{
						uv = vec2( uv.x()
							, mix( uv.y(), 1.0_f - uv.y(), config.fneedYI ) );
						uv = scaleUV( anim.texScl.xy(), uv );
						uv = rotateUV( anim.texRot.xy(), uv );
						uv = translateUV( anim.texTrn.xy(), uv );
					}
					FI;

					uv.x() = ( uv.x() + config.tleSet.x() ) / config.tleSet.z();
					uv.y() = ( uv.y() + config.tleSet.y() ) / config.tleSet.w();

					IF( m_writer, config.isTileAnim )
					{
						uv.x() += anim.tleSet.x() / anim.tleSet.z();
						uv.y() += anim.tleSet.y() / anim.tleSet.w();
					}
					FI;

					m_writer.returnStmt( uv );
				}
				, InTextureConfigData{ m_writer, "config" }
				, InTextureAnimData{ m_writer, "anim" }
				, sdw::InVec2{ m_writer, "uv" } );
		}

		return m_transformUV( pconfig, panim, puv );
	}

	sdw::RetVec3 Utils::transformUVW( TextureConfigData const & pconfig
		, TextureAnimData const & panim
		, sdw::Vec3 const puvw )
	{
		if ( !m_transformUVW )
		{
			m_transformUVW = m_writer.implementFunction< sdw::Vec3 >( "transformUVW"
				, [&]( TextureConfigData const & config
					, TextureAnimData const & anim
					, sdw::Vec3 uvw )
				{
					uvw = vec3( uvw.x()
						, mix( uvw.y(), 1.0_f - uvw.y(), config.fneedYI )
						, uvw.z() );
					uvw = scaleUV( config.texScl.xyz(), uvw );
					uvw = rotateUV( config.texRot.xyz(), uvw );
					uvw = translateUV( config.texTrn.xyz(), uvw );

					IF( m_writer, config.isTrnfAnim )
					{
						uvw = vec3( uvw.x()
							, mix( uvw.y(), 1.0_f - uvw.y(), config.fneedYI )
							, uvw.z() );
						uvw = scaleUV( anim.texScl.xyz(), uvw );
						uvw = rotateUV( anim.texRot.xyz(), uvw );
						uvw = translateUV( anim.texTrn.xyz(), uvw );
					}
					FI;

					m_writer.returnStmt( uvw );
				}
				, InTextureConfigData{ m_writer, "config" }
				, InTextureAnimData{ m_writer, "anim" }
				, sdw::InVec3{ m_writer, "uvw" } );
		}

		return m_transformUVW( pconfig, panim, puvw );
	}

	void Utils::swap( sdw::Float const pA
		, sdw::Float const & pB )
	{
		if ( !m_swap1F )
		{
			m_swap1F = m_writer.implementFunction< sdw::Void >( "c3d_swap1F"
				, [&]( sdw::Float A
					, sdw::Float B )
				{
					auto tmp = m_writer.declLocale( "tmp", A );
					A = B;
					B = tmp;
				}
				, sdw::InOutFloat{ m_writer, "A" }
				, sdw::InOutFloat{ m_writer, "B" } );
		}

		m_swap1F( pA, pB );
	}

	sdw::RetFloat Utils::distanceSquared( sdw::Vec2 const pA
		, sdw::Vec2 const & pB )
	{
		if ( !m_distanceSquared2F )
		{
			m_distanceSquared2F = m_writer.implementFunction< sdw::Float >( "c3d_distanceSquared2F"
				, [&]( sdw::Vec2 A
					, sdw::Vec2 const & B )
				{
					A -= B;
					m_writer.returnStmt( dot( A, A ) );
				}
				, sdw::InVec2{ m_writer, "A" }
				, sdw::InVec2{ m_writer, "B" } );
		}

		return m_distanceSquared2F( pA, pB );
	}

	sdw::RetFloat Utils::distanceSquared( sdw::Vec3 const pA
		, sdw::Vec3 const & pB )
	{
		if ( !m_distanceSquared3F )
		{
			m_distanceSquared3F = m_writer.implementFunction< sdw::Float >( "c3d_distanceSquared3F"
				, [&]( sdw::Vec3 A
					, sdw::Vec3 const & B )
				{
					A -= B;
					m_writer.returnStmt( dot( A, A ) );
				}
				, sdw::InVec3{ m_writer, "A" }
				, sdw::InVec3{ m_writer, "B" } );
		}

		return m_distanceSquared3F( pA, pB );
	}

	sdw::RetVec3 Utils::calcVSPosition( sdw::Vec2 const & puv
		, sdw::Float const & pdepth
		, sdw::Mat4 const & pinvProj )
	{
		if ( !m_calcVSPosition )
		{
			m_calcVSPosition = m_writer.implementFunction< sdw::Vec3 >( "c3d_calcVSPosition"
				, [&]( sdw::Vec2 const & uv
					, sdw::Float const & depth
					, sdw::Mat4 const & invProj )
				{
					auto csPosition = m_writer.declLocale( "csPosition"
						, vec3( uv * 2.0_f - 1.0_f, depth ) );
					auto vsPosition = m_writer.declLocale( "vsPosition"
						, invProj * vec4( csPosition, 1.0_f ) );
					m_writer.returnStmt( vsPosition.xyz() / vsPosition.w() );
				}
				, sdw::InVec2{ m_writer, "uv" }
				, sdw::InFloat{ m_writer, "depth" }
				, sdw::InMat4{ m_writer, "invProj" } );
		}

		return m_calcVSPosition( puv
			, pdepth
			, pinvProj );
	}

	sdw::RetVec3 Utils::calcWSPosition( sdw::Vec2 const & puv
		, sdw::Float const & pdepth
		, sdw::Mat4 const & pinvViewProj )
	{
		if ( !m_calcWSPosition )
		{
			m_calcWSPosition = m_writer.implementFunction< sdw::Vec3 >( "c3d_calcWSPosition"
				, [&]( sdw::Vec2 const & uv
					, sdw::Float const & depth
					, sdw::Mat4 const & invViewProj )
				{
					auto csPosition = m_writer.declLocale( "psPosition"
						, vec3( uv * 2.0_f - 1.0_f, depth ) );
					auto wsPosition = m_writer.declLocale( "wsPosition"
						, invViewProj * vec4( csPosition, 1.0_f ) );
					m_writer.returnStmt( wsPosition.xyz() / wsPosition.w() );
				}
				, sdw::InVec2{ m_writer, "uv" }
				, sdw::InFloat{ m_writer, "depth" }
				, sdw::InMat4{ m_writer, "invViewProj" } );
		}

		return m_calcWSPosition( puv
			, pdepth
			, pinvViewProj );
	}

	sdw::RetVec3 Utils::getMapNormal( sdw::Vec2 const & puv
		, sdw::Vec3 const & pnormal
		, sdw::Vec3 const & pposition )
	{
		if ( !m_getMapNormal )
		{
			m_getMapNormal = m_writer.implementFunction< sdw::Vec3 >( "c3d_getMapNormal"
				, [&]( sdw::Vec2 const & uv
					, sdw::Vec3 const & normal
					, sdw::Vec3 const & position )
				{
					auto c3d_mapNormal( m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapNormal" ) );

					auto mapNormal = m_writer.declLocale( "mapNormal"
						, c3d_mapNormal.sample( uv.xy() ).xyz() );
					mapNormal = sdw::fma( mapNormal
						, vec3( 2.0_f )
						, vec3( -1.0_f ) );
					auto Q1 = m_writer.declLocale( "Q1"
						, dFdx( position ) );
					auto Q2 = m_writer.declLocale( "Q2"
						, dFdy( position ) );
					auto st1 = m_writer.declLocale( "st1"
						, dFdx( uv ) );
					auto st2 = m_writer.declLocale( "st2"
						, dFdy( uv ) );
					auto N = m_writer.declLocale( "N"
						, normalize( normal ) );
					auto T = m_writer.declLocale( "T"
						, normalize( Q1 * st2.t() - Q2 * st1.t() ) );
					auto B = m_writer.declLocale( "B"
						, -normalize( cross( N, T ) ) );
					auto tbn = m_writer.declLocale( "tbn"
						, mat3( T, B, N ) );
					m_writer.returnStmt( normalize( tbn * mapNormal ) );
				}
				, sdw::InVec2{ m_writer, "uv" }
				, sdw::InVec3{ m_writer, "normal" }
				, sdw::InVec3{ m_writer, "position" } );
		}

		return m_getMapNormal( puv, pnormal, pposition );
	}

	sdw::RetFloat Utils::rescaleDepth( sdw::Float const & pdepth
		, sdw::Float const & pnearPlane
		, sdw::Float const & pfarPlane )
	{
		if ( !m_rescaleDepth )
		{
			m_rescaleDepth = m_writer.implementFunction< sdw::Float >( "c3d_rescaleDepth"
				, [&]( sdw::Float const & depth
					, sdw::Float const & nearPlane
					, sdw::Float const & farPlane )
				{
					m_writer.returnStmt( 2.0_f * nearPlane / ( farPlane + nearPlane - depth * ( farPlane - nearPlane ) ) );
				}
				, sdw::InFloat{ m_writer, "depth" }
				, sdw::InFloat{ m_writer, "nearPlane" }
				, sdw::InFloat{ m_writer, "farPlane" } );
		}

		return m_rescaleDepth( pdepth
			, pnearPlane
			, pfarPlane );
	}

	sdw::RetVec4 Utils::computeAccumulation( sdw::Float const & pdepth
		, sdw::Vec3 const & pcolour
		, sdw::Float const & palpha
		, sdw::Float const & pnearPlane
		, sdw::Float const & pfarPlane
		, sdw::Float const & paccumulationOperator )
	{
		if ( !m_computeAccumulation )
		{
			m_computeAccumulation = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeAccumulation"
				, [&]( sdw::Float const & depth
					, sdw::Vec3 const & colour
					, sdw::Float const & alpha
					, sdw::Float const & nearPlane
					, sdw::Float const & farPlane
					, sdw::Float const & accumulationOperator )
				{
					auto weight = m_writer.declLocale< sdw::Float >( "weight"
						, 1.0_f );

					SWITCH( m_writer, m_writer.cast< sdw::Int >( accumulationOperator ) )
					{
						CASE( 0 )
						{
							// Naive
							weight = 1.0_f - rescaleDepth( depth
								, nearPlane
								, farPlane );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 1 )
						{
							// (10)
							weight = max( pow( clamp( 1.0_f - depth, 0.0_f, 1.0_f ), 3.0_f ) * 3000.0_f, 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 2 )
						{
							// (9)
							weight = max( min( 0.03_f / ( pow( abs( depth ) / 200.0_f, 4.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 3 )
						{
							// (8)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 10.0_f, 3.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 4 )
						{
							// (7)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 5.0_f, 2.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 5 )
						{
							// (other)
							auto a = m_writer.declLocale( "a"
								, min( alpha, 1.0_f ) * 8.0_f + 0.01_f );
							auto b = m_writer.declLocale( "b"
								, -depth * 0.95_f + 1.0_f );
							weight = clamp( a * a * a * 100000000.0_f * b * b * b, 0.01_f, 300.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 6 )
						{
							// (other)
							auto a = m_writer.declLocale( "a"
								, min( alpha, 1.0_f ) * 8.0_f + 0.01_f );
							auto b = m_writer.declLocale( "b"
								, -depth * 0.95_f + 1.0_f );
							// If your scene has a lot of content very close to the far plane,
							// then include this line (one rsqrt instruction):
							b /= sqrt( 10000.0_f * abs( depth ) );
							weight = clamp( a * a * a * 100000000.0_f * b * b * b, 0.01_f, 300.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 7 )
						{
							// (yet another one)
							weight = max( min( 1.0_f, max( max( colour.r(), colour.g() ), colour.b() ) * alpha ), alpha ) * clamp( 0.03_f / ( 0.00001_f + pow( depth / 200.0_f, 4.0_f ) ), 0.01_f, 3000.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
					}
					HCTIWS;

					m_writer.returnStmt( vec4( colour * alpha, alpha ) * weight );
				}
				, sdw::InFloat{ m_writer, "depth" }
				, sdw::InVec3{ m_writer, "colour" }
				, sdw::InFloat{ m_writer, "alpha" }
				, sdw::InFloat{ m_writer, "nearPlane" }
				, sdw::InFloat{ m_writer, "farPlane" }
				, sdw::InFloat{ m_writer, "accumulationOperator" } );
		}

		return m_computeAccumulation( pdepth
			, pcolour
			, palpha
			, pnearPlane
			, pfarPlane
			, paccumulationOperator );
	}

	sdw::RetVec3 Utils::fresnelSchlick( sdw::Float const & pproduct
		, sdw::Vec3 const & pf0 )
	{
		if ( !m_fresnelSchlick )
		{
			m_fresnelSchlick = m_writer.implementFunction< sdw::Vec3 >( "c3d_fresnelSchlick"
				, [&]( sdw::Float const & product
					, sdw::Vec3 const & f0 )
				{
					auto f90 = clamp( dot( f0, vec3( 50.0_f * 0.33_f ) ), 0.0_f, 1.0_f );
					m_writer.returnStmt( sdw::fma( max( vec3( f90 ), f0 ) - f0
						, vec3( pow( clamp( 1.0_f - product, 0.0_f, 1.0_f ), 5.0_f ) )
						, f0 ) );
				}
				, sdw::InFloat{ m_writer, "product" }
				, sdw::InVec3{ m_writer, "f0" } );
		}

		return m_fresnelSchlick( pproduct, pf0 );
	}

	void Utils::parallaxMapping( sdw::Vec2 & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, TextureConfigData const & textureConfig )
	{
		return parallaxMapping( texCoords
			, dFdxCoarse( texCoords )
			, dFdyCoarse( texCoords )
			, viewDir
			, heightMap
			, textureConfig );
	}

	void Utils::parallaxMapping( DerivTex & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, TextureConfigData const & textureConfig )
	{
		parallaxMapping( texCoords.uv()
			, texCoords.dx()
			, texCoords.dy()
			, viewDir
			, heightMap
			, textureConfig );
	}

	void Utils::parallaxMapping( sdw::Vec2 ptexCoords
		, sdw::Vec2 const & pdx
		, sdw::Vec2 const & pdy
		, sdw::Vec3 const & pviewDir
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, TextureConfigData const & ptextureConfig )
	{
		if ( !m_parallaxMapping )
		{
			m_parallaxMapping = m_writer.implementFunction< sdw::Vec2 >( "c3d_parallaxMapping",
				[&]( sdw::Vec2 const & texCoords
					, sdw::Vec2 const & dx
					, sdw::Vec2 const & dy
					, sdw::Vec3 const & viewDir
					, sdw::CombinedImage2DRgba32 const & heightMap
					, TextureConfigData const & textureConfig )
				{
					IF( m_writer, textureConfig.hgtEnbl == 0.0_f )
					{
						m_writer.returnStmt( texCoords );
					}
					FI;

					// number of depth layers
					auto minLayers = m_writer.declLocale( "minLayers"
						, 32.0_f );
					auto maxLayers = m_writer.declLocale( "maxLayers"
						, 64.0_f );
					auto numLayers = m_writer.declLocale( "numLayers"
						, mix( maxLayers
							, minLayers
							, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), viewDir ) ) ) );
					// calculate the size of each layer
					auto layerDepth = m_writer.declLocale( "layerDepth"
						, 1.0_f / numLayers );
					// depth of current layer
					auto currentLayerDepth = m_writer.declLocale( "currentLayerDepth"
						, 0.0_f );
					// the amount to shift the texture coordinates per layer (from vector P)
					auto p = m_writer.declLocale( "p"
						, viewDir.xy() / viewDir.z() * textureConfig.hgtFact );
					auto deltaTexCoords = m_writer.declLocale( "deltaTexCoords"
						, p / numLayers );

					auto currentTexCoords = m_writer.declLocale( "currentTexCoords"
						, texCoords );
					auto heightIndex = m_writer.declLocale( "heightIndex"
						, m_writer.cast< sdw::UInt >( textureConfig.hgtMask ) );
					auto sampled = m_writer.declLocale( "sampled"
						, heightMap.grad( currentTexCoords, dx, dy ) );
					auto currentDepthMapValue = m_writer.declLocale( "currentDepthMapValue"
						, sampled[heightIndex] );

					WHILE( m_writer, currentLayerDepth < currentDepthMapValue )
					{
						// shift texture coordinates along direction of P
						currentTexCoords -= deltaTexCoords;
						// get depthmap value at current texture coordinates
						sampled = heightMap.grad( currentTexCoords, dx, dy );
						currentDepthMapValue = sampled[heightIndex];
						// get depth of next layer
						currentLayerDepth += layerDepth;
					}
					ELIHW;

					// get texture coordinates before collision (reverse operations)
					auto prevTexCoords = m_writer.declLocale( "prevTexCoords"
						, currentTexCoords + deltaTexCoords );

					// get depth after and before collision for linear interpolation
					auto afterDepth = m_writer.declLocale( "afterDepth"
						, currentDepthMapValue - currentLayerDepth );
					sampled = heightMap.grad( prevTexCoords, dx, dy );
					auto beforeDepth = m_writer.declLocale( "beforeDepth"
						, sampled[heightIndex] - currentLayerDepth + layerDepth );

					// interpolation of texture coordinates
					auto weight = m_writer.declLocale( "weight"
						, vec2( afterDepth / ( afterDepth - beforeDepth ) ) );
					auto finalTexCoords = m_writer.declLocale( "finalTexCoords"
						, sdw::fma( prevTexCoords
							, weight
							, currentTexCoords * ( vec2( 1.0_f ) - weight ) ) );

					m_writer.returnStmt( finalTexCoords );
				}
				, sdw::InVec2{ m_writer, "texCoords" }
				, sdw::InVec2{ m_writer, "dx" }
				, sdw::InVec2{ m_writer, "dy" }
				, sdw::InVec3{ m_writer, "viewDir" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "heightMap" }
				, InTextureConfigData{ m_writer, "textureConfig" } );
		}

		ptexCoords = m_parallaxMapping( ptexCoords
			, pdx
			, pdy
			, pviewDir
			, pheightMap
			, ptextureConfig );
	}

	sdw::RetFloat Utils::parallaxShadow( sdw::Vec3 const & plightDir
		, sdw::Vec2 const & pinitialTexCoord
		, sdw::Float const & pinitialHeight
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, TextureConfigData const & ptextureConfig )
	{
		if ( !m_parallaxShadow )
		{
			m_parallaxShadow = m_writer.implementFunction< sdw::Float >( "c3d_parallaxSoftShadowMultiplier"
				, [&]( sdw::Vec3 const & lightDir
					, sdw::Vec2 const & initialTexCoord
					, sdw::Float const & initialHeight
					, sdw::CombinedImage2DRgba32 const & heightMap
					, TextureConfigData const & textureConfig )
				{
					IF( m_writer, textureConfig.hgtEnbl == 0.0_f )
					{
						m_writer.returnStmt( 1.0_f );
					}
					FI;

					auto shadowMultiplier = m_writer.declLocale( "shadowMultiplier"
						, 1.0_f );
					auto minLayers = m_writer.declLocale( "minLayers"
						, 10.0_f );
					auto maxLayers = m_writer.declLocale( "maxLayers"
						, 20.0_f );

					// calculate lighting only for surface oriented to the light source
					IF( m_writer, dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) > 0.0_f )
					{
						// calculate initial parameters
						auto numSamplesUnderSurface = m_writer.declLocale( "numSamplesUnderSurface"
							, 0.0_f );
						shadowMultiplier = 0.0_f;
						auto numLayers = m_writer.declLocale( "numLayers"
							, mix( maxLayers
								, minLayers
								, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) ) ) );
						auto layerHeight = m_writer.declLocale( "layerHeight"
							, initialHeight / numLayers );
						auto texStep = m_writer.declLocale( "deltaTexCoords"
							, ( lightDir.xy() * textureConfig.hgtFact ) / lightDir.z() / numLayers );

						// current parameters
						auto currentLayerHeight = m_writer.declLocale( "currentLayerHeight"
							, initialHeight - layerHeight );
						auto currentTextureCoords = m_writer.declLocale( "currentTextureCoords"
							, initialTexCoord + texStep );
						auto heightIndex = m_writer.declLocale( "heightIndex"
							, m_writer.cast< sdw::UInt >( textureConfig.hgtMask ) );
						auto sampled = m_writer.declLocale( "sampled"
							, heightMap.sample( currentTextureCoords ) );
						auto heightFromTexture = m_writer.declLocale( "heightFromTexture"
							, sampled[heightIndex] );
						auto stepIndex = m_writer.declLocale( "stepIndex"
							, 1_i );

						// while point is below depth 0.0 )
						WHILE( m_writer, currentLayerHeight > 0.0_f )
						{
							// if point is under the surface
							IF( m_writer, heightFromTexture < currentLayerHeight )
							{
								// calculate partial shadowing factor
								numSamplesUnderSurface += 1.0_f;
								auto newShadowMultiplier = m_writer.declLocale( "newShadowMultiplier"
									, ( currentLayerHeight - heightFromTexture )
									* ( 1.0_f - m_writer.cast< sdw::Float >( stepIndex ) / numLayers ) );
								shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
							}
							FI;

							// offset to the next layer
							stepIndex += 1_i;
							currentLayerHeight -= layerHeight;
							currentTextureCoords += texStep;
							sampled = heightMap.sample( currentTextureCoords );
							heightFromTexture = sampled[heightIndex];
						}
						ELIHW;

						// Shadowing factor should be 1 if there were no points under the surface
						IF( m_writer, numSamplesUnderSurface < 1.0_f )
						{
							shadowMultiplier = 1.0_f;
						}
						ELSE
						{
							shadowMultiplier = 1.0_f - shadowMultiplier;
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadowMultiplier );
				}
				, sdw::InVec3{ m_writer, "lightDir" }
				, sdw::InVec2{ m_writer, "initialTexCoord" }
				, sdw::InFloat{ m_writer, "initialHeight" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "heightMap" }
				, InTextureConfigData{ m_writer, "textureConfig" } );
		}

		return m_parallaxShadow( plightDir
			, pinitialTexCoord
			, pinitialHeight
			, pheightMap
			, ptextureConfig );
	}

	sdw::RetVec4 Utils::clipToScreen( sdw::Vec4 const & pin )
	{
		if ( !m_clipToScreen )
		{
			m_clipToScreen = m_writer.implementFunction< sdw::Vec4 >( "c3d_clipToScreen"
				, [&]( sdw::Vec4 const & in )
				{
					auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );
					auto out = m_writer.declLocale( "out", in );

					IF( m_writer, abs( out.w() ) < epsilon )
					{
						out.w() = epsilon;
					}
					FI;

					out.xyz() /= out.w();
					out.xyz() = fma( out.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) );
					m_writer.returnStmt( out );
				}
				, sdw::InVec4{ m_writer, "in" } );
		}

		return m_clipToScreen( pin );
	}

	sdw::RetBoolean Utils::isSaturated( sdw::Vec3 const & pp )
	{
		if ( !m_isSaturated3D )
		{
			m_isSaturated3D = m_writer.implementFunction< sdw::Boolean >( "c3d_isSaturated"
				, [&]( sdw::Vec3 const & p )
				{
					m_writer.returnStmt( p.x() == clamp( p.x(), 0.0_f, 1.0_f )
						&& p.y() == clamp( p.y(), 0.0_f, 1.0_f )
						&& p.z() == clamp( p.z(), 0.0_f, 1.0_f ) );
				}
				, sdw::InVec3{ m_writer, "p" } );
		}

		return m_isSaturated3D( pp );
	}

	sdw::RetBoolean Utils::isSaturated( sdw::IVec3 const & pp
		, sdw::Int const & pimax )
	{
		if ( !m_isSaturated3DImg )
		{
			m_isSaturated3DImg = m_writer.implementFunction< sdw::Boolean >( "c3d_isSaturatedImg"
				, [&]( sdw::IVec3 const & p
					, sdw::Int const & imax )
				{
					m_writer.returnStmt( p.x() == clamp( p.x(), 0_i, imax )
						&& p.y() == clamp( p.y(), 0_i, imax )
						&& p.z() == clamp( p.z(), 0_i, imax ) );
				}
				, sdw::InIVec3{ m_writer, "p" }
				, sdw::InInt{ m_writer, "imax" } );
		}

		return m_isSaturated3DImg( pp, pimax );
	}

	sdw::RetUInt Utils::encodeColor( sdw::Vec4 const & pcolor )
	{
		if ( !m_encodeColor )
		{
			m_encodeColor = m_writer.implementFunction<  sdw::UInt >( "c3d_encodeColor"
				, [&]( sdw::Vec4 const & color )
				{
					auto hdrRange = m_writer.declConstant( "hdrRange", 10.0_f );

					// normalize color to LDR
					auto hdr = m_writer.declLocale( "hdr"
						, length( color.rgb() ) );
					color.rgb() /= hdr;

					// encode LDR color and HDR range
					auto iColor = m_writer.declLocale( "iColor"
						, uvec3( color.rgb() * 255.0_f ) );
					auto iHDR = m_writer.declLocale( "iHDR"
						, m_writer.cast<  sdw::UInt >( clamp( hdr / hdrRange, 0.0_f, 1.0_f ) * 127.0_f ) );
					auto colorMask = m_writer.declLocale( "colorMask"
						, ( iHDR << 24_u ) | ( iColor.r() << 16_u ) | ( iColor.g() << 8_u ) | iColor.b() );

					// encode alpha into highest bit
					auto iAlpha = m_writer.declLocale( "iAlpha"
						, m_writer.ternary( color.a() > 0.0_f, 1_u, 0_u ) );
					colorMask |= ( iAlpha << 31_u );

					m_writer.returnStmt( colorMask );
				}
				, sdw::InVec4{ m_writer, "color" } );
		}

		return m_encodeColor( pcolor );
	}

	sdw::RetUInt Utils::encodeNormal( sdw::Vec3 const & pnormal )
	{
		if ( !m_encodeNormal )
		{
			m_encodeNormal = m_writer.implementFunction<  sdw::UInt >( "c3d_encodeNormal"
				, [&]( sdw::Vec3 const & normal )
				{
					auto iNormal = m_writer.declLocale( "iNormal"
						, ivec3( normal * 255.0_f ) );
					auto uNormalSigns = m_writer.declLocale< sdw::UVec3 >( "uNormalSigns"
						, uvec3( m_writer.cast<  sdw::UInt >( ( iNormal.x() >> 5 ) & 0x04000000 )
							, m_writer.cast<  sdw::UInt >( ( iNormal.y() >> 14 ) & 0x00020000 )
							, m_writer.cast<  sdw::UInt >( ( iNormal.z() >> 23 ) & 0x00000100 ) ) );
					auto uNormal = m_writer.declLocale( "uNormal"
						, uvec3( abs( iNormal ) ) );
					auto normalMask = m_writer.declLocale( "normalMask"
						, ( uNormalSigns.x() | ( uNormal.x() << 18_u )
							| uNormalSigns.y() | ( uNormal.y() << 9_u )
							| uNormalSigns.z() | uNormal.z() ) );
					m_writer.returnStmt( normalMask );
				}
				, sdw::InVec3{ m_writer, "normal" } );
		}

		return m_encodeNormal( pnormal );
	}

	sdw::RetVec4 Utils::decodeColor( sdw::UInt const & pcolorMask )
	{
		if ( !m_decodeColor )
		{
			m_decodeColor = m_writer.implementFunction< sdw::Vec4 >( "c3d_decodeColor"
				, [&]( sdw::UInt const & colorMask )
				{
					auto hdrRange = m_writer.declConstant( "hdrRange", 10.0_f );

					auto color = m_writer.declLocale< sdw::Vec4 >( "color" );
					auto hdr = m_writer.declLocale( "hdr"
						, m_writer.cast< sdw::Float >( ( colorMask >> 24u ) & 0x0000007f_u ) );
					color.r() = m_writer.cast< sdw::Float >( ( colorMask >> 16u ) & 0x000000ff_u );
					color.g() = m_writer.cast< sdw::Float >( ( colorMask >> 8u ) & 0x000000ff_u );
					color.b() = m_writer.cast< sdw::Float >( colorMask & 0x000000ff_u );

					hdr /= 127.0_f;
					color.rgb() /= vec3( 255.0_f );

					color.rgb() *= hdr * hdrRange;

					color.a() = m_writer.cast< sdw::Float >( ( colorMask >> 31u ) & 0x00000001_u );

					m_writer.returnStmt( color );
				}
				, sdw::InUInt{ m_writer, "colorMask" } );
		}

		return m_decodeColor( pcolorMask );
	}

	sdw::RetVec3 Utils::decodeNormal( sdw::UInt const & pnormalMask )
	{
		if ( !m_decodeNormal )
		{
			m_decodeNormal = m_writer.implementFunction< sdw::Vec3 >( "c3d_decodeNormal"
				, [&]( sdw::UInt const & normalMask )
				{
					auto iNormal = m_writer.declLocale( "iNormal"
						, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 18_u ) & 0x000000ff_u )
							, m_writer.cast< sdw::Int >( ( normalMask >> 9_u ) & 0x000000ff_u )
							, m_writer.cast< sdw::Int >( normalMask & 0x000000ff_u ) ) );
					auto iNormalSigns = m_writer.declLocale( "iNormalSigns"
						, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 25_u ) & 0x00000002_u )
							, m_writer.cast< sdw::Int >( ( normalMask >> 16_u ) & 0x00000002_u )
							, m_writer.cast< sdw::Int >( ( normalMask >> 7_u ) & 0x00000002_u ) ) );
					iNormalSigns = 1_i - iNormalSigns;
					auto normal = m_writer.declLocale( "normal"
						, vec3( iNormal ) / 255.0f );
					normal *= vec3( iNormalSigns );
					m_writer.returnStmt( normal );
				}
				, sdw::InUInt{ m_writer, "normalMask" } );
		}

		return m_decodeNormal( pnormalMask );
	}

	sdw::RetVec4 Utils::encodeFloatRGBA( sdw::Float const & pv )
	{
		if ( !m_encodeFloatRGBA )
		{
			m_encodeFloatRGBA = m_writer.implementFunction< sdw::Vec4 >( "c3d_encodeFloatRGBA"
				, [&]( sdw::Float const & v )
			{
				auto bitEnc = vec4( 1.0_f, 255.0_f, 65025.0_f, 16581375.0_f );
				auto enc = m_writer.declLocale( "enc"
					, bitEnc * v );
				enc = fract( enc );
				enc -= vec4( enc.yz(), enc.ww() ) * vec4( 1.0_f / 255.0_f, 0.0_f, 0.0_f, 0.0_f );
				m_writer.returnStmt( enc );
			}
			, sdw::InFloat{ m_writer, "v" } );
		}

		return m_encodeFloatRGBA( pv );
	}

	sdw::RetUInt Utils::flatten( sdw::UVec3 const & pp
		, sdw::UVec3 const & pdim )
	{
		if ( !m_flatten3D )
		{
			m_flatten3D = m_writer.implementFunction<  sdw::UInt >( "c3d_flatten3D"
				, [&]( sdw::UVec3 const & coord
					, sdw::UVec3 const & dim )
				{
					m_writer.returnStmt( ( coord.z() * dim.x() * dim.y() ) + ( coord.y() * dim.x() ) + coord.x() );
				}
				, sdw::InUVec3{ m_writer, "coord" }
				, sdw::InUVec3{ m_writer, "dim" } );
		}

		return m_flatten3D( pp, pdim );
	}

	sdw::RetUVec3 Utils::unflatten( sdw::UInt const & pp
		, sdw::UVec3 const & pdim )
	{
		if ( !m_unflatten3D )
		{
			m_unflatten3D = m_writer.implementFunction< sdw::UVec3 >( "c3d_unflatten3D"
				, [&]( sdw::UInt idx
					, sdw::UVec3 const & dim )
				{
					auto z = m_writer.declLocale( "z"
						, idx / ( dim.x() * dim.y() ) );
					idx -= ( z * dim.x() * dim.y() );
					auto y = m_writer.declLocale( "y"
						, idx / dim.x() );
					auto x = m_writer.declLocale( "x"
						, idx % dim.x() );
					m_writer.returnStmt( uvec3( x, y, z ) );
				}
				, sdw::InUInt{ m_writer, "idx" }
				, sdw::InUVec3{ m_writer, "dim" } );
		}

		return m_unflatten3D( pp, pdim );
	}

	sdw::Mat3 Utils::getTBN( sdw::Vec3 const & normal
		, sdw::Vec3 const & tangent
		, sdw::Vec3 const & bitangent )
	{
		return mat3( normalize( tangent ), normalize( bitangent ), normal );
	}

	bool Utils::isGeometryMap( TextureFlags const & flags
		, PassFlags const & passFlags )
	{
		return checkFlag( flags, TextureFlag::eOpacity )
			|| checkFlag( flags, TextureFlag::eNormal )
			|| ( checkFlag( flags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) );
	}

	bool Utils::isGeometryOnlyMap( TextureFlags const & flags
		, PassFlags const & passFlags )
	{
		return flags == TextureFlag::eOpacity
			|| flags == TextureFlag::eNormal
			|| flags == TextureFlag::eHeight
			|| flags == ( TextureFlag::eNormal | TextureFlag::eHeight );
	}

	sdw::RetVec4 Utils::sampleUntiled( sdw::CombinedImage2DRgba32 const & pmap
		, sdw::Vec2 const & ptexCoords
		, sdw::Vec2 const & pddx
		, sdw::Vec2 const & pddy )
	{
		if ( !m_hash4 )
		{
			m_hash4 = m_writer.implementFunction< sdw::Vec4 >( "hash4"
				, [&]( sdw::Vec2 const & p )
				{
					m_writer.returnStmt( fract( sin( vec4( 1.0_f + dot( p, vec2( 37.0_f, 17.0_f ) )
						, 2.0_f + dot( p, vec2( 11.0_f, 47.0_f ) )
						, 3.0_f + dot( p, vec2( 41.0_f, 29.0_f ) )
						, 4.0_f + dot( p, vec2( 23.0_f, 31.0_f ) ) ) ) * 103.0_f ) );
				}
				, sdw::InVec2{ m_writer, "p" } );
		}

		if ( !m_sampleUntiled )
		{
			m_sampleUntiled = m_writer.implementFunction< sdw::Vec4 >( "sampleUntiled"
				, [&]( sdw::CombinedImage2DRgba32 const & map
					, sdw::Vec2 const & texCoords
					, sdw::Vec2 const & ddx
					, sdw::Vec2 const & ddy )
				{
					auto iuv = m_writer.declLocale( "iuv", floor( texCoords ) );
					auto fuv = m_writer.declLocale( "fuv", fract( texCoords ) );

					// generate per-tile transform
					auto ofa = m_writer.declLocale( "ofa", m_hash4( iuv + vec2( 0.0_f, 0.0_f ) ) );
					auto ofb = m_writer.declLocale( "ofb", m_hash4( iuv + vec2( 1.0_f, 0.0_f ) ) );
					auto ofc = m_writer.declLocale( "ofc", m_hash4( iuv + vec2( 0.0_f, 1.0_f ) ) );
					auto ofd = m_writer.declLocale( "ofd", m_hash4( iuv + vec2( 1.0_f, 1.0_f ) ) );

					// transform per-tile uvs
					ofa.zw() = sign( ofa.zw() - 0.5_f );
					ofb.zw() = sign( ofb.zw() - 0.5_f );
					ofc.zw() = sign( ofc.zw() - 0.5_f );
					ofd.zw() = sign( ofd.zw() - 0.5_f );

					// uv's, and derivarives (for correct mipmapping)
					auto uva = m_writer.declLocale( "uva", texCoords * ofa.zw() + ofa.xy() );
					auto ddxa = m_writer.declLocale( "ddxa", ddx * ofa.zw() );
					auto ddya = m_writer.declLocale( "ddya", ddy * ofa.zw() );
					auto uvb = m_writer.declLocale( "uvb", texCoords * ofb.zw() + ofb.xy() );
					auto ddxb = m_writer.declLocale( "ddxb", ddx * ofb.zw() );
					auto ddyb = m_writer.declLocale( "ddyb", ddy * ofb.zw() );
					auto uvc = m_writer.declLocale( "uvc", texCoords * ofc.zw() + ofc.xy() );
					auto ddxc = m_writer.declLocale( "ddxc", ddx * ofc.zw() );
					auto ddyc = m_writer.declLocale( "ddyc", ddy * ofc.zw() );
					auto uvd = m_writer.declLocale( "uvd", texCoords * ofd.zw() + ofd.xy() );
					auto ddxd = m_writer.declLocale( "ddxd", ddx * ofd.zw() );
					auto ddyd = m_writer.declLocale( "ddyd", ddy * ofd.zw() );

					// fetch and blend
					auto b = m_writer.declLocale( "b", smoothStep( vec2( 0.25_f ), vec2( 0.75_f ), fuv ) );

					m_writer.returnStmt( mix( mix( map.grad( uva, ddxa, ddya ), map.grad( uvb, ddxb, ddyb ), vec4( b.x() ) )
						, mix( map.grad( uvc, ddxc, ddyc ), map.grad( uvd, ddxd, ddyd ), vec4( b.x() ) )
						, vec4( b.y() ) ) );
				}
				, sdw::InCombinedImage2DRgba32{ m_writer, "map" }
				, sdw::InVec2{ m_writer, "texCoords" }
				, sdw::InVec2{ m_writer, "ddx" }
				, sdw::InVec2{ m_writer, "ddy" } );
		}

		return m_sampleUntiled( pmap, ptexCoords, pddx, pddy );
	}

	sdw::RetVec4 Utils::sampleUntiled( sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec2 const & texCoords )
	{
		return sampleUntiled( map
			, texCoords
			, dFdx( texCoords )
			, dFdy( texCoords ) );
	}

	sdw::RetVec4 Utils::sampleUntiled( sdw::CombinedImage2DRgba32 const & map
		, DerivTex const & texCoords )
	{
		return sampleUntiled( map
			, texCoords.uv()
			, texCoords.dx()
			, texCoords.dy() );
	}
}
