/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToonPass_H___
#define ___C3D_ToonPass_H___

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#ifndef CU_PlatformWindows
#	define C3D_ToonMaterial_API
#else
#	ifdef ToonMaterial_EXPORTS
#		define C3D_ToonMaterial_API __declspec( dllexport )
#	else
#		define C3D_ToonMaterial_API __declspec( dllimport )
#	endif
#endif

namespace toon
{
	static float constexpr MinMaterialEdgeWidth = 0.001f;
	static float constexpr MaxMaterialEdgeWidth = 1000.0f;

	struct ToonProfileData
	{
		float edgeWidth;
		float depthFactor;
		float normalFactor;
		float objectFactor;
		castor::Point4f edgeColour;
		float smoothBand;
		castor::Point3f pad;
	};

	struct EdgesData
	{
		explicit EdgesData( std::atomic_bool & dirty )
			: edgeColour{ dirty, castor::HdrRgbaColour::fromPredefined( castor::PredefinedRgbaColour::eOpaqueBlack ) }
			, edgeWidth{ dirty, castor::makeRangedValue( 1.0f, MinMaterialEdgeWidth, MaxMaterialEdgeWidth ) }
			, depthFactor{ dirty, castor::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, normalFactor{ dirty, castor::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, objectFactor{ dirty, castor::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, smoothBand{ dirty, 1.0f }
		{
		}

		castor::AtomicGroupChangeTracked< castor::HdrRgbaColour > edgeColour;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > edgeWidth;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > depthFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > normalFactor;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > objectFactor;
		castor::AtomicGroupChangeTracked< float > smoothBand;
	};

	struct EdgesComponent
		: public castor3d::BaseDataPassComponentT< EdgesData >
	{
		struct ComponentsShader
			: castor3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			C3D_ToonMaterial_API void fillComponents( sdw::type::BaseStruct & components
				, castor3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_ToonMaterial_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, castor3d::shader::Materials const & materials
				, castor3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_ToonMaterial_API void blendComponents( castor3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, castor3d::shader::BlendComponents & res
				, castor3d::shader::BlendComponents const & src )const override;
		};

		C3D_ToonMaterial_API explicit EdgesComponent( castor3d::Pass & pass );

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

			castor3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponents )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponents );
		}

		C3D_ToonMaterial_API void accept( castor3d::PassVisitorBase & vis )override;

		C3D_ToonMaterial_API void fillProfileBuffer( ToonProfileData & buffer )const;

		void setSmoothBandWidth( float value )
		{
			m_value.smoothBand = value;
		}

		void setEdgeWidth( float value )
		{
			*m_value.edgeWidth = value;
		}

		void setDepthFactor( float value )
		{
			*m_value.depthFactor = value;
		}

		void setNormalFactor( float value )
		{
			*m_value.normalFactor = value;
		}

		void setObjectFactor( float value )
		{
			*m_value.objectFactor = value;
		}

		void setEdgeColour( castor::HdrRgbaColour const & value )
		{
			m_value.edgeColour = value;
		}

		float getSmoothBandWidth()const
		{
			return m_value.smoothBand.value();
		}

		float getEdgeWidth()const
		{
			return m_value.edgeWidth->value();
		}

		float getDepthFactor()const
		{
			return m_value.depthFactor->value();
		}

		float getNormalFactor()const
		{
			return m_value.normalFactor->value();
		}

		float getObjectFactor()const
		{
			return m_value.objectFactor->value();
		}

		castor::HdrRgbaColour getEdgeColour()const
		{
			return *m_value.edgeColour;
		}

		C3D_ToonMaterial_API static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
	};
}

#endif
