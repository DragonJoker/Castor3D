#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	PbrMaterial::PbrMaterial( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: BaseMaterial{ writer, std::move( expr ), enabled }
		, m_albRough{ getMember< sdw::Vec4 >( "albRough" ) }
		, m_spcMetal{ getMember< sdw::Vec4 >( "spcMetal" ) }
		, albedo{ m_albRough.xyz() }
		, roughness{ m_albRough.w() }
		, specular{ m_spcMetal.xyz() }
		, metalness{ m_spcMetal.w() }
	{
	}

	sdw::Vec3 PbrMaterial::colour()const
	{
		return m_albRough.rgb();
	}

	ast::type::StructPtr PbrMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "PbrMaterial" );

		if ( result->empty() )
		{
			result->declMember( "albRough", ast::type::Kind::eVec4F );
			result->declMember( "spcMetal", ast::type::Kind::eVec4F );
			result->declMember( "common", ast::type::Kind::eVec4F );
			result->declMember( "opacity", ast::type::Kind::eVec4F );
			result->declMember( "reflRefr", ast::type::Kind::eVec4F );
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
		}

		return result;
	}

	std::unique_ptr< sdw::Struct > PbrMaterial::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
	}

	void PbrMaterial::doCreate( sdw::SampledImageT< FImgBufferRgba32 > & materials
		, sdw::Int & offset )
	{
		m_albRough = materials.fetch( sdw::Int{ offset++ } );
		m_spcMetal = materials.fetch( sdw::Int{ offset++ } );
	}

	//*********************************************************************************************

	PbrMaterials::PbrMaterials( sdw::ShaderWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMaterials::declare( bool hasSsbo
		, uint32_t binding
		, uint32_t set )
	{
		m_type = PbrMaterial::declare( m_writer );

		if ( hasSsbo )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< PbrMaterial > >( m_writer
				, PassBufferName
				, m_type->getType()
				, binding
				, set
				, true );
		}
		else
		{
			auto c3d_materials = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_materials"
				, binding
				, set );
			m_getMaterial = m_writer.implementFunction< PbrMaterial >( "getMaterial"
				, [this, &c3d_materials]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< PbrMaterial >( "result"
						, *m_type );
					auto offset = m_writer.declLocale( "offset"
						, m_writer.cast< sdw::Int >( index ) * sdw::Int( MaxMaterialComponentsCount ) );
					result.create( c3d_materials, offset );
					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}
	}

	PbrMaterial PbrMaterials::getMaterial( sdw::UInt const & index )const
	{
		if ( m_ssbo )
		{
			return ( *m_ssbo )[index - 1_u];
		}
		else
		{
			return m_getMaterial( index - 1_u );
		}
	}

	BaseMaterialUPtr PbrMaterials::getBaseMaterial( sdw::UInt const & index )const
	{
		auto material = m_writer.declLocale( "material"
			, getMaterial( index ) );
		return std::make_unique< PbrMaterial >( material );
	}

	//*****************************************************************************************
}
