/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ToonPass_H___
#define ___C3D_ToonPass_H___

#include <Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp>
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
	template< typename TypeT >
	class ToonPassT
		: public TypeT
	{
	public:
		ToonPassT( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );

		static castor::AttributeParsersBySection createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );

		void fillData( castor3d::PassBuffer::PassDataPtr & data )const;

		void setSmoothBandWidth( float value )
		{
			m_smoothBandWidth = value;
		}

		float getSmoothBandWidth()const
		{
			return m_smoothBandWidth;
		}

	protected:
		void doAccept( castor3d::PassVisitorBase & vis )override;
		void doAccept( castor3d::TextureConfiguration & config
			, castor3d::PassVisitorBase & vis )override;

	public:
		static castor::String const Name;

	private:
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
		~ToonPhongPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

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
		~ToonBlinnPhongPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};

	class ToonMetallicRoughnessPass
		: public ToonPassT< castor3d::MetallicRoughnessPbrPass >
	{
		using ToonPass = ToonPassT< castor3d::MetallicRoughnessPbrPass >;

	public:
		explicit ToonMetallicRoughnessPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonMetallicRoughnessPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonMetallicRoughnessPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};

	class ToonSpecularGlossinessPass
		: public ToonPassT< castor3d::SpecularGlossinessPbrPass >
	{
		using ToonPass = ToonPassT< castor3d::SpecularGlossinessPbrPass >;

	public:
		explicit ToonSpecularGlossinessPass( castor3d::Material & parent
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		ToonSpecularGlossinessPass( castor3d::Material & parent
			, castor3d::PassTypeID typeID
			, castor3d::PassFlags initialFlags = castor3d::PassFlag::eNone );
		~ToonSpecularGlossinessPass();

		static castor3d::PassSPtr create( castor3d::Material & parent );
		static castor::AttributeParsersBySection createParsers();
		static castor::StrUInt32Map createSections();

		void accept( castor3d::PassBuffer & buffer )const override;
		uint32_t getPassSectionID()const override;
		uint32_t getTextureSectionID()const override;
		bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;

	public:
		C3D_ToonMaterial_API static castor::String const Type;
	};
}

#include "ToonPass.inl"

#endif
