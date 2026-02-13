/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMaterialsFile___
#define ___C3D_GltfMaterialsFile___

#include <Castor3D/Material/MaterialModule.hpp>
#include <Castor3D/Miscellaneous/MiscellaneousModule.hpp>

#include <Castor3D/ImporterFile.hpp>

#ifndef CU_PlatformWindows
#	define C3D_GltfMat_API
#else
#	ifdef GltfMaterialImporter_EXPORTS
#		define C3D_GltfMat_API __declspec(dllexport)
#	else
#		define C3D_GltfMat_API __declspec(dllimport)
#	endif
#endif

namespace fastgltf
{
	class Asset;
	struct Sampler;
}

namespace c3d_gltf
{
	inline const c3d::String DefaultMaterial = cuT( "GLTF_DefaultMaterial" );

	struct NameContainer
	{
		c3d::HashMap< size_t, c3d::String > namesByIndex;
		c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > namesByRawName;
		c3d::HashSet< c3d::String > names;
	};

	class GltfMaterialsFile
	{
	public:
		C3D_GltfMat_API GltfMaterialsFile( c3d::Engine & engine
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::String const & prefix
			, c3d::String const & name
			, c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > const & materialsNames );
		C3D_GltfMat_API GltfMaterialsFile( c3d::Engine & engine
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::String const & prefix
			, c3d::String const & name );
		C3D_GltfMat_API ~GltfMaterialsFile()noexcept;
		C3D_GltfMat_API c3d::MaterialImporterUPtr createMaterialImporter( c3d::Engine & engine );

		C3D_GltfMat_API c3d::String getSamplerName( c3d::Engine const & engine
			, fastgltf::Sampler const & impSampler )const;
		C3D_GltfMat_API c3d::String getMaterialName( size_t index )const;

		bool isValid()const noexcept
		{
			return m_asset != nullptr;
		}

		fastgltf::Asset const & getAsset()const noexcept
		{
			return *m_asset;
		}

		c3d::String const & getName()const noexcept
		{
			return m_name;
		}

		auto & getMaterials()const noexcept
		{
			return m_materials;
		}

	private:
		void doPrelistMaterials( c3d::Parameters const & parameters
			, c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > const & materialsNames );
		void doPrelistMaterials( c3d::Parameters const & parameters );

		c3d::String getInternalName( c3d::String const & name )const
		{
			return m_prefix + name;
		}

	private:
		c3d::RawUniquePtr< fastgltf::Asset > m_asset{};
		c3d::StringMap< size_t > m_materials;
		mutable NameContainer m_materialNames;
		c3d::String m_prefix;
		c3d::String m_name;
	};

	static c3d::String getElementName( auto const & elements
		, size_t index
		, c3d::StringView baseName )
	{
		c3d::String result = c3d::makeString( elements[index].name );

		if ( result.empty() )
			result = c3d::String{ baseName } + cuT( "-" ) + c3d::string::toString( index );
		else
			result = c3d::ImporterFile::reworkName( result, baseName, index );

		return result;
	}

	static c3d::String getElementName( auto const & elements
		, size_t index
		, c3d::StringView baseName
		, NameContainer & names )
	{
		if ( auto it = names.namesByIndex.find( index );
			it != names.namesByIndex.end() )
			return it->second;

		auto rawName = c3d::makeString( elements[index].name );
		auto result = rawName;
		if ( result.empty() )
			result = baseName;
		else
			result = c3d::ImporterFile::reworkName( result, baseName, index );

		if ( auto it = names.names.find( result );
			it != names.names.end() )
			result += cuT( "-" ) + c3d::string::toString( index );

		names.namesByRawName.try_emplace( rawName ).first->second.try_emplace( c3d::u32( index ), result );
		names.namesByIndex.emplace( index, result );
		names.names.emplace( result );
		return result;
	}
}

#endif
