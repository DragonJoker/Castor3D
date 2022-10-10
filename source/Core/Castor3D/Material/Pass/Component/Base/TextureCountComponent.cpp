#include "Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor3d
{
	//*********************************************************************************************

	TextureCountComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{

	}

	void TextureCountComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "textureCount" ) )
		{
			type.declMember( "textureCount", ast::type::Kind::eUInt );
			inits.emplace_back( makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void TextureCountComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk()
			, 0u
			, 0u );
	}

	//*********************************************************************************************

	castor::String const TextureCountComponent::TypeName = C3D_MakePassComponentName( "texcount" );

	TextureCountComponent::TextureCountComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	void TextureCountComponent::accept( PassVisitorBase & vis )
	{
	}

	PassComponentUPtr TextureCountComponent::doClone( Pass & pass )const
	{
		return std::make_unique< TextureCountComponent >( pass );
	}

	void TextureCountComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, getOwner()->getTextureUnitsCount()
			, 0u );
	}

	//*********************************************************************************************
}
