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
		using Materials = PbrMaterials;
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	template<>
	struct ShaderMaterialTraitsT< MaterialType::eSpecularGlossiness >
	{
		using Materials = PbrMaterials;
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	struct PbrMaterial
		: public BaseMaterial
	{
		friend class PbrMaterials;

		C3D_API PbrMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		C3D_API sdw::Vec3 colour()const override;

	private:
		void doCreate( sdw::SampledImageT< FImgBufferRgba32 > & materials
			, sdw::Int & offset )override;

	protected:
		sdw::Vec4 m_albRough;
		sdw::Vec4 m_spcMetal;

	public:
		sdw::Vec3 albedo;
		sdw::Float roughness;
		sdw::Vec3 specular;
		sdw::Float metalness;
	};

	class PbrMaterials
		: public Materials
	{
	public:
		C3D_API explicit PbrMaterials( sdw::ShaderWriter & writer );
		C3D_API void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )override;
		C3D_API PbrMaterial getMaterial( sdw::UInt const & index )const;
		C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

	private:
		std::unique_ptr< sdw::ArraySsboT< PbrMaterial > > m_ssbo;
		sdw::Function< PbrMaterial, sdw::InUInt > m_getMaterial;
	};
}

#endif
