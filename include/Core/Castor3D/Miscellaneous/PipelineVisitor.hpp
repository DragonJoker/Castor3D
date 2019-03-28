/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffectVisitor_HPP___
#define ___C3D_PostEffectVisitor_HPP___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		16/05/2018
	\~english
	\brief		Post effect visitor base class.
	\~french
	\brief		Classe de base d'un visiteur d'effet post rendu.
	*/
	class PipelineVisitorBase
	{
	protected:
		inline PipelineVisitorBase()
		{
		}

	public:
		virtual inline ~PipelineVisitorBase()
		{
		}
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlag type
			, sdw::Shader const & shader ) = 0;
		/**@}*/
		/**
		*\~english
		*name
		*	Global post effect configuration.
		*\~french
		*name
		*	Configuration globale de l'effet.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, float & value ) = 0;
		virtual void visit( castor::String const & name
			, int32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, uint32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point2ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point3ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4i & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Point4ui & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value ) = 0;
		/**@}*/
		/**
		*\~english
		*name
		*	UBO configuration.
		*\~french
		*name
		*	Configuration d'UBO.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, float & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, int32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, uint32_t & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2f & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2i & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2ui & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3f & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3i & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3ui & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4f & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4i & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4ui & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Matrix4x4f & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< uint32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< float > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< int32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< uint32_t > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2f > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2i > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3f > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3i > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4f > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4i > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4ui > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Matrix4x4f > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< float > > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< int32_t > > & value ) = 0;
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value ) = 0;
		/**@}*/
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		16/05/2018
	\~english
	\brief		Post effect visitor base class.
	\~french
	\brief		Classe de base d'un visiteur d'effet post rendu.
	*/
	class PipelineVisitor
		: public PipelineVisitorBase
	{
	protected:
		inline PipelineVisitor()
		{
		}

	public:
		virtual inline ~PipelineVisitor()
		{
		}
		/**
		*\~english
		*name
		*	Shader source.
		*\~french
		*name
		*	Source de shader.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlag type
			, sdw::Shader const & shader )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Global post effect configuration.
		*\~french
		*name
		*	Configuration globale de l'effet.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, float & value )override
		{
		}

		virtual void visit( castor::String const & name
			, int32_t & value )override
		{
		}

		virtual void visit( castor::String const & name
			, uint32_t & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point2f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point2i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point2ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point3f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point3i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point3ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point4f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point4i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Point4ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::Matrix4x4f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::RangedValue< float > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value )override
		{
		}
		/**@}*/
		/**
		*\~english
		*name
		*	UBO configuration.
		*\~french
		*name
		*	Configuration d'UBO.
		**/
		/**@{*/
		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, float & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, int32_t & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, uint32_t & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point2ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point3ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4i & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Point4ui & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::Matrix4x4f & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< float > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< int32_t > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::RangedValue< uint32_t > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< float > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< int32_t > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< uint32_t > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2f > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2i > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point2ui > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3f > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3i > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point3ui > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4f > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4i > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Point4ui > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::Matrix4x4f > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< float > > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< int32_t > > & value )override
		{
		}

		virtual void visit( castor::String const & name
			, ashes::ShaderStageFlags shaders
			, castor::String const & ubo
			, castor::String const & uniform
			, castor::ChangeTracked< castor::RangedValue< uint32_t > > & value )override
		{
		}
		/**@}*/
	};
}

#endif
