#include "Castor3D/Material/Pass/Component/Other/DefaultReflRefrComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

namespace c3d
{
	//*********************************************************************************************

	void DefaultReflRefrComponent::ReflRefrShader::computeWithTransmission( shader::ReflectionModel & reflections
		, shader::BlendComponents & components
		, shader::LightSurface const & lightSurface
		, shader::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, shader::CameraData const & camera
		, shader::RenderData const & render
		, shader::DirectLighting & lighting
		, shader::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, shader::ReflectionRefraction & output
		, shader::DebugOutputCategory const & debugOutput )const
	{
		if ( mippedScene.isEnabled() )
		{
			reflections.computeWithTransmission( components
				, lightSurface
				, backgroundModel
				, mippedScene
				, camera
				, sceneUv * render.invRenderSize()
				, envMapIndex
				, output
				, debugOutput );
		}
		else
		{
			computeWithoutTransmission( reflections
				, components
				, lightSurface
				, backgroundModel
				, camera
				, render
				, lighting
				, indirect
				, sceneUv
				, envMapIndex
				, incident
				, output
				, debugOutput );
		}
	}

	void DefaultReflRefrComponent::ReflRefrShader::computeWithoutTransmission( shader::ReflectionModel & reflections
		, shader::BlendComponents & components
		, shader::LightSurface const & lightSurface
		, shader::BackgroundModel & backgroundModel
		, shader::CameraData const & camera
		, shader::RenderData const & render
		, shader::DirectLighting & lighting
		, shader::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, shader::ReflectionRefraction & output
		, shader::DebugOutputCategory const & debugOutput )const
	{
		reflections.computeWithoutTransmission( components
			, lightSurface.N().value()
			, lightSurface.worldPosition().value().xyz()
			, lightSurface.V().value()
			, lightSurface.NdotV().value()
			, backgroundModel
			, envMapIndex
			, output
			, debugOutput );
	}

	//*********************************************************************************************

	String const DefaultReflRefrComponent::TypeName = C3D_MakePassReflectionComponentName( "default" );

	DefaultReflRefrComponent::DefaultReflRefrComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	PassComponentUPtr DefaultReflRefrComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, DefaultReflRefrComponent >( pass );
	}

	//*********************************************************************************************
}
