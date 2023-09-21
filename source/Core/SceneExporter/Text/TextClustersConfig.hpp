/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextClustersConfig_H___
#define ___CSE_TextClustersConfig_H___

#include <Castor3D/Render/Clustered/ClustersConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ClustersConfig >
		: public TextWriterT< castor3d::ClustersConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ClustersConfig const & config
			, StringStream & file )override;
	};
}

#endif
