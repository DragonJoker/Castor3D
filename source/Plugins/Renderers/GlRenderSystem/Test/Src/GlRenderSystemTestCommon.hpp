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
		GlTestCase( std::string const & p_name, Castor3D::Engine & p_engine );

	protected:
		Castor3D::Engine & m_engine;
	};
}

#endif
