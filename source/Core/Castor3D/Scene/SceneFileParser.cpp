#include "Castor3D/Scene/SceneFileParser.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

CU_ImplementSmartPtr( c3d, SceneFileParser )

namespace c3d
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
			result.overlays = makeUnique< OverlayContext >();
			result.gui = makeUnique< GuiContext >();
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
			Set< String > sections;
			Set< String > parsers;
			Set< String > keywords;
			keywords.emplace( "true" );
			keywords.emplace( "false" );
			keywords.emplace( "screen_size" );

			for ( uint32_t i = 0u; i < uint32_t( PixelFormat::eCOUNT ); ++i )
			{
				keywords.insert( getFormatName( PixelFormat( i ) ) );
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
							if ( param->getType() == ParameterType::eCheckedText )
							{
								for ( auto & [keyword, id] : static_cast< ParserParameter< ParameterType::eCheckedText > const & >( *param ).m_values )
								{
									keywords.insert( keyword );
								}
							}
							else if ( param->getType() == ParameterType::eBitwiseOred32BitsCheckedText )
							{
								for ( auto & [keyword, id] : static_cast< ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText > const & >( *param ).m_values )
								{
									keywords.insert( keyword );
								}
							}
							else if ( param->getType() == ParameterType::eBitwiseOred64BitsCheckedText )
							{
								for ( auto & [keyword, id] : static_cast< ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText > const & >( *param ).m_values )
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

	FileParserContextUPtr SceneFileParser::initialiseParser( Path const & path )
	{
		return doInitialiseParser( path );
	}

	FileParserContextUPtr SceneFileParser::doInitialiseParser( Path const & path )
	{
		auto result = FileParser::doInitialiseParser( path );
		auto & context = getData();
		File::listDirectoryFiles( path.getPath(), context.files, true );

		for ( auto const & fileName : context.files )
		{
			if ( fileName.getExtension() == cuT( "csna" ) )
			{
				context.csnaFiles.push_back( fileName );
			}
		}

		return result;
	}

	void SceneFileParser::doCleanupParser( PreprocessedFile & preprocessed )
	{
		auto & context = getData();
		context.csnaFiles.clear();
		context.files.clear();
		context.overlays = {};
		context.enableFullLoading = {};
	}

	void SceneFileParser::doValidate( PreprocessedFile & preprocessed )
	{
	}

	String SceneFileParser::doGetSectionName( SectionId section )const
	{
		String result;
		static const StrSectionIdMap baseSections{ registerSceneFileSections() };

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

	RawUniquePtr< FileParser > SceneFileParser::doCreateParser()const
	{
		return makeRawUnique< SceneFileParser >( *getEngine() );
	}
}
//****************************************************************************************************
