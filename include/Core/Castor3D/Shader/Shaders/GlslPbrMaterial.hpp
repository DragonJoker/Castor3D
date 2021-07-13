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
	template<>
	struct ShaderMaterialTraitsT< MaterialType::eMetallicRoughness >
	{
		using LightMaterial = PbrLightMaterial;
	};

	template<>
	struct ShaderMaterialTraitsT< MaterialType::eSpecularGlossiness >
	{
		using LightMaterial = PbrLightMaterial;
	};

	struct PbrLightMaterial
		: public LightMaterial
	{
		C3D_API PbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		template< MaterialType MaterialT
			, typename ... ParamsT >
		void create( ParamsT const & ... params );

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

		sdw::Vec3 albedo;
		sdw::Float roughness;
		sdw::Vec3 specular;
		sdw::Float metalness;
	};

	template< MaterialType MaterialT >
	struct PbrLightMaterialCreatorT;

	template<>
	struct PbrLightMaterialCreatorT< MaterialType::ePhong >
	{
		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess )
		{
			material.albedo = albedo;
			material.specular = specular.rgb();
			material.metalness = LightMaterial::computeMetalness( albedo, specular );
			material.roughness = LightMaterial::computeRoughness( LightMaterial::computeGlossiness( shininess ) );
		}

		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & diffuse
			, sdw::Float const & gamma
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess )
		{
			create( material
				, pow( max( diffuse, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
				, specular
				, shininess );
		}

		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data2.a() );
		}
	};

	template<>
	struct PbrLightMaterialCreatorT< MaterialType::eMetallicRoughness >
	{
		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness )
		{
			material.albedo = albedo;
			material.specular = specular;
			material.metalness = metalness;
			material.roughness = roughness;
		}

		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data3.a(), data2.a() );
		}
	};

	template<>
	struct PbrLightMaterialCreatorT< MaterialType::eSpecularGlossiness >
	{
		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness )
		{
			material.albedo = albedo;
			material.specular = specular;
			material.metalness = metalness;
			material.roughness = roughness;
		}

		static void create( PbrLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data3.a(), data2.a() );
		}
	};

	template< MaterialType MaterialT
		, typename ... ParamsT >
	void PbrLightMaterial::create( ParamsT const & ... params )
	{
		PbrLightMaterialCreatorT< MaterialT >::create( *this, params... );
	}
}

#endif
