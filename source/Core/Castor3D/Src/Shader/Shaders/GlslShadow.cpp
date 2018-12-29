#include "GlslShadow.hpp"

#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//***********************************************************************************************

		ast::expr::ExprList makeFnArg( sdw::Shader & shader
			, OutputComponents const & value )
		{
			ast::expr::ExprList result;
			auto args = sdw::makeFnArg( shader, value.m_diffuse );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( shader, value.m_specular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			return result;
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( ShaderWriter & writer )
			: OutputComponents{ { writer, "outDiffuse" }, { writer, "outSpecular" } }
		{
		}

		OutputComponents::OutputComponents( InOutVec3 const & diffuse
			, InOutVec3 const & specular )
			: m_diffuse{ diffuse }
			, m_specular{ specular }
			, m_expr{ expr::makeComma( makeExpr( m_diffuse ), makeExpr( m_specular ) ) }
		{
		}

		ast::expr::Expr * OutputComponents::getExpr()const
		{
			return m_expr.get();
		}

		sdw::Shader * OutputComponents::getShader()const
		{
			return findShader( m_diffuse, m_specular );
		}

		//***********************************************************************************************

		auto constexpr minRawDirectionalOffset = 0.001f;
		auto constexpr maxRawDirectionalSlopeOffset = 0.001f;
		auto constexpr minPcfDirectionalOffset = 0.001f;
		auto constexpr maxPcfDirectionalSlopeOffset = 0.001f;
		auto constexpr minSpotOffset = 0.00001f;
		auto constexpr maxSpotSlopeOffset = 0.0001f;
		auto constexpr minPointOffset = 0.00001f;
		auto constexpr maxPointSlopeOffset = 0.0001f;
		auto constexpr varianceDirectionalBias = 0.15f;
		auto constexpr varianceDirectionalMin = 0.000001f;
		auto constexpr varianceSpotBias = 0.15f;
		auto constexpr varianceSpotMin = 0.000001f;
		auto constexpr variancePointBias = 0.15f;
		auto constexpr variancePointMin = 0.000001f;

		castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
		castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
		castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );
		castor::String const Shadow::MapDepthDirectional = cuT( "c3d_mapDepthDirectional" );
		castor::String const Shadow::MapDepthSpot = cuT( "c3d_mapDepthSpot" );
		castor::String const Shadow::MapDepthPoint = cuT( "c3d_mapDepthPoint" );

		Shadow::Shadow( ShaderWriter & writer
			, bool isZeroToOneDepth )
			: m_writer{ writer }
			, m_isZeroToOneDepth{ isZeroToOneDepth }
		{
		}

		void Shadow::declare( uint32_t & index
			, uint32_t maxCascades )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			auto c3d_maxCascadeCount = m_writer.declConstant( "c3d_maxCascadeCount"
				, UInt( DirectionalMaxCascadesCount ) );
			auto c3d_mapShadowDirectional = m_writer.declSampledImage< FImg2DArrayRgba32 >( MapShadowDirectional, index++, 1u );
			auto c3d_mapShadowSpot = m_writer.declSampledImage< FImg2DArrayRgba32 >( MapShadowSpot, index++, 1u );
			auto c3d_mapShadowPoint = m_writer.declSampledImage< FImgCubeArrayRgba32 >( MapShadowPoint, index++, 1u );
			doDeclareGetRandom();
			doDeclareTextureProj();
			doDeclareFilterPCF();
			doDeclareTextureProjCascade();
			doDeclareFilterPCFCascade();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareGetLightSpacePosition();
			doDeclareComputeDirectionalShadow();
			doDeclareComputeSpotShadow();
			doDeclareComputePointShadow();
			doDeclareVolumetric();
		}

		void Shadow::declareDirectional( ShadowType type
			, uint32_t & index
			, uint32_t maxCascades )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			auto c3d_maxCascadeCount = m_writer.declConstant( "c3d_maxCascadeCount"
				, UInt( DirectionalMaxCascadesCount ) );
			auto c3d_mapShadowDirectional = m_writer.declSampledImage< FImg2DArrayRgba32 >( MapShadowDirectional, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureProjCascade();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureProjCascade();
				doDeclareFilterPCFCascade();
				break;

			case ShadowType::eVariance:
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareGetLightSpacePosition();
			doDeclareComputeOneDirectionalShadow( type );
			doDeclareOneVolumetric( type );
		}

		void Shadow::declarePoint( ShadowType type
			, uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			auto c3d_mapShadowPoint = m_writer.declSampledImage< FImgCubeRgba32 >( MapShadowPoint, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				doDeclareFilterOnePCF();
				break;

			case ShadowType::eVariance:
				doDeclareGetShadowOffset();
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareComputeOnePointShadow( type );
		}

		void Shadow::declareSpot( ShadowType type
			, uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			auto c3d_mapShadowSpot = m_writer.declSampledImage< FImg2DRgba32 >( MapShadowSpot, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				doDeclareFilterOnePCF();
				break;

			case ShadowType::eVariance:
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareGetLightSpacePosition();
			doDeclareComputeOneSpotShadow( type );
		}

		Float Shadow::computeDirectionalShadow( Int const & shadowType
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, Vec3 const & normal )const
		{
			return m_computeDirectional( shadowType
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, cascadeIndex
				, normal );
		}

		Float Shadow::computeSpotShadow( Int const & shadowType
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, Int const & index )const
		{
			return m_computeSpot( shadowType
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal
				, index );
		}

		Float Shadow::computePointShadow( Int const & shadowType
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, Float const & farPlane
			, Int const & index )const
		{
			return m_computePoint( shadowType
				, worldSpacePosition
				, lightDirection
				, normal
				, farPlane
				, index );
		}

		void Shadow::computeVolumetric( Int const & shadowType
			, Vec2 const & clipSpacePosition
			, Vec3 const & worldSpacePosition
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, Vec3 const & lightColour
			, Vec2 const & lightIntensity
			, UInt const & lightVolumetricSteps
			, Float const & lightVolumetricScattering
			, OutputComponents & parentOutput )const
		{
			m_computeVolumetric( shadowType
				, clipSpacePosition
				, worldSpacePosition
				, eyePosition
				, lightMatrix
				, lightDirection
				, cascadeIndex
				, lightColour
				, lightIntensity
				, lightVolumetricSteps
				, lightVolumetricScattering
				, parentOutput );
		}

		Vec4 Shadow::getLightSpacePosition( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition )const
		{
			return m_getLightSpacePosition( lightMatrix
				, worldSpacePosition );
		}

		Float Shadow::computeDirectionalShadow( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, Vec3 const & normal )const
		{
			return m_computeOneDirectional( lightMatrix
				, worldSpacePosition
				, lightDirection
				, cascadeIndex
				, normal );
		}

		Float Shadow::computeSpotShadow( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal )const
		{
			return m_computeOneSpot( lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal );
		}

		Float Shadow::computePointShadow( Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, sdw::Float const & farPlane )const
		{
			return m_computeOnePoint( worldSpacePosition
				, lightDirection
				, normal
				, farPlane );
		}

		void Shadow::computeVolumetric( Vec2 const & clipSpacePosition
			, Vec3 const & worldSpacePosition
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, Vec3 const & lightColour
			, Vec2 const & lightIntensity
			, UInt const & lightVolumetricSteps
			, Float const & lightVolumetricScattering
			, OutputComponents & parentOutput )const
		{
			m_computeOneVolumetric( clipSpacePosition
				, worldSpacePosition
				, eyePosition
				, lightMatrix
				, lightDirection
				, cascadeIndex
				, lightColour
				, lightIntensity
				, lightVolumetricSteps
				, lightVolumetricScattering
				, parentOutput );
		}

		Float Shadow::chebyshevUpperBound( Vec2 const & moments
			, Float const & distance
			, Float const & maxVariance
			, Float const & varianceBias )const
		{
			return m_chebyshevUpperBound( moments
				, distance
				, maxVariance
				, varianceBias );
		}

		Float Shadow::getShadowOffset( Vec3 const & normal
			, Vec3 const & lightDirection
			, Float const & minOffset
			, Float const & maxSlopeOffset )const
		{
			return m_getShadowOffset( normal
				, lightDirection
				, minOffset
				, maxSlopeOffset );
		}

		Float Shadow::textureProj( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, SampledImage2DArrayRgba32 const & shadowMap
			, Int const & index
			, Float const & bias )const
		{
			return m_textureProj( lightSpacePosition
				, offset
				, shadowMap
				, index
				, bias );
		}

		Float Shadow::filterPCF( Vec4 const & lightSpacePosition
			, SampledImage2DArrayRgba32 const & shadowMap
			, Int const & index
			, sdw::Vec2 const & invTexDim
			, Float const & bias )const
		{
			return m_filterPCF( lightSpacePosition
				, shadowMap
				, index
				, invTexDim
				, bias );
		}

		Float Shadow::textureProj( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, SampledImage2DRgba32 const & shadowMap
			, Float const & bias )const
		{
			return m_textureOneProj( lightSpacePosition
				, offset
				, shadowMap
				, bias );
		}

		Float Shadow::filterPCF( Vec4 const & lightSpacePosition
			, SampledImage2DRgba32 const & shadowMap
			, sdw::Vec2 const & invTexDim
			, Float const & bias )const
		{
			return m_filterOnePCF( lightSpacePosition
				, shadowMap
				, invTexDim
				, bias );
		}

		Float Shadow::textureProjCascade( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, SampledImage2DArrayRgba32 const & shadowMap
			, UInt const & cascadeIndex
			, Float const & bias )const
		{
			return m_textureProjCascade( lightSpacePosition
				, offset
				, shadowMap
				, cascadeIndex
				, bias );
		}

		Float Shadow::filterPCFCascade( Vec4 const & lightSpacePosition
			, SampledImage2DArrayRgba32 const & shadowMap
			, sdw::Vec2 const & invTexDim
			, UInt const & cascadeIndex
			, Float const & bias )const
		{
			return m_filterPCFCascade( lightSpacePosition
				, shadowMap
				, invTexDim
				, cascadeIndex
				, bias );
		}

		void Shadow::doDeclareGetRandom()
		{
			m_getRandom = m_writer.implementFunction< Float >( "getRandom"
				, [this]( Vec4 const & seed )
				{
					auto p = m_writer.declLocale( "p"
						, dot( seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
					m_writer.returnStmt( fract( sin( p ) * 43758.5453_f ) );
				}
				, InVec4( m_writer, "seed" ) );
		}

		void Shadow::doDeclareGetShadowOffset()
		{
			m_getShadowOffset = m_writer.implementFunction< Float >( "getShadowOffset"
				, [this]( Vec3 const & normal
					, Vec3 const & lightDirection
					, Float const & minOffset
					, Float const & maxSlopeOffset )
				{
					auto cosAlpha = m_writer.declLocale( "cosAlpha"
						, clamp( dot( normal, normalize( lightDirection ) ), 0.0_f, 1.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, sqrt( 1.0_f - cosAlpha ) );
					m_writer.returnStmt( minOffset + maxSlopeOffset * offset );
				}
				, InVec3( m_writer, "normal" )
				, InVec3( m_writer, "lightDirection" )
				, InFloat( m_writer, "minOffset" )
				, InFloat( m_writer, "maxSlopeOffset" ) );
		}

		void Shadow::doDeclareChebyshevUpperBound()
		{
			m_chebyshevUpperBound = m_writer.implementFunction< Float >( "chebyshevUpperBound"
				, [this]( Vec2 const & moments
					, Float const & distance
					, Float const & minVariance
					, Float const & varianceBias )
				{
					auto p = m_writer.declLocale( "p"
						, step( moments.x() + varianceBias, distance ) );
					auto variance = m_writer.declLocale( "variance"
						, moments.y() - m_writer.paren( moments.x() * moments.x() ) );
					variance = sdw::max( variance, minVariance );
					auto d = m_writer.declLocale( "d"
						, distance - moments.x() );
					variance /= variance + d * d;
					m_writer.returnStmt( sdw::max( p, variance ) );
				}
				, InVec2{ m_writer, "moments" }
				, InFloat{ m_writer, "distance" }
				, InFloat{ m_writer, "minVariance" }
				, InFloat{ m_writer, "varianceBias" } );
		}

		void Shadow::doDeclareTextureProj()
		{
			m_textureProj = m_writer.implementFunction< Float >( "textureProj"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, SampledImage2DArrayRgba32 const & shadowMap
					, Int const & index
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( "shadow"
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( "shadowCoord"
						, lightSpacePosition );

					IF( m_writer, abs( shadowCoord.z() ) < 1.0_f )
					{
						auto uv = m_writer.declLocale( "uv"
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( "dist"
							, texture( shadowMap, vec3( uv, m_writer.cast< Float >( index ) ) ).r() );

						IF( m_writer, shadowCoord.w() > 0 )
						{
							shadow = step( shadowCoord.z() - bias, dist );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InVec2{ m_writer, "offset" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCF()
		{
			m_filterPCF = m_writer.implementFunction< Float >( "filterPCF"
				, [this]( Vec4 const & lightSpacePosition
					, SampledImage2DArrayRgba32 const & shadowMap
					, Int const & index
					, Vec2 const & invTexDim
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( "scale"
						, 1.0_f );
					auto dx = m_writer.declLocale( "dx"
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( "dy"
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProj( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, index
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InVec2{ m_writer, "invTexDim" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareTextureOneProj()
		{
			m_textureOneProj = m_writer.implementFunction< Float >( "textureProj"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, SampledImage2DRgba32 const & shadowMap
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( "shadow"
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( "shadowCoord"
						, lightSpacePosition );

					IF( m_writer, abs( shadowCoord.z() ) < 1.0_f )
					{
						auto uv = m_writer.declLocale( "uv"
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( "dist"
							, texture( shadowMap, uv ).r() );

						IF( m_writer, shadowCoord.w() > 0 )
						{
							shadow = step( shadowCoord.z() - bias, dist );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InVec2{ m_writer, "offset" }
				, InSampledImage2DRgba32{ m_writer, "shadowMap" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterOnePCF()
		{
			m_filterOnePCF = m_writer.implementFunction< Float >( "filterPCF"
				, [this]( Vec4 const & lightSpacePosition
					, SampledImage2DRgba32 const & shadowMap
					, Vec2 const & invTexDim
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( "scale"
						, 1.0_f );
					auto dx = m_writer.declLocale( "dx"
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( "dy"
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProj( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InSampledImage2DRgba32{ m_writer, "shadowMap" }
				, InVec2{ m_writer, "invTexDim" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareTextureProjCascade()
		{
			m_textureProjCascade = m_writer.implementFunction< Float >( "textureProjCascade"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, SampledImage2DArrayRgba32 const & shadowMap
					, UInt const & cascadeIndex
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( "shadow"
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( "shadowCoord"
						, lightSpacePosition );

					IF( m_writer, shadowCoord.z() > -1.0_f && shadowCoord.z() < 1.0_f )
					{
						auto uv = m_writer.declLocale( "uv"
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( "dist"
							, texture( shadowMap, vec3( uv
								, m_writer.cast< Float >( cascadeIndex ) ) ).r() );

						IF( m_writer, shadowCoord.w() > 0 )
						{
							shadow = step( shadowCoord.z() - bias, dist );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InVec2{ m_writer, "offset" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCFCascade()
		{
			m_filterPCFCascade = m_writer.implementFunction< Float >( "filterPCFCascade"
				, [this]( Vec4 const & lightSpacePosition
					, SampledImage2DArrayRgba32 const & shadowMap
					, Vec2 const & invTexDim
					, UInt const & cascadeIndex
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( "scale"
						, 1.0_f );
					auto dx = m_writer.declLocale( "dx"
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( "dy"
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProjCascade( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, cascadeIndex
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InVec2{ m_writer, "invTexDim" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareGetLightSpacePosition()
		{
			m_getLightSpacePosition = m_writer.implementFunction< Vec4 >( "getLightSpacePosition"
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition )
				{
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
					auto projCoords = m_writer.declLocale( "projCoords"
						, lightSpacePosition );
					//projCoords.x() = sdw::fma( projCoords.x(), 0.5_f, 0.5_f );
					//projCoords.y() = sdw::fma( projCoords.y(), 0.5_f, 0.5_f );

					if ( !m_isZeroToOneDepth )
					{
						projCoords.z() = sdw::fma( projCoords.z(), 0.5_f, 0.5_f );
					}

					m_writer.returnStmt( projCoords / lightSpacePosition.w() );
				}
				, InMat4( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" ) );
		}

		void Shadow::doDeclareComputeDirectionalShadow()
		{
			m_computeDirectional = m_writer.implementFunction< Float >( "computeDirectionalShadow"
				, [this]( Int const & shadowType
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getVariable< SampledImage2DArrayRgba32 >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					auto result = m_writer.declLocale( "result"
						, 0.0_f );
					
					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( "offset"
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						result = filterPCFCascade( lightSpacePosition
							, c3d_mapShadowDirectional
							, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TextureSize ) ) )
							, cascadeIndex
							, bias );
					}
					ELSEIF( shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( "moments"
							, texture( c3d_mapShadowDirectional
								, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( cascadeIndex ) ) ).xy() );
						result = chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) );
					}
					ELSE
					{
						auto bias = m_writer.declLocale( "offset"
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						result = textureProjCascade( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, cascadeIndex
							, bias );
					}
					FI;
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "shadowType" )
				, InMat4( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightDirection" )
				, InUInt( m_writer, "cascadeIndex" )
				, InVec3( m_writer, "normal" ) );
		}

		void Shadow::doDeclareComputeSpotShadow()
		{
			m_computeSpot = m_writer.implementFunction< Float >( "computeSpotShadow"
				, [this]( Int const & shadowType
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal
					, Int const & index )
				{
					auto c3d_mapShadowSpot = m_writer.getVariable< SampledImage2DArrayRgba32 >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					lightSpacePosition.xy() += vec2( 0.5_f );
					auto result = m_writer.declLocale( "result"
						, 0.0_f );

					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						result = filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, index
							, vec2( Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) )
							, bias );
					}
					ELSEIF( shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( "moments"
							, texture( c3d_mapShadowSpot, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( index ) ) ).xy() );
						result = chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceSpotMin )
							, Float( varianceSpotBias ) );
					}
					ELSE
					{
						auto bias = m_writer.declLocale( "bias"
						, getShadowOffset( normal
							, lightDirection
							, Float( minSpotOffset )
							, Float( maxSpotSlopeOffset ) ) );
						result = textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot
							, index
							, bias );
					}
					FI;
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "shadowType" )
				, InMat4( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightDirection" )
				, InVec3( m_writer, "normal" )
				, InInt( m_writer, "index" ) );
		}

		void Shadow::doDeclareComputePointShadow()
		{
			m_computePoint = m_writer.implementFunction< Float >( "computePointShadow"
				, [this]( Int const & shadowType
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightPosition
					, Vec3 const & normal
					, Float const & farPlane
					, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.getVariable< SampledImageCubeArrayRgba32 >( Shadow::MapShadowPoint );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, worldSpacePosition - lightPosition );
					auto depth = m_writer.declLocale( "depth"
						, length( vertexToLight ) / farPlane );
					auto result = m_writer.declLocale( "result"
						, 0.0_f );

					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
							, 0.0_f );
						auto x = m_writer.declLocale( "x"
							, -offset );
						auto y = m_writer.declLocale( "y"
							, -offset );
						auto z = m_writer.declLocale( "z"
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5 ) );
						auto shadowMapDepth = m_writer.declLocale< Float >( "shadowMapDepth" );

						for( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									shadowMapDepth = texture( c3d_mapShadowPoint, vec4( vertexToLight + vec3( x, y, z ), m_writer.cast< Float >( index ) ) ).x();
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						result = shadowFactor / numSamplesUsed;
					}
					ELSEIF( shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
							, 0.0_f );
						auto x = m_writer.declLocale( "x"
							, -offset );
						auto y = m_writer.declLocale( "y"
							, -offset );
						auto z = m_writer.declLocale( "z"
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5 ) );
						auto moments = m_writer.declLocale< Vec2 >( "moments" );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									moments = texture( c3d_mapShadowPoint
										, vec4( vertexToLight + vec3( x, y, z ), m_writer.cast< Float >( index ) ) ).xy();
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						result = shadowFactor / numSamplesUsed;
					}
					ELSE
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
							, texture( c3d_mapShadowPoint, vec4( vertexToLight, m_writer.cast< Float >( index ) ) ).x() );
						result = step( depth - bias, shadowMapDepth );
					}
					FI;
					m_writer.returnStmt( result );
				}
				, InInt( m_writer, "shadowType" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightPosition" )
				, InVec3( m_writer, "normal" )
				, InFloat( m_writer, "farPlane" )
				, InInt( m_writer, "index" ) );
		}

		void Shadow::doDeclareVolumetric()
		{
			OutputComponents output{ m_writer };
			m_computeVolumetric = m_writer.implementFunction< sdw::Void >( "computeVolumetric"
				, [this]( Int const & shadowType
					, Vec2 const & clipSpacePosition
					, Vec3 const & worldSpacePosition
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, Vec3 const & lightColour
					, Vec2 const & lightIntensity
					, UInt const & lightVolumetricSteps
					, Float const & lightVolumetricScattering
					, OutputComponents & parentOutput )
				{
					auto volumetricDither = m_writer.declLocale< Mat4 >( "volumetricDither"
						, mat4(
							vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f ),
							vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f ),
							vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f ),
							vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) ) );

					auto rayVector = m_writer.declLocale( "rayVector"
						, worldSpacePosition - eyePosition );
					auto rayLength = m_writer.declLocale( "rayLength"
						, length( rayVector ) );
					auto rayDirection = m_writer.declLocale( "rayDirection"
						, rayVector / rayLength );
					auto stepLength = m_writer.declLocale( "stepLength"
						, rayLength / m_writer.cast< Float >( lightVolumetricSteps ) );
					auto step = m_writer.declLocale( "step"
						, rayDirection * stepLength );
					auto screenUV = m_writer.declLocale( "screenUV"
						, uvec2( m_writer.cast< UInt >( clipSpacePosition.x() )
							, m_writer.cast< UInt >( clipSpacePosition.y() ) ) );
					auto ditherValue = m_writer.declLocale( "ditherValue"
						, volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );

					auto currentPosition = m_writer.declLocale( "currentPosition"
						, eyePosition + step * ditherValue );
					auto volumetric = m_writer.declLocale( "volumetric"
						, 0.0_f );

					auto RdotL = m_writer.declLocale( "RdotL"
						, dot( rayDirection, lightDirection ) );
					auto sqVolumetricScattering = m_writer.declLocale( "sqVolumetricScattering"
						, lightVolumetricScattering * lightVolumetricScattering );
					auto dblVolumetricScattering = m_writer.declLocale( "dblVolumetricScattering"
						, 2.0_f * lightVolumetricScattering );
					auto oneMinusVolumeScattering = m_writer.declLocale( "oneMinusVolumeScattering"
						, 1.0_f - sqVolumetricScattering );
					auto scattering = m_writer.declLocale( "scattering"
						, oneMinusVolumeScattering / m_writer.paren( 4.0_f
							* Float{ Pi< float > }
							* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 0.0_f ), 1.5_f ) ) );
					auto maxCount = m_writer.declLocale( "maxCount"
						, m_writer.cast< Int >( lightVolumetricSteps ) );

					FOR( m_writer, Int, i, 0, i < maxCount, ++i )
					{
						IF ( m_writer, m_computeDirectional( shadowType, lightMatrix, currentPosition, lightDirection, cascadeIndex, vec3( 0.0_f ) ) < 0.5_f )
						{
							volumetric += scattering;
						}
						FI;

						currentPosition += step;
					}
					ROF;

					volumetric /= m_writer.cast< Float >( lightVolumetricSteps );
					parentOutput.m_diffuse += volumetric * lightIntensity.x() * 1.0_f * lightColour;
					parentOutput.m_specular += volumetric * lightIntensity.y() * 1.0_f * lightColour;
				}
				, InInt{ m_writer, "shadowType" }
				, InVec2{ m_writer, "clipSpacePosition" }
				, InVec3{ m_writer, "worldSpacePosition" }
				, InVec3{ m_writer, "eyePosition" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightDirection" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InVec3{ m_writer, "lightColour" }
				, InVec2{ m_writer, "lightIntensity" }
				, InUInt{ m_writer, "lightVolumetricSteps" }
				, InFloat{ m_writer, "lightVolumetricScattering" }
				, output );
		}

		void Shadow::doDeclareComputeOneDirectionalShadow( ShadowType type )
		{
			m_computeOneDirectional = m_writer.implementFunction< Float >( "computeDirectionalShadow"
				, [this, type]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getVariable< SampledImage2DArrayRgba32 >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( filterPCFCascade( lightSpacePosition
							, c3d_mapShadowDirectional
							, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TextureSize ) ) )
							, cascadeIndex
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( "moments"
							, texture( c3d_mapShadowDirectional
								, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( cascadeIndex ) ) ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) ) );
					}
					else
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( textureProjCascade( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, cascadeIndex
							, bias ) );
					}
				}
				, InParam< Mat4 >( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightDirection" )
				, InUInt( m_writer, "cascadeIndex" )
				, InVec3( m_writer, "normal" ) );
		}

		void Shadow::doDeclareComputeOneSpotShadow( ShadowType type )
		{
			m_computeOneSpot = m_writer.implementFunction< Float >( "computeSpotShadow"
				, [this, type]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal )
				{
					auto c3d_mapShadowSpot = m_writer.getVariable< SampledImage2DRgba32 >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					lightSpacePosition.xy() += vec2( 0.5_f );

					if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, vec2( Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) )
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( "moments"
							, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceSpotMin )
							, Float( varianceSpotBias ) ) );
					}
					else
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot
							, bias ) );
					}
				}
				, InParam< Mat4 >( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightDirection" )
				, InVec3( m_writer, "normal" ) );
		}

		void Shadow::doDeclareComputeOnePointShadow( ShadowType type )
		{
			m_computeOnePoint = m_writer.implementFunction< Float >( "computePointShadow"
				, [this, type]( Vec3 const & worldSpacePosition
					, Vec3 const & lightPosition
					, Vec3 const & normal
					, Float const & farPlane )
				{
					auto c3d_mapShadowPoint = m_writer.getVariable< SampledImageCubeRgba32 >( MapShadowPoint );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, worldSpacePosition - lightPosition );
					auto depth = m_writer.declLocale( "depth"
						, length( vertexToLight ) / farPlane );

					if( type == ShadowType::ePCF )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
							, 0.0_f );
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto x = m_writer.declLocale( "x"
							, -offset );
						auto y = m_writer.declLocale( "y"
							, -offset );
						auto z = m_writer.declLocale( "z"
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5 ) );
						auto shadowMapDepth = m_writer.declLocale< Float >( "shadowMapDepth" );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									shadowMapDepth = texture( c3d_mapShadowPoint, vertexToLight + vec3( x, y, z ) ).x();
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
							, 0.0_f );
						auto x = m_writer.declLocale( "x"
							, -offset );
						auto y = m_writer.declLocale( "y"
							, -offset );
						auto z = m_writer.declLocale( "z"
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5 ) );
						auto moments = m_writer.declLocale< Vec2 >( "moments" );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									moments = texture( c3d_mapShadowPoint
										, vertexToLight + vec3( x, y, z ) ).xy();
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					else
					{
						auto bias = m_writer.declLocale( "bias"
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
							, texture( c3d_mapShadowPoint, vec3( vertexToLight ) ).r() );
						m_writer.returnStmt( step( depth - bias, shadowMapDepth ) );
					}
				}
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightPosition" )
				, InVec3( m_writer, "normal" )
				, InFloat( m_writer, "farPlane" ) );
		}

		void Shadow::doDeclareOneVolumetric( ShadowType type )
		{
			OutputComponents output{ m_writer };
			m_computeOneVolumetric = m_writer.implementFunction< sdw::Void >( "computeVolumetric"
				, [this]( Vec2 const & clipSpacePosition
					, Vec3 const & worldSpacePosition
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, Vec3 const & lightColour
					, Vec2 const & lightIntensity
					, UInt const & lightVolumetricSteps
					, Float const & lightVolumetricScattering
					, OutputComponents & parentOutput )
				{
					auto volumetricDither = m_writer.declLocale< Mat4 >( "volumetricDither"
						, mat4(
							vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f ),
							vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f ),
							vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f ),
							vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) ) );

					auto rayVector = m_writer.declLocale( "rayVector"
						, worldSpacePosition - eyePosition );
					auto rayLength = m_writer.declLocale( "rayLength"
						, length( rayVector ) );
					auto rayDirection = m_writer.declLocale( "rayDirection"
						, rayVector / rayLength );
					auto stepLength = m_writer.declLocale( "stepLength"
						, rayLength / m_writer.cast< Float >( lightVolumetricSteps ) );
					auto step = m_writer.declLocale( "step"
						, rayDirection * stepLength );
					auto screenUV = m_writer.declLocale( "screenUV"
						, uvec2( m_writer.cast< UInt >( clipSpacePosition.x() )
							, m_writer.cast< UInt >( clipSpacePosition.y() ) ) );
					auto ditherValue = m_writer.declLocale( "ditherValue"
						, volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );

					auto currentPosition = m_writer.declLocale( "currentPosition"
						, eyePosition + step * ditherValue );
					auto volumetric = m_writer.declLocale( "volumetric"
						, 0.0_f );

					auto RdotL = m_writer.declLocale( "RdotL"
						, dot( rayDirection, lightDirection ) );
					auto sqVolumetricScattering = m_writer.declLocale( "sqVolumetricScattering"
						, lightVolumetricScattering * lightVolumetricScattering );
					auto dblVolumetricScattering = m_writer.declLocale( "dblVolumetricScattering"
						, 2.0_f * lightVolumetricScattering );
					auto oneMinusVolumeScattering = m_writer.declLocale( "oneMinusVolumeScattering"
						, 1.0_f - sqVolumetricScattering );
					auto scattering = m_writer.declLocale( "scattering"
						, oneMinusVolumeScattering / m_writer.paren( 4.0_f
							* Float{ Pi< float > }
							* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 0.0_f ), 1.5_f ) ) );
					auto maxCount = m_writer.declLocale( "maxCount"
						, m_writer.cast< Int >( lightVolumetricSteps ) );

					FOR( m_writer, Int, i, 0, i < maxCount, ++i )
					{
						IF ( m_writer, m_computeOneDirectional( lightMatrix, currentPosition, lightDirection, cascadeIndex, vec3( 0.0_f ) ) > 0.5_f )
						{
							volumetric += scattering;
						}
						FI;

						currentPosition += step;
					}
					ROF;

					volumetric /= m_writer.cast< Float >( lightVolumetricSteps );
					parentOutput.m_diffuse += volumetric * lightIntensity.x() * 1.0_f * lightColour;
					parentOutput.m_specular += volumetric * lightIntensity.y() * 1.0_f * lightColour;
				}
				, InVec2{ m_writer, "renderPos" }
				, InVec3{ m_writer, "worldSpacePosition" }
				, InVec3{ m_writer, "eyePosition" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightDirection" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InVec3{ m_writer, "lightColour" }
				, InVec2{ m_writer, "lightIntensity" }
				, InUInt{ m_writer, "lightVolumetricSteps" }
				, InFloat{ m_writer, "lightVolumetricScattering" }
				, output );
		}
	}
}
