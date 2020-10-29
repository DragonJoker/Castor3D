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

		BaseMaterial::BaseMaterial( ast::Shader * shader
			, ast::expr::ExprPtr expr )
			: StructInstance{ shader, std::move( expr ) }
			, m_common{ getMember< Vec4 >( "m_common" ) }
			, m_reflRefr{ getMember< Vec4 >( "m_reflRefr" ) }
			, m_sssInfo{ getMember< Vec4 >( "m_sssInfo" ) }
			, m_opacity{ m_common.x() }
			, m_emissive{ m_common.y() }
			, m_alphaRef{ m_common.z() }
			, m_gamma{ m_common.w() }
			, m_refractionRatio{ m_reflRefr.x() }
			, m_hasRefraction{ shader, makeCast( shader->getTypesCache().getInt(), makeExpr( *shader, m_reflRefr.y() ) ) }
			, m_hasReflection{ shader, makeCast( shader->getTypesCache().getInt(), makeExpr( *shader, m_reflRefr.z() ) ) }
			, m_bwAccumulationOperator{ m_reflRefr.w() }
			, m_subsurfaceScatteringEnabled{ shader, makeCast( shader->getTypesCache().getInt(), makeExpr( *shader, m_sssInfo.x() ) ) }
			, m_gaussianWidth{ shader, makeExpr( *shader, m_sssInfo.y() ) }
			, m_subsurfaceScatteringStrength{ shader, makeExpr( *shader, m_sssInfo.z() ) }
			, m_transmittanceProfileSize{ shader, makeCast( shader->getTypesCache().getInt(), makeExpr( *shader, m_sssInfo.w() ) ) }
			, m_transmittanceProfile{ getMemberArray< Vec4 >( "m_transmittanceProfile" ) }
		{
		}

		//*****************************************************************************************

		LegacyMaterial::LegacyMaterial( ast::Shader * shader
			, ast::expr::ExprPtr expr )
			: BaseMaterial{ shader, std::move( expr ) }
			, m_diffAmb{ getMember< Vec4 >( "m_diffAmb" ) }
			, m_specShin{ getMember< Vec4 >( "m_specShin" ) }
			, m_ambient{ m_diffAmb.w() }
			, m_specular{ m_specShin.xyz() }
			, m_shininess{ m_specShin.w() }
		{
		}

		Vec3 LegacyMaterial::m_diffuse()const
		{
			return m_diffAmb.rgb();
		}

		std::unique_ptr< sdw::Struct > LegacyMaterial::declare( ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::StructPtr LegacyMaterial::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "LegacyMaterial" );

			if ( result->empty() )
			{
				result->declMember( "m_diffAmb", ast::type::Kind::eVec4F );
				result->declMember( "m_specShin", ast::type::Kind::eVec4F );
				result->declMember( "m_common", ast::type::Kind::eVec4F );
				result->declMember( "m_reflRefr", ast::type::Kind::eVec4F );
				result->declMember( "m_sssInfo", ast::type::Kind::eVec4F );
				result->declMember( "m_transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
			}

			return result;
		}

		//*****************************************************************************************

		MetallicRoughnessMaterial::MetallicRoughnessMaterial( ast::Shader * shader
			, ast::expr::ExprPtr expr )
			: BaseMaterial{ shader, std::move( expr ) }
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
				result->declMember( "m_reflRefr", ast::type::Kind::eVec4F );
				result->declMember( "m_sssInfo", ast::type::Kind::eVec4F );
				result->declMember( "m_transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
			}

			return result;
		}

		//*****************************************************************************************

		SpecularGlossinessMaterial::SpecularGlossinessMaterial( ast::Shader * shader
			, ast::expr::ExprPtr expr )
			: BaseMaterial{ shader, std::move( expr ) }
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

		//*********************************************************************************************

		LegacyMaterials::LegacyMaterials( ShaderWriter & writer )
			: Materials{ writer }
		{
		}

		void LegacyMaterials::declare( bool hasSsbo )
		{
			m_type = LegacyMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< LegacyMaterial > >( m_writer
					, PassBufferName
					, m_type->getType()
					, getPassBufferIndex()
					, 0u );
			}
			else
			{
				auto c3d_materials = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_materials", getPassBufferIndex(), 0u );
				m_getMaterial = m_writer.implementFunction< LegacyMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< LegacyMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_diffAmb = c3d_materials.fetch( Int{ offset++ } );
						result.m_specShin = c3d_materials.fetch( Int{ offset++ } );
						result.m_common = c3d_materials.fetch( Int{ offset++ } );
						result.m_reflRefr = c3d_materials.fetch( Int{ offset++ } );
						result.m_sssInfo = c3d_materials.fetch( Int{ offset++ } );

						for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
						{
							result.m_transmittanceProfile[i] = c3d_materials.fetch( Int{ offset++ } );
						}

						m_writer.returnStmt( result );
					}
					, InUInt{ m_writer, "index" } );
			}
		}

		LegacyMaterial LegacyMaterials::getMaterial( UInt const & index )const
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

		BaseMaterialUPtr LegacyMaterials::getBaseMaterial( UInt const & index )const
		{
			auto material = m_writer.declLocale( "material"
				, getMaterial( index ) );
			return std::make_unique< LegacyMaterial >( material );
		}

		//*********************************************************************************************

		PbrMRMaterials::PbrMRMaterials( ShaderWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrMRMaterials::declare( bool hasSsbo )
		{
			m_type = MetallicRoughnessMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< MetallicRoughnessMaterial > >( m_writer
					, PassBufferName
					, m_type->getType()
					, getPassBufferIndex()
					, 0u );
			}
			else
			{
				auto c3d_materials = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_materials", getPassBufferIndex(), 0u );
				m_getMaterial = m_writer.implementFunction< MetallicRoughnessMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< MetallicRoughnessMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_albRough = c3d_materials.fetch( Int{ offset++ } );
						result.m_metDiv = c3d_materials.fetch( Int{ offset++ } );
						result.m_common = c3d_materials.fetch( Int{ offset++ } );
						result.m_reflRefr = c3d_materials.fetch( Int{ offset++ } );
						result.m_sssInfo = c3d_materials.fetch( Int{ offset++ } );

						for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
						{
							result.m_transmittanceProfile[i] = c3d_materials.fetch( Int{ offset++ } );
						}

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

		void PbrSGMaterials::declare( bool hasSsbo )
		{
			m_type = SpecularGlossinessMaterial::declare( m_writer );

			if ( hasSsbo )
			{
				m_ssbo = std::make_unique< ArraySsboT< SpecularGlossinessMaterial > >( m_writer
					, PassBufferName
					, m_type->getType()
					, getPassBufferIndex()
					, 0u );
			}
			else
			{
				auto c3d_materials = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_materials", getPassBufferIndex(), 0u );
				m_getMaterial = m_writer.implementFunction< SpecularGlossinessMaterial >( "getMaterial"
					, [this, &c3d_materials]( UInt const & index )
					{
						auto result = m_writer.declLocale< SpecularGlossinessMaterial >( "result"
							, *m_type );
						auto offset = m_writer.declLocale( "offset"
							, m_writer.cast< Int >( index ) * Int( MaxMaterialComponentsCount ) );
						result.m_diffDiv = c3d_materials.fetch( Int{ offset++ } );
						result.m_specGloss = c3d_materials.fetch( Int{ offset++ } );
						result.m_common = c3d_materials.fetch( Int{ offset++ } );
						result.m_reflRefr = c3d_materials.fetch( Int{ offset++ } );
						result.m_sssInfo = c3d_materials.fetch( Int{ offset++ } );

						for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
						{
							result.m_transmittanceProfile[i] = c3d_materials.fetch( Int{ offset++ } );
						}

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
