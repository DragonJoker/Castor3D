/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoConfigUbo_H___
#define ___C3D_SsaoConfigUbo_H___

#include "Castor3D/Render/Ssao/SsaoModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct SsaoConfigData
			: public sdw::StructInstanceHelperT< "C3D_SsaoConfigData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec4Field< "projInfo" >
				, sdw::IntField< "numSamples" >
				, sdw::IntField< "numSpiralTurns" >
				, sdw::FloatField< "projScale" >
				, sdw::FloatField< "radius" >
				, sdw::FloatField< "invRadius" >
				, sdw::FloatField< "radius2" >
				, sdw::FloatField< "invRadius2" >
				, sdw::FloatField< "bias" >
				, sdw::FloatField< "intensity" >
				, sdw::FloatField< "intensityDivR6" >
				, sdw::FloatField< "farPlaneZ" >
				, sdw::FloatField< "edgeSharpness" >
				, sdw::UIntField< "blurStepSize" >
				, sdw::UIntField< "blurRadius" >
				, sdw::IntField< "highQuality" >
				, sdw::IntField< "blurHighQuality" >
				, sdw::IntField< "logMaxOffset" >
				, sdw::IntField< "maxMipLevel" >
				, sdw::FloatField< "minRadius" >
				, sdw::IntField< "variation" >
				, sdw::UIntField< "bendStepCount" >
				, sdw::FloatField< "bendStepSize" > >
		{
			SDW_DeclStructInstance( C3D_API, SsaoConfigData );

			SsaoConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, projInfo{ getMember< "projInfo" >() }
				, numSamples{ getMember< "numSamples" >() }
				, numSpiralTurns{ getMember< "numSpiralTurns" >() }
				, projScale{ getMember< "projScale" >() }
				, radius{ getMember< "radius" >() }
				, invRadius{ getMember< "invRadius" >() }
				, radius2{ getMember< "radius2" >() }
				, invRadius2{ getMember< "invRadius2" >() }
				, bias{ getMember< "bias" >() }
				, intensity{ getMember< "intensity" >() }
				, intensityDivR6{ getMember< "intensityDivR6" >() }
				, farPlaneZ{ getMember< "farPlaneZ" >() }
				, edgeSharpness{ getMember< "edgeSharpness" >() }
				, blurStepSize{ getMember< "blurStepSize" >() }
				, blurRadius{ getMember< "blurRadius" >() }
				, highQuality{ getMember< "highQuality" >() }
				, blurHighQuality{ getMember< "blurHighQuality" >() }
				, logMaxOffset{ getMember< "logMaxOffset" >() }
				, maxMipLevel{ getMember< "maxMipLevel" >() }
				, minRadius{ getMember< "minRadius" >() }
				, variation{ getMember< "variation" >() }
				, bendStepCount{ getMember< "bendStepCount" >() }
				, bendStepSize{ getMember< "bendStepSize" >() }
			{
			}

			sdw::Vec4 projInfo;
			sdw::Int numSamples;
			sdw::Int numSpiralTurns;
			sdw::Float projScale;
			sdw::Float radius;
			sdw::Float invRadius;
			sdw::Float radius2;
			sdw::Float invRadius2;
			sdw::Float bias;
			sdw::Float intensity;
			sdw::Float intensityDivR6;
			sdw::Float farPlaneZ;
			sdw::Float edgeSharpness;
			sdw::UInt blurStepSize;
			sdw::UInt blurRadius;
			sdw::Int highQuality;
			sdw::Int blurHighQuality;
			sdw::Int logMaxOffset;
			sdw::Int maxMipLevel;
			sdw::Float minRadius;
			sdw::Int variation;
			sdw::UInt bendStepCount;
			sdw::Float bendStepSize;
		};
	}

	struct SsaoUboConfiguration
	{
		//   vec4(-2.0f / (width*P[0][0]),
		//		  -2.0f / (height*P[1][1]),
		//		  ( 1.0f - P[0][2]) / P[0][0],
		//		  ( 1.0f + P[1][2]) / P[1][1])
		//
		//	where P is the projection matrix that maps camera space points
		//	to [-1, 1] x [-1, 1].  That is, SsaoConfigUbo::getProjectUnitMatrix().
		Point4f projInfo;
		// Integer number of samples to take at each pixel.
		int32_t numSamples;
		// This is the number of turns around the circle that the spiral pattern makes.
		// This should be prime to prevent taps from lining up.
		int32_t numSpiralTurns;
		// The height in pixels of a 1m object if viewed from 1m away.
		// You can compute it from your projection matrix.  The actual value is just
		// a scale factor on radius; you can simply hardcode this to a constant (~500)
		// and make your radius value unitless (...but resolution dependent.)
		float projScale;
		// World-space AO radius in scene units (r).  e.g., 1.0m.
		float radius;
		// 1 / radius.
		float invRadius;
		// Squared radius.
		float radius2;
		// 1 / (squared radius).
		float invRadius2;
		// Bias to avoid AO in smooth corners, e.g., 0.01m.
		float bias;
		// intensity.
		float intensity;
		// intensity / radius ^ 6.
		float intensityDivR6;
		// Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling.
		// This need not match the real far plane.
		float farPlaneZ;
		// Increase to make depth edges crisper. Decrease to reduce flicker.
		float edgeSharpness;
		// Step in 2-pixel intervals since we already blurred against neighbors in the
		// first AO pass.  This constant can be increased while R decreases to improve
		// performance at the expense of some dithering artifacts.
		// 
		// Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
		// unobjectionable after shading was applied but eliminated most temporal incoherence
		// from using small numbers of sample taps.
		uint32_t blurStepSize;
		// Filter radius in pixels. This will be multiplied by blurStepSize.
		uint32_t blurRadius;
		int32_t highQuality;
		int32_t blurHighQuality;
		// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower
		// miplevel to maintain reasonable spatial locality in the cache
		// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
		// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
		int32_t logMaxOffset;
		// This must be less than or equal to MaxLinearizedDepthMipLevel.
		int32_t maxMipLevel;
		// pixels
		float minRadius;
		int32_t variation;
		// The bending normals ray steps count.
		uint32_t bendStepCount;
		// The bending normals ray step size.
		float bendStepSize;
	};

	class SsaoConfigUbo
		: public UboT< SsaoUboConfiguration >
	{
	public:
		C3D_API explicit SsaoConfigUbo( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO content.
		 *\param[in]	config	The SSAO configuration.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour le contenu de l'UBO.
		 *\param[in]	config	La configuratio du SSAO.
		 *\param[in]	camera	La caméra de rendu.
		 */
		C3D_API void cpuUpdate( SsaoConfig const & config
			, Camera const & camera
			, Size const & renderSize );
	};
}

#define C3D_SsaoConfig( Writer, Binding, Set )\
	sdw::UniformBuffer ssaoConfig{ Writer\
		, "C3D_SsaoConfig"\
		, "c3d_ssaoConfig"\
		, uint32_t( Binding )\
		, uint32_t( Set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_ssaoConfigData = ssaoConfig.declMember< c3d::shader::SsaoConfigData >( "d" );\
	ssaoConfig.end()

#endif
