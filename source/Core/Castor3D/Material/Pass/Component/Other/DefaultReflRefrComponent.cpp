#include "Castor3D/Material/Pass/Component/Other/DefaultReflRefrComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor3d
{
	//*********************************************************************************************

	void DefaultReflRefrComponent::ReflRefrShader::computeReflRefr( shader::ReflectionModel & reflections
		, shader::BlendComponents & components
		, shader::LightSurface const & lightSurface
		, sdw::Vec4 const & position
		, shader::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, shader::CameraData const & camera
		, shader::DirectLighting & lighting
		, shader::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, sdw::UInt const & hasReflection
		, sdw::UInt const & hasRefraction
		, sdw::Float const & refractionRatio
		, shader::ReflectionRefraction & output
		, shader::DebugOutput & debugOutput )const
	{
		if ( mippedScene.isEnabled() )
		{
			auto debugOutputBlock = debugOutput.pushBlock( cuT( "Reflections" ) );
			reflections.computeCombined( components
				, lightSurface
				, lightSurface.worldPosition().value().xyz()
				, backgroundModel
				, mippedScene
				, camera
				, sceneUv / vec2( camera.renderSize() )
				, envMapIndex
				, components.hasReflection
				, components.hasRefraction
				, components.refractionRatio
				, output
				, debugOutputBlock );
		}
		else
		{
			computeReflRefr( reflections
				, components
				, lightSurface
				, backgroundModel
				, camera
				, lighting
				, indirect
				, sceneUv
				, envMapIndex
				, incident
				, components.hasReflection
				, components.hasRefraction
				, components.refractionRatio
				, output
				, debugOutput );
		}
	}

	void DefaultReflRefrComponent::ReflRefrShader::computeReflRefr( shader::ReflectionModel & reflections
		, shader::BlendComponents & components
		, shader::LightSurface const & lightSurface
		, shader::BackgroundModel & backgroundModel
		, shader::CameraData const & camera
		, shader::DirectLighting & lighting
		, shader::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, sdw::UInt const & hasReflection
		, sdw::UInt const & hasRefraction
		, sdw::Float const & refractionRatio
		, shader::ReflectionRefraction & output
		, shader::DebugOutput & debugOutput )const
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Reflections" ) );
		reflections.computeCombined( components
			, lightSurface
			, backgroundModel
			, envMapIndex
			, components.hasReflection
			, components.hasRefraction
			, components.refractionRatio
			, output
			, debugOutputBlock );
	}

	//*********************************************************************************************

	castor::String const DefaultReflRefrComponent::TypeName = C3D_MakePassReflectionComponentName( "default" );

	DefaultReflRefrComponent::DefaultReflRefrComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	PassComponentUPtr DefaultReflRefrComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, DefaultReflRefrComponent >( pass );
	}

	//*********************************************************************************************
}
