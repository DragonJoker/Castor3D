#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		BaseMaterial::BaseMaterial( castor::String const & type )
			: Type( type )
		{
		}

		BaseMaterial::BaseMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "BaseMaterial " ), writer, name )
		{
		}

		BaseMaterial::BaseMaterial( castor::String const & type
			, GlslWriter * writer
			, castor::String const & name )
			: Type( type, writer, name )
		{
		}

		Float BaseMaterial::m_opacity()const
		{
			return m_common().x();
		}

		Float BaseMaterial::m_emissive()const
		{
			return m_common().y();
		}

		Float BaseMaterial::m_alphaRef()const
		{
			return m_common().z();
		}

		Float BaseMaterial::m_gamma()const
		{
			return m_common().w();
		}

		Float BaseMaterial::m_refractionRatio()const
		{
			return m_reflRefr().x();
		}

		Int BaseMaterial::m_hasRefraction()const
		{
			return m_writer->cast< Int >( m_reflRefr().y() );
		}

		Int BaseMaterial::m_hasReflection()const
		{
			return m_writer->cast< Int >( m_reflRefr().z() );
		}

		Float BaseMaterial::m_exposure()const
		{
			return m_reflRefr().w();
		}

		Int BaseMaterial::m_subsurfaceScatteringEnabled()const
		{
			return m_writer->cast< Int >( m_sssInfo().r() );
		}

		Float BaseMaterial::m_gaussianWidth()const
		{
			return m_writer->cast< Int >( m_sssInfo().g() );
		}

		Float BaseMaterial::m_subsurfaceScatteringStrength()const
		{
			return m_writer->cast< Int >( m_sssInfo().b() );
		}

		Int BaseMaterial::m_transmittanceProfileSize()const
		{
			return m_writer->cast< Int >( m_sssInfo().a() );
		}

		glsl::Array< glsl::Vec4 > BaseMaterial::m_transmittanceProfile()const
		{
			return glsl::Array< Vec4 >( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_transmittanceProfile" ), 10u );
		}
		
		Vec4 BaseMaterial::m_common()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) );
		}

		Vec4 BaseMaterial::m_reflRefr()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) );
		}

		Vec4 BaseMaterial::m_sssInfo()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_sssInfo" ) );
		}

		//*****************************************************************************************

		LegacyMaterial::LegacyMaterial()
			: BaseMaterial( cuT( "LegacyMaterial " ) )
		{
		}

		LegacyMaterial::LegacyMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "LegacyMaterial " ), writer, name )
		{
		}

		LegacyMaterial & LegacyMaterial::operator=( LegacyMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		void LegacyMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "LegacyMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffAmb" ) );
			material.declMember< Vec4 >( cuT( "m_specShin" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittanceProfile" ), 10u );
			material.end();
		}

		Vec3 LegacyMaterial::m_diffuse()const
		{
			return m_diffAmb().rgb();
		}

		Float LegacyMaterial::m_ambient()const
		{
			return m_diffAmb().a();
		}

		Vec3 LegacyMaterial::m_specular()const
		{
			return m_specShin().rgb();
		}

		Float LegacyMaterial::m_shininess()const
		{
			return m_specShin().a();
		}

		Vec4 LegacyMaterial::m_diffAmb()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffAmb" ) );
		}

		Vec4 LegacyMaterial::m_specShin()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specShin" ) );
		}

		//*****************************************************************************************

		MetallicRoughnessMaterial::MetallicRoughnessMaterial()
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ) )
		{
		}

		MetallicRoughnessMaterial::MetallicRoughnessMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ), writer, name )
		{
		}

		MetallicRoughnessMaterial & MetallicRoughnessMaterial::operator=( MetallicRoughnessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		void MetallicRoughnessMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "MetallicRoughnessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_albRough" ) );
			material.declMember< Vec4 >( cuT( "m_metDiv" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittanceProfile" ), 10u );
			material.end();
		}
		
		Vec3 MetallicRoughnessMaterial::m_diffuse()const
		{
			return m_albRough().rgb();
		}

		Vec3 MetallicRoughnessMaterial::m_albedo()const
		{
			return m_albRough().rgb();
		}

		Float MetallicRoughnessMaterial::m_roughness()const
		{
			return m_albRough().a();
		}

		Float MetallicRoughnessMaterial::m_metallic()const
		{
			return m_metDiv().r();
		}

		Vec4 MetallicRoughnessMaterial::m_albRough()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_albRough" ) );
		}

		Vec4 MetallicRoughnessMaterial::m_metDiv()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_metDiv" ) );
		}

		//*****************************************************************************************

		SpecularGlossinessMaterial::SpecularGlossinessMaterial()
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ) )
		{
		}

		SpecularGlossinessMaterial::SpecularGlossinessMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ), writer, name )
		{
		}

		SpecularGlossinessMaterial & SpecularGlossinessMaterial::operator=( SpecularGlossinessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		void SpecularGlossinessMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "SpecularGlossinessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffDiv" ) );
			material.declMember< Vec4 >( cuT( "m_specGloss" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittanceProfile" ), 10u );
			material.end();
		}

		Vec3 SpecularGlossinessMaterial::m_diffuse()const
		{
			return m_diffDiv().rgb();
		}

		Vec3 SpecularGlossinessMaterial::m_specular()const
		{
			return m_specGloss().rgb();
		}

		Float SpecularGlossinessMaterial::m_glossiness()const
		{
			return m_specGloss().a();
		}

		Vec4 SpecularGlossinessMaterial::m_diffDiv()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffDiv" ) );
		}

		Vec4 SpecularGlossinessMaterial::m_specGloss()const
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specGloss" ) );
		}

		//*********************************************************************************************

		Materials::Materials( GlslWriter & writer )
			: m_writer{ writer }
		{
		}

		//*********************************************************************************************

		LegacyMaterials::LegacyMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void LegacyMaterials::declare()
		{
			LegacyMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				materials.declMemberArray< LegacyMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declSampler< SamplerBuffer >( cuT( "c3d_materials" ), 0u, 0u );
				m_getMaterial = m_writer.implementFunction< LegacyMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< LegacyMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_diffAmb() = texelFetch( c3d_materials, offset++ );
						result.m_specShin () = texelFetch( c3d_materials, offset++ );
						result.m_common () = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr () = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo () = texelFetch( c3d_materials, offset++ );

						for ( uint32_t i = 0; i < 10u; ++i )
						{
							result.m_transmittanceProfile()[i] = texelFetch( c3d_materials, offset++ );
						}

						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}

		LegacyMaterial LegacyMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
				return tmp[index - 1];
			}
			else
			{
				return m_getMaterial( index - 1 );
			}
		}

		BaseMaterialUPtr LegacyMaterials::getBaseMaterial( Int const & index )const
		{
			auto material = m_writer.declLocale( cuT( "material" )
				, getMaterial( index ) );
			return std::make_unique< LegacyMaterial >( material );
		}

		//*********************************************************************************************

		PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrMRMaterials::declare()
		{
			MetallicRoughnessMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				auto c3d_materials = materials.declMemberArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declSampler< SamplerBuffer >( cuT( "c3d_materials" ), 0u, 0u );
				m_getMaterial = m_writer.implementFunction< MetallicRoughnessMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< MetallicRoughnessMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_albRough() = texelFetch( c3d_materials, offset++ );
						result.m_metDiv() = texelFetch( c3d_materials, offset++ );
						result.m_common() = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr() = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo() = texelFetch( c3d_materials, offset++ );

						for ( uint32_t i = 0; i < 10u; ++i )
						{
							result.m_transmittanceProfile()[i] = texelFetch( c3d_materials, offset++ );
						}

						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}
		
		MetallicRoughnessMaterial PbrMRMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
				return tmp[index - 1];
			}
			else
			{
				return m_getMaterial( index - 1 );
			}
		}

		BaseMaterialUPtr PbrMRMaterials::getBaseMaterial( Int const & index )const
		{
			auto material = m_writer.declLocale( cuT( "material" )
				, getMaterial( index ) );
			return std::make_unique< MetallicRoughnessMaterial >( material );
		}

		//*********************************************************************************************

		PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrSGMaterials::declare()
		{
			SpecularGlossinessMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				materials.declMemberArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declSampler< SamplerBuffer >( cuT( "c3d_materials" ), 0u, 0u );
				m_getMaterial = m_writer.implementFunction< SpecularGlossinessMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< SpecularGlossinessMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_diffDiv() = texelFetch( c3d_materials, offset++ );
						result.m_specGloss() = texelFetch( c3d_materials, offset++ );
						result.m_common() = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr() = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo() = texelFetch( c3d_materials, offset++ );

						for ( uint32_t i = 0; i < 10u; ++i )
						{
							result.m_transmittanceProfile()[i] = texelFetch( c3d_materials, offset++ );
						}

						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}

		SpecularGlossinessMaterial PbrSGMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
				return tmp[index - 1];
			}
			else
			{
				return m_getMaterial( index - 1 );
			}
		}

		BaseMaterialUPtr PbrSGMaterials::getBaseMaterial( Int const & index )const
		{
			auto material = m_writer.declLocale( cuT( "material" )
				, getMaterial( index ) );
			return std::make_unique< SpecularGlossinessMaterial >( material );
		}

		//*********************************************************************************************
	}
}
