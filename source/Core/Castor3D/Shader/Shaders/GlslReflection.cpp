#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d::shader
{
	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, PassFlags const & passFlags )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_passFlags{ passFlags }
	{
	}

	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	std::unique_ptr< ReflectionModel > ReflectionModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, MaterialType materialType
		, PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )
	{
		std::unique_ptr< ReflectionModel > result;

		if ( materialType == MaterialType::ePhong )
		{
			result = std::make_unique< PhongReflectionModel>( writer
				, utils
				, passFlags
				, envMapBinding
				, envMapSet );
		}
		else
		{
			result = std::make_unique< PbrReflectionModel>( writer
				, utils
				, passFlags
				, envMapBinding
				, envMapSet );
		}

		return result;
	}

	std::unique_ptr< ReflectionModel > ReflectionModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, MaterialType materialType
		, uint32_t envMapBinding
		, uint32_t envMapSet )
	{
		std::unique_ptr< ReflectionModel > result;

		if ( materialType == MaterialType::ePhong )
		{
			result = std::make_unique< PhongReflectionModel>( writer
				, utils
				, envMapBinding
				, envMapSet );
		}
		else
		{
			result = std::make_unique< PbrReflectionModel>( writer
				, utils
				, envMapBinding
				, envMapSet );
		}

		return result;
	}
}
