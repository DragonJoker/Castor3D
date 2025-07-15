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
			: dampeningFactor{ dirty, c3d::makeRangedValue( 5.0f, 0.0f, 100.0f ) }
			, depthSofteningDistance{ dirty, c3d::makeRangedValue( 0.5f, 0.0f, 100.0f ) }
			, refractionDistortionFactor{ dirty, c3d::makeRangedValue( 0.04f, 0.0f, 1.0f ) }
			, refractionHeightFactor{ dirty, c3d::makeRangedValue( 2.5f, 0.0f, 100.0f ) }
			, refractionDistanceFactor{ dirty, c3d::makeRangedValue( 15.0f, 0.0f, 100.0f ) }
			, noiseTiling{ dirty, c3d::makeRangedValue( 1.0f, 0.0001f, 10.0f ) }
			, ssrStepSize{ dirty, c3d::makeRangedValue( 0.5f, 0.0001f, 10.0f ) }
			, ssrForwardStepsCount{ dirty, c3d::makeRangedValue( 20u, 1u, 100u ) }
			, ssrBackwardStepsCount{ dirty, c3d::makeRangedValue( 10u, 1u, 100u ) }
			, ssrDepthMult{ dirty, c3d::makeRangedValue( 20.0f, 0.0f, 100.0f ) }
			, foamHeightStart{ dirty, c3d::makeRangedValue( 0.8f, 0.0f, 10.0f ) }
			, foamFadeDistance{ dirty, c3d::makeRangedValue( 0.4f, 0.0f, 10.0f ) }
			, foamTiling{ dirty, c3d::makeRangedValue( 2.0f, 0.0f, 20.0f ) }
			, foamAngleExponent{ dirty, c3d::makeRangedValue( 80.0f, 0.0f, 100.0f ) }
			, foamBrightness{ dirty, c3d::makeRangedValue( 4.0f, 0.0f, 100.0f ) }
			, foamNoiseTiling{ dirty, c3d::makeRangedValue( 0.02f, 0.0f, 1.0f ) }
		{
		}

		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > dampeningFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > depthSofteningDistance;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > refractionDistortionFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > refractionHeightFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > refractionDistanceFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > noiseTiling;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > ssrStepSize;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< uint32_t > > ssrForwardStepsCount;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< uint32_t > > ssrBackwardStepsCount;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > ssrDepthMult;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamHeightStart;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamFadeDistance;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamTiling;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamAngleExponent;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamBrightness;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > foamNoiseTiling;
	};

	struct WaterComponent
		: public c3d::BaseDataPassComponentT< WaterData >
	{
		struct ComponentsShader
			: c3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( c3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( c3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, c3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, c3d::shader::Materials const & materials
				, c3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( c3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, c3d::shader::BlendComponents & res
				, c3d::shader::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::shader::Material const & material
				, c3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		explicit WaterComponent( c3d::Pass & pass );

		class Plugin
			: public c3d::PassComponentPlugin
		{
		public:
			explicit Plugin( c3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< c3d::PassComponent, WaterComponent >( pass );
			}

			c3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return c3d::makeRawUnique< ComponentsShader >( *this );
			}
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponents )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponents );
		}

		static bool isComponentAvailable( c3d::ComponentModeFlags componentsMask
			, c3d::shader::Materials const & materials );

		void onAddToPass()const override;

		void accept( c3d::ConfigurationVisitorBase & vis )override;

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

		static c3d::String const TypeName;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		bool doWriteText( c3d::String const & tabs
			, c3d::Path const & folder
			, c3d::String const & subfolder
			, c3d::StringStream & file )const override;
	};

	CU_DeclareSmartPtr( water, WaterComponent, );
}

#endif
