/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Layout_H___
#define ___C3D_Layout_H___

#include "Castor3D/Gui/GuiModule.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	class Spacer
	{
	public:
		static uint32_t constexpr Dynamic = ~( 0u );

	public:
		Spacer( uint32_t size )
			: m_size{ size }
		{
		}

		bool isDynamic()const noexcept
		{
			return m_size == Dynamic;
		}

		uint32_t getSize()const noexcept
		{
			return m_size;
		}

	private:
		uint32_t m_size;
	};
	class Layout
		: public castor::Named
	{
	public:
		struct LayoutCtrl
		{
			enum Type
			{
				eControl,
				eSpacer,
			};

			union Value
			{
				ControlRPtr control;
				SpacerRPtr spacer;
			};

			LayoutCtrl( Control & c
				, LayoutCtrlFlags f )
				: type{ eControl }
				, flags{ f }
			{
				value.control = &c;
			}

			LayoutCtrl( Spacer & s
				, LayoutCtrlFlags f )
				: type{ eSpacer }
				, flags{ f }
			{
				value.spacer = &s;
			}

			auto control()const
			{
				return type == eControl
					? value.control
					: nullptr;
			}

			auto spacer()const
			{
				return type == eSpacer
					? value.spacer
					: nullptr;
			}

			Type type;
			Value value;
			LayoutCtrlFlags flags;
		};

	public:
		/**@name General */
		//@{

		C3D_API virtual ~Layout()noexcept = default;
		C3D_API Layout( Layout const & )noexcept = delete;
		C3D_API Layout & operator=( Layout const & )noexcept = delete;
		C3D_API Layout( Layout && )noexcept = delete;
		C3D_API Layout & operator=( Layout && )noexcept = delete;
		/**
		*\brief
		*	Constructor
		*\param[in] typeName
		*	The layout type name.
		*/
		C3D_API explicit Layout( castor::String const & typeName
			, LayoutControl & container );
		/**
		*\brief
		*	Marks the layout as dirty.
		*/
		C3D_API void markDirty();

		//@}
		/**@name Controls management */
		//@{

		/**
		*\brief
		*	Adds a control.
		*\param[in] control
		*	The control.
		*/
		C3D_API void addControl( Control & control
			, LayoutCtrlFlags flags = {} );

		/**
		*\brief
		*	Adds a space.
		*\param[in] size
		*	The space size
		*/
		C3D_API void addSpacer( uint32_t size = Spacer::Dynamic
			, LayoutCtrlFlags flags = {} );

		//@}

		LayoutControl & getContainer()const noexcept
		{
			return m_container;
		}

	protected:
		/**
		*\brief
		*	Updates the layout and its controls.
		*/
		C3D_API void update();

	private:
		/**
		*\brief
		*	Updates the dependant data.
		*/
		virtual void doUpdate() = 0;

	protected:
		LayoutControl & m_container;
		std::vector< LayoutCtrl > m_controls;
		std::vector< SpacerUPtr > m_spacers;
		CpuFrameEvent * m_event{};
		bool m_changed{ true };
	};
}

#endif
