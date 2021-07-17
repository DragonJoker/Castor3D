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
	castor::String const PassBufferName = cuT( "Materials" );

	struct Material
		: public sdw::StructInstance
	{
		friend class Materials;
		C3D_API Material( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API void create( sdw::SampledImageT< FImgBufferRgba32 > & materials
			, sdw::Int & offset );

		C3D_API sdw::Vec3 colour()const;

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Vec4 colourDiv;
		sdw::Vec4 specDiv;
		sdw::Vec4 specific;

	private:
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

	CU_DeclareSmartPtr( Material );

	class Materials
	{
	public:
		C3D_API explicit Materials( sdw::ShaderWriter & writer );
		virtual ~Materials() = default;
		C3D_API void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set );
		C3D_API Material getMaterial( sdw::UInt const & index )const;

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::Struct > m_type;
		std::unique_ptr< sdw::ArraySsboT< Material > > m_ssbo;
		sdw::Function< Material, sdw::InUInt > m_getMaterial;
	};
}

#endif
