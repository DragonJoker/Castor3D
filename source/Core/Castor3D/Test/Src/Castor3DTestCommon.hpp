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
	inline std::string to_string< Castor::Angle >( Castor::Angle const & p_value )
	{
		return Castor::string::to_string( p_value.degrees() );
	}

	template<>
	inline std::string to_string< Castor3D::Scene >( Castor3D::Scene const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::SceneNode >( Castor3D::SceneNode const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Animable >( Castor3D::Animable const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::MovableObject >( Castor3D::MovableObject const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Geometry >( Castor3D::Geometry const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Light >( Castor3D::Light const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Camera >( Castor3D::Camera const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::LightCategory >( Castor3D::LightCategory const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::DirectionalLight >( Castor3D::DirectionalLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::PointLight >( Castor3D::PointLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::SpotLight >( Castor3D::SpotLight const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::Viewport >( Castor3D::Viewport const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::Material >( Castor3D::Material const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Pass >( Castor3D::Pass const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::Mesh >( Castor3D::Mesh const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Submesh >( Castor3D::Submesh const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::Skeleton >( Castor3D::Skeleton const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::Bone >( Castor3D::Bone const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::Animation >( Castor3D::Animation const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::SkeletonAnimation >( Castor3D::SkeletonAnimation const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::SkeletonAnimationObject >( Castor3D::SkeletonAnimationObject const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::KeyFrame >( Castor3D::KeyFrame const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::AnimatedObjectGroup >( Castor3D::AnimatedObjectGroup const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::AnimatedObject >( Castor3D::AnimatedObject const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::AnimationInstance >( Castor3D::AnimationInstance const & p_value )
	{
		return Castor::string::string_cast< char >( p_value.GetAnimation().GetName() );
	}

	template<>
	inline std::string to_string< Castor3D::SkeletonAnimationInstance >( Castor3D::SkeletonAnimationInstance const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::SkeletonAnimationInstanceObject >( Castor3D::SkeletonAnimationInstanceObject const & p_value )
	{
		return std::string{};
	}

	template<>
	inline std::string to_string< Castor3D::InterpolatorType >( Castor3D::InterpolatorType const & p_value )
	{
		static std::map< Castor3D::InterpolatorType, std::string > Names
		{
			{ Castor3D::InterpolatorType::eNearest, "Nearest" },
			{ Castor3D::InterpolatorType::eLinear, "Linear" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string to_string< Castor3D::AnimationType >( Castor3D::AnimationType const & p_value )
	{
		static std::map< Castor3D::AnimationType, std::string > Names
		{
			{ Castor3D::AnimationType::eMovable, "Movable" },
			{ Castor3D::AnimationType::eMesh, "Mesh" },
			{ Castor3D::AnimationType::eSkeleton, "Skeleton" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string to_string< Castor3D::SkeletonAnimationObjectType >( Castor3D::SkeletonAnimationObjectType const & p_value )
	{
		static std::map< Castor3D::SkeletonAnimationObjectType, std::string > Names
		{
			{ Castor3D::SkeletonAnimationObjectType::eBone, "Bone" },
			{ Castor3D::SkeletonAnimationObjectType::eNode, "Node" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string to_string< Castor3D::AnimationState >( Castor3D::AnimationState const & p_value )
	{
		static std::map< Castor3D::AnimationState, std::string > Names
		{
			{ Castor3D::AnimationState::ePlaying, "Playing" },
			{ Castor3D::AnimationState::eStopped, "Stopped" },
			{ Castor3D::AnimationState::ePaused, "Paused" },
		};
		return Names[p_value];
	}

	template<>
	inline std::string to_string< Castor3D::ViewportType >( Castor3D::ViewportType const & p_value )
	{
		static std::map< Castor3D::ViewportType, std::string > Names
		{
			{ Castor3D::ViewportType::eOrtho, "Ortho" },
			{ Castor3D::ViewportType::ePerspective, "Perspective" },
			{ Castor3D::ViewportType::eFrustum, "Frustum" },
		};
		return Names[p_value];
	}

	//*********************************************************************************************

	class C3DTestCase
		: public TestCase
	{
	public:
		C3DTestCase( std::string const & p_name, Castor3D::Engine & p_engine );

	protected:
		using TestCase::compare;
		bool compare( Castor3D::Scene const & p_a, Castor3D::Scene const & p_b );
		bool compare( Castor3D::SceneNode const & p_a, Castor3D::SceneNode const & p_b );
		bool compare( Castor3D::Animable const & p_a, Castor3D::Animable const & p_b );
		bool compare( Castor3D::MovableObject const & p_a, Castor3D::MovableObject const & p_b );
		bool compare( Castor3D::Camera const & p_a, Castor3D::Camera const & p_b );
		bool compare( Castor3D::Light const & p_a, Castor3D::Light const & p_b );
		bool compare( Castor3D::Geometry const & p_a, Castor3D::Geometry const & p_b );
		bool compare( Castor3D::LightCategory const & p_a, Castor3D::LightCategory const & p_b );
		bool compare( Castor3D::DirectionalLight const & p_a, Castor3D::DirectionalLight const & p_b );
		bool compare( Castor3D::PointLight const & p_a, Castor3D::PointLight const & p_b );
		bool compare( Castor3D::SpotLight const & p_a, Castor3D::SpotLight const & p_b );
		bool compare( Castor3D::Viewport const & p_a, Castor3D::Viewport const & p_b );
		bool compare( Castor3D::Mesh const & p_a, Castor3D::Mesh const & p_b );
		bool compare( Castor3D::Submesh const & p_a, Castor3D::Submesh const & p_b );
		bool compare( Castor3D::Skeleton const & p_a, Castor3D::Skeleton const & p_b );
		bool compare( Castor3D::Bone const & p_a, Castor3D::Bone const & p_b );
		bool compare( Castor3D::Animation const & p_a, Castor3D::Animation const & p_b );
		bool compare( Castor3D::SkeletonAnimation const & p_a, Castor3D::SkeletonAnimation const & p_b );
		bool compare( Castor3D::SkeletonAnimationObject const & p_a, Castor3D::SkeletonAnimationObject const & p_b );
		bool compare( Castor3D::KeyFrame const & p_a, Castor3D::KeyFrame const & p_b );
		bool compare( Castor3D::AnimatedObjectGroup const & p_a, Castor3D::AnimatedObjectGroup const & p_b );
		bool compare( Castor3D::AnimatedObject const & p_a, Castor3D::AnimatedObject const & p_b );
		bool compare( Castor3D::AnimationInstance const & p_a, Castor3D::AnimationInstance const & p_b );
		bool compare( Castor3D::SkeletonAnimationInstance const & p_a, Castor3D::SkeletonAnimationInstance const & p_b );
		bool compare( Castor3D::SkeletonAnimationInstanceObject const & p_a, Castor3D::SkeletonAnimationInstanceObject const & p_b );

	protected:
		Castor3D::Engine & m_engine;
		Castor::Path m_testDataFolder;
	};

	//************************************************************************************************
}

#endif
