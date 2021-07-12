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
		using LightingModel = PhongLightingModel;
		using LightMaterial = PhongLightMaterial;
		using ReflectionModel = PhongReflectionModel;
	};

	struct PhongLightMaterial
		: public sdw::StructInstance
	{
		template< MaterialType MaterialT >
		struct CreatorT;

		template<>
		struct CreatorT< MaterialType::ePhong >
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
		struct CreatorT< MaterialType::eMetallicRoughness >
		{
			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness )
			{
				material.albedo = albedo;
				material.specular = specular;
				material.shininess = LightingModel::computeShininess( LightingModel::computeRoughness( roughness ) );
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
		struct CreatorT< MaterialType::eSpecularGlossiness >
		{
			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness )
			{
				material.albedo = albedo;
				material.specular = specular;
				material.shininess = LightingModel::computeShininess( LightingModel::computeRoughness( roughness ) );
			}

			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec4 const & data3
				, sdw::Vec4 const & data2 )
			{
				create( material, albedo, data3.rgb(), data3.a(), data2.a() );
			}
		};

		C3D_API PhongLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API PhongLightMaterial & operator=( PhongLightMaterial const & rhs );

		template< MaterialType MaterialT
			, typename ... ParamsT >
			void create( ParamsT const & ... params )
		{
			CreatorT< MaterialT >::create( *this, params... );
		}

		C3D_API void create( Material const & material );

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float ambient;
		sdw::Float shininess;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};
}

#endif
