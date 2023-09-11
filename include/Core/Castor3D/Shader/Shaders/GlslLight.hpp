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
	struct Light
		: public sdw::StructInstanceHelperT< "C3D_Light"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "colour" >
			, sdw::FloatField< "radius" >
			, sdw::Vec2Field< "intensity" >
			, sdw::IntField< "shadowMapIndex" >
			, sdw::UIntField< "cascadeCount" >
			, sdw::Vec3Field< "posDir" >
			, sdw::FloatField< "exponent" > >
	{
		friend class LightsBuffer;
		friend struct DirectionalLight;
		friend struct PointLight;
		friend struct SpotLight;

		Light( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto colour()const { return getMember< "colour" >(); }
		auto intensity()const { return getMember< "intensity" >(); }
		auto radius()const { return getMember< "radius" >(); }
		auto shadowMapIndex()const { return getMember< "shadowMapIndex" >(); }

	private:
		auto posDir()const { return getMember< "posDir" >(); }
		auto exponent()const { return getMember< "exponent" >(); }
		auto cascadeCount()const { return getMember< "cascadeCount" >(); }
	};

	struct DirectionalLight
		: public sdw::StructInstanceHelperT< "C3D_DirectionalLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" > >
	{
		DirectionalLight( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto base()const { return getMember< "base" >(); }

		auto colour()const { return base().colour(); }
		auto intensity()const { return base().intensity(); }
		auto shadowMapIndex()const { return base().shadowMapIndex(); }
		auto cascadeCount()const { return base().cascadeCount(); }

		auto direction()const { return base().posDir(); }
	};

	struct PointLight
		: public sdw::StructInstanceHelperT< "C3D_PointLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" > >
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

		auto colour()const { return base().colour(); }
		auto intensity()const { return base().intensity(); }
		auto radius()const { return base().radius(); }
		auto shadowMapIndex()const { return base().shadowMapIndex(); }

		auto position()const { return base().posDir(); }
	};

	struct SpotLight
		: public sdw::StructInstanceHelperT< "C3D_SpotLight"
			, sdw::type::MemoryLayout::eC
			, sdw::StructFieldT< Light, "base" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "outerCutOffCos" >
			, sdw::FloatField< "innerCutOff" >
			, sdw::FloatField< "outerCutOff" >
			, sdw::FloatField< "innerCutOffSin" >
			, sdw::FloatField< "outerCutOffSin" >
			, sdw::FloatField< "innerCutOffCos" > >
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

		auto colour()const { return base().colour(); }
		auto intensity()const { return base().intensity(); }
		auto radius()const { return base().radius(); }
		auto shadowMapIndex()const { return base().shadowMapIndex(); }

		auto position()const { return base().posDir(); }
		auto exponent()const { return base().exponent(); }
		auto direction()const { return getMember< "direction" >(); }
		auto innerCutOff()const { return getMember< "innerCutOff" >(); }
		auto outerCutOff()const { return getMember< "outerCutOff" >(); }
		auto innerCutOffCos()const { return getMember< "innerCutOffCos" >(); }
		auto outerCutOffCos()const { return getMember< "outerCutOffCos" >(); }
		auto innerCutOffSin()const { return getMember< "innerCutOffSin" >(); }
		auto outerCutOffSin()const { return getMember< "outerCutOffSin" >(); }
		// SpecificValues
		auto cutOffsCosDiff()const { return innerCutOffCos() - outerCutOffCos(); }
	};

	class LightsBuffer
		: public BufferT< BufferData >
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

		sdw::UInt getClusteredGridScale()const
		{
			return getFourthCount();
		}

	private:
		sdw::Vec4 getLightData( sdw::UInt & index )const;

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
		*	Clustered Lighting, all paths
		*/
		//\{
		C3D_API void computeCombinedDifSpec( ClusteredLights & clusteredLights
			, BlendComponents const & components
			, BackgroundModel & backgroundModel
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, sdw::Vec2 const screenPosition
			, sdw::Float const viewDepth
			, DebugOutput & debugOutput
			, OutputComponents & output );
		//\}
		/**
		*\name
		*	Diffuse only.
		*/
		//\{
		C3D_API void computeCombinedDif( BlendComponents const & components
			, BackgroundModel & backgroundModel
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DebugOutput & debugOutput
			, sdw::Vec3 & output );
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
		C3D_API sdw::Vec3 getCascadeFactors( DirectionalShadowData const light
			, sdw::Vec3 viewVertex
			, sdw::UInt maxCascadeCount );
		//\}

		Shadow & getShadowModel()const noexcept
		{
			return *m_shadowModel;
		}

		sdw::UInt getClusteredGridScale()const noexcept
		{
			CU_Require( m_lightsBuffer );
			return m_lightsBuffer->getClusteredGridScale();
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
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4Array
			, sdw::InUInt
			, sdw::InUInt > m_getCascadeFactors;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4Array
			, sdw::InUInt > m_getTileFactors;
	};
}

#endif
