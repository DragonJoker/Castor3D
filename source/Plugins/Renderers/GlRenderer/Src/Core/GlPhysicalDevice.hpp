/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Core/PhysicalDevice.hpp>

namespace gl_renderer
{
	/**
	*\~french
	*\brief
	*	Classe contenant les informations liées au GPU physique.
	*\~english
	*\brief
	*	Wraps informations about physical GPU.
	*/
	class PhysicalDevice
		: public renderer::PhysicalDevice
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de rendu.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderer
		*	The rendering instance.
		*/
		PhysicalDevice( Renderer & renderer );
		/**
		*\copydoc	renderer::PhysicalDevice::initialise
		*/
		void initialise()override;
		/**
		*\copydoc	renderer::PhysicalDevice::isSPIRVSupported
		*/
		bool isSPIRVSupported()const override;
		/**
		*\copydoc	renderer::PhysicalDevice::isGLSLSupported
		*/
		bool isGLSLSupported()const override;

		bool find( std::string const & name )const;
		bool findAny( renderer::StringArray const & names )const;
		bool findAll( renderer::StringArray const & names )const;
		bool hasSPIRVShaderBinaryFormat()const;

		inline int getMajor()const
		{
			return m_major;
		}

		inline int getMinor()const
		{
			return m_minor;
		}

		inline uint32_t getGlslVersion()const
		{
			return m_glslVersion;
		}

	private:
		void doGetValue( GLenum name, int32_t & value )const;
		void doGetValue( GLenum name, uint32_t & value )const;
		void doGetValues( GLenum name, int32_t( &value )[2] )const;
		void doGetValues( GLenum name, uint32_t( &value )[2] )const;
		void doGetValues( GLenum name, int32_t( &value )[3] )const;
		void doGetValues( GLenum name, uint32_t( &value )[3] )const;
		void doGetValuesI( GLenum name, int32_t( &value )[2] )const;
		void doGetValuesI( GLenum name, uint32_t( &value )[2] )const;
		void doGetValuesI( GLenum name, int32_t( &value )[3] )const;
		void doGetValuesI( GLenum name, uint32_t( &value )[3] )const;
		void doGetValue( GLenum name, int64_t & value )const;
		void doGetValue( GLenum name, uint64_t & value )const;
		void doGetValues( GLenum name, int64_t( &value )[2] )const;
		void doGetValues( GLenum name, uint64_t( &value )[2] )const;
		void doGetValues( GLenum name, int64_t( &value )[3] )const;
		void doGetValues( GLenum name, uint64_t( &value )[3] )const;
		void doGetValuesI( GLenum name, int64_t( &value )[2] )const;
		void doGetValuesI( GLenum name, uint64_t( &value )[2] )const;
		void doGetValuesI( GLenum name, int64_t( &value )[3] )const;
		void doGetValuesI( GLenum name, uint64_t( &value )[3] )const;
		void doGetValue( GLenum name, float & value )const;
		void doGetValues( GLenum name, float( &value )[2] )const;
		void doGetValues( GLenum name, float( &value )[3] )const;
		void doGetValuesI( GLenum name, float( &value )[2] )const;
		void doGetValuesI( GLenum name, float( &value )[3] )const;

	private:
		Renderer & m_renderer;
		renderer::StringArray m_deviceExtensionNames;
		renderer::StringArray m_deviceSPIRVExtensionNames;
		std::vector< GlShaderBinaryFormat > m_shaderBinaryFormats;
		int m_major{ 0 };
		int m_minor{ 0 };
		uint32_t m_glslVersion{ 0u };
	};
}
