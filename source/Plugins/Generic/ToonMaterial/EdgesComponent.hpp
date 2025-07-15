/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToonPass_H___
#define ___C3D_ToonPass_H___

#include "Shaders/GlslToonLighting.hpp"

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

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
		c3d::Point4f edgeColour;
		float smoothBand;
		c3d::Point3f pad;
	};

	struct EdgesData
	{
		explicit EdgesData( std::atomic_bool & dirty )
			: edgeColour{ dirty, c3d::HdrRgbaColour::fromPredefined( c3d::PredefinedRgbaColour::eOpaqueBlack ) }
			, edgeWidth{ dirty, c3d::makeRangedValue( 1.0f, MinMaterialEdgeWidth, MaxMaterialEdgeWidth ) }
			, depthFactor{ dirty, c3d::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, normalFactor{ dirty, c3d::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, objectFactor{ dirty, c3d::makeRangedValue( 1.0f, 0.0f, 1.0f ) }
			, smoothBand{ dirty, 1.0f }
		{
		}

		c3d::AtomicGroupChangeTracked< c3d::HdrRgbaColour > edgeColour;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > edgeWidth;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > depthFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > normalFactor;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > objectFactor;
		c3d::AtomicGroupChangeTracked< float > smoothBand;
	};

	struct EdgesComponent
		: public c3d::BaseDataPassComponentT< EdgesData >
	{
		struct ComponentsShader
			: c3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( c3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			C3D_ToonMaterial_API void fillComponents( c3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, c3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_ToonMaterial_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, c3d::shader::Materials const & materials
				, c3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_ToonMaterial_API void blendComponents( c3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, c3d::shader::BlendComponents & res
				, c3d::shader::BlendComponents const & src )const override;
		};

		C3D_ToonMaterial_API explicit EdgesComponent( c3d::Pass & pass );

		class Plugin
			: public c3d::PassComponentPlugin
		{
		public:
			explicit Plugin( c3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< c3d::PassComponent, EdgesComponent >( pass );
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;

			c3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return c3d::makeRawUnique< ComponentsShader >( *this );
			}
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponents )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponents );
		}

		C3D_ToonMaterial_API void accept( c3d::ConfigurationVisitorBase & vis )override;

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

		void setEdgeColour( c3d::HdrRgbaColour const & value )
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

		c3d::HdrRgbaColour getEdgeColour()const
		{
			return *m_value.edgeColour;
		}

		C3D_ToonMaterial_API static c3d::String const TypeName;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		bool doWriteText( c3d::String const & tabs
			, c3d::Path const & folder
			, c3d::String const & subfolder
			, c3d::StringStream & file )const override;
	};

	CU_DeclareSmartPtr( toon, EdgesComponent, C3D_ToonMaterial_API );
}

#endif
