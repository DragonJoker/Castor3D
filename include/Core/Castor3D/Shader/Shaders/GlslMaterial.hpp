/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMaterials_H___
#define ___C3D_GlslMaterials_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	template< MaterialType MaterialT >
	struct ShaderMaterialTraitsT;

	template<>
	struct ShaderMaterialTraitsT< MaterialType::ePhong >
	{
		using Materials = PhongMaterials;
		using LightingModel = PhongLightingModel;
		using LightMaterial = PhongLightMaterial;
		using ReflectionModel = PhongReflectionModel;
	};

	template<>
	struct ShaderMaterialTraitsT< MaterialType::eMetallicRoughness >
	{
		using Materials = PbrMRMaterials;
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	template<>
	struct ShaderMaterialTraitsT< MaterialType::eSpecularGlossiness >
	{
		using Materials = PbrSGMaterials;
		using LightingModel = PbrLightingModel;
		using LightMaterial = PbrLightMaterial;
		using ReflectionModel = PbrReflectionModel;
	};

	castor::String const PassBufferName = cuT( "Materials" );

	struct BaseMaterial
		: public sdw::StructInstance
	{
		friend class Materials;
		virtual ~BaseMaterial() = default;

		C3D_API virtual sdw::Vec3 colour()const = 0;

	protected:
		C3D_API BaseMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	protected:
		sdw::Vec4 m_common;
		sdw::Vec4 m_opacityTransmission;
		sdw::Vec4 m_reflRefr;
		sdw::Vec4 m_sssInfo;

	public:
		sdw::Array< sdw::Vec4 > transmittanceProfile;

		sdw::Float opacity;
		sdw::Vec3 transmission;
		sdw::Float emissive;
		sdw::Float alphaRef;
		sdw::Float gamma;
		sdw::Float refractionRatio;
		sdw::Int hasRefraction;
		sdw::Int hasReflection;
		sdw::Float bwAccumulationOperator;
		sdw::Int subsurfaceScatteringEnabled;
		sdw::Float gaussianWidth;
		sdw::Float subsurfaceScatteringStrength;
		sdw::Int transmittanceProfileSize;
	};

	CU_DeclareSmartPtr( BaseMaterial );

	class Materials
	{
	protected:
		C3D_API explicit Materials( sdw::ShaderWriter & writer );

	public:
		virtual ~Materials() = default;
		C3D_API virtual void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set ) = 0;
		C3D_API virtual BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const = 0;

	protected:
		void doFetch( BaseMaterial & result
			, sdw::SampledImageT< FImgBufferRgba32 > & c3d_materials
			, sdw::Int & offset );

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::Struct > m_type;
	};
}

#endif
