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
		SDW_DeclStructInstance( C3D_API, Material );

		C3D_API sdw::Vec3 colour()const;

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Vec4 colourDiv;
		sdw::Vec4 specDiv;
		sdw::Vec4 edgeFactors;
		sdw::Vec4 edgeColour;
		sdw::Vec4 specific;

	private:
		sdw::Vec4 m_common;
		sdw::Vec4 m_opacityTransmission;
		sdw::Vec4 m_reflRefr;

	public:
		sdw::Float opacity;
		sdw::Vec3 transmission;
		sdw::Float emissive;
		sdw::Float alphaRef;
		sdw::UInt sssProfileIndex;
		sdw::Float refractionRatio;
		sdw::Int hasRefraction;
		sdw::Int hasReflection;
		sdw::Float bwAccumulationOperator;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;
	};

	CU_DeclareSmartPtr( Material );

	class Materials
	{
	public:
		C3D_API explicit Materials( sdw::ShaderWriter & writer );
		C3D_API explicit Materials( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API void declare( uint32_t binding
			, uint32_t set );
		C3D_API Material getMaterial( sdw::UInt const & index )const;

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::ArraySsboT< Material > > m_ssbo;
	};

	struct SssProfile
		: public sdw::StructInstance
	{
		friend class SssProfiles;
		C3D_API SssProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, SssProfile );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Vec4 m_sssInfo;

	public:
		sdw::Array< sdw::Vec4 > transmittanceProfile;
		sdw::Int transmittanceProfileSize;
		sdw::Float gaussianWidth;
		sdw::Float subsurfaceScatteringStrength;
	};

	CU_DeclareSmartPtr( SssProfile );

	class SssProfiles
	{
	public:
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer );
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API void declare( uint32_t binding
			, uint32_t set );
		C3D_API SssProfile getProfile( sdw::UInt const & index )const;

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::ArraySsboT< SssProfile > > m_ssbo;
	};
}

#endif
