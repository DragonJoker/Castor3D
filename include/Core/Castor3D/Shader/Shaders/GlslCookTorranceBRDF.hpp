/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class CookTorranceBRDF
	{
	public:
		C3D_API explicit CookTorranceBRDF( sdw::ShaderWriter & writer
			, Utils & utils
			, BRDFHelpers & brdf );
		C3D_API sdw::RetVec3 compute( sdw::Vec3 const & radiance
			, sdw::Vec2 const & intensity
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & f0
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, Surface surface
			, OutputComponents & output );
		C3D_API sdw::RetVec3 compute( Light const & light
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & f0
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, Surface surface
			, OutputComponents & output );
		C3D_API sdw::RetVec3 compute( sdw::Vec3 const & radiance
			, sdw::Vec2 const & intensity
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & f0
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Vec3 const & iridescenceFresnel
			, sdw::Float const & iridescenceFactor
			, Surface surface
			, OutputComponents & output );
		C3D_API sdw::RetVec3 compute( Light const & light
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & f0
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Vec3 const & iridescenceFresnel
			, sdw::Float const & iridescenceFactor
			, Surface surface
			, OutputComponents & output );
		C3D_API void computeAON( Light const & light
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Float const & smoothBand
			, Surface surface
			, OutputComponents & output );
		C3D_API void computeAON( Light const & light
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Float const & smoothBand
			, sdw::Vec3 const & iridescenceFresnel
			, sdw::Float const & iridescenceFactor
			, Surface surface
			, OutputComponents & output );
		C3D_API sdw::RetVec3 computeSpecular( Light const & light
			, sdw::Float const & HdotV
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Vec3 const & position
			, sdw::Vec3 const & normal );
		C3D_API sdw::Vec3 computeDiffuse( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuse( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuseAON( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & smoothBand
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuseAON( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & smoothBand
			, Surface surface );

	private:
		void declareComputeCookTorrance();
		void declareComputeCookTorranceAON();
		void declareComputeCookTorranceSpecular();
		void declareComputeCookTorranceDiffuse();
		void declareComputeCookTorranceDiffuseAON();

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		BRDFHelpers & m_brdf;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorrance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorranceIridescence;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorranceAON;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorranceIridescenceAON;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3 > m_computeCookTorranceSpecular;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InSurface > m_computeCookTorranceDiffuse;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, InSurface > m_computeCookTorranceDiffuseAON;
	};
}

#endif
