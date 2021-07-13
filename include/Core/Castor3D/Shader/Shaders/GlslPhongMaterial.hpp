/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongMaterialModel_H___
#define ___C3D_GlslPhongMaterialModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	template<>
	struct ShaderMaterialTraitsT< MaterialType::ePhong >
	{
		using LightMaterial = PhongLightMaterial;
		using ReflectionModel = PhongReflectionModel;
	};

	struct PhongLightMaterial
		: public LightMaterial
	{
		C3D_API PhongLightMaterial( sdw::ShaderWriter & writer
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
		sdw::Float ambient;
		sdw::Vec3 specular;
		sdw::Float shininess;
	};

	template< MaterialType MaterialT >
	struct PhongLightMaterialCreatorT;

	template<>
	struct PhongLightMaterialCreatorT< MaterialType::ePhong >
	{
		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess
			, sdw::Float const & ambient )
		{
			material.albedo = albedo;
			material.specular = specular;
			material.ambient = ambient;
			material.shininess = shininess;
		}

		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & diffuse
			, sdw::Float const & gamma
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess
			, sdw::Float const & ambient )
		{
			create( material
				, pow( max( diffuse, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
				, specular
				, shininess
				, ambient );
		}

		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data2.a(), 0.0_f );
		}

		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient )
		{
			create( material, albedo, data3.rgb(), data2.a(), ambient );
		}
	};

	template<>
	struct PhongLightMaterialCreatorT< MaterialType::eMetallicRoughness >
	{
		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness )
		{
			material.albedo = albedo;
			material.specular = specular;
			material.shininess = LightMaterial::computeShininess( LightMaterial::computeRoughness( roughness ) );
		}

		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data3.a(), data2.a() );
		}
	};

	template<>
	struct PhongLightMaterialCreatorT< MaterialType::eSpecularGlossiness >
	{
		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness )
		{
			material.albedo = albedo;
			material.specular = specular;
			material.shininess = LightMaterial::computeShininess( LightMaterial::computeRoughness( roughness ) );
		}

		static void create( PhongLightMaterial & material
			, sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 )
		{
			create( material, albedo, data3.rgb(), data3.a(), data2.a() );
		}
	};

	template< MaterialType MaterialT
		, typename ... ParamsT >
	void PhongLightMaterial::create( ParamsT const & ... params )
	{
		PhongLightMaterialCreatorT< MaterialT >::create( *this, params... );
	}

}

#endif
