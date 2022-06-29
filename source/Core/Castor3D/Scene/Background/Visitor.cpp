#include "Castor3D/Scene/Background/Visitor.hpp"

namespace castor3d
{
	BackgroundVisitor::BackgroundVisitor( Config config )
		: PipelineVisitor{ std::move( config ) }
	{
	}
}
