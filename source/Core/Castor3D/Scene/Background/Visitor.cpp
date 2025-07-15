#include "Castor3D/Scene/Background/Visitor.hpp"

namespace c3d
{
	BackgroundVisitor::BackgroundVisitor( Config config )
		: ConfigurationVisitor{ c3d::move( config ) }
	{
	}
}
