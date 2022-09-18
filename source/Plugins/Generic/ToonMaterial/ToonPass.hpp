/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToonPass_H___
#define ___C3D_ToonPass_H___

#include <Castor3D/Material/Pass/PBR/PbrPass.hpp>
#include <Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp>
#include <Castor3D/Material/Pass/Phong/PhongPass.hpp>

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

	template< typename TypeT >
	class ToonPassT
		: public TypeT
	{
	public:
		ToonPassT( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );

		static castor::AttributeParsers createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID
			, uint32_t remapSectionID
			, castor::String const & remapSectionName
			, uint32_t remapChannelSectionID );

		void fillData( ToonProfileData & data )const;
		void cloneData( ToonPassT & result )const;

		void setSmoothBandWidth( float value )
		{
			*m_smoothBandWidth = value;
		}

		void setEdgeWidth( float value )
		{
			*m_edgeWidth = value;
		}

		void setDepthFactor( float value )
		{
			*m_depthFactor = value;
		}

		void setNormalFactor( float value )
		{
			*m_normalFactor = value;
		}

		void setObjectFactor( float value )
		{
			*m_objectFactor = value;
		}

		void setEdgeColour( castor::RgbaColour const & value )
		{
			m_edgeColour = value;
		}

		float getSmoothBandWidth()const
		{
			return m_smoothBandWidth.value();
		}

		float getEdgeWidth()const
		{
			return m_edgeWidth->value();
		}

		float getDepthFactor()const
		{
			return m_depthFactor->value();
		}

		float getNormalFactor()const
		{
			return m_normalFactor->value();
		}

		float getObjectFactor()const
		{
			return m_objectFactor->value();
		}

		castor::RgbaColour getEdgeColour()const
		{
			return *m_edgeColour;
		}

	protected:
		void doAccept( castor3d::PassVisitorBase & vis )override;
		void doAccept( castor3d::TextureConfiguration & config
			, castor3d::PassVisitorBase & vis )override;

	public:
		static castor::String const Name;

	protected:
		castor::GroupChangeTracked< castor::RgbaColour > m_edgeColour;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_edgeWidth;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_depthFactor;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_normalFactor;
		castor::GroupChangeTracked< castor::RangedValue< float > > m_objectFactor;
		castor::GroupChangeTracked< float > m_smoothBandWidth;
	};

	class ToonPhongPass
		: public ToonPassT< castor3d::PhongPass >
	{
		using ToonPass = ToonPassT< castor3d::PhongPass >;

	public:
		explicit ToonPhongPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonPhongPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsers createParsers();
		static castor::StrUInt32Map createSections();

		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	private:
		castor3d::PassSPtr doClone( castor3d::Material & material )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};

	class ToonBlinnPhongPass
		: public ToonPassT< castor3d::BlinnPhongPass >
	{
		using ToonPass = ToonPassT< castor3d::BlinnPhongPass >;

	public:
		explicit ToonBlinnPhongPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonBlinnPhongPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsers createParsers();
		static castor::StrUInt32Map createSections();

		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	private:
		castor3d::PassSPtr doClone( castor3d::Material & material )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};

	class ToonPbrPass
		: public ToonPassT< castor3d::PbrPass >
	{
		using ToonPass = ToonPassT< castor3d::PbrPass >;

	public:
		explicit ToonPbrPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonPbrPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsers createParsers();
		static castor::StrUInt32Map createSections();

		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	private:
		castor3d::PassSPtr doClone( castor3d::Material & material )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};
}

#include "ToonPass.inl"

#endif
