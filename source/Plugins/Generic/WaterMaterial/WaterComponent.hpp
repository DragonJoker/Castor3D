/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterComponent_H___
#define ___C3D_WaterComponent_H___

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace water
{
	struct WaterData
	{
		explicit WaterData( std::atomic_bool & dirty )
			: dampeningFactor{ dirty, castor::makeRangedValue( 5.0f, 0.0f, 100.0f ) }
			, depthSofteningDistance{ dirty, castor::makeRangedValue( 0.5f, 0.0f, 100.0f ) }
			, refractionDistortionFactor{ dirty, castor::makeRangedValue( 0.04f, 0.0f, 1.0f ) }
			, refractionHeightFactor{ dirty, castor::makeRangedValue( 2.5f, 0.0f, 100.0f ) }
			, refractionDistanceFactor{ dirty, castor::makeRangedValue( 15.0f, 0.0f, 100.0f ) }
			, noiseTiling{ dirty, castor::makeRangedValue( 1.0f, 0.0001f, 10.0f ) }
			, ssrStepSize{ dirty, castor::makeRangedValue( 0.5f, 0.0001f, 10.0f ) }
			, ssrForwardStepsCount{ dirty, castor::makeRangedValue( 20u, 1u, 100u ) }
			, ssrBackwardStepsCount{ dirty, castor::makeRangedValue( 10u, 1u, 100u ) }
			, ssrDepthMult{ dirty, castor::makeRangedValue( 20.0f, 0.0f, 100.0f ) }
			, foamHeightStart{ dirty, castor::makeRangedValue( 0.8f, 0.0f, 10.0f ) }
			, foamFadeDistance{ dirty, castor::makeRangedValue( 0.4f, 0.0f, 10.0f ) }
			, foamTiling{ dirty, castor::makeRangedValue( 2.0f, 0.0f, 20.0f ) }
			, foamAngleExponent{ dirty, castor::makeRangedValue( 80.0f, 0.0f, 100.0f ) }
			, foamBrightness{ dirty, castor::makeRangedValue( 4.0f, 0.0f, 100.0f ) }
			, foamNoiseTiling{ dirty, castor::makeRangedValue( 0.02f, 0.0f, 1.0f ) }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > dampeningFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > depthSofteningDistance;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionDistortionFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionHeightFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionDistanceFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > noiseTiling;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > ssrStepSize;
		castor::AtomicGroupChangeTracked< castor::RangedValue< uint32_t > > ssrForwardStepsCount;
		castor::AtomicGroupChangeTracked< castor::RangedValue< uint32_t > > ssrBackwardStepsCount;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > ssrDepthMult;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamHeightStart;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamFadeDistance;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamTiling;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamAngleExponent;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamBrightness;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > foamNoiseTiling;
	};

	struct WaterComponent
		: public castor3d::BaseDataPassComponentT< WaterData >
	{
		struct ComponentsShader
			: castor3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( castor3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, castor3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, castor3d::shader::Materials const & materials
				, castor3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( castor3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, castor3d::shader::BlendComponents & res
				, castor3d::shader::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		explicit WaterComponent( castor3d::Pass & pass );

		class Plugin
			: public castor3d::PassComponentPlugin
		{
		public:
			explicit Plugin( castor3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< castor3d::PassComponent, WaterComponent >( pass );
			}

			castor3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponents )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponents );
		}

		static bool isComponentAvailable( castor3d::ComponentModeFlags componentsMask
			, castor3d::shader::Materials const & materials );

		void accept( castor3d::ConfigurationVisitorBase & vis )override;

		void fillProfileBuffer( WaterProfileData & buffer )const;

		void setDampeningFactor( float value )
		{
			*m_value.dampeningFactor = value;
		}

		void setDepthSofteningDistance( float value )
		{
			*m_value.depthSofteningDistance = value;
		}

		void setRefractionDistortionFactor( float value )
		{
			*m_value.refractionDistortionFactor = value;
		}

		void setRefractionHeightFactor( float value )
		{
			*m_value.refractionHeightFactor = value;
		}

		void setRefractionDistanceFactor( float value )
		{
			*m_value.refractionDistanceFactor = value;
		}

		void setNoiseTiling( float value )
		{
			*m_value.noiseTiling = value;
		}

		void setSsrStepSize( float value )
		{
			*m_value.ssrStepSize = value;
		}

		void setSsrForwardStepsCount( uint32_t value )
		{
			*m_value.ssrForwardStepsCount = value;
		}

		void setSsrBackwardStepsCount( uint32_t value )
		{
			*m_value.ssrBackwardStepsCount = value;
		}

		void setSsrDepthMult( float value )
		{
			*m_value.ssrDepthMult = value;
		}

		void setFoamHeightStart( float value )
		{
			*m_value.foamHeightStart = value;
		}

		void setFoamFadeDistance( float value )
		{
			*m_value.foamFadeDistance = value;
		}

		void setFoamTiling( float value )
		{
			*m_value.foamTiling = value;
		}

		void setFoamNoiseTiling( float value )
		{
			*m_value.foamNoiseTiling = value;
		}

		void setFoamAngleExponent( float value )
		{
			*m_value.foamAngleExponent = value;
		}

		void setFoamBrightness( float value )
		{
			*m_value.foamBrightness = value;
		}

		float getDampeningFactor()const
		{
			return m_value.dampeningFactor.value().value();
		}

		float getDepthSofteningDistance()const
		{
			return m_value.depthSofteningDistance.value().value();
		}

		float getRefractionDistortionFactor()const
		{
			return m_value.refractionDistortionFactor.value().value();
		}

		float getRefractionHeightFactor()const
		{
			return m_value.refractionHeightFactor.value().value();
		}

		float getRefractionDistanceFactor()const
		{
			return m_value.refractionDistanceFactor.value().value();
		}

		float getNoiseTiling()const
		{
			return m_value.noiseTiling.value().value();
		}

		float getSsrStepSize()const
		{
			return m_value.ssrStepSize.value().value();
		}

		uint32_t getSsrForwardStepsCount()const
		{
			return m_value.ssrForwardStepsCount.value().value();
		}

		uint32_t getSsrBackwardStepsCount()const
		{
			return m_value.ssrBackwardStepsCount.value().value();
		}

		float getSsrDepthMult()const
		{
			return m_value.ssrDepthMult.value().value();
		}

		float getFoamHeightStart()const
		{
			return m_value.foamHeightStart.value().value();
		}

		float getFoamFadeDistance()const
		{
			return m_value.foamFadeDistance.value().value();
		}

		float getFoamTiling()const
		{
			return m_value.foamTiling.value().value();
		}

		float getFoamNoiseTiling()const
		{
			return m_value.foamNoiseTiling.value().value();
		}

		float getFoamAngleExponent()const
		{
			return m_value.foamAngleExponent.value().value();
		}

		float getFoamBrightness()const
		{
			return m_value.foamBrightness.value().value();
		}

		static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
	};

	CU_DeclareSmartPtr( water, WaterComponent, );
}

#endif
