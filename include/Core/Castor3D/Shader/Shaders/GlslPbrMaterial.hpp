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
		template< MaterialType MaterialT >
		struct CreatorT;

		template<>
		struct CreatorT< MaterialType::ePhong >
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
		struct CreatorT< MaterialType::eMetallicRoughness >
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
		struct CreatorT< MaterialType::eSpecularGlossiness >
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

		C3D_API PbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API PbrLightMaterial & operator=( PbrLightMaterial const & rhs );

		template< MaterialType MaterialT
			, typename ... ParamsT >
			void create( ParamsT const & ... params )
		{
			CreatorT< MaterialT >::create( *this, params... );
		}

		C3D_API void createFromPbr( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2 );
		C3D_API void create( Material const & material );

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float metalness;
		sdw::Float roughness;
	};
}

#endif
