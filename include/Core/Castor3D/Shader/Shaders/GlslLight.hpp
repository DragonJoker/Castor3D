/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLight_H___
#define ___C3D_GlslLight_H___

#include "SdwModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslBuffer.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	struct LightData
		: public sdw::StructInstanceHelperT < "C3D_LightData"
		, sdw::type::MemoryLayout::eStd430
		, sdw::Vec4Field< "data" > >
	{
		LightData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto data()const { return getMember< "data" >(); }
	};

	struct ShadowData
		: public sdw::StructInstanceHelperT< "C3D_ShadowData"
			, sdw::type::MemoryLayout::eC
			, sdw::IntField< "shadowMapIndex" >
			, sdw::UIntField< "shadowType" >
			, sdw::FloatField< "pcfFilterSize" >
			, sdw::UIntField< "pcfSampleCount" >
			, sdw::Vec2Field< "rawShadowOffsets" >
			, sdw::Vec2Field< "pcfShadowOffsets" >
			, sdw::FloatField< "vsmMinVariance" >
			, sdw::FloatField< "vsmLightBleedingReduction" >
			, sdw::UIntField< "volumetricSteps" >
			, sdw::FloatField< "volumetricScattering" > >
	{
		ShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto shadowMapIndex()const { return getMember< "shadowMapIndex" >(); }
		auto shadowType()const { return getMember< "shadowType" >(); }
		auto pcfFilterSize()const { return getMember< "pcfFilterSize" >(); }
		auto pcfSampleCount()const { return getMember< "pcfSampleCount" >(); }
		auto rawShadowOffsets()const { return getMember< "rawShadowOffsets" >(); }
		auto pcfShadowOffsets()const { return getMember< "pcfShadowOffsets" >(); }
		auto vsmMinVariance()const { return getMember< "vsmMinVariance" >(); }
		auto vsmLightBleedingReduction()const { return getMember< "vsmLightBleedingReduction" >(); }
		auto volumetricSteps()const { return getMember< "volumetricSteps" >(); }
		auto volumetricScattering()const { return getMember< "volumetricScattering" >(); }

		C3D_API void updateShadowType( ShadowType type );
	};

	struct Light
		: public sdw::StructInstanceHelperT< "C3D_Light"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "colour" >
			, sdw::FloatField< "farPlane" >
			, sdw::Vec2Field< "intensity" >
			, sdw::StructFieldT< ShadowData, "shadows" > >
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
		auto shadows()const { return getMember< "shadows" >(); }
	};

	struct DirectionalLight
		: public sdw::StructInstanceHelperT< "C3D_DirectionalLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "cascadeCount" >
			, sdw::FloatArrayField< "splitDepths", ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) >
			, sdw::FloatArrayField< "splitScales", ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) >
			, sdw::Mat4ArrayField< "transforms", MaxDirectionalCascadesCount > >
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
		auto shadows()const { return base().shadows(); }

		C3D_API sdw::UInt cascadeCount()const;
	};

	struct PointLight
		: public sdw::StructInstanceHelperT< "C3D_PointLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "position" >
			, sdw::FloatField< "pad0" >
			, sdw::Vec3Field< "attenuation" >
			, sdw::FloatField< "pad1" > >
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
		auto shadows()const { return base().shadows(); }
	};

	struct SpotLight
		: public sdw::StructInstanceHelperT< "C3D_SpotLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "position" >
			, sdw::FloatField< "exponent" >
			, sdw::Vec3Field< "attenuation" >
			, sdw::FloatField< "innerCutOff" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "outerCutOff" >
			, sdw::Mat4Field< "transform" > >
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
		auto shadows()const { return base().shadows(); }
		// SpecificValues
		auto cutOffsDiff()const { return innerCutOff() - outerCutOff(); }
	};

	class LightsBuffer
		: public BufferT< LightData >
	{
	public:
		C3D_API LightsBuffer( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		C3D_API DirectionalLight getDirectionalLight( sdw::UInt const & offset );
		C3D_API PointLight getPointLight( sdw::UInt const & offset );
		C3D_API SpotLight getSpotLight( sdw::UInt const & offset );

		sdw::UInt getDirectionalsEnd()const
		{
			return getFirstCount();
		}

		sdw::UInt getPointsEnd()const
		{
			return getSecondCount();
		}

		sdw::UInt getSpotsEnd()const
		{
			return getThirdCount();
		}

	private:
		sdw::Vec4 getLightData( sdw::UInt & index )const;

		void getShadows( sdw::Vec4 & lightData
			, ShadowData shadows
			, sdw::UInt & offset );
		void getBaseLight( sdw::Vec4 & lightData
			, Light light
			, sdw::UInt & offset );

	private:
		sdw::Function< shader::DirectionalLight
			, sdw::InUInt > m_getDirectionalLight;
		sdw::Function< shader::PointLight
			, sdw::InUInt > m_getPointLight;
		sdw::Function< shader::SpotLight
			, sdw::InUInt > m_getSpotLight;
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
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
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
				, lightType
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
				, lightType
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
				, lightType
				, lightsBufBinding
				, lightsBufSet
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
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void computeCombinedDif( BlendComponents const & components
			, BackgroundModel & background
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
			, BackgroundModel & background
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
		C3D_API DirectionalLight getDirectionalLight( sdw::UInt const & offset );
		C3D_API PointLight getPointLight( sdw::UInt const & offset );
		C3D_API SpotLight getSpotLight( sdw::UInt const & offset );
		C3D_API sdw::Vec3 getCascadeFactors( DirectionalLight const light
			, sdw::Vec3 viewVertex
			, sdw::UInt maxCascadeCount );
		//\}

		inline Shadow & getShadowModel()const
		{
			return *m_shadowModel;
		}

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
		ShadowUPtr m_shadowModel;
		SssTransmittanceUPtr m_sssTransmittance;
		LightsBufferUPtr m_lightsBuffer;
		sdw::Function< sdw::Float
			, InBlendComponents
			, sdw::InVec3 > m_getFinalTransmission;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloatArray
			, sdw::InUInt
			, sdw::InUInt > m_getCascadeFactors;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4Array
			, sdw::InUInt > m_getTileFactors;
	};
}

#endif
