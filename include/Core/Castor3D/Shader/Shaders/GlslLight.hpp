/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLight_H___
#define ___C3D_GlslLight_H___

#include "SdwModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	struct LightData
		: public sdw::StructInstanceHelperT < "C3D_LightData"
		, sdw::type::MemoryLayout::eStd140
		, sdw::Vec4ArrayField< "data", getMaxLightComponentsCount() > >
	{
		LightData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto data()const { return getMember< "data" >(); }
	};

	struct LightSurface
		: public sdw::StructInstance
	{
		C3D_API LightSurface( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API LightSurface( sdw::Vec3 const eye
			, sdw::Vec3 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );

		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, sdw::type::TypePtr type );
		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );

		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, std::string const & name
			, sdw::Vec3 const eye
			, sdw::Vec3 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts = true
			, bool enableFresnel = true
			, bool enableIridescence = true );
		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, std::string const & name
			, sdw::Vec3 const world
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts = false
			, bool enableFresnel = false
			, bool enableIridescence = false );
		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, Utils & utils
			, std::string const & name
			, sdw::Vec3 const eye
			, sdw::Vec3 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, sdw::Vec3 const f0
			, BlendComponents const & components
			, bool enableDotProducts = true
			, bool enableFresnel = true
			, bool enableIridescence = true );

		C3D_API void updateW( sdw::Vec3 const W )const;
		C3D_API void updateN( sdw::Vec3 const N )const;
		C3D_API void updateL( sdw::Vec3 const L )const;

		C3D_API void updateN( Utils & utils
			, sdw::Vec3 const N
			, sdw::Vec3 const f0
			, BlendComponents const & components )const;
		C3D_API void updateL( Utils & utils
			, sdw::Vec3 const L
			, sdw::Vec3 const f0
			, BlendComponents const & components )const;

		auto eyePosition()const { return m_eyePosition; }
		auto worldPosition()const { return m_worldPosition; }
		auto viewPosition()const { return m_viewPosition; }
		auto clipPosition()const { return m_clipPosition; }
		auto vertexToLight()const { return m_vertexToLight; }
		auto V()const { return m_V; }
		auto N()const { return m_N; }
		auto L()const { return m_L; }
		auto H()const { return m_H; }
		auto lengthV()const { return m_lengthV; }
		auto lengthL()const { return m_lengthL; }
		auto NdotV()const { return m_NdotV; }
		sdw::Float NdotL()const { return m_NdotL; }
		sdw::Float NdotH()const { return m_NdotH; }
		sdw::Vec3 F()const { return m_F; }
		sdw::Vec3 spcF()const { return m_spcF; }
		sdw::Vec3 difF()const { return m_difF; }

	private:
		sdw::Vec3 m_eyePosition;
		sdw::Vec3 m_worldPosition;
		sdw::Vec3 m_viewPosition;
		sdw::Vec3 m_clipPosition;
		sdw::Vec3 m_vertexToLight;
		sdw::Vec3 m_V;
		sdw::Vec3 m_N;
		sdw::Vec3 m_L;
		sdw::Vec3 m_H;
		sdw::Float m_lengthV;
		sdw::Float m_lengthL;
		mutable sdw::Float m_NdotV;

		mutable sdw::DefaultedT< sdw::Float > m_NdotL;
		mutable sdw::DefaultedT< sdw::Float > m_NdotH;
		mutable sdw::DefaultedT< sdw::Float > m_HdotV;

		mutable sdw::DefaultedT< sdw::Vec3 > m_F;
		mutable sdw::DefaultedT< sdw::Vec3 > m_spcF;
		mutable sdw::DefaultedT< sdw::Vec3 > m_difF;

	private:
		auto HdotV()const { return m_HdotV; }

		C3D_API void doUpdateF( Utils & utils
			, sdw::Vec3 const f0
			, BlendComponents const & components
			, sdw::Float const & dotProduct )const;
		C3D_API static sdw::expr::ExprPtr makeInit( sdw::type::BaseStructPtr type
			, sdw::Vec3 const eye
			, sdw::Vec3 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );
	};

	struct Light
		: public sdw::StructInstanceHelperT< "C3D_Light"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec3Field< "colour" >
			, sdw::FloatField< "shadowMapIndex" >
			, sdw::Vec2Field< "intensity" >
			, sdw::FloatField< "farPlane" >
			, sdw::FloatField< "shadowType" >
			, sdw::Vec2Field< "rawShadowOffsets" >
			, sdw::Vec2Field< "pcfShadowOffsets" >
			, sdw::Vec2Field< "vsmShadowVariance" >
			, sdw::FloatField< "volumetricSteps" >
			, sdw::FloatField< "volumetricScattering" > >
	{
		Light( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto colour()const { return getMember< "colour" >(); }
		auto intensity()const { return getMember< "intensity" >(); }
		auto farPlane()const { return getMember< "farPlane" >(); }
		auto volumetricScattering()const { return getMember< "volumetricScattering" >(); }
		auto rawShadowOffsets()const { return getMember< "rawShadowOffsets" >(); }
		auto pcfShadowOffsets()const { return getMember< "pcfShadowOffsets" >(); }
		auto vsmShadowVariance()const { return getMember< "vsmShadowVariance" >(); }

		C3D_API sdw::Int shadowMapIndex()const;
		C3D_API sdw::Int shadowType()const;
		C3D_API sdw::UInt volumetricSteps()const;
		C3D_API void updateShadowType( ShadowType type );
	};

	struct DirectionalLight
		: public sdw::StructInstanceHelperT< "C3D_DirectionalLight"
			, sdw::type::MemoryLayout::eStd140
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "cascadeCount" >
			, sdw::Vec4Field< "splitDepths" >
			, sdw::Vec4Field< "splitScales" >
			, sdw::Mat4ArrayField< "transforms", DirectionalMaxCascadesCount > >
	{
		DirectionalLight( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto base()const { return getMember< "base" >(); }
		auto direction()const { return getMember< "direction" >(); }
		auto splitDepths()const { return getMember< "splitDepths" >(); }
		auto splitScales()const { return getMember< "splitScales" >(); }
		auto transforms()const { return getMember< "transforms" >(); }

		C3D_API sdw::UInt cascadeCount()const;
	};

	struct PointLight
		: public sdw::StructInstanceHelperT< "C3D_PointLight"
			, sdw::type::MemoryLayout::eStd140
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "position" >
			, sdw::FloatField< "pad0" >
			, sdw::Vec3Field< "attenuation" >
			, sdw::FloatField< "pad1" >
			, sdw::Vec4Field< "pad2" >
			, sdw::Mat4ArrayField< "pad3", DirectionalMaxCascadesCount > >
	{
		PointLight( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Float getAttenuationFactor( sdw::Float const & distance )const;

	public:
		auto base()const { return getMember< "base" >(); }
		auto position()const { return getMember< "position" >(); }
		auto attenuation()const { return getMember< "attenuation" >(); }
	};

	struct SpotLight
		: public sdw::StructInstanceHelperT< "C3D_SpotLight"
			, sdw::type::MemoryLayout::eStd140
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "position" >
			, sdw::FloatField< "exponent" >
			, sdw::Vec3Field< "attenuation" >
			, sdw::FloatField< "innerCutOff" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "outerCutOff" >
			, sdw::Mat4Field< "transform" >
			, sdw::Mat4ArrayField< "pad", DirectionalMaxCascadesCount - 1u > >
	{
		C3D_API SpotLight( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Float getAttenuationFactor( sdw::Float const & distance )const;

	public:
		auto base()const { return getMember< "base" >(); }
		auto position()const { return getMember< "position" >(); }
		auto exponent()const { return getMember< "exponent" >(); }
		auto attenuation()const { return getMember< "attenuation" >(); }
		auto innerCutOff()const { return getMember< "innerCutOff" >(); }
		auto direction()const { return getMember< "direction" >(); }
		auto outerCutOff()const { return getMember< "outerCutOff" >(); }
		auto transform()const { return getMember< "transform" >(); }
		// SpecificValues
		auto cutOffsDiff()const { return innerCutOff() - outerCutOff(); }
	};

	class Lights
	{
	public:
		C3D_API Lights( Engine const & engine
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Materials const & materials
			, BRDFHelpers & brdf
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );
		C3D_API Lights( Engine const & engine
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
			, bool enableVolumetric );
		C3D_API Lights( Engine const & engine
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
			, bool enableVolumetric );

		Lights( Engine const & engine
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Materials const & materials
			, BRDFHelpers & brdf
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles )
			: Lights{ engine
				, lightingModelId
				, backgroundModelId
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, true }
		{
		}

		Lights( Engine const & engine
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
			, uint32_t shadowMapSet )
			: Lights{ engine
				, lightingModelId
				, backgroundModelId
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightsBufBinding
				, lightsBufSet
				, shadowMapBinding
				, shadowMapSet
				, true }
		{
		}

		Lights( Engine const & engine
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
			, uint32_t shadowMapSet )
			: Lights{ engine
				, lightingModelId
				, backgroundModelId
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightType
				, lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet
				, true }
		{
		}

		Lights( Engine const & engine
			, Materials const & materials
			, BRDFHelpers & brdf
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric )
			: Lights{ engine
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, enableVolumetric }
		{
		}

		Lights( Engine const & engine
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
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightsBufBinding
				, lightsBufSet
				, shadowMapBinding
				, shadowMapSet
				, enableVolumetric }
		{
		}

		Lights( Engine const & engine
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
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightType
				, lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet
				, enableVolumetric }
		{
		}

		Lights( Engine const & engine
			, Materials const & materials
			, BRDFHelpers & brdf
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles )
			: Lights{ engine
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, true }
		{
		}

		Lights( Engine const & engine
			, Materials const & materials
			, BRDFHelpers & brdf
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet )
			: Lights{ engine
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightsBufBinding
				, lightsBufSet
				, shadowMapBinding
				, shadowMapSet
				, true }
		{
		}

		Lights( Engine const & engine
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
			, uint32_t shadowMapSet )
			: Lights{ engine
				, 0u
				, 0u
				, materials
				, brdf
				, utils
				, shadowOptions
				, sssProfiles
				, lightType
				, lightUbo
				, lightBinding
				, lightSet
				, shadowMapBinding
				, shadowMapSet
				, true }
		{
		}
		/**
		*\name
		*	Lighting Model dependant
		*/
		//\{
		/**
		*\name
		*	Forward renderring
		*/
		//\{
		C3D_API void computeCombinedDifSpec( BlendComponents const & components
			, SceneData const & sceneData
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void computeCombinedDif( BlendComponents const & components
			, SceneData const & sceneData
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		//\}
		/**
		*\name
		*	Deferred renderring
		*/
		//\{
		C3D_API void computeDifSpec( DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void computeDifSpec( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void computeDifSpec( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API sdw::Vec3 computeDif( DirectionalLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDif( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDif( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		//\}
		/**
		*\name
		*	Utils
		*/
		//\{
		C3D_API sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const incident );
		C3D_API bool hasIblSupport();
		C3D_API LightingModel * getLightingModel();
		//\}
		//\}
		/**
		*\name
		*	Light accessors
		*/
		//\{
		C3D_API DirectionalLight getDirectionalLight( sdw::UInt const & index );
		C3D_API PointLight getPointLight( sdw::UInt const & index );
		C3D_API SpotLight getSpotLight( sdw::UInt const & index );
		C3D_API sdw::Vec3 getCascadeFactors( sdw::Vec3 viewVertex
			, sdw::Vec4 splitDepths
			, sdw::UInt index );
		//\}

		inline Shadow & getShadowModel()const
		{
			return *m_shadowModel;
		}

		inline DirectionalLight getDirectionalLight()const
		{
			CU_Require( m_directionalLight );
			return *m_directionalLight;
		}

		inline PointLight getPointLight()const
		{
			CU_Require( m_pointLight );
			return *m_pointLight;
		}

		inline SpotLight getSpotLight()const
		{
			CU_Require( m_spotLight );
			return *m_spotLight;
		}

	private:
		C3D_API Light getBaseLight( sdw::UInt & offset );
		C3D_API void doDeclareLightsBuffer( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclareDirectionalLightUbo( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclarePointLightUbo( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclareSpotLightUbo( uint32_t binding
			, uint32_t set );

	private:
		LightingModelID m_lightingModelId;
		BackgroundModelID m_backgroundModelId;
		sdw::ShaderWriter & m_writer;
		Engine const & m_engine;
		Materials const & m_materials;
		BRDFHelpers & m_brdf;
		Utils & m_utils;
		bool m_enableVolumetric;
		LightingModelUPtr m_lightingModel;
		std::shared_ptr< Shadow > m_shadowModel;
		std::shared_ptr< SssTransmittance > m_sssTransmittance;
		std::unique_ptr< sdw::StorageBuffer > m_ssbo;
		std::unique_ptr< sdw::RImageBufferRgba32 > m_tbo;
		sdw::Function< sdw::Float
			, InBlendComponents
			, sdw::InVec3 > m_getFinalTransmission;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4
			, sdw::InUInt > m_getCascadeFactors;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4Array
			, sdw::InUInt > m_getTileFactors;
		sdw::Function< shader::Light
			, sdw::InOutUInt > m_getBaseLight;
		sdw::Function< shader::DirectionalLight
			, sdw::InUInt > m_getDirectionalLight;
		sdw::Function< shader::PointLight
			, sdw::InUInt > m_getPointLight;
		sdw::Function< shader::SpotLight
			, sdw::InUInt > m_getSpotLight;
		std::unique_ptr< DirectionalLight > m_directionalLight;
		std::unique_ptr< PointLight > m_pointLight;
		std::unique_ptr< SpotLight > m_spotLight;
	};
}

#endif
