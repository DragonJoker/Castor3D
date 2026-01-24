#include "Castor3D/Scene/CscnImporterFile.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/Gui_Parsers.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace c3d
{
	namespace imprt
	{
		class FilteredPreprocessedFile
			: public PreprocessedFile
		{
		public:
			explicit FilteredPreprocessedFile( FileParser & parser )
				: PreprocessedFile{ parser }
			{
				m_curSections.emplace_back( SectionId( CSCNSection::eRoot ), false );
			}

			FilteredPreprocessedFile( FileParser & parser
				, FileParserContextUPtr context )
				: PreprocessedFile{ parser, c3d::move( context ) }
			{
				m_curSections.emplace_back( SectionId( CSCNSection::eRoot ), false );
			}

			SectionId getCurrentSection()const noexcept
			{
				return m_curSections.back().first;
			}

		protected:
			void doAddParserAction( Path file
				, uint64_t line
				, String name
				, SectionId section
				, ParserFunctionAndParams function
				, String params
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
					doAddAllowedParserAction( c3d::move( file )
						, line
						, name
						, section
						, c3d::move( function )
						, c3d::move( params )
						, implicit );
				}

				if ( name == cuT( "}" ) )
				{
					m_curSections.pop_back();
				}
			}

		private:
			virtual bool doFilterSectionOut( SectionId section )const noexcept = 0;
			virtual void doAddAllowedParserAction( Path file
				, uint64_t line
				, String name
				, SectionId section
				, ParserFunctionAndParams function
				, String params
				, bool implicit ) = 0;

			Vector< Pair< SectionId, bool > > m_curSections;
		};

		class FinalSceneFinder
			: public FilteredPreprocessedFile
		{
		public:
			using FilteredPreprocessedFile::FilteredPreprocessedFile;

			String const & getMainScene()const noexcept
			{
				return m_mainScene;
			}

		private:
			bool doFilterSectionOut( SectionId section )const noexcept override
			{
				return section != SectionId( CSCNSection::eWindow )
					&& section != SectionId( CSCNSection::eRenderTarget );
			}

			void doAddAllowedParserAction( Path
				, uint64_t
				, String name
				, SectionId
				, ParserFunctionAndParams function
				, String params
				, bool implicit )override
			{
				if ( name == cuT( "scene" ) )
				{
					m_mainScene = c3d::move( params );
				}
			}

			String m_mainScene;
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
			PreprocessedSceneFile( FileParser & parser
				, FileParserContextUPtr context
				, String mainSceneName
				, String prefix
				, Scene & scene )
				: FilteredPreprocessedFile{ parser, c3d::move( context ) }
				, m_mainSceneName{ c3d::move( mainSceneName ) }
				, m_scene{ &scene }
				, m_prefix{ c3d::move( prefix ) }
			{
			}

			uint32_t getCategory()const
			{
				uint32_t result{};

				switch ( getCurrentSection() )
				{
				case SectionId( CSCNSection::eSampler ):
					result = uint32_t( Category::eSampler );
					break;
				case SectionId( CSCNSection::eLight ):
				case SectionId( CSCNSection::eShadows ):
				case SectionId( CSCNSection::eLpv ):
				case SectionId( CSCNSection::eRaw ):
				case SectionId( CSCNSection::ePcf ):
				case SectionId( CSCNSection::eVsm ):
				case SectionId( CSCNSection::eLightGroup ):
				case SectionId( CSCNSection::eLightGroupShadows ):
				case SectionId( CSCNSection::eLightGroupShadowsLpv ):
				case SectionId( CSCNSection::eLightGroupShadowsRaw ):
				case SectionId( CSCNSection::eLightGroupShadowsPcf ):
				case SectionId( CSCNSection::eLightGroupShadowsVsm ):
					result = uint32_t( Category::eLight );
					break;
				case SectionId( CSCNSection::eNode ):
					result = uint32_t( Category::eNode );
					break;
				case SectionId( CSCNSection::eObject ):
				case SectionId( CSCNSection::eObjectMaterials ):
					result = uint32_t( Category::eObject );
					break;
				case SectionId( CSCNSection::eMesh ):
				case SectionId( CSCNSection::eSubmesh ):
				case SectionId( CSCNSection::eBillboard ):
				case SectionId( CSCNSection::eBillboardList ):
				case SectionId( CSCNSection::eParticleSystem ):
				case SectionId( CSCNSection::eParticle ):
				case SectionId( CSCNSection::eMeshDefaultMaterials ):
				case SectionId( CSCNSection::eSkeleton ):
				case SectionId( CSCNSection::eMeshAnimation ):
				case SectionId( CSCNSection::eSkeletonArmature ):
				case SectionId( CSCNSection::eSkeletonNode ):
				case SectionId( CSCNSection::eSkeletonBone ):
				case SectionId( CSCNSection::eSkeletonAnimation ):
				case SectionId( CSCNSection::eSkeletonAnimationObject ):
				case SectionId( CSCNSection::eSkeletonAnimationKeyframe ):
				case SectionId( CSCNSection::eSkeletonAnimationKeyframeObject ):
					result = uint32_t( Category::eMesh );
					break;
				case SectionId( CSCNSection::eMaterial ):
				case SectionId( CSCNSection::ePass ):
				case SectionId( CSCNSection::eTextureUnit ):
				case SectionId( CSCNSection::eShaderProgram ):
				case SectionId( CSCNSection::eShaderStage ):
				case SectionId( CSCNSection::eUBOVariable ):
				case SectionId( CSCNSection::eTextureAnimation ):
				case SectionId( CSCNSection::eTextureTransform ):
					result = uint32_t( Category::eMaterial );
					break;
				case SectionId( CSCNSection::eTexture ):
					result = uint32_t( Category::eTexture );
					break;
				case SectionId( CSCNSection::ePanelOverlay ):
				case SectionId( CSCNSection::eBorderPanelOverlay ):
				case SectionId( CSCNSection::eTextOverlay ):
					result = uint32_t( Category::eOverlay );
					break;
				case SectionId( GUISection::eGUI ):
				case SectionId( GUISection::eTheme ):
				case SectionId( GUISection::eButtonStyle ):
				case SectionId( GUISection::eEditStyle ):
				case SectionId( GUISection::eComboStyle ):
				case SectionId( GUISection::eListStyle ):
				case SectionId( GUISection::eSliderStyle ):
				case SectionId( GUISection::eStaticStyle ):
				case SectionId( GUISection::ePanelStyle ):
				case SectionId( GUISection::eProgressStyle ):
				case SectionId( GUISection::eExpandablePanelStyle ):
				case SectionId( GUISection::eFrameStyle ):
				case SectionId( GUISection::eScrollBarStyle ):
				case SectionId( GUISection::eButton ):
				case SectionId( GUISection::eStatic ):
				case SectionId( GUISection::eSlider ):
				case SectionId( GUISection::eComboBox ):
				case SectionId( GUISection::eListBox ):
				case SectionId( GUISection::eEdit ):
				case SectionId( GUISection::ePanel ):
				case SectionId( GUISection::eProgress ):
				case SectionId( GUISection::eExpandablePanel ):
				case SectionId( GUISection::eExpandablePanelHeader ):
				case SectionId( GUISection::eExpandablePanelExpand ):
				case SectionId( GUISection::eExpandablePanelContent ):
				case SectionId( GUISection::eFrame ):
				case SectionId( GUISection::eFrameContent ):
				case SectionId( GUISection::eBoxLayout ):
				case SectionId( GUISection::eLayoutCtrl ):
					result = uint32_t( Category::eGui );
					break;
				case SectionId( CSCNSection::eSceneImport ):
					result = uint32_t( Category::eImport );
					break;
				default:
					result = uint32_t( Category::eOther );
					break;
				}

				return result;
			}

			uint32_t getCategoryActionsCount( uint32_t category )const
			{
				return m_totalCat[category];
			}

			uint32_t incCategoryActions( uint32_t category, uint32_t count = 1u )
			{
				return m_currentCat[category] += count;
			}

			uint32_t getCategoriesCount()const
			{
				return uint32_t( std::count_if( m_totalCat.begin(), m_totalCat.end()
					, []( uint32_t value )
					{
						return value > 0u;
					} ) );
			}

			xchar const * getCategoryName( uint32_t section )const
			{
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
			bool doFilterSectionOut( SectionId section )const noexcept override
			{
				return section == SectionId( CSCNSection::eWindow )
					|| section == SectionId( CSCNSection::eFont )
					|| section == SectionId( CSCNSection::ePanelOverlay )
					|| section == SectionId( CSCNSection::eBorderPanelOverlay )
					|| section == SectionId( CSCNSection::eTextOverlay )
					|| section == SectionId( CSCNSection::eSkybox )
					|| section == SectionId( CSCNSection::eSsao )
					|| section == SectionId( CSCNSection::eHdrConfig )
					|| section == SectionId( CSCNSection::eVoxelConeTracing )
					|| section == SectionId( CSCNSection::eClusters )
					|| section == SectionId( CSCNSection::eColourGrading )
					|| section == SectionId( CSCNSection::eSdfFont );
			}

			void doAddAllowedParserAction( Path file
				, uint64_t line
				, String name
				, SectionId section
				, ParserFunctionAndParams function
				, String params
				, bool implicit )override
			{
				auto category = getCategory();
				m_totalCat[category]++;

				// If we are parsing the main imported scene, we replace it with the current scene
				if ( function.resultSection != section
					&& function.resultSection == SectionId( CSCNSection::eScene )
					&& name == cuT( "scene" )
					&& params == m_mainSceneName )
				{
					function.params = {};
					auto currentScene = m_scene;
					auto prefix = m_prefix;
					function.function = [currentScene, prefix]( FileParserContext & context
						, void * blockContext
						, ParserParameterArray const & )
						{
							auto rootContext = reinterpret_cast< RootContext * >( blockContext );
							auto newBlockContext = new SceneContext{};
							context.allocatedBlocks.emplace_back( newBlockContext
								, makeContextDeleter< SceneContext >() );

							newBlockContext->root = rootContext;
							newBlockContext->scene = currentScene;
							newBlockContext->prefix = getPrefix( *rootContext ) + prefix;
							newBlockContext->root->mapScenes.try_emplace( currentScene->getName(), newBlockContext->scene );
							newBlockContext->overlays = makeUnique< OverlayContext >();
							newBlockContext->overlays->root = rootContext;
							newBlockContext->overlays->scene = newBlockContext;

							context.pendingSection = SectionId( CSCNSection::eScene );
							context.pendingBlock = newBlockContext;
							return true;
						};
				}

				PreprocessedFile::doAddParserAction( c3d::move( file )
					, line
					, c3d::move( name )
					, section
					, c3d::move( function )
					, c3d::move( params )
					, implicit );
			}

		private:
			String m_mainSceneName;
			SceneRPtr m_scene;
			String m_prefix;
			Array< uint32_t, Category::eCount > m_totalCat{};
			Array< uint32_t, Category::eCount > m_currentCat{};
		};
	}

	MbString const CscnImporterFile::Name = "CSCN Importer";

	CscnImporterFile::CscnImporterFile( Engine & engine
		, Scene * scene
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
		: ImporterFile{ engine, scene, path, parameters, progress }
		, m_parser{ engine, progress }
	{
		// Find the main scene
		String mainSceneName;
		{
			SceneFileParser parser{ engine };
			imprt::FinalSceneFinder finder{ parser, parser.initialiseParser( path ) };
			parser.processFile( path, finder );
			mainSceneName = finder.getMainScene();
		}

		// Then import
		imprt::PreprocessedSceneFile preprocessed{ m_parser, m_parser.initialiseParser( path ), mainSceneName, getPrefix(), *scene };
		m_parser.processFile( path, preprocessed );


		if ( progress )
		{
			setProgressBarGlobalTitle( progress
				, cuT( "Importing scene file " ) + path.getFileName( true ) );
			stepProgressBarGlobalStartLocal( progress
				, cuT( "Preprocessing scene file" )
				, 1u );

			auto index = incProgressBarGlobalRange( progress
				, uint32_t( imprt::PreprocessedSceneFile::Category::eCount ) );
			auto actionConnection = preprocessed.onAction.connect( [progress, index, &preprocessed]( [[maybe_unused]] SectionId section
				, [[maybe_unused]] PreprocessedFile::Action const & action )
				{
					auto category = preprocessed.getCategory();
					auto status = preprocessed.incCategoryActions( category );
					auto total = preprocessed.getCategoryActionsCount( category );
					setProgressBarGlobalStep( progress
						, cuT( "Importing scene..." )
						, index + category );
					setProgressBarLocal( progress
						, preprocessed.getCategoryName( category )
						, string::toString( status ) + cuT( " / " ) + string::toString( total )
						, total
						, status );
				} );
		}

		if ( !preprocessed.parse() )
		{
			Logger::logError( cuT( "Can't import scene" ) );
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
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		return makeUniqueDerived< ImporterFile, CscnImporterFile >( engine, scene, path, parameters, progress );
	}

	//*********************************************************************************************
}
