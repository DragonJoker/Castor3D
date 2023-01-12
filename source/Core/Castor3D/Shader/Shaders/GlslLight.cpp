#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

#define C3D_DebugLightBuffer 0

namespace castor3d::shader
{
	//*********************************************************************************************

	LightSurface::LightSurface( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_eyePosition{ getMember< sdw::Vec3 >( "eyePosition" ) }
		, m_worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
		, m_viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
		, m_clipPosition{ getMember< sdw::Vec3 >( "clipPosition" ) }
		, m_vertexToLight{ getMember< sdw::Vec3 >( "vertexToLight" ) }
		, m_V{ getMember< sdw::Vec3 >( "V" ) }
		, m_N{ getMember< sdw::Vec3 >( "N" ) }
		, m_L{ getMember< sdw::Vec3 >( "L" ) }
		, m_H{ getMember< sdw::Vec3 >( "H" ) }
		, m_lengthV{ getMember< sdw::Float >( "lengthV" ) }
		, m_lengthL{ getMember< sdw::Float >( "lengthL" ) }
		, m_NdotV{ getMember< sdw::Float >( "NdotV" ) }
		, m_NdotL{ getMember< sdw::Float >( "NdotL", 0.0_f ) }
		, m_NdotH{ getMember< sdw::Float >( "NdotH", 0.0_f ) }
		, m_HdotV{ getMember< sdw::Float >( "HdotV", 0.0_f ) }
		, m_F{ getMember< sdw::Vec3 >( "F", vec3( 0.0_f ) ) }
		, m_spcF{ getMember< sdw::Vec3 >( "spcF", m_F ) }
		, m_difF{ getMember< sdw::Vec3 >( "difF", m_F ) }
	{
	}

	LightSurface::LightSurface( sdw::Vec3 const eye
		, sdw::Vec3 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
		: LightSurface{ findWriterMandat( eye, world, view, clip, normal )
			, makeInit( makeType( findTypesCache( eye, world, view, clip, normal ), enableDotProducts, enableFresnel, enableIridescence ), eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence )
			, true }
	{
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, sdw::type::TypePtr type )
	{
		return std::static_pointer_cast< sdw::type::BaseStruct >( type );
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto type = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_LightSurface"
				+ ( enableDotProducts ? std::string{ "Prods" } : std::string{} )
				+ ( enableFresnel ? std::string{ "F" } : std::string{} )
				+ ( enableIridescence ? std::string{ "I" } : std::string{} ) );

		if ( type->empty() )
		{
			type->declMember( "eyePosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "worldPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "viewPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "clipPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "vertexToLight", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "V", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "N", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "L", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "H", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "lengthV", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "lengthL", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "NdotV", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "NdotL", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "NdotH", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "HdotV", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "F", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel );
			type->declMember( "spcF", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel && enableIridescence );
			type->declMember( "difF", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel && enableIridescence );
		}

		return type;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, std::string const & name
		, sdw::Vec3 const eye
		, sdw::Vec3 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = writer.declLocale< LightSurface >( name
			, LightSurface{ eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence } );

		if ( enableDotProducts )
		{
			result.m_NdotV = max( 0.0_f, dot( result.N(), result.V() ) );
		}

		return result;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, std::string const & name
		, sdw::Vec3 const world
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		return create( writer, name, vec3( 0.0_f ), world, vec3( 0.0_f ), clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, Utils & utils
		, std::string const & name
		, sdw::Vec3 const eye
		, sdw::Vec3 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = create( writer, name, eye, world, view, clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
		result.updateN( utils, normal, f0, components );
		return result;
	}

	void LightSurface::updateW( sdw::Vec3 const w )const
	{
		worldPosition() = w;
		V() = normalize( eyePosition() - w );
		lengthV() = length( eyePosition() - w );
		H() = normalize( L() + V() );

		m_NdotH = max( 0.0_f, dot( N(), H() ) );
		m_NdotV = max( 0.0_f, dot( N(), V() ) );
		m_HdotV = max( 0.0_f, dot( H(), V() ) );
	}

	void LightSurface::updateN( sdw::Vec3 const n )const
	{
		N() = n;

		m_NdotL = max( 0.0_f, dot( N(), L() ) );
		m_NdotH = max( 0.0_f, dot( N(), H() ) );
		m_NdotV = max( 0.0_f, dot( N(), V() ) );
	}

	void LightSurface::updateL( sdw::Vec3 const l )const
	{
		vertexToLight() = l;
		L() = normalize( vertexToLight() );
		lengthL() = length( vertexToLight() );
		H() = normalize( L() + V() );

		m_NdotL = max( 0.0_f, dot( N(), L() ) );
		m_NdotH = max( 0.0_f, dot( N(), H() ) );
		m_HdotV = max( 0.0_f, dot( H(), V() ) );
	}

	void LightSurface::updateN( Utils & utils
		, sdw::Vec3 const n
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateN( n );
		doUpdateF( utils, f0, components, NdotV() );
	}

	void LightSurface::updateL( Utils & utils
		, sdw::Vec3 const l
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateL( l );
		doUpdateF( utils, f0, components, HdotV() );
	}

	void LightSurface::doUpdateF( Utils & utils
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, sdw::Float const & dotProduct )const
	{
		auto & writer = *getWriter();
		m_F = utils.conductorFresnel( dotProduct, f0 );

		if ( m_difF )
		{
			IF( writer, components.iridescenceFactor != 0.0_f )
			{
				// Blend default specular Fresnel with iridescence Fresnel
				m_spcF = mix( F(), components.iridescenceFresnel, vec3( components.iridescenceFactor ) );
				// Use the maximum component of the iridescence Fresnel color
				// Maximum is used instead of the RGB value to not get inverse colors for the diffuse BRDF
				m_difF = mix( F()
					, vec3( max( max( components.iridescenceFresnel.r(), components.iridescenceFresnel.g() ), components.iridescenceFresnel.b() ) )
					, vec3( components.iridescenceFactor ) );
			}
			ELSE
			{
				m_spcF = F();
				m_difF = F();
			}
			FI;
		}
	}

	sdw::expr::ExprPtr LightSurface::makeInit( sdw::type::BaseStructPtr type
		, sdw::Vec3 const eye
		, sdw::Vec3 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		sdw::expr::ExprList inits;
		inits.push_back( makeExpr( eye ) ); // eyePosition
		inits.push_back( makeExpr( world ) ); // worldPosition
		inits.push_back( makeExpr( view ) ); // viewPosition
		inits.push_back( makeExpr( clip ) ); // clipPosition
		inits.push_back( makeExpr( vec3(  0.0_f ) ) ); // vertexToLight
		inits.push_back( makeExpr( normalize( eye - world ) ) ); // V
		inits.push_back( makeExpr( normal ) ); // N
		inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // L
		inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // H
		inits.push_back( makeExpr( length( eye - world ) ) ); // lengthV
		inits.push_back( makeExpr( 0.0_f ) ); // lengthL
		inits.push_back( makeExpr( 0.0_f ) ); // NdotV

		if ( enableDotProducts )
		{
			inits.push_back( makeExpr( 0.0_f ) ); // NdotL
			inits.push_back( makeExpr( 0.0_f ) ); // NdotH
			inits.push_back( makeExpr( 0.0_f ) ); // HdotV
		}

		if ( enableFresnel )
		{
			inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // F

			if ( enableIridescence )
			{
				inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // spcF
				inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // difF
			}
		}

		return sdw::makeAggrInit( type, std::move( inits ) );
	}

	//*********************************************************************************************

	sdw::Int Light::shadowMapIndex()const
	{
		return getWriter()->cast< sdw::Int >( getMember< "shadowMapIndex" >() );
	}

	sdw::Int Light::shadowType()const
	{
		return getWriter()->cast< sdw::Int >( getMember< "shadowType" >() );
	}

	sdw::UInt Light::volumetricSteps()const
	{
		return getWriter()->cast< sdw::UInt >( getMember< "volumetricSteps" >() );
	}

	void Light::updateShadowType( ShadowType type )
	{
		getMember< "shadowType" >() = sdw::Float{ float( type ) };
	}

	//*********************************************************************************************

	sdw::UInt DirectionalLight::cascadeCount()const
	{
		return getWriter()->cast< sdw::UInt >( getMember< "cascadeCount" >() );
	}

	//*********************************************************************************************

	sdw::Float PointLight::getAttenuationFactor( sdw::Float const & distance )const
	{
		return sdw::fma( attenuation().z()
			, distance * distance
			, sdw::fma( attenuation().y()
				, distance
				, attenuation().x() ) );
	}

	//*********************************************************************************************

	sdw::Float SpotLight::getAttenuationFactor( sdw::Float const & distance )const
	{
		return sdw::fma( attenuation().z()
			, distance * distance
			, sdw::fma( attenuation().y()
				, distance
				, attenuation().x() ) );
	}

	//*********************************************************************************************

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: m_lightingModelId{ lightingModelId }
		, m_backgroundModelId{ backgroundModelId }
		, m_writer{ *materials.getWriter() }
		, m_engine{ engine }
		, m_materials{ materials }
		, m_brdf{ brdf }
		, m_utils{ utils }
		, m_enableVolumetric{ enableVolumetric }
		, m_shadowModel{ std::make_shared< Shadow >( shadowOptions, m_writer ) }
		, m_sssTransmittance{ ( sssProfiles
			? std::make_shared< SssTransmittance >( m_writer
				, std::move( shadowOptions )
				, *sssProfiles )
			: nullptr ) }
	{
	}

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, uint32_t lightsBufBinding
		, uint32_t lightsBufSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
		: Lights{ engine
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shadowOptions
			, sssProfiles
			, enableVolumetric }
	{
		m_shadowModel->declare( shadowMapBinding, shadowMapSet );
		doDeclareLightsBuffer( lightsBufBinding, lightsBufSet );
	}

	Lights::Lights( Engine const & engine
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, Materials const & materials
		, BRDFHelpers & brdf
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, LightType lightType
		, bool lightUbo
		, uint32_t lightBinding
		, uint32_t lightSet
		, uint32_t & shadowMapBinding
		, uint32_t shadowMapSet
		, bool enableVolumetric )
		: Lights{ engine
			, lightingModelId
			, backgroundModelId
			, materials
			, brdf
			, utils
			, shadowOptions
			, sssProfiles
			, enableVolumetric }
	{
		if ( lightUbo )
		{
			// Deferred rendering mode
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				m_shadowModel->declareDirectional( shadowMapBinding, shadowMapSet );
				doDeclareDirectionalLightUbo( lightBinding, lightSet );
				break;
			case castor3d::LightType::ePoint:
				m_shadowModel->declarePoint( shadowMapBinding, shadowMapSet );
				doDeclarePointLightUbo( lightBinding, lightSet );
				break;
			case castor3d::LightType::eSpot:
				m_shadowModel->declareSpot( shadowMapBinding, shadowMapSet );
				doDeclareSpotLightUbo( lightBinding, lightSet );
				break;
			default:
				break;
			}
		}
		else
		{
			// Forward rendering mode
			m_shadowModel->declare( shadowMapBinding, shadowMapSet );
			doDeclareLightsBuffer( lightBinding, lightSet );
		}
	}

	void Lights::computeCombinedDifSpec( BlendComponents const & components
		, SceneData const & sceneData
		, BackgroundModel & background
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & parentOutput )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto begin = m_writer.declLocale( "c3d_begin"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_writer.cast< sdw::UInt >( sceneData.directionalLightCount ) );

			FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
			{
					lightingModel->compute( getDirectionalLight( dir )
						, components
						, background
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< sdw::UInt >( sceneData.pointLightCount );

			FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
			{
					lightingModel->compute( getPointLight( point )
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< sdw::UInt >( sceneData.spotLightCount );

			FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
			{
					lightingModel->compute( getSpotLight( spot )
						, components
						, lightSurface
						, receivesShadows
						, parentOutput );
			}
			ROF;
		}
	}

	void Lights::computeCombinedDif( BlendComponents const & components
		, SceneData const & sceneData
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			auto begin = m_writer.declLocale( "c3d_begin"
				, 0_u );
			auto end = m_writer.declLocale( "c3d_end"
				, m_writer.cast< sdw::UInt >( sceneData.directionalLightCount ) );

			FOR( m_writer, sdw::UInt, dir, begin, dir < end, ++dir )
			{
					output += lightingModel->computeDiffuse( getDirectionalLight( dir )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;

			begin = end;
			end += m_writer.cast< sdw::UInt >( sceneData.pointLightCount );

			FOR( m_writer, sdw::UInt, point, begin, point < end, ++point )
			{
					output += lightingModel->computeDiffuse( getPointLight( point )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;

			begin = end;
			end += m_writer.cast< sdw::UInt >( sceneData.spotLightCount );

			FOR( m_writer, sdw::UInt, spot, begin, spot < end, ++spot )
			{
					output += lightingModel->computeDiffuse( getSpotLight( spot )
						, components
						, lightSurface
						, receivesShadows );
			}
			ROF;
		}
	}

	void Lights::computeDifSpec( DirectionalLight const & light
		, BlendComponents const & components
		, BackgroundModel & background
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, background
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	void Lights::computeDifSpec( PointLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	void Lights::computeDifSpec( SpotLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( auto lightingModel = getLightingModel() )
		{
			lightingModel->compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
	}

	sdw::Vec3 Lights::computeDif( DirectionalLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
	}

	sdw::Vec3 Lights::computeDif( PointLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
	}

	sdw::Vec3 Lights::computeDif( SpotLight const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const & receivesShadows )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->computeDiffuse( light
				, components
				, lightSurface
				, receivesShadows )
			: sdw::vec3( 0.0_f );
	}

	sdw::Float Lights::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const incident )
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->getFinalTransmission( components, incident )
			: static_cast< sdw::Float const & >( components.transmission );
	}

	bool Lights::hasIblSupport()
	{
		auto lightingModel = getLightingModel();
		return lightingModel
			? lightingModel->hasIblSupport()
			: false;
	}

	LightingModel * Lights::getLightingModel()
	{
		if ( !m_lightingModel )
		{
			CU_Require( m_lightingModelId != 0u );
			CU_Require( m_backgroundModelId != 0u );
			m_lightingModel = m_engine.getLightingModelFactory().create( m_lightingModelId
				, m_backgroundModelId
				, m_writer
				, m_materials
				, m_utils
				, m_brdf
				, *m_shadowModel
				, *this
				, m_enableVolumetric );
		}

		return m_lightingModel.get();
	}

	DirectionalLight Lights::getDirectionalLight( sdw::UInt const & pindex )
	{
		if ( !m_getDirectionalLight )
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( "c3d_getDirectionalLight"
				, [this]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< DirectionalLight >( "result" );

#if C3D_DebugLightBuffer
					result.m_direction = vec3( 0.0_f, -0.7071068287_f, 0.7071067691_f );
					result.m_transform = mat4( vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f, 1.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f, 0.0_f, 1.0_f, 0.0_f )
						, vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
#else
					auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
					result.base() = getBaseLight( offset );

					auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
					auto lightData = m_writer.declLocale( "lightData"
						, lightsData[offset++] );
					result.direction() = normalize( lightData.xyz() );
					result.getMember< "cascadeCount" >() = lightData.w();
					lightData = lightsData[offset++];
					result.splitDepths() = lightData;
					lightData = lightsData[offset++];
					result.splitScales() = lightData;
					auto col0 = m_writer.declLocale< sdw::Vec4 >( "col0" );
					auto col1 = m_writer.declLocale< sdw::Vec4 >( "col1" );
					auto col2 = m_writer.declLocale< sdw::Vec4 >( "col2" );
					auto col3 = m_writer.declLocale< sdw::Vec4 >( "col3" );

					for ( uint32_t i = 0u; i < DirectionalMaxCascadesCount; ++i )
					{
						col0 = lightsData[offset]; ++offset;
						col1 = lightsData[offset]; ++offset;
						col2 = lightsData[offset]; ++offset;
						col3 = lightsData[offset]; ++offset;
						result.transforms()[i] = mat4( col0, col1, col2, col3 );
					}
#endif

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getDirectionalLight( pindex );
	}

	PointLight Lights::getPointLight( sdw::UInt const & pindex )
	{
		if ( !m_getPointLight )
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( "c3d_getPointLight"
				, [this]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< PointLight >( "result" );
					auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
					result.base() = getBaseLight( offset );

					auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
					auto lightData = m_writer.declLocale( "lightData"
						, lightsData[offset++] );
					result.position() = lightData.xyz();
					lightData = lightsData[offset++];
					result.attenuation() = lightData.xyz();

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getPointLight( pindex );
	}

	SpotLight Lights::getSpotLight( sdw::UInt const & pindex )
	{
		if ( !m_getSpotLight )
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( "c3d_getSpotLight"
				, [this]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< SpotLight >( "result" );
					auto offset = m_writer.declLocale( "offset", index * sdw::UInt( getMaxLightComponentsCount() ) );
					result.base() = getBaseLight( offset );

					auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
					auto lightData = m_writer.declLocale( "lightData"
						, lightsData[offset++] );
					result.position() = lightData.xyz();
					result.exponent() = lightData.w();
					lightData = lightsData[offset++];
					result.attenuation() = lightData.xyz();
					result.innerCutOff() = lightData.w();
					lightData = lightsData[offset++];
					result.direction() = normalize( lightData.xyz() );
					result.outerCutOff() = lightData.w();
					result.transform() = mat4( lightsData[offset + 0_u]
						, lightsData[offset + 1_u]
						, lightsData[offset + 2_u]
						, lightsData[offset + 3_u] );

					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}

		return m_getSpotLight( pindex );
	}

	sdw::Vec3 Lights::getCascadeFactors( sdw::Vec3 pviewVertex
		, sdw::Vec4 psplitDepths
		, sdw::UInt pindex )
	{
		if ( !m_getCascadeFactors )
		{
			m_getCascadeFactors = m_writer.implementFunction< sdw::Vec3 >( "c3d_getCascadeFactors"
				, [&]( sdw::Vec3 viewVertex
					, sdw::Vec4 splitDepths
					, sdw::UInt index )
				{
					auto splitDiff = m_writer.declLocale( "splitDiff"
						, ( splitDepths[index + 1u] - splitDepths[index] ) / 16.0f );
					auto splitMax = m_writer.declLocale( "splitMax"
						, splitDepths[index] - splitDiff );
					splitDiff *= 2.0f;
					auto splitMin = m_writer.declLocale( "splitMin"
						, splitMax + splitDiff );

					IF( m_writer, viewVertex.z() < splitMin )
					{
						m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
							, 1.0_f
							, 0.0_f ) );
					}
					FI;
					IF( m_writer, viewVertex.z() >= splitMin && viewVertex.z() < splitMax )
					{
						auto factor = m_writer.declLocale( "factor"
							, ( viewVertex.z() - splitMax ) / splitDiff );
						m_writer.returnStmt( vec3( m_writer.cast< sdw::Float >( index ) + 1.0_f
							, factor
							, 1.0_f - factor ) );
					}
					FI;

					m_writer.returnStmt( vec3( 0.0_f, 1.0_f, 0.0_f ) );
				}
				, sdw::InVec3( m_writer, "viewVertex" )
				, sdw::InVec4( m_writer, "splitDepths" )
				, sdw::InUInt( m_writer, "index" ) );
		}

		return m_getCascadeFactors( pviewVertex, psplitDepths, pindex );
	}

	Light Lights::getBaseLight( sdw::UInt & poffset )
	{
		if ( !m_getBaseLight )
		{
			m_getBaseLight = m_writer.implementFunction< Light >( "c3d_getBaseLight"
				, [this]( sdw::UInt offset )
				{
					auto result = m_writer.declLocale< Light >( "result" );
	#if C3D_DebugLightBuffer
					result.m_colourIndex = vec4( 1.0_f, 1.0f, 1.0f, -1.0f );
					result.m_intensityFarPlane = vec4( 0.8_f, 1.0f, 1.0f, 0.0f );
	#else
					auto lightsData = m_ssbo->getMemberArray< sdw::Vec4 >( "data" );
					auto lightData = m_writer.declLocale( "lightData"
						, lightsData[offset++] );
					result.colour() = lightData.xyz();
					result.getMember< "shadowMapIndex" >() = lightData.w();
					lightData = lightsData[offset++];
					result.intensity() = lightData.xy();
					result.farPlane() = lightData.z();
					result.getMember< "shadowType" >() = lightData.w();
					lightData = lightsData[offset++];
					result.rawShadowOffsets() = lightData.xy();
					result.pcfShadowOffsets() = lightData.zw();
					lightData = lightsData[offset++];
					result.vsmShadowVariance() = lightData.xy();
					result.getMember< "volumetricSteps" >() = lightData.z();
					result.volumetricScattering() = lightData.w();
	#endif

					m_writer.returnStmt( result );
				}
				, sdw::InOutUInt( m_writer, "offset" ) );
		}

		return m_getBaseLight( poffset );
	}

	void Lights::doDeclareLightsBuffer( uint32_t binding
		, uint32_t set )
	{
		m_ssbo = std::make_unique< sdw::StorageBuffer >( m_writer
			, "C3D_Lights"
			, "c3d_lights"
			, binding
			, set );
		m_ssbo->declMember< sdw::UVec4 >( "counts" );
		m_ssbo->declMemberArray< sdw::Vec4 >( "data" );
		m_ssbo->end();
	}

	void Lights::doDeclareDirectionalLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_directionalLight = std::make_unique< DirectionalLight >( lightUbo.declMember< DirectionalLight >( "l" ) );
		lightUbo.end();
	}

	void Lights::doDeclarePointLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_pointLight = std::make_unique< PointLight >( lightUbo.declMember< PointLight >( "l" ) );
		lightUbo.end();
	}

	void Lights::doDeclareSpotLightUbo( uint32_t binding
		, uint32_t set )
	{
		sdw::StorageBuffer lightUbo{ m_writer, "C3D_LightsSsbo", "c3d_lights", binding, set };
		m_spotLight = std::make_unique< SpotLight >( lightUbo.declMember< SpotLight >( "l" ) );
		lightUbo.end();
	}

	//*********************************************************************************************
}
