#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"

#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		TextureAnimData::TextureAnimData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, texTrn{ getMember< sdw::Vec4 >( "texTrn" ) }
			, texRot{ getMember< sdw::Vec4 >( "texRot" ) }
			, texScl{ getMember< sdw::Vec4 >( "texScl" ) }
			, tleSet{ getMember< sdw::Vec4 >( "tleSet" ) }
		{
		}

		std::unique_ptr< sdw::Struct > TextureAnimData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
		}

		ast::type::BaseStructPtr TextureAnimData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_TextureAnimData" );

			if ( result->empty() )
			{
				result->declMember( "texTrn", ast::type::Kind::eVec4F );
				result->declMember( "texRot", ast::type::Kind::eVec4F );
				result->declMember( "texScl", ast::type::Kind::eVec4F );
				result->declMember( "tleSet", ast::type::Kind::eVec4F );
			}

			return result;
		}

		//*********************************************************************************************

		TextureAnimations::TextureAnimations( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void TextureAnimations::declare( uint32_t binding
			, uint32_t set )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< TextureAnimData > >( m_writer
				, TextureAnimationBufferName
				, binding
				, set
				, true );
		}

		TextureAnimData TextureAnimations::getTextureAnimation( sdw::UInt const & index )const
		{
			return ( *m_ssbo )[index - 1_u];
		}

		//*********************************************************************************************
	}
}
