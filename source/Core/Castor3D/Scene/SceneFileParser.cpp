#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

#define C3D_PrintParsers 0

CU_ImplementSmartPtr( castor3d, SceneFileParser )

namespace castor3d
{
	//*********************************************************************************************

	namespace scnfile
	{
		static RootContext createContext( Engine & engine )
		{
			RootContext result{};
			result.engine = &engine;
			result.logger = &engine.getLogger();
			result.gui.controls = &static_cast< ControlsManager & >( *engine.getUserInputListener() );
			result.gui.stylesHolder.push( result.gui.controls );
			return result;
		}
	}

	//****************************************************************************************************

	UInt32StrMap SceneFileParser::comparisonModes;

	SceneFileParser::SceneFileParser( Engine & engine )
		: OwnedBy< Engine >( engine )
		, DataHolderT< RootContext >{ scnfile::createContext( engine ) }
		, FileParser{ engine.getLogger(), getSceneFileRootSection(), &getData() }
	{
		for ( auto const & it : getEngine()->getAdditionalParsers() )
		{
			registerParsers( it.first, it.second );
		}

		registerParsers( "c3d.scene", createSceneFileParsers( engine ) );

#if C3D_PrintParsers
		std::set< castor::String > sections;
		std::set< castor::String > parsers;
		std::set< castor::String > keywords;
		keywords.insert( "true" );
		keywords.insert( "false" );
		keywords.insert( "screen_size" );

		for ( uint32_t i = 0u; i < uint32_t( castor::PixelFormat::eCount ); ++i )
		{
			keywords.insert( castor::getFormatName( castor::PixelFormat( i ) ) );
		}

		for ( auto & addParserIt : getAdditionalParsers() )
		{
			for ( auto & [_, section] : addParserIt.second.sections )
			{
				sections.insert( section );
			}

			for ( auto & [name, sectionsParsers] : addParserIt.second.parsers )
			{
				parsers.insert( name );

				for ( auto & [_, parser] : sectionsParsers )
				{
					for ( auto & param : parser.params )
					{
						if ( param->getType() == castor::ParameterType::eCheckedText )
						{
							for ( auto & [keyword, id] : static_cast< castor::ParserParameter< castor::ParameterType::eCheckedText > const & >( *param ).m_values )
							{
								keywords.insert( keyword );
							}
						}
						else if ( param->getType() == castor::ParameterType::eBitwiseOred32BitsCheckedText )
						{
							for ( auto & [keyword, id] : static_cast< castor::ParserParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText > const & >( *param ).m_values )
							{
								keywords.insert( keyword );
							}
						}
						else if ( param->getType() == castor::ParameterType::eBitwiseOred64BitsCheckedText )
						{
							for ( auto & [keyword, id] : static_cast< castor::ParserParameter< castor::ParameterType::eBitwiseOred64BitsCheckedText > const & >( *param ).m_values )
							{
								keywords.insert( keyword );
							}
						}
					}
				}
			}
		}

		log::debug << "Registered sections\n   ";
		for ( auto & section : sections )
		{
			log::debug << " " << section;
		}
		log::debug << std::endl << "Registered parsers\n   ";
		for ( auto & parser : parsers )
		{
			log::debug << " " << parser;
		}
		log::debug << std::endl << "Registered keywords\n   ";
		for ( auto & keyword : keywords )
		{
			log::debug << " " << keyword;
		}
		log::debug << std::endl;
#endif
	}

	castor::FileParserContextUPtr SceneFileParser::initialiseParser( castor::Path const & path )
	{
		return doInitialiseParser( path );
	}

	castor::FileParserContextUPtr SceneFileParser::doInitialiseParser( castor::Path const & path )
	{
		auto result = FileParser::doInitialiseParser( path );
		auto & context = getData();
		castor::File::listDirectoryFiles( path.getPath(), context.files, true );

		for ( auto fileName : context.files )
		{
			if ( fileName.getExtension() == "csna" )
			{
				context.csnaFiles.push_back( fileName );
			}
		}

		return result;
	}

	void SceneFileParser::doCleanupParser( castor::PreprocessedFile & preprocessed )
	{
		auto & context = getData();
		context.csnaFiles.clear();
		context.files.clear();
		context.overlays = {};
		context.enableFullLoading = {};
	}

	void SceneFileParser::doValidate( castor::PreprocessedFile & preprocessed )
	{
	}

	castor::String SceneFileParser::doGetSectionName( castor::SectionId section )const
	{
		castor::String result;
		static const std::map< uint32_t, castor::String > baseSections{ registerSceneFileSections() };
		auto it = baseSections.find( section );

		if ( it != baseSections.end() )
		{
			return it->second;
		}

		for ( auto const & parsers : getEngine()->getAdditionalParsers() )
		{
			auto sectionIt = parsers.second.sections.find( section );

			if ( sectionIt != parsers.second.sections.end() )
			{
				return sectionIt->second;
			}
		}

		CU_Failure( "Section not found" );
		return cuT( "unknown" );
	}

	std::unique_ptr< castor::FileParser > SceneFileParser::doCreateParser()const
	{
		return std::make_unique< SceneFileParser >( *getEngine() );
	}
}
//****************************************************************************************************
