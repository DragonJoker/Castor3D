#include "TextClustersConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d
{
	TextWriter< ClustersConfig >::TextWriter( String const & tabs )
		: TextWriterT< ClustersConfig >{ tabs }
	{
	}

	bool TextWriter< ClustersConfig >::operator()( ClustersConfig const & object
		, StringStream & file )
	{
		static const Array< String, uint32_t( ClusterSplitScheme::eCount ) > splitSchemes =
		{
			getName( ClusterSplitScheme::eExponentialBase ),
			getName( ClusterSplitScheme::eLinear ),
			getName( ClusterSplitScheme::eExponentialLinearHybrid ),
		};

		bool result{ false };
		log::info << tabs() << cuT( "Writing ClustersConfig" ) << std::endl;

		if ( auto block{ beginBlock( file, cuT( "clusters" ) ) } )
		{
			result = writeOpt( file, cuT( "enabled" ), object.enabled, true )
				&& writeOpt( file, cuT( "split_scheme" ), splitSchemes[uint32_t( object.splitScheme.value() )], splitSchemes[uint32_t( ClusterSplitScheme::eExponentialLinearHybrid )] )
				&& writeOpt( file, cuT( "min_distance" ), object.minDistance.value(), 1.0f );
		}

		return result;
	}
}
