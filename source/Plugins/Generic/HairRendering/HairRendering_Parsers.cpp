#include "HairRendering/HairRendering_Parsers.hpp"

#include "HairRendering/HairMeshGenerator.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace hair
{
	namespace parse
	{
		struct Context
		{
			c3d::MeshContext * mesh{};
			c3d::Parameters parameters{};
		};

		enum class Section
			: c3d::SectionId
		{
			eRoot = c3d::makeSectionName( 'H', 'A', 'I', 'R', 'M', 'E', 'S', 'H' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserHairMesh, c3d::MeshContext, Context )
		{
			newBlockContext->mesh = blockContext;
		}
		CU_EndAttributePushNewBlock( Section::eRoot )

		static CU_ImplementAttributeParserBlock( parserHairMeshEnd, Context )
		{
			c3d::MeshFactory const & factory = getEngine( *blockContext->mesh )->getMeshFactory();
			auto generator = factory.create( Generator::Type );
			generator->generate( *blockContext->mesh->mesh
				, blockContext->parameters );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserFilePath, Context )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( auto value = params[0]->get< c3d::Path >();
				!value.empty() )
			{
				if ( c3d::File::fileExists( value ) )
					blockContext->parameters.add( Generator::ParamFilePath, value );
				else if ( c3d::File::fileExists( context.file.getPath() / value ) )
					blockContext->parameters.add( Generator::ParamFilePath, context.file.getPath() / value );
				else
					CU_ParsingError( cuT( "Couldn't find file " ) + value );
			}
			else
			{
				CU_ParsingError( cuT( "Path is empty" ) );
			}
		}
		CU_EndAttribute()
	}

	c3d::AttributeParsers createParsers()
	{
		c3d::AttributeParsers result;

		addParserT( result
			, c3d::CSCNSection::eMesh
			, parse::Section::eRoot
			, Generator::Type
			, &parse::parserHairMesh );
		addParserT( result
			, parse::Section::eRoot
			, c3d::CSCNSection::eMesh
			, cuT( "}" )
			, &parse::parserHairMeshEnd );
		addParserT( result
			, parse::Section::eRoot
			, Generator::ParamFilePath
			, &parse::parserFilePath
			, { c3d::makeParameter< c3d::ParameterType::ePath >() } );

		return result;
	}

	c3d::StrSectionIdMap createSections()
	{
		return
		{
			{ c3d::SectionId( parse::Section::eRoot ), Generator::Type },
		};
	}
}
