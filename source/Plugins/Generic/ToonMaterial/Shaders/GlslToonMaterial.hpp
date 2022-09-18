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
		SDW_DeclStructInstance( , ToonPhongLightMaterial );
		ToonPhongLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, sdw::Float const & ambient )override;
		void create( sdw::Vec3 const & vtxColour
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const override;
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

	private:
		void doBlendWith( c3d::LightMaterial const & material
			, sdw::Float const & weight )override;
	};

	Writer_Parameter( ToonPhongLightMaterial );

	struct ToonPbrLightMaterial
		: public c3d::LightMaterial
	{
		SDW_DeclStructInstance( , ToonPbrLightMaterial );
		ToonPbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, sdw::Float const & ambient )override;
		void create( sdw::Vec3 const & vtxColour
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const override;
		sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		sdw::Float getMetalness()const override;
		sdw::Float getRoughness()const override;

		sdw::Float & roughness;
		sdw::Float & metalness;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;
		sdw::Vec4 edgeColour;
		sdw::Float smoothBand;

	private:
		void doBlendWith( c3d::LightMaterial const & material
			, sdw::Float const & weight )override;
	};

	Writer_Parameter( ToonPbrLightMaterial );
}

#endif
