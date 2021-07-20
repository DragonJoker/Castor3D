/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonMaterialModel_H___
#define ___C3D_GlslToonMaterialModel_H___

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	struct ToonPhongLightMaterial
		: public c3d::LightMaterial
	{
		ToonPhongLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient )override;
		void create( c3d::Material const & material )override;
		void output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const override;
		sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		sdw::Float getMetalness()const override;
		sdw::Float getRoughness()const override;

		static sdw::Float computeGlossiness( sdw::Float const & shininess );
		static sdw::Float computeShininess( sdw::Float const & glossiness );

		sdw::Float & ambient;
		sdw::Float & shininess;
		sdw::Float smoothBand;
	};

	Writer_Parameter( ToonPhongLightMaterial );

	struct ToonPbrLightMaterial
		: public c3d::LightMaterial
	{
		ToonPbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient )override;
		void create( c3d::Material const & material )override;
		void output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const override;
		sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		sdw::Float getMetalness()const override;
		sdw::Float getRoughness()const override;

		bool isSpecularGlossiness()const
		{
			return m_isSpecularGlossiness;
		}

		sdw::Float & roughness;
		sdw::Float & metalness;
		sdw::Float smoothBand;

	protected:
		bool m_isSpecularGlossiness{};
	};

	Writer_Parameter( ToonPbrLightMaterial );

	struct ToonPbrMRLightMaterial
		: public ToonPbrLightMaterial
	{
		ToonPbrMRLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: ToonPbrLightMaterial{ writer, std::move( expr ), enabled }
		{
			m_isSpecularGlossiness = false;
		}
	};

	struct ToonPbrSGLightMaterial
		: public ToonPbrLightMaterial
	{
		ToonPbrSGLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: ToonPbrLightMaterial{ writer, std::move( expr ), enabled }
		{
			m_isSpecularGlossiness = true;
		}
	};
}

#endif
