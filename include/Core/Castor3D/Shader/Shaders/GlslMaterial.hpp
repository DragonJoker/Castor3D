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

	castor::String const PassBufferName = cuT( "Materials" );

	struct BaseMaterial
		: public sdw::StructInstance
	{
		friend class Materials;
		virtual ~BaseMaterial() = default;

		C3D_API void create( sdw::SampledImageT< FImgBufferRgba32 > & materials
			, sdw::Int & offset );

		C3D_API virtual sdw::Vec3 colour()const = 0;

	protected:
		C3D_API BaseMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		virtual void doCreate( sdw::SampledImageT< FImgBufferRgba32 > & materials
			, sdw::Int & offset ) = 0;

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
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::Struct > m_type;
	};
}

#endif
