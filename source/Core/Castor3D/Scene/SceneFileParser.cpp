#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

CU_ImplementSmartPtr( castor3d, SceneFileParser )

namespace castor3d
{
	//*********************************************************************************************

	namespace scnfile
	{
		static bool constexpr C3D_PrintParsers = false;

		static RootContext createContext( Engine & engine )
		{
			RootContext result{};
			result.engine = &engine;
			result.logger = &engine.getLogger();
			result.overlays = castor::makeUnique< OverlayContext >();
			result.gui = castor::makeUnique< GuiContext >();
			result.gui->controls = &static_cast< ControlsManager & >( *engine.getUserInputListener() );
			result.gui->stylesHolder.push( result.gui->controls );
			return result;
		}
	}

	//****************************************************************************************************

	CU_ImplementAttributeParser( parserDefaultEnd )
	{
		// Nothing else to do than push the block
	}
	CU_EndAttributePop()

	//****************************************************************************************************

	SceneFileParser::SceneFileParser( Engine & engine
		, ProgressBar * progress )
		: OwnedBy< Engine >( engine )
		, DataHolderT< RootContext >{ scnfile::createContext( engine ) }
		, FileParser{ engine.getLogger(), getSceneFileRootSection(), &getData() }
	{
		getData().overlays->root = &getData();
		getData().gui->root = &getData();
		getData().progress = progress;

		for ( auto const & [name, parsers] : getEngine()->getAdditionalParsers() )
		{
			registerParsers( name, parsers );
		}

		registerParsers( cuT( "c3d.scene" ), createSceneFileParsers( engine ) );

		if constexpr ( scnfile::C3D_PrintParsers )
		{
			castor::Set< castor::String > sections;
			castor::Set< castor::String > parsers;
			castor::Set< castor::String > keywords;
			keywords.emplace( "true" );
			keywords.emplace( "false" );
			keywords.emplace( "screen_size" );

			for ( uint32_t i = 0u; i < uint32_t( castor::PixelFormat::eCount ); ++i )
			{
				keywords.insert( castor::getFormatName( castor::PixelFormat( i ) ) );
			}

			for ( auto & [_, addParsers] : getAdditionalParsers() )
			{
				for ( auto & [a, section] : addParsers.sections )
				{
					sections.insert( section );
				}

				for ( auto & [name, sectionsParsers] : addParsers.parsers )
				{
					parsers.insert( name );

					for ( auto & [b, parser] : sectionsParsers )
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
		}
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

		for ( auto const & fileName : context.files )
		{
			if ( fileName.getExtension() == cuT( "csna" ) )
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
		static const castor::Map< uint32_t, castor::String > baseSections{ registerSceneFileSections() };

		if ( auto it = baseSections.find( section );
			it != baseSections.end() )
		{
			return it->second;
		}

		for ( auto const & [_, parsers] : getEngine()->getAdditionalParsers() )
		{
			auto sectionIt = parsers.sections.find( section );

			if ( sectionIt != parsers.sections.end() )
			{
				return sectionIt->second;
			}
		}

		CU_Failure( "Section not found" );
		return cuT( "unknown" );
	}

	castor::RawUniquePtr< castor::FileParser > SceneFileParser::doCreateParser()const
	{
		return castor::make_unique< SceneFileParser >( *getEngine() );
	}
}
//****************************************************************************************************
