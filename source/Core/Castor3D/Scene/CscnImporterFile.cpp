#include "Castor3D/Scene/CscnImporterFile.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/CmshImporter.hpp"
#include "Castor3D/Gui/Gui_Parsers.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	namespace import
	{
		class FilteredPreprocessedFile
			: public castor::PreprocessedFile
		{
		public:
			explicit FilteredPreprocessedFile( castor::FileParser & parser )
				: PreprocessedFile{ parser }
			{
				m_curSections.emplace_back( castor::SectionId( CSCNSection::eRoot ), false );
			}

			FilteredPreprocessedFile( castor::FileParser & parser
				, castor::FileParserContextUPtr context )
				: PreprocessedFile{ parser, castor::move( context ) }
			{
				m_curSections.emplace_back( castor::SectionId( CSCNSection::eRoot ), false );
			}

			castor::SectionId getCurrentSection()const noexcept
			{
				return m_curSections.back().first;
			}

		protected:
			void doAddParserAction( castor::Path file
				, uint64_t line
				, castor::String name
				, castor::SectionId section
				, castor::ParserFunctionAndParams function
				, castor::String params
				, bool implicit )final
			{
				if ( auto nextSection = function.resultSection;
					name != cuT( "}" ) && nextSection != m_curSections.back().first )
				{
					auto ignored = m_curSections.back().second;
					m_curSections.emplace_back( nextSection
						, ignored || doFilterSectionOut( nextSection ) );
				}

				if ( !m_curSections.back().second )
				{
					doAddAllowedParserAction( castor::move( file )
						, line
						, name
						, section
						, castor::move( function )
						, castor::move( params )
						, implicit );
				}

				if ( name == cuT( "}" ) )
				{
					m_curSections.pop_back();
				}
			}

		private:
			virtual bool doFilterSectionOut( castor::SectionId section )const noexcept = 0;
			virtual void doAddAllowedParserAction( castor::Path file
				, uint64_t line
				, castor::String name
				, castor::SectionId section
				, castor::ParserFunctionAndParams function
				, castor::String params
				, bool implicit ) = 0;

			castor::Vector< castor::Pair< castor::SectionId, bool > > m_curSections;
		};

		class FinalSceneFinder
			: public FilteredPreprocessedFile
		{
		public:
			explicit FinalSceneFinder( castor::FileParser & parser )
				: FilteredPreprocessedFile{ parser }
			{
			}

			FinalSceneFinder( castor::FileParser & parser
				, castor::FileParserContextUPtr context )
				: FilteredPreprocessedFile{ parser, castor::move( context ) }
			{
			}

			castor::String const & getMainScene()const noexcept
			{
				return m_mainScene;
			}

		private:
			bool doFilterSectionOut( castor::SectionId section )const noexcept override
			{
				return section != castor::SectionId( CSCNSection::eWindow )
					&& section != castor::SectionId( CSCNSection::eRenderTarget );
			}

			void doAddAllowedParserAction( castor::Path
				, uint64_t
				, castor::String name
				, castor::SectionId
				, castor::ParserFunctionAndParams function
				, castor::String params
				, bool implicit )override
			{
				if ( name == cuT( "scene" ) )
				{
					m_mainScene = castor::move( params );
				}
			}

			castor::String m_mainScene;
		};

		class PreprocessedSceneFile
			: public FilteredPreprocessedFile
		{
		public:
			enum Category : uint32_t
			{
				eTexture,
				eSampler,
				eMaterial,
				eMesh,
				eNode,
				eObject,
				eLight,
				eOverlay,
				eGui,
				eImport,
				eOther,
				eCount,
			};

		public:
			PreprocessedSceneFile( castor::FileParser & parser
				, castor::FileParserContextUPtr context
				, castor::String mainSceneName
				, castor::String prefix
				, Scene & scene )
				: FilteredPreprocessedFile{ parser, castor::move( context ) }
				, m_mainSceneName{ castor::move( mainSceneName ) }
				, m_scene{ &scene }
				, m_prefix{ castor::move( prefix ) }
			{
			}

			castor::SectionId getCategory( castor::String const & name
				, castor::SectionId curSection
				, castor::SectionId nextSection
				, bool implicit )
			{
				castor::SectionId result{};

				switch ( getCurrentSection() )
				{
				case uint32_t( CSCNSection::eSampler ):
					result = uint32_t( Category::eSampler );
					break;
				case uint32_t( CSCNSection::eLight ):
				case uint32_t( CSCNSection::eShadows ):
				case uint32_t( CSCNSection::eLpv ):
				case uint32_t( CSCNSection::eRaw ):
				case uint32_t( CSCNSection::ePcf ):
				case uint32_t( CSCNSection::eVsm ):
					result = uint32_t( Category::eLight );
					break;
				case uint32_t( CSCNSection::eNode ):
					result = uint32_t( Category::eNode );
					break;
				case uint32_t( CSCNSection::eObject ):
				case uint32_t( CSCNSection::eObjectMaterials ):
					result = uint32_t( Category::eObject );
					break;
				case uint32_t( CSCNSection::eMesh ):
				case uint32_t( CSCNSection::eSubmesh ):
				case uint32_t( CSCNSection::eBillboard ):
				case uint32_t( CSCNSection::eBillboardList ):
				case uint32_t( CSCNSection::eParticleSystem ):
				case uint32_t( CSCNSection::eParticle ):
				case uint32_t( CSCNSection::eMeshDefaultMaterials ):
				case uint32_t( CSCNSection::eSkeleton ):
				case uint32_t( CSCNSection::eMorphAnimation ):
					result = uint32_t( Category::eMesh );
					break;
				case uint32_t( CSCNSection::eMaterial ):
				case uint32_t( CSCNSection::ePass ):
				case uint32_t( CSCNSection::eTextureUnit ):
				case uint32_t( CSCNSection::eShaderProgram ):
				case uint32_t( CSCNSection::eShaderStage ):
				case uint32_t( CSCNSection::eUBOVariable ):
				case uint32_t( CSCNSection::eTextureAnimation ):
				case uint32_t( CSCNSection::eTextureTransform ):
					result = uint32_t( Category::eMaterial );
					break;
				case uint32_t( CSCNSection::eTexture ):
					result = uint32_t( Category::eTexture );
					break;
				case uint32_t( CSCNSection::ePanelOverlay ):
				case uint32_t( CSCNSection::eBorderPanelOverlay ):
				case uint32_t( CSCNSection::eTextOverlay ):
					result = uint32_t( Category::eOverlay );
					break;
				case uint32_t( GUISection::eGUI ):
				case uint32_t( GUISection::eTheme ):
				case uint32_t( GUISection::eButtonStyle ):
				case uint32_t( GUISection::eEditStyle ):
				case uint32_t( GUISection::eComboStyle ):
				case uint32_t( GUISection::eListStyle ):
				case uint32_t( GUISection::eSliderStyle ):
				case uint32_t( GUISection::eStaticStyle ):
				case uint32_t( GUISection::ePanelStyle ):
				case uint32_t( GUISection::eProgressStyle ):
				case uint32_t( GUISection::eExpandablePanelStyle ):
				case uint32_t( GUISection::eFrameStyle ):
				case uint32_t( GUISection::eScrollBarStyle ):
				case uint32_t( GUISection::eButton ):
				case uint32_t( GUISection::eStatic ):
				case uint32_t( GUISection::eSlider ):
				case uint32_t( GUISection::eComboBox ):
				case uint32_t( GUISection::eListBox ):
				case uint32_t( GUISection::eEdit ):
				case uint32_t( GUISection::ePanel ):
				case uint32_t( GUISection::eProgress ):
				case uint32_t( GUISection::eExpandablePanel ):
				case uint32_t( GUISection::eExpandablePanelHeader ):
				case uint32_t( GUISection::eExpandablePanelExpand ):
				case uint32_t( GUISection::eExpandablePanelContent ):
				case uint32_t( GUISection::eFrame ):
				case uint32_t( GUISection::eFrameContent ):
				case uint32_t( GUISection::eBoxLayout ):
				case uint32_t( GUISection::eLayoutCtrl ):
					result = uint32_t( Category::eGui );
					break;
				case uint32_t( CSCNSection::eSceneImport ):
					result = uint32_t( Category::eImport );
					break;
				default:
					result = uint32_t( Category::eOther );
					break;
				}

				return result;
			}

			uint32_t getCategoryActionsCount( castor::SectionId section )const
			{
				return m_totalCat[section];
			}

			uint32_t incCategoryActions( castor::SectionId section, uint32_t count = 1u )
			{
				return m_currentCat[section] += count;
			}

			uint32_t getCategoriesCount()const
			{
				return uint32_t( std::count_if( m_totalCat.begin(), m_totalCat.end()
					, []( uint32_t value )
					{
						return value > 0u;
					} ) );
			}

			castor::xchar const * getCategoryName( castor::SectionId section )const
			{
				using namespace castor3d;
				switch ( section )
				{
				case uint32_t( Category::eSampler ):
					return cuT( "Loading Samplers" );
				case uint32_t( Category::eLight ):
					return cuT( "Loading Lights" );
				case uint32_t( Category::eNode ):
					return cuT( "Loading Nodes" );
				case uint32_t( Category::eObject ):
					return cuT( "Loading Objects" );
				case uint32_t( Category::eMesh ):
					return cuT( "Loading Meshes" );
				case uint32_t( Category::eMaterial ):
					return cuT( "Loading Materials" );
				case uint32_t( Category::eTexture ):
					return cuT( "Loading Textures" );
				case uint32_t( Category::eOverlay ):
					return cuT( "Loading Overlays" );
				case uint32_t( Category::eImport ):
					return cuT( "Processing Imports" );
				case uint32_t( Category::eGui ):
					return cuT( "Loading GUI" );
				default:
					return cuT( "Others" );
				}
			}

		private:
			bool doFilterSectionOut( castor::SectionId section )const noexcept override
			{
				return section == castor::SectionId( CSCNSection::eWindow )
					|| section == castor::SectionId( CSCNSection::eFont )
					|| section == castor::SectionId( CSCNSection::ePanelOverlay )
					|| section == castor::SectionId( CSCNSection::eBorderPanelOverlay )
					|| section == castor::SectionId( CSCNSection::eTextOverlay )
					|| section == castor::SectionId( CSCNSection::eSkybox )
					|| section == castor::SectionId( CSCNSection::eSsao )
					|| section == castor::SectionId( CSCNSection::eHdrConfig )
					|| section == castor::SectionId( CSCNSection::eVoxelConeTracing )
					|| section == castor::SectionId( CSCNSection::eClusters )
					|| section == castor::SectionId( CSCNSection::eColourGrading )
					|| section == castor::SectionId( CSCNSection::eSdfFont );
			}

			void doAddAllowedParserAction( castor::Path file
				, uint64_t line
				, castor::String name
				, castor::SectionId section
				, castor::ParserFunctionAndParams function
				, castor::String params
				, bool implicit )override
			{
				auto nextSection = function.resultSection;
				auto category = getCategory( name
					, section
					, nextSection
					, implicit );
				m_totalCat[category]++;

				// If we are parsing the main imported scene, we replace it with the current scene
				if ( function.resultSection != section
					&& function.resultSection == castor::SectionId( CSCNSection::eScene )
					&& name == cuT( "scene" )
					&& params == m_mainSceneName )
				{
					function.params = {};
					auto currentScene = m_scene;
					auto prefix = m_prefix;
					function.function = [currentScene, prefix]( castor::FileParserContext & context
						, void * blockContext
						, castor::ParserParameterArray const & )
						{
							auto rootContext = reinterpret_cast< RootContext * >( blockContext );
							auto newBlockContext = new SceneContext{};
							context.allocatedBlocks.emplace_back( newBlockContext
								, castor::makeContextDeleter< SceneContext >() );

							newBlockContext->root = rootContext;
							newBlockContext->scene = currentScene;
							newBlockContext->prefix = getPrefix( *rootContext ) + prefix;
							newBlockContext->root->mapScenes.try_emplace( currentScene->getName(), newBlockContext->scene );
							newBlockContext->overlays = castor::makeUnique< OverlayContext >();
							newBlockContext->overlays->root = rootContext;
							newBlockContext->overlays->scene = newBlockContext;

							context.pendingSection = castor::SectionId( CSCNSection::eScene );
							context.pendingBlock = newBlockContext;
							return true;
						};
				}

				castor::PreprocessedFile::doAddParserAction( castor::move( file )
					, line
					, castor::move( name )
					, section
					, castor::move( function )
					, castor::move( params )
					, implicit );
			}

		private:
			castor::String m_mainSceneName;
			SceneRPtr m_scene;
			castor::String m_prefix;
			castor::Array< uint32_t, Category::eCount > m_totalCat{};
			castor::Array< uint32_t, Category::eCount > m_currentCat{};
		};
	}

	castor::MbString const CscnImporterFile::Name = "CSCN Importer";

	CscnImporterFile::CscnImporterFile( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
		: ImporterFile{ engine, scene, path, parameters, progress }
		, m_parser{ engine, progress }
	{
		// Find the main scene
		castor::String mainSceneName;
		{
			SceneFileParser parser{ engine };
			import::FinalSceneFinder finder{ parser, parser.initialiseParser( path ) };
			parser.processFile( path, finder );
			mainSceneName = finder.getMainScene();
		}

		// Then import
		import::PreprocessedSceneFile preprocessed{ m_parser, m_parser.initialiseParser( path ), mainSceneName, getPrefix(), *scene };
		m_parser.processFile( path, preprocessed );


		if ( progress )
		{
			setProgressBarGlobalTitle( progress
				, cuT( "Importing scene file " ) + path.getFileName( true ) );
			stepProgressBarGlobalStartLocal( progress
				, cuT( "Preprocessing scene file" )
				, 1u );

			auto index = incProgressBarGlobalRange( progress
				, uint32_t( import::PreprocessedSceneFile::Category::eCount ) );
			auto actionConnection = preprocessed.onAction.connect( [progress, index, &preprocessed]( castor::SectionId section
				, castor::PreprocessedFile::Action const & action )
				{
					section = preprocessed.getCategory( action.name, section, action.function.resultSection, action.implicit );
					auto status = preprocessed.incCategoryActions( section );
					auto total = preprocessed.getCategoryActionsCount( section );
					setProgressBarGlobalStep( progress
						, cuT( "Importing scene..." )
						, index + section );
					setProgressBarLocal( progress
						, preprocessed.getCategoryName( section )
						, castor::string::toString( status ) + cuT( " / " ) + castor::string::toString( total )
						, total
						, status );
				} );
		}

		if ( !preprocessed.parse() )
		{
			castor::Logger::logError( cuT( "Can't import scene" ) );
		}
	}

	CscnImporterFile::~CscnImporterFile()
	{
		for ( auto it = m_parser.scenesBegin(); it != m_parser.scenesEnd(); ++it )
		{
			if ( it->second != getScene() )
			{
				getOwner()->removeScene( it->first );
			}
		}
	}

	ImporterFileUPtr CscnImporterFile::create( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		return castor::makeUniqueDerived< ImporterFile, CscnImporterFile >( engine, scene, path, parameters, progress );
	}

	//*********************************************************************************************
}
