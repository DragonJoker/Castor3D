/*
See LICENSE file in root folder
*/
#ifndef ___GC_ShaderSource_HPP___
#define ___GC_ShaderSource_HPP___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/propgrid/propgrid.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	enum class UniformType
	{
		eFloat,
		eInt,
		eUInt,
		eVec2f,
		eVec3f,
		eVec4f,
		eVec2i,
		eVec3i,
		eVec4i,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eMat4f,
	};
	inline wxString const & getName( UniformType value )
	{
		static castor::Vector< wxString > const Names
		{
			wxT( "Float" ),
			wxT( "Int" ),
			wxT( "UInt" ),
			wxT( "Vec2f" ),
			wxT( "Vec3f" ),
			wxT( "Vec4f" ),
			wxT( "Vec2i" ),
			wxT( "Vec3i" ),
			wxT( "Vec4i" ),
			wxT( "Vec2ui" ),
			wxT( "Vec3ui" ),
			wxT( "Vec4ui" ),
			wxT( "Mat4f" ),
		};
		return Names[size_t( value )];
	}

	template< typename T >
	struct UniformTyper;
	template<>
	struct UniformTyper< float >
	{
		static UniformType constexpr value = UniformType::eFloat;
	};
	template<>
	struct UniformTyper< int32_t >
	{
		static UniformType constexpr value = UniformType::eInt;
	};
	template<>
	struct UniformTyper< uint32_t >
	{
		static UniformType constexpr value = UniformType::eUInt;
	};
	template<>
	struct UniformTyper< castor::Point2f >
	{
		static UniformType constexpr value = UniformType::eVec2f;
	};
	template<>
	struct UniformTyper< castor::Point3f >
	{
		static UniformType constexpr value = UniformType::eVec3f;
	};
	template<>
	struct UniformTyper< castor::Point4f >
	{
		static UniformType constexpr value = UniformType::eVec4f;
	};
	template<>
	struct UniformTyper< castor::Point2i >
	{
		static UniformType constexpr value = UniformType::eVec2i;
	};
	template<>
	struct UniformTyper< castor::Point3i >
	{
		static UniformType constexpr value = UniformType::eVec3i;
	};
	template<>
	struct UniformTyper< castor::Point4i >
	{
		static UniformType constexpr value = UniformType::eVec4i;
	};
	template<>
	struct UniformTyper< castor::Point2ui >
	{
		static UniformType constexpr value = UniformType::eVec2ui;
	};
	template<>
	struct UniformTyper< castor::Point3ui >
	{
		static UniformType constexpr value = UniformType::eVec3ui;
	};
	template<>
	struct UniformTyper< castor::Point4ui >
	{
		static UniformType constexpr value = UniformType::eVec4ui;
	};
	template<>
	struct UniformTyper< castor::Matrix4x4f >
	{
		static UniformType constexpr value = UniformType::eMat4f;
	};

	class UniformValueBase
	{
	protected:
		UniformValueBase( wxString const & name
			, UniformType type
			, bool isTracked
			, bool isRanged )
			: m_name{ name }
			, m_type{ type }
			, m_isTracked{ isTracked }
			, m_isRanged{ isRanged }
		{
		}

	public:
		virtual ~UniformValueBase() = default;

		inline wxString const & getName()const
		{
			return m_name;
		}

		inline UniformType getType()const
		{
			return m_type;
		}

		inline bool isTracked()const
		{
			return m_isTracked;
		}

		inline bool isRanged()const
		{
			return m_isRanged;
		}

	private:
		wxString m_name;
		UniformType m_type;
		bool m_isTracked;
		bool m_isRanged;
	};

	template< typename T >
	class UniformValue
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = ValueType &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = false;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value;
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< castor::ChangeTracked< T > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = castor::ChangeTracked< ValueType > & ;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType & value
			, bool isTracked )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< castor::RangedValue< T > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = castor::RangedValue< ValueType > &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};

	template< typename T >
	class UniformValue< castor::ChangeTracked< castor::RangedValue< T > > >
		: public UniformValueBase
	{
	public:
		using ValueType = T;
		using RefType = castor::ChangeTracked< castor::RangedValue< ValueType > > &;
		using ConstRefType = ValueType const &;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = false;

	public:
		UniformValue( wxString const & name
			, RefType value )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value.value().value();
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
	};
	
	template< typename T >
	class TrackedUniformValue
		: public UniformValueBase
	{
	public:
		using UniformValueType = UniformValue< T >;
		using ValueType = typename UniformValueType::ValueType;
		using RefType = typename UniformValueType::RefType;
		using ConstRefType = typename UniformValueType::ConstRefType;
		static bool constexpr IsTracked = true;
		static bool constexpr IsRanged = UniformValueType::IsRanged;

	public:
		TrackedUniformValue( wxString const & name
			, RefType value
			, bool * control )
			: UniformValueBase{ name, UniformTyper< ValueType >::value, IsTracked, IsRanged }
			, m_value{ value }
			, m_control{ control }
		{
		}

		inline ConstRefType getValue()const
		{
			return m_value;
		}

		inline RefType getValue()
		{
			return m_value;
		}

	private:
		RefType m_value;
		bool * m_control;
	};

	template< typename ValueT >
	castor::RawUniquePtr< UniformValueBase > makeUniformValue( wxString const & name
		, ValueT & value )
	{
		return castor::make_unique< UniformValue< ValueT > >( name, value );
	}
	
	template< typename ValueT, typename ControlT >
	castor::RawUniquePtr< UniformValueBase > makeUniformValue( wxString const & name
		, ValueT & value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > control )
	{
		return castor::make_unique< UniformValue< ValueT > >( name, value );
	}

	struct UniformBufferValues
	{
		explicit UniformBufferValues( castor::String pname
			, VkShaderStageFlags pstages = {}
			, castor::Vector< castor::RawUniquePtr< UniformValueBase > > puniforms = {} )
			: name{ castor::move( pname ) }
			, stages{ pstages }
			, uniforms{ castor::move( puniforms ) }
		{
		}

		UniformBufferValues( UniformBufferValues const & ) = delete;
		UniformBufferValues( UniformBufferValues && ) = default;
		UniformBufferValues & operator=( UniformBufferValues const & ) = delete;
		UniformBufferValues & operator=( UniformBufferValues && ) = default;

		wxString name;
		VkShaderStageFlags stages;
		castor::Vector< castor::RawUniquePtr< UniformValueBase > > uniforms;
	};

	struct ShaderEntryPoint
	{
		ast::Shader * shader;
		castor3d::SpirVShader source;
		ast::EntryPoint entryPoint;
	};

	struct ShaderSource
	{
		explicit ShaderSource( castor::String pname
			, castor::Vector< ShaderEntryPoint > psources = {}
			, castor::Vector< UniformBufferValues > pubos = {} )
			: name{ castor::move( pname ) }
			, sources{ castor::move( psources ) }
			, ubos{ castor::move( pubos ) }
		{
		}

		ShaderSource( ShaderSource const & ) = delete;
		ShaderSource( ShaderSource && ) = default;
		ShaderSource & operator=( ShaderSource const & ) = delete;
		ShaderSource & operator=( ShaderSource && ) = default;

		castor::String name;
		castor::Vector< ShaderEntryPoint > sources;
		castor::Vector< UniformBufferValues > ubos;
	};

	using ShaderSources = castor::Vector< ShaderSource >;
}

#endif
