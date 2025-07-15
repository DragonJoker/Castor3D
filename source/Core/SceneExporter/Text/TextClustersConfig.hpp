/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextClustersConfig_H___
#define ___CSE_TextClustersConfig_H___

#include <Castor3D/Render/Clustered/ClustersConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ClustersConfig >
		: public TextWriterT< ClustersConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ClustersConfig const & config
			, StringStream & file )override;
	};
}

#endif
