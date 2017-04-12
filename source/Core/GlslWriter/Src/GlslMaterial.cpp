#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace Castor;

namespace GLSL
{
	//*********************************************************************************************

	Materials::Materials( GlslWriter & p_writer
		, int p_offsetRR, int p_indexRR
		, int p_offsetEM, int p_indexEM
		, int p_offsetRF, int p_indexRF
		, int p_offsetRL, int p_indexRL
		, int p_offsetOP, int p_indexOP )
		: m_writer{ p_writer }
	{
		auto c3d_materials = m_writer.GetUniform< SamplerBuffer >( cuT( "c3d_materials" ) );
		
		m_int = m_writer.ImplementFunction< Int >( cuT( "Mat_GetInt" )
			, [this]( Int const & p_index, Int const & p_offset, Int const & p_id )
			{
				auto c3d_materials = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_writer.Return( Int( texelFetch( c3d_materials, p_index * DoGetMaterialSize() + p_offset )[p_index] ) );
			}, InInt{ &m_writer, cuT( "p_index" ) }
			, InInt{ &m_writer, cuT( "p_offset" ) }
			, InInt{ &m_writer, cuT( "p_id" ) } );

		m_float = m_writer.ImplementFunction< Float >( cuT( "Mat_GetFloat" )
			, [this]( Int const & p_index, Int const & p_offset, Int const & p_id )
			{
				auto c3d_materials = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_writer.Return( texelFetch( c3d_materials, p_index * DoGetMaterialSize() + p_offset )[p_index] );
			}, InInt{ &m_writer, cuT( "p_index" ) }
			, InInt{ &m_writer, cuT( "p_offset" ) }
			, InInt{ &m_writer, cuT( "p_id" ) } );

		m_vec3 = m_writer.ImplementFunction< Vec3 >( cuT( "Mat_GetVec3" )
			, [this]( Int const & p_index, Int const & p_offset, Int const & p_id )
			{
				auto c3d_materials = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_writer.Return( texelFetch( c3d_materials, p_index * DoGetMaterialSize() + p_offset ).xyz() );
			}, InInt{ &m_writer, cuT( "p_index" ) }
			, InInt{ &m_writer, cuT( "p_offset" ) }
			, InInt{ &m_writer, cuT( "p_id" ) } );

		m_refractionRatio = m_writer.ImplementFunction< Float >( cuT( "Mat_GetRefractionRatio" )
			, [this, p_offsetRR, p_indexRR]( Int const & p_index )
			{
				m_writer.Return( m_float( p_index, Int( p_offsetRR ), Int( p_indexRR ) ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_refraction = m_writer.ImplementFunction< Int >( cuT( "Mat_GetRefraction" )
			, [this, p_offsetRF, p_indexRF]( Int const & p_index )
			{
				m_writer.Return( m_int( p_index, Int( p_offsetRF ), Int( p_indexRF ) ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_reflection = m_writer.ImplementFunction< Int >( cuT( "Mat_GetReleaction" )
			, [this, p_offsetRL, p_indexRL]( Int const & p_index )
			{
				m_writer.Return( m_int( p_index, Int( p_offsetRL ), Int( p_indexRL ) ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_envMapIndex = m_writer.ImplementFunction< Int >( cuT( "Mat_GetEnvMapIndex" )
			, [this, p_offsetEM, p_indexEM]( Int const & p_index )
			{
				m_writer.Return( m_int( p_index, Int( p_offsetEM ), Int( p_indexEM ) ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_opacity = m_writer.ImplementFunction< Float >( cuT( "Mat_GetOpacity" )
			, [this, p_offsetOP, p_indexOP]( Int const & p_index )
			{
				m_writer.Return( m_float( p_index, Int( p_offsetOP ), Int( p_indexOP ) ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );
	}

	Float Materials::GetRefractionRatio( Int const & p_index )const
	{
		return m_refractionRatio( p_index );
	}

	Float Materials::GetOpacity( Int const & p_index )const
	{
		return m_opacity( p_index );
	}

	Int Materials::GetRefraction( Int const & p_index )const
	{
		return m_refraction( p_index );
	}

	Int Materials::GetReflection( Int const & p_index )const
	{
		return m_reflection( p_index );
	}

	Int Materials::GetEnvMapIndex( Int const & p_index )const
	{
		return m_envMapIndex( p_index );
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & p_writer )
		: Materials{ p_writer
			, 0, 0
			, 0, 1
			, 0, 2
			, 0, 3
			, 3, 1
		}
	{
		m_diffuse = m_writer.ImplementFunction< Vec3 >( cuT( "Mat_GetDiffuse" )
			, [this]( Int const & p_index )
			{
				m_writer.Return( m_vec3( p_index, 1_i, 0_i ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_ambient = m_writer.ImplementFunction< Float >( cuT( "Mat_GetAmbient" )
			, [this]( Int const & p_index )
			{
				m_writer.Return( m_float( p_index, 1_i, 3_i ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_specular = m_writer.ImplementFunction< Vec3 >( cuT( "Mat_GetSpecular" )
			, [this]( Int const & p_index )
			{
				m_writer.Return( m_vec3( p_index, 2_i, 0_i ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_emissive = m_writer.ImplementFunction< Float >( cuT( "Mat_GetEmissive" )
			, [this]( Int const & p_index )
			{
				m_writer.Return( m_float( p_index, 2_i, 3_i ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );

		m_shininess = m_writer.ImplementFunction< Float >( cuT( "Mat_GetShininess" )
			, [this]( Int const & p_index )
			{
				m_writer.Return( m_float( p_index, 3_i, 0_i ) );
			}, InInt{ &m_writer, cuT( "p_index" ) } );
	}

	Vec3 LegacyMaterials::GetDiffuse( Int const & p_index )const
	{
		return m_diffuse( p_index );
	}

	Float LegacyMaterials::GetAmbient( Int const & p_index )const
	{
		return m_ambient( p_index );
	}

	Vec3 LegacyMaterials::GetSpecular( Int const & p_index )const
	{
		return m_specular( p_index );
	}

	Float LegacyMaterials::GetEmissive( Int const & p_index )const
	{
		return m_emissive( p_index );
	}

	Float LegacyMaterials::GetShininess( Int const & p_index )const
	{
		return m_shininess( p_index );
	}

	//*********************************************************************************************
}
