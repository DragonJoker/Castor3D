/* See LICENSE file in root folder */
#ifndef ___GLT_COMMON_H___
#define ___GLT_COMMON_H___

#include <UnitTest.hpp>

#include <Animation/Animation.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Scene/Animation/AnimatedObject.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
#include <Scene/Animation/AnimationInstance.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/MovableObject.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Scene/Light/Light.hpp>

namespace Testing
{
	class GlTestCase
		: public TestCase
	{
	public:
		GlTestCase( std::string const & p_name, castor3d::Engine & engine );

	protected:
		castor3d::Engine & m_engine;
	};
}

#endif
