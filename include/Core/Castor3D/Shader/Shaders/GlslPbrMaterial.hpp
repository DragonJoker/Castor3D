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
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	template<>
	struct ShaderMaterialTraitsT< MaterialType::eSpecularGlossiness >
	{
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	struct PbrLightMaterial
		: public sdw::StructInstance
	{
		C3D_API PbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API PbrLightMaterial & operator=( PbrLightMaterial const & rhs );

		template< MaterialType MaterialT
			, typename ... ParamsT >
		void create( ParamsT const & ... params );

		C3D_API void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient );
		C3D_API void create( Material const & material );
		C3D_API void output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 );
		C3D_API sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight );
		C3D_API void adjustDirectSpecular( sdw::Vec3 & directSpecular )const;
		C3D_API sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const;
		C3D_API sdw::Float getMetalness()const;
		C3D_API sdw::Float getRoughness()const;

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float metalness;
		sdw::Float roughness;
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
			material.metalness = LightingModel::computeMetalness( albedo, specular );
			material.roughness = LightingModel::computeRoughness( LightingModel::computeGlossiness( shininess ) );
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
