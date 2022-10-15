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
	//*********************************************************************************************

	sdw::Float DerivTex::computeMip( sdw::Vec2 const & texSize )const
	{
		// see https://registry.khronos.org/OpenGL/extensions/EXT/EXT_shader_texture_lod.txt
		auto dSdx = dPdx().x();
		auto dSdy = dPdy().x();
		auto dTdx = dPdx().y();
		auto dTdy = dPdy().y();
		auto dUdx = texSize.x() * dSdx;
		auto dUdy = texSize.x() * dSdy;
		auto dVdx = texSize.y() * dTdx;
		auto dVdy = texSize.y() * dTdy;
		return 0.5_f * log2( max( dUdx * dUdx + dVdx * dVdx
			, dUdy * dUdy + dVdy * dVdy ) );
	}

	//*********************************************************************************************

	Utils::Utils( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	LightingModelPtr Utils::createLightingModel( Engine const & engine
		, Materials const & materials
		, castor::String const & name
		, ShadowOptions shadowsOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return engine.getLightingModelFactory().create( name
			, m_writer
			, materials
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

	sdw::Vec4 Utils::sampleMap( sdw::CombinedImage2DRgba32 const map
		, sdw::Vec2 const texCoords )
	{
		return map.sample( texCoords );
	}

	sdw::Vec4 Utils::sampleMap( sdw::CombinedImage2DRgba32 const map
		, sdw::Vec3 const texCoords )
	{
		return map.sample( texCoords.xy() );
	}

	sdw::Vec4 Utils::sampleMap( sdw::CombinedImage2DRgba32 const map
		, DerivTex const texCoords )
	{
		return map.grad( texCoords.uv(), texCoords.dPdx(), texCoords.dPdy() );
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
					uv = scaleUV( config.scale().xy(), uv );
					uv = rotateUV( config.rotate().xy(), uv );
					uv = translateUV( config.translate().xy(), uv );

					IF( m_writer, config.isTrnfAnim() )
					{
						uv = vec2( uv.x()
							, mix( uv.y(), 1.0_f - uv.y(), config.fneedYI ) );
						uv = scaleUV( anim.texScl.xy(), uv );
						uv = rotateUV( anim.texRot.xy(), uv );
						uv = translateUV( anim.texTrn.xy(), uv );
					}
					FI;

					uv.x() = ( uv.x() + config.tileSet().x() ) / config.tileSet().z();
					uv.y() = ( uv.y() + config.tileSet().y() ) / config.tileSet().w();

					IF( m_writer, config.isTileAnim() )
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
					uvw = scaleUV( config.scale().xyz(), uvw );
					uvw = rotateUV( config.rotate().xyz(), uvw );
					uvw = translateUV( config.translate().xyz(), uvw );

					IF( m_writer, config.isTrnfAnim() )
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
		, sdw::UInt const & paccumulationOperator )
	{
		if ( !m_computeAccumulation )
		{
			m_computeAccumulation = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeAccumulation"
				, [&]( sdw::Float const & depth
					, sdw::Vec3 const & colour
					, sdw::Float const & alpha
					, sdw::Float const & nearPlane
					, sdw::Float const & farPlane
					, sdw::UInt const & accumulationOperator )
				{
					auto weight = m_writer.declLocale< sdw::Float >( "weight"
						, 1.0_f );

					SWITCH( m_writer, accumulationOperator )
					{
						CASE( 0u )
						{
							// Naive
							weight = 1.0_f - rescaleDepth( depth
								, nearPlane
								, farPlane );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 1u )
						{
							// (10)
							weight = max( pow( clamp( 1.0_f - depth, 0.0_f, 1.0_f ), 3.0_f ) * 3000.0_f, 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 2u )
						{
							// (9)
							weight = max( min( 0.03_f / ( pow( abs( depth ) / 200.0_f, 4.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 3u )
						{
							// (8)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 10.0_f, 3.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 4u )
						{
							// (7)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 5.0_f, 2.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 5u )
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
						CASE( 6u )
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
						CASE( 7u )
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
				, sdw::InUInt{ m_writer, "accumulationOperator" } );
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
					sdw::Float f90 = clamp( dot( f0, vec3( 50.0_f * 0.33_f ) ), 0.0_f, 1.0_f );
					m_writer.returnStmt( sdw::fma( max( vec3( f90 ), f0 ) - f0
						, vec3( pow( clamp( 1.0_f - product, 0.0_f, 1.0_f ), 5.0_f ) )
						, f0 ) );
				}
				, sdw::InFloat{ m_writer, "product" }
				, sdw::InVec3{ m_writer, "f0" } );
		}

		return m_fresnelSchlick( pproduct, pf0 );
	}

	sdw::RetVec4 Utils::clipToScreen( sdw::Vec4 const & pin )
	{
		if ( !m_clipToScreen )
		{
			m_clipToScreen = m_writer.implementFunction< sdw::Vec4 >( "c3d_clipToScreen"
				, [&]( sdw::Vec4 const & in )
				{
					auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );
					auto result = m_writer.declLocale( "result", in );

					IF( m_writer, abs( result.w() ) < epsilon )
					{
						result.w() = epsilon;
					}
					FI;

					result.xyz() /= result.w();
					result.xyz() = fma( result.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) );
					m_writer.returnStmt( result );
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

	//*********************************************************************************************
}
