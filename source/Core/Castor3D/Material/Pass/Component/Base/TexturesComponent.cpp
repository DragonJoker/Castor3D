#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, TexturesComponent )

namespace castor3d
{
	//*********************************************************************************************

	void TexturesComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !surface )
		{
			return;
		}

		if ( surface->hasMember( "texture0" ) )
		{
			auto & cache = materials.getWriter()->getTypesCache();

			if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				components.declMember( "texCoords", shader::DerivTex::makeType( cache ) );
				components.declMember( "texture0", shader::DerivTex::makeType( cache ) );

				if ( surface->hasMember( "texture1" ) )
				{
					components.declMember( "texture1", shader::DerivTex::makeType( cache ) );
				}

				if ( surface->hasMember( "texture2" ) )
				{
					components.declMember( "texture2", shader::DerivTex::makeType( cache ) );
				}

				if ( surface->hasMember( "texture3" ) )
				{
					components.declMember( "texture3", shader::DerivTex::makeType( cache ) );
				}
			}
			else
			{
				components.declMember( "texCoords", sdw::type::Kind::eVec3F );
				components.declMember( "texture0", sdw::type::Kind::eVec3F );

				if ( surface->hasMember( "texture1" ) )
				{
					components.declMember( "texture1", sdw::type::Kind::eVec3F );
				}

				if ( surface->hasMember( "texture2" ) )
				{
					components.declMember( "texture2", sdw::type::Kind::eVec3F );
				}

				if ( surface->hasMember( "texture3" ) )
				{
					components.declMember( "texture3", sdw::type::Kind::eVec3F );
				}
			}
		}
		else if ( surface->hasMember( "texture" ) )
		{
			components.declMember( "texCoords", sdw::type::Kind::eVec2F );
			components.declMember( "texture0", sdw::type::Kind::eVec2F );
		}
	}

	void TexturesComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "texture0" ) )
		{
			return;
		}

		if ( components.hasMember( "texCoords" ) )
		{
			auto texType = components.getMember( "texCoords" ).type;
			inits.emplace_back( sdw::getZeroValue( texType ) );
		}

		auto texType = components.getMember( "texture0" ).type;

		if ( surface )
		{
			if ( surface->hasMember( "texture0" ) )
			{
				if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
				{
					inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture0" ) ) );

					if ( surface->hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture1" ) ) );
					}
					else if ( components.hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture2" ) ) );
					}
					else if ( components.hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< shader::DerivTex >( "texture3" ) ) );
					}
					else if ( components.hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}
				}
				else
				{
					inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture0" ) ) );

					if ( surface->hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture1" ) ) );
					}
					else if ( components.hasMember( "texture1" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture2" ) ) );
					}
					else if ( components.hasMember( "texture2" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}

					if ( surface->hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "texture3" ) ) );
					}
					else if ( components.hasMember( "texture3" ) )
					{
						inits.emplace_back( sdw::getZeroValue( texType ) );
					}
				}
			}
			else if ( surface->hasMember( "texture" ) )
			{
				inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec2 >( "texture" ) ) );
			}
			else
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}
		}
		else
		{
			inits.emplace_back( sdw::getZeroValue( texType ) );

			if ( components.hasMember( "texture1" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}

			if ( components.hasMember( "texture2" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}

			if ( components.hasMember( "texture3" ) )
			{
				inits.emplace_back( sdw::getZeroValue( texType ) );
			}
		}
	}

	void TexturesComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			if ( res.hasMember( "texture0" ) && src.hasMember( "texture0" ) )
			{
				res.getMember< shader::DerivTex >( "texture0", true ).value() += src.getMember< shader::DerivTex >( "texture0", true ).value() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture0", true ).dPdx() += src.getMember< shader::DerivTex >( "texture0", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture0", true ).dPdy() += src.getMember< shader::DerivTex >( "texture0", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture1" ) && src.hasMember( "texture1" ) )
			{
				res.getMember< shader::DerivTex >( "texture1", true ).value() += src.getMember< shader::DerivTex >( "texture1", true ).value() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture1", true ).dPdx() += src.getMember< shader::DerivTex >( "texture1", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture1", true ).dPdy() += src.getMember< shader::DerivTex >( "texture1", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture2" ) && src.hasMember( "texture2" ) )
			{
				res.getMember< shader::DerivTex >( "texture2", true ).value() += src.getMember< shader::DerivTex >( "texture2", true ).value() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture2", true ).dPdx() += src.getMember< shader::DerivTex >( "texture2", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture2", true ).dPdy() += src.getMember< shader::DerivTex >( "texture2", true ).dPdy() * passMultiplier;
			}

			if ( res.hasMember( "texture3" ) && src.hasMember( "texture3" ) )
			{
				res.getMember< shader::DerivTex >( "texture3", true ).value() += src.getMember< shader::DerivTex >( "texture3", true ).value() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture3", true ).dPdx() += src.getMember< shader::DerivTex >( "texture3", true ).dPdx() * passMultiplier;
				res.getMember< shader::DerivTex >( "texture3", true ).dPdy() += src.getMember< shader::DerivTex >( "texture3", true ).dPdy() * passMultiplier;
			}
		}
		else
		{
			res.getMember< sdw::Vec3 >( "texture0", true ) += src.getMember< sdw::Vec3 >( "texture0", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture1", true ) += src.getMember< sdw::Vec3 >( "texture1", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture2", true ) += src.getMember< sdw::Vec3 >( "texture2", true ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "texture3", true ) += src.getMember< sdw::Vec3 >( "texture3", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	bool TexturesComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return textures.configCount != 0;
	}

	//*********************************************************************************************

	castor::String const TexturesComponent::TypeName = C3D_MakePassBaseComponentName( "textures" );

	TexturesComponent::TexturesComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	PassComponentUPtr TexturesComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, TexturesComponent >( pass );
	}

	//*********************************************************************************************
}
