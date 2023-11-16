/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterComponent_H___
#define ___C3D_WaterComponent_H___

#include "Shaders/GlslWaterLighting.hpp"

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace water
{
	struct SsrConfiguration
	{
		float stepSize{ 0.5f };
		uint32_t forwardStepsCount{ 20u };
		uint32_t backwardStepsCount{ 10u };
		float depthMult{ 20.0f };
	};
	struct WaterProfileData
	{
		float dampeningFactor{ 5.0f };
		float depthSofteningDistance{ 0.5f };
		float refractionDistortionFactor{ 0.04f };
		float refractionHeightFactor{ 2.5f };
		float refractionDistanceFactor{ 15.0f };
		float pad0{};
		float pad1{};
		float pad2{};
		SsrConfiguration ssr;
	};

	struct WaterData
	{
		explicit WaterData( std::atomic_bool & dirty )
			: dampeningFactor{ dirty, castor::makeRangedValue( 5.0f, 0.0f, 100.0f ) }
			, depthSofteningDistance{ dirty, castor::makeRangedValue( 0.5f, 0.0f, 100.0f ) }
			, refractionDistortionFactor{ dirty, castor::makeRangedValue( 0.04f, 0.0f, 1.0f ) }
			, refractionHeightFactor{ dirty, castor::makeRangedValue( 2.5f, 0.0f, 100.0f ) }
			, refractionDistanceFactor{ dirty, castor::makeRangedValue( 15.0f, 0.0f, 100.0f ) }
			, ssrStepSize{ dirty, castor::makeRangedValue( 0.5f, 0.0001f, 10.0f ) }
			, ssrForwardStepsCount{ dirty, castor::makeRangedValue( 20u, 1u, 100u ) }
			, ssrBackwardStepsCount{ dirty, castor::makeRangedValue( 10u, 1u, 100u ) }
			, ssrDepthMult{ dirty, castor::makeRangedValue( 20.0f, 0.0f, 100.0f ) }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > dampeningFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > depthSofteningDistance;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionDistortionFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionHeightFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > refractionDistanceFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > ssrStepSize;
		castor::AtomicGroupChangeTracked< castor::RangedValue< uint32_t > > ssrForwardStepsCount;
		castor::AtomicGroupChangeTracked< castor::RangedValue< uint32_t > > ssrBackwardStepsCount;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > ssrDepthMult;
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

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< castor3d::PassComponent, WaterComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;

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
