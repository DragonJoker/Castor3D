/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrMaterialModel_H___
#define ___C3D_GlslPbrMaterialModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

namespace castor3d::shader
{
	struct PbrLightMaterial
		: public LightMaterial
	{
		C3D_API PbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		C3D_API void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient )override;
		C3D_API void create( Material const & material )override;
		C3D_API void output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const override;
		C3D_API sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		C3D_API void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		C3D_API sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		C3D_API sdw::Float getMetalness()const override;
		C3D_API sdw::Float getRoughness()const override;

		bool isSpecularGlossiness()const
		{
			return m_isSpecularGlossiness;
		}

		sdw::Float & roughness;
		sdw::Float & metalness;

	protected:
		bool m_isSpecularGlossiness{};
	};

	struct PbrMRLightMaterial
		: public PbrLightMaterial
	{
		PbrMRLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: PbrLightMaterial{ writer, std::move( expr ), enabled }
		{
			m_isSpecularGlossiness = false;
		}
	};

	struct PbrSGLightMaterial
		: public PbrLightMaterial
	{
		PbrSGLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: PbrLightMaterial{ writer, std::move( expr ), enabled }
		{
			m_isSpecularGlossiness = true;
		}
	};
}

#endif
