#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"
#include "DepthOfFieldPostEffect/DepthOfFieldPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParserData.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

namespace dof
{
	//*********************************************************************************************

	namespace parse
	{
		class TextWriter
			: public castor::TextWriterT< DepthOfFieldConfig >
		{
		public:
			explicit TextWriter( castor::String const & tabs )
				: TextWriterT< DepthOfFieldConfig >{ tabs }
			{
			}

			bool operator()( DepthOfFieldConfig const & object
				, castor::StringStream & file )override
			{
				castor3d::log::info << cuT( "Writing Depth of Field" ) << std::endl;
				bool result = false;

				if ( auto block{ beginBlock( file, cuT( "depth_of_field" ) ) } )
				{
					result = writeOpt( file, cuT( "focal_distance" ), object.focalDistance.value(), 10.0f )
						&& writeOpt( file, cuT( "focal_length" ), object.focalLength.value(), 1.0f )
						&& writeOpt( file, cuT( "bokeh_scale" ), object.bokehScale.value(), 1.0f )
						&& writeOpt( file, cuT( "enable_far_blur" ), object.enableFarBlur, true );
				}

				return result;
			}
		};

		enum class Section
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'D', 'O', 'F', 'R' ),
		};

		struct DoFContext
		{
			castor3d::TargetContext * target{};
			DepthOfFieldUboConfiguration config{};
		};

		static CU_ImplementAttributeParserNewBlock( parserDepthOfField, castor3d::TargetContext, DoFContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}

			newBlockContext->target = blockContext;
		}
		CU_EndAttributePushNewBlock( Section::eRoot )

		static CU_ImplementAttributeParserBlock( parserFocalDistance, DoFContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->config.focalDistance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFocalLength, DoFContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->config.focalLength );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBokehScale, DoFContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->config.bokehScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnableFarBlur, DoFContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->config.enableFarBlur = params[0]->get< bool >() ? 1u : 0u;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, DoFContext )
		{
			if ( blockContext->target
				&& blockContext->target->renderTarget )
			{
				castor3d::Parameters parameters;
				parameters.add( PostEffect::FocalDistance, blockContext->config.focalDistance );
				parameters.add( PostEffect::FocalLength, blockContext->config.focalLength );
				parameters.add( PostEffect::BokehScale, blockContext->config.bokehScale );
				parameters.add( PostEffect::BokehScale, blockContext->config.enableFarBlur );

				auto effect = blockContext->target->renderTarget->getPostEffect( PostEffect::Type );
				effect->enable( true );
				effect->setParameters( parameters );
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void DepthOfFieldConfig::setParameters( castor3d::Parameters parameters )
	{
		float value{};
		uint32_t enable{};

		if ( parameters.get( PostEffect::FocalDistance, value ) )
		{
			focalDistance = value;
		}

		if ( parameters.get( PostEffect::FocalLength, value ) )
		{
			focalLength = value;
		}

		if ( parameters.get( PostEffect::BokehScale, value ) )
		{
			bokehScale = value;
		}

		if ( parameters.get( PostEffect::BokehScale, enable ) )
		{
			enableFarBlur = enable != 0u;
		}
	}

	void DepthOfFieldConfig::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Depth Of Field" ) );
		visitor.visit( cuT( "Focal Distance" ), focalDistance );
		visitor.visit( cuT( "Focal Length" ), focalLength );
		visitor.visit( cuT( "Bokeh Scale" ), bokehScale );
		visitor.visit( cuT( "Enable Far Blur" ), enableFarBlur );
	}

	bool DepthOfFieldConfig::write( castor::StringStream & file, castor::String const & tabs )const
	{
		return dof::parse::TextWriter{ tabs }( *this, file );
	}

	castor::AttributeParsers DepthOfFieldConfig::createParsers()
	{
		using namespace castor;
		AttributeParsers result;

		addParserT( result, castor3d::CSCNSection::eRenderTarget, parse::Section::eRoot, cuT( "depth_of_field" ), parse::parserDepthOfField );
		addParserT( result, parse::Section::eRoot, cuT( "focal_distance" ), parse::parserFocalDistance, { makeDefaultedParameter< ParameterType::eFloat >( 10.0f ) } );
		addParserT( result, parse::Section::eRoot, cuT( "focal_length" ), parse::parserFocalLength, { makeDefaultedParameter< ParameterType::eFloat >( 1.0f ) } );
		addParserT( result, parse::Section::eRoot, cuT( "bokeh_scale" ), parse::parserBokehScale, { makeDefaultedParameter< ParameterType::eFloat >( 1.0f ) } );
		addParserT( result, parse::Section::eRoot, cuT( "enable_far_blur" ), parse::parserEnableFarBlur, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParserT( result, parse::Section::eRoot, castor3d::CSCNSection::eRenderTarget, cuT( "}" ), parse::parserEnd );

		return result;
	}

	castor::StrUInt32Map DepthOfFieldConfig::createSections()
	{
		return
		{
			{ uint32_t( parse::Section::eRoot ), PostEffect::Type },
		};
	}

	//*********************************************************************************************

	castor::MbString const DepthOfFieldUbo::Buffer = "DepthOfField";
	castor::MbString const DepthOfFieldUbo::Data = "c3d_dofData";

	DepthOfFieldUbo::DepthOfFieldUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	DepthOfFieldUbo::~DepthOfFieldUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void DepthOfFieldUbo::cpuUpdate( DepthOfFieldConfig const & data )
	{
		struct Points
		{
			std::array< castor::Point4f, 64 > points64;
			std::array< castor::Point4f, 16 > points16;

			Points()
			{
				float constexpr goldenAngle = 2.39996323f;
				size_t idx64 = 0;
				size_t idx16 = 0;

				for ( int j = 0; j < 80; ++j )
				{
					auto theta = float( j ) * goldenAngle;
					auto r = float( sqrt( j ) / sqrt( 80.0 ) );

					castor::Point4f p{ r * cos( theta ), r * sin( theta ), 0.0f, 0.0f };

					if ( j % 5 == 0 )
					{
						points16[idx16] = p;
						++idx16;
					}
					else
					{
						points64[idx64] = p;
						++idx64;
					}
				}
			}
		};
		static Points points;

		auto & dst = m_ubo.getData();
		dst.focalDistance = data.focalDistance.value();
		dst.focalLength = data.focalLength.value();
		dst.bokehScale = data.bokehScale.value();
		dst.pixelStepFull = data.pixelStepFull;
		dst.pixelStepHalf = data.pixelStepHalf;
		dst.enableFarBlur = data.enableFarBlur ? 1u : 0u;
		dst.points16 = points.points16;
		dst.points64 = points.points64;
	}

	//*********************************************************************************************
}
