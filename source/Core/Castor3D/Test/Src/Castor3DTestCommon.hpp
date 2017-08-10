/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DT_COMMON_H___
#define ___C3DT_COMMON_H___

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
	//*********************************************************************************************

	template<>
	inline std::string toString< castor::Angle >( castor::Angle const & p_value )
	{
		return castor::string::toString( p_value.degrees() );
	}

	template<>
	inline std::string toString< castor3d::Scene >( castor3d::Scene const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::SceneNode >( castor3d::SceneNode const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Animable >( castor3d::Animable const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::MovableObject >( castor3d::MovableObject const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Geometry >( castor3d::Geometry const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Light >( castor3d::Light const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Camera >( castor3d::Camera const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::LightCategory >( castor3d::LightCategory const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::DirectionalLight >( castor3d::DirectionalLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::PointLight >( castor3d::PointLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::SpotLight >( castor3d::SpotLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::Viewport >( castor3d::Viewport const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::Material >( castor3d::Material const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Pass >( castor3d::Pass const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::Mesh >( castor3d::Mesh const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Submesh >( castor3d::Submesh const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::Skeleton >( castor3d::Skeleton const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::Bone >( castor3d::Bone const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::Animation >( castor3d::Animation const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::SkeletonAnimation >( castor3d::SkeletonAnimation const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::SkeletonAnimationObject >( castor3d::SkeletonAnimationObject const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::KeyFrame >( castor3d::KeyFrame const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::AnimatedObjectGroup >( castor3d::AnimatedObjectGroup const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::AnimatedObject >( castor3d::AnimatedObject const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getName() );
	}

	template<>
	inline std::string toString< castor3d::AnimationInstance >( castor3d::AnimationInstance const & p_value )
	{
		return castor::string::stringCast< char >( p_value.getAnimation().getName() );
	}

	template<>
	inline std::string toString< castor3d::SkeletonAnimationInstance >( castor3d::SkeletonAnimationInstance const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::SkeletonAnimationInstanceObject >( castor3d::SkeletonAnimationInstanceObject const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string toString< castor3d::InterpolatorType >( castor3d::InterpolatorType const & p_value )
	{
		static std::map< castor3d::InterpolatorType, std::string > Names
		{
			{ castor3d::InterpolatorType::eNearest, "Nearest" },
			{ castor3d::InterpolatorType::eLinear, "Linear" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string toString< castor3d::AnimationType >( castor3d::AnimationType const & p_value )
	{
		static std::map< castor3d::AnimationType, std::string > Names
		{
			{ castor3d::AnimationType::eMovable, "Movable" },
			{ castor3d::AnimationType::eMesh, "Mesh" },
			{ castor3d::AnimationType::eSkeleton, "Skeleton" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string toString< castor3d::SkeletonAnimationObjectType >( castor3d::SkeletonAnimationObjectType const & p_value )
	{
		static std::map< castor3d::SkeletonAnimationObjectType, std::string > Names
		{
			{ castor3d::SkeletonAnimationObjectType::eBone, "Bone" },
			{ castor3d::SkeletonAnimationObjectType::eNode, "Node" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string toString< castor3d::AnimationState >( castor3d::AnimationState const & p_value )
	{
		static std::map< castor3d::AnimationState, std::string > Names
		{
			{ castor3d::AnimationState::ePlaying, "Playing" },
			{ castor3d::AnimationState::eStopped, "Stopped" },
			{ castor3d::AnimationState::ePaused, "Paused" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string toString< castor3d::ViewportType >( castor3d::ViewportType const & p_value )
	{
		static std::map< castor3d::ViewportType, std::string > Names
		{
			{ castor3d::ViewportType::eOrtho, "Ortho" },
			{ castor3d::ViewportType::ePerspective, "Perspective" },
			{ castor3d::ViewportType::eFrustum, "Frustum" },
		};
		return Names[p_value];
	}

	//*********************************************************************************************

	class C3DTestCase
		: public TestCase
	{
	public:
		C3DTestCase( std::string const & p_name, castor3d::Engine & engine );

	protected:
		void DeCleanupEngine();
		using TestCase::compare;
		bool compare( castor::Angle const & p_a, castor::Angle const & p_b );
		bool compare( castor::Quaternion const & p_a, castor::Quaternion const & p_b );
		bool compare( castor3d::Scene const & p_a, castor3d::Scene const & p_b );
		bool compare( castor3d::SceneNode const & p_a, castor3d::SceneNode const & p_b );
		bool compare( castor3d::Animable const & p_a, castor3d::Animable const & p_b );
		bool compare( castor3d::MovableObject const & p_a, castor3d::MovableObject const & p_b );
		bool compare( castor3d::Camera const & p_a, castor3d::Camera const & p_b );
		bool compare( castor3d::Light const & p_a, castor3d::Light const & p_b );
		bool compare( castor3d::Geometry const & p_a, castor3d::Geometry const & p_b );
		bool compare( castor3d::LightCategory const & p_a, castor3d::LightCategory const & p_b );
		bool compare( castor3d::DirectionalLight const & p_a, castor3d::DirectionalLight const & p_b );
		bool compare( castor3d::PointLight const & p_a, castor3d::PointLight const & p_b );
		bool compare( castor3d::SpotLight const & p_a, castor3d::SpotLight const & p_b );
		bool compare( castor3d::Viewport const & p_a, castor3d::Viewport const & p_b );
		bool compare( castor3d::Mesh const & p_a, castor3d::Mesh const & p_b );
		bool compare( castor3d::Submesh const & p_a, castor3d::Submesh const & p_b );
		bool compare( castor3d::Skeleton const & p_a, castor3d::Skeleton const & p_b );
		bool compare( castor3d::Bone const & p_a, castor3d::Bone const & p_b );
		bool compare( castor3d::Animation const & p_a, castor3d::Animation const & p_b );
		bool compare( castor3d::SkeletonAnimation const & p_a, castor3d::SkeletonAnimation const & p_b );
		bool compare( castor3d::SkeletonAnimationObject const & p_a, castor3d::SkeletonAnimationObject const & p_b );
		bool compare( castor3d::KeyFrame const & p_a, castor3d::KeyFrame const & p_b );
		bool compare( castor3d::AnimatedObjectGroup const & p_a, castor3d::AnimatedObjectGroup const & p_b );
		bool compare( castor3d::AnimatedObject const & p_a, castor3d::AnimatedObject const & p_b );
		bool compare( castor3d::AnimationInstance const & p_a, castor3d::AnimationInstance const & p_b );
		bool compare( castor3d::SkeletonAnimationInstance const & p_a, castor3d::SkeletonAnimationInstance const & p_b );
		bool compare( castor3d::SkeletonAnimationInstanceObject const & p_a, castor3d::SkeletonAnimationInstanceObject const & p_b );

	protected:
		castor3d::Engine & m_engine;
		castor::Path m_testDataFolder;
	};

	//************************************************************************************************
}

#endif
