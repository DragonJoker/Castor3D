/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBRDFHelpers_H___
#define ___C3D_GlslBRDFHelpers_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	struct MicrofacetDistributionSample
		: public sdw::StructInstanceHelperT< "C3D_MicrofacetDistributionSample"
		, sdw::type::MemoryLayout::eC
		, sdw::FloatField< "pdf" >
		, sdw::FloatField< "cosTheta" >
		, sdw::FloatField< "sinTheta" >
		, sdw::FloatField< "phi" > >
	{
		MicrofacetDistributionSample( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		auto pdf()const { return getMember< "pdf" >(); }
		auto cosTheta()const { return getMember< "cosTheta" >(); }
		auto sinTheta()const { return getMember< "sinTheta" >(); }
		auto phi()const { return getMember< "phi" >(); }
	};

	Writer_Parameter( MicrofacetDistributionSample );

	class BRDFHelpers
	{
	public:
		C3D_API explicit BRDFHelpers( sdw::ShaderWriter & writer );

		// Visibility (Geometry) functions
		C3D_API sdw::RetFloat visibilitySmithGGXCorrelated( sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Float const & alphaRoughness );
		C3D_API sdw::RetFloat visibilityGGX( sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Float const & alphaRoughness );
		C3D_API sdw::RetFloat visibilityBeckmann( sdw::Float const & NdotL
			, sdw::Float const & NdotV
			, sdw::Float const & NdotH
			, sdw::Float const & VdotH );
		C3D_API sdw::RetFloat visibilityAshikhmin( sdw::Float const & NdotL
			, sdw::Float const & NdotV );
		C3D_API sdw::RetFloat visibilitySheen( sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Float const & sheenRoughness );

		// Distribution functions
		C3D_API sdw::RetFloat distributionBlinn( sdw::Float const & NdotH
			, sdw::Float const & alphaRoughness );
		C3D_API sdw::RetFloat distributionBeckmann( sdw::Float const & NdotH
			, sdw::Float const & alphaRoughness );
		C3D_API sdw::RetFloat distributionGGX( sdw::Float const & NdotH
			, sdw::Float const & alphaRoughness );
		C3D_API sdw::RetFloat distributionCharlie( sdw::Float const & NdotH
			, sdw::Float const & sheenRoughness );

		// Importance sampling
		C3D_API RetMicrofacetDistributionSample importanceSampleGGX( sdw::Vec2 const & xi
			, sdw::Float const & roughness );
		C3D_API RetMicrofacetDistributionSample importanceSampleCharlie( sdw::Vec2 const & xi
			, sdw::Float const & roughness );
		C3D_API sdw::RetVec4 getImportanceSample( MicrofacetDistributionSample const & is
			, sdw::Vec3 const & n );

		// Other functions
		C3D_API sdw::RetVec2 hammersley( sdw::UInt const & i
			, sdw::UInt const & n );
		C3D_API void computeSpecularBrdfs( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, sdw::Vec3 const & reflectedDiffuse
			, sdw::Vec3 const & reflectedSpecular
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 & metal
			, sdw::Vec3 & dielectric );
		C3D_API sdw::RetVec3 computeFresnel( sdw::Float const & NdotV
			, sdw::Float const & roughness
			, sdw::Vec3 const & F0
			, sdw::Float const & specularWeight );
	private:
		sdw::RetFloat radicalInverse( sdw::UInt const & inBits );
		sdw::RetFloat lambdaSheenNumericHelper( sdw::Float const & cosTheta
			, sdw::Float const & roughness );
		sdw::RetFloat lambdaSheen( sdw::Float const & cosTheta
			, sdw::Float const & alphaG );

	private:
		sdw::ShaderWriter & m_writer;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_visibilitySmithGGXCorrelated;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_visibilityGGX;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_visibilityBeckmann;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_visibilityAshikhmin;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_visibilitySheen;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionBlinn;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionBeckmann;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionGGX;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionCharlie;

		sdw::Function< MicrofacetDistributionSample
			, sdw::InVec2
			, sdw::InFloat > m_importanceSampleGGX;
		sdw::Function< MicrofacetDistributionSample
			, sdw::InVec2
			, sdw::InFloat > m_importanceSampleCharlie;
		sdw::Function< sdw::Vec4
			, InMicrofacetDistributionSample
			, sdw::InVec3 > m_getImportanceSample;

		sdw::Function< sdw::Vec2
			, sdw::InUInt
			, sdw::InUInt > m_hammersley;
		sdw::Function< sdw::Float
			, sdw::InUInt > m_radicalInverse;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_lambdaSheenNumericHelper;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_lambdaSheen;
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeFresnel;
	};
}

#endif
