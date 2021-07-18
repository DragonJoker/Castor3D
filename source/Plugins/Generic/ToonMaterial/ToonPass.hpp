/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToonPass_H___
#define ___C3D_ToonPass_H___

#include <Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp>
#include <Castor3D/Material/Pass/Phong/PhongPass.hpp>

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
	class ToonPhongPass
		: public castor3d::PhongPass
	{
	public:
		explicit ToonPhongPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonPhongPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonPhongPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::AttributeParsersBySection createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

		void setSmoothBandWidth( float value )
		{
			m_smoothBandWidth = value;
		}

		void setEdgeWidth( float value )
		{
			*m_edgeWidth = value;
		}

		void setStepsCount( uint32_t value )
		{
			*m_stepsCount = value;
		}

		float getSmoothBandWidth()const
		{
			return m_smoothBandWidth;
		}

		float getEdgeWidth()const
		{
			return m_edgeWidth->value();
		}

		uint32_t getStepsCount()const
		{
			return m_stepsCount->value();
		}

	protected:
		void doAccept( castor3d::PassVisitorBase & vis )override;
		void doAccept( castor3d::TextureConfiguration & config
			, castor3d::PassVisitorBase & vis )override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
		static castor::String const Name;
		static float constexpr MinEdgeWidth = 0.001f;
		static float constexpr MaxEdgeWidth = 1000.0f;
		static uint32_t constexpr MinStepsCount = 2u;
		static uint32_t constexpr MaxStepsCount = 256u;

	private:
		castor::GroupChangeTracked< float > m_smoothBandWidth;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_edgeWidth;
		castor::GroupChangeTracked< castor::RangedValue< uint32_t > > m_stepsCount;
	};

	class ToonBlinnPhongPass
		: public ToonPhongPass
	{
	public:
		explicit ToonBlinnPhongPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonBlinnPhongPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonBlinnPhongPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::AttributeParsersBySection createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );
		static castor::StrUInt32Map createSections();

		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};

	class ToonMetallicRoughnessPass
		: public castor3d::MetallicRoughnessPbrPass
	{
	public:
		explicit ToonMetallicRoughnessPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonMetallicRoughnessPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonMetallicRoughnessPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::AttributeParsersBySection createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

		void setSmoothBandWidth( float value )
		{
			m_smoothBandWidth = value;
		}

		void setEdgeWidth( float value )
		{
			*m_edgeWidth = value;
		}

		void setStepsCount( uint32_t value )
		{
			*m_stepsCount = value;
		}

		float getSmoothBandWidth()const
		{
			return m_smoothBandWidth;
		}

		float getEdgeWidth()const
		{
			return m_edgeWidth->value();
		}

		uint32_t getStepsCount()const
		{
			return m_stepsCount->value();
		}

	protected:
		void doAccept( castor3d::PassVisitorBase & vis )override;
		void doAccept( castor3d::TextureConfiguration & config
			, castor3d::PassVisitorBase & vis )override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
		static float constexpr MinEdgeWidth = 0.001f;
		static float constexpr MaxEdgeWidth = 1000.0f;
		static uint32_t constexpr MinStepsCount = 2u;
		static uint32_t constexpr MaxStepsCount = 256u;

	private:
		castor::GroupChangeTracked< float > m_smoothBandWidth;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_edgeWidth;
		castor::GroupChangeTracked< castor::RangedValue< uint32_t > > m_stepsCount;
	};

	class ToonSpecularGlossinessPass
		: public castor3d::SpecularGlossinessPbrPass
	{
	public:
		explicit ToonSpecularGlossinessPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonSpecularGlossinessPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonSpecularGlossinessPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::AttributeParsersBySection createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

		void setSmoothBandWidth( float value )
		{
			m_smoothBandWidth = value;
		}

		void setEdgeWidth( float value )
		{
			*m_edgeWidth = value;
		}

		void setStepsCount( uint32_t value )
		{
			*m_stepsCount = value;
		}

		float getSmoothBandWidth()const
		{
			return m_smoothBandWidth;
		}

		float getEdgeWidth()const
		{
			return m_edgeWidth->value();
		}

		uint32_t getStepsCount()const
		{
			return m_stepsCount->value();
		}

	protected:
		void doAccept( castor3d::PassVisitorBase & vis )override;
		void doAccept( castor3d::TextureConfiguration & config
			, castor3d::PassVisitorBase & vis )override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
		static float constexpr MinEdgeWidth = 0.001f;
		static float constexpr MaxEdgeWidth = 1000.0f;
		static uint32_t constexpr MinStepsCount = 2u;
		static uint32_t constexpr MaxStepsCount = 256u;

	private:
		castor::GroupChangeTracked< float > m_smoothBandWidth;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_edgeWidth;
		castor::GroupChangeTracked< castor::RangedValue< uint32_t > > m_stepsCount;
	};
}

#endif
