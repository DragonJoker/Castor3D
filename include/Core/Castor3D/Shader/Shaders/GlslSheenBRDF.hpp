/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSheenBRDF_H___
#define ___C3D_GlslSheenBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class SheenBRDF
	{
	public:
		C3D_API explicit SheenBRDF( sdw::ShaderWriter & writer
			, Utils & utils );
		C3D_API sdw::RetVec2 compute( sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Float const & NdotL
			, sdw::Float const & roughness );

	private:
		void declareDistribution();
		void declareVisibility();
		void declareComputeSheen();

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionCharlie;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_lambdaSheenNumericHelper;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_lambdaSheen;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_visibility;
		sdw::Function< sdw::Vec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_computeSheen;
	};
}

#endif
