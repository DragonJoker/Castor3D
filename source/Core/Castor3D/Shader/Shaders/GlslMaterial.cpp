#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		BaseMaterial::BaseMaterial( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_common{ getMember< Vec4 >( "m_common" ) }
			, m_opacityTransmission{ getMember< Vec4 >( "m_opacity" ) }
			, m_reflRefr{ getMember< Vec4 >( "m_reflRefr" ) }
			, m_sssInfo{ getMember< Vec4 >( "m_sssInfo" ) }
			, m_transmittanceProfile{ getMemberArray< Vec4 >( "m_transmittanceProfile" ) }
			, m_opacity{ m_opacityTransmission.w() }
			, m_transmission{ m_opacityTransmission.xyz() }
			, m_emissive{ m_common.y() }
			, m_alphaRef{ m_common.z() }
			, m_gamma{ m_common.w() }
			, m_refractionRatio{ m_reflRefr.x() }
			, m_hasRefraction{ writer.cast< Int >( m_reflRefr.y() ) }
			, m_hasReflection{ writer.cast< Int >( m_reflRefr.z() ) }
			, m_bwAccumulationOperator{ m_reflRefr.w() }
			, m_subsurfaceScatteringEnabled{ writer.cast< Int >( m_sssInfo.x() ) }
			, m_gaussianWidth{ m_sssInfo.y() }
			, m_subsurfaceScatteringStrength{ m_sssInfo.z() }
			, m_transmittanceProfileSize{ writer.cast< Int >( m_sssInfo.w() ) }
		{
		}

		//*****************************************************************************************

		PhongMaterial::PhongMaterial( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: BaseMaterial{ writer, std::move( expr ), enabled }
			, m_diffAmb{ getMember< Vec4 >( "m_diffAmb" ) }
			, m_specShin{ getMember< Vec4 >( "m_specShin" ) }
			, m_ambient{ m_diffAmb.w() }
			, m_specular{ m_specShin.xyz() }
			, m_shininess{ m_specShin.w() }
		{
		}

		Vec3 PhongMaterial::m_diffuse()const
		{
			return m_diffAmb.rgb();
		}

		std::unique_ptr< sdw::Struct > PhongMaterial::declare( ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::StructPtr PhongMaterial::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "LegacyMaterial" );

			if ( result->empty() )
			{
				result->declMember( "m_diffAmb", ast::type::Kind::eVec4F );
				result->declMember( "m_specShin", ast::type::Kind::eVec4F );
				result->declMember( "m_common", ast::type::Kind::eVec4F );
				result->declMember( "m_opacity", ast::type::Kind::eVec4F );
				result->declMember( "m_reflRefr", ast::type::Kind::eVec4F );
				result->declMember( "m_sssInfo", ast::type::Kind::eVec4F );
				result->declMember( "m_transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
			}

			return result;
		}

		//*****************************************************************************************

		MetallicRoughnessMaterial::MetallicRoughnessMaterial( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: BaseMaterial{ writer, std::move( expr ), enabled }
			, m_albRough{ getMember< Vec4 >( "m_albRough" ) }
			, m_metDiv{ getMember< Vec4 >( "m_metDiv" ) }
			, m_albedo{ m_albRough.xyz() }
			, m_roughness{ m_albRough.w() }
			, m_metallic{ m_metDiv.x() }
		{
		}

		Vec3 MetallicRoughnessMaterial::m_diffuse()const
		{
			return m_albRough.rgb();
		}

		std::unique_ptr< sdw::Struct > MetallicRoughnessMaterial::declare( ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::StructPtr MetallicRoughnessMaterial::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "MetallicRoughnessMaterial" );

			if ( result->empty() )
			{
				result->declMember( "m_albRough", ast::type::Kind::eVec4F );
				result->declMember( "m_metDiv", ast::type::Kind::eVec4F );
				result->declMember( "m_common", ast::type::Kind::eVec4F );
				result->declMember( "m_opacity", ast::type::Kind::eVec4F );
				result->declMember( "m_reflRefr", ast::type::Kind::eVec4F );
				result->declMember( "m_sssInfo", ast::type::Kind::eVec4F );
				result->declMember( "m_transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
			}

			return result;
		}

		//*****************************************************************************************

		SpecularGlossinessMaterial::SpecularGlossinessMaterial( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: BaseMaterial{ writer, std::move( expr ), enabled }
			, m_diffDiv{ getMember< Vec4 >( "m_diffDiv" ) }
			, m_specGloss{ getMember< Vec4 >( "m_specGloss" ) }
			, m_specular{ m_specGloss.xyz() }
			, m_glossiness{ m_specGloss.w() }
		{
		}

		Vec3 SpecularGlossinessMaterial::m_diffuse()const
		{
			return m_diffDiv.rgb();
		}

		std::unique_ptr< sdw::Struct > SpecularGlossinessMaterial::declare( ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::StructPtr SpecularGlossinessMaterial::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "SpecularGlossinessMaterial" );

			if ( result->empty() )
			{
				result->declMember( "m_diffDiv", ast::type::Kind::eVec4F );
				result->declMember( "m_specGloss", ast::type::Kind::eVec4F );
				result->declMember( "m_common", ast::type::Kind::eVec4F );
				result->declMember( "m_opacity", ast::type::Kind::eVec4F );
				result->declMember( "m_reflRefr", ast::type::Kind::eVec4F );
				result->declMember( "m_sssInfo", ast::type::Kind::eVec4F );
				result->declMember( "m_transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
			}

			return result;
		}

		//*********************************************************************************************

		Materials::Materials( ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void Materials::doFetch( BaseMaterial & result
			, sdw::SampledImageT< FImgBufferRgba32 > & c3d_materials
			, sdw::Int & offset )
		{
			result.m_common = c3d_materials.fetch( Int{ offset++ } );
			result.m_opacityTransmission = c3d_materials.fetch( Int{ offset++ } );
			result.m_reflRefr = c3d_materials.fetch( Int{ offset++ } );
			result.m_sssInfo = c3d_materials.fetch( Int{ offset++ } );

			for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
			{
				result.m_transmittanceProfile[i] = c3d_materials.fetch( Int{ offset++ } );
			}
		}

		//*********************************************************************************************

		PhongMaterials::PhongMaterials( ShaderWriter & writer )
			: Materials{ writer }
		{
		}

		void PhongMaterials::declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )
		{
			m_type = PhongMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< PhongMaterial > >( m_writer
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
				m_getMaterial = m_writer.implementFunction< PhongMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< PhongMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_diffAmb = c3d_materials.fetch( Int{ offset++ } );
						result.m_specShin = c3d_materials.fetch( Int{ offset++ } );
						doFetch( result, c3d_materials, offset );
						m_writer.returnStmt( result );
					}
					, InUInt{ m_writer, "index" } );
			}
		}

		PhongMaterial PhongMaterials::getMaterial( UInt const & index )const
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

		BaseMaterialUPtr PhongMaterials::getBaseMaterial( UInt const & index )const
		{
			auto material = m_writer.declLocale( "material"
				, getMaterial( index ) );
			return std::make_unique< PhongMaterial >( material );
		}

		//*********************************************************************************************

		PbrMRMaterials::PbrMRMaterials( ShaderWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrMRMaterials::declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )
		{
			m_type = MetallicRoughnessMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< MetallicRoughnessMaterial > >( m_writer
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
				m_getMaterial = m_writer.implementFunction< MetallicRoughnessMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< MetallicRoughnessMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_albRough = c3d_materials.fetch( Int{ offset++ } );
						result.m_metDiv = c3d_materials.fetch( Int{ offset++ } );
						doFetch( result, c3d_materials, offset );
						m_writer.returnStmt( result );
					}
					, InUInt{ m_writer, "index" } );
			}
		}
		
		MetallicRoughnessMaterial PbrMRMaterials::getMaterial( UInt const & index )const
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

		BaseMaterialUPtr PbrMRMaterials::getBaseMaterial( UInt const & index )const
		{
			auto material = m_writer.declLocale( "material"
				, getMaterial( index ) );
			return std::make_unique< MetallicRoughnessMaterial >( material );
		}

		//*********************************************************************************************

		PbrSGMaterials::PbrSGMaterials( ShaderWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrSGMaterials::declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )
		{
			m_type = SpecularGlossinessMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< SpecularGlossinessMaterial > >( m_writer
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
				m_getMaterial = m_writer.implementFunction< SpecularGlossinessMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< SpecularGlossinessMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_diffDiv = c3d_materials.fetch( Int{ offset++ } );
						result.m_specGloss = c3d_materials.fetch( Int{ offset++ } );
						doFetch( result, c3d_materials, offset );
						m_writer.returnStmt( result );
					}
					, InUInt{ m_writer, "index" } );
			}
		}

		SpecularGlossinessMaterial PbrSGMaterials::getMaterial( UInt const & index )const
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

		BaseMaterialUPtr PbrSGMaterials::getBaseMaterial( UInt const & index )const
		{
			auto material = m_writer.declLocale( "material"
				, getMaterial( index ) );
			return std::make_unique< SpecularGlossinessMaterial >( material );
		}

		//*********************************************************************************************
	}
}
