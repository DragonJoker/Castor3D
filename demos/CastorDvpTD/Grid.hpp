#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	struct PathNode
	{
		uint32_t m_x;
		uint32_t m_y;
	};

	using Path = castor::Vector< PathNode >;

	struct Cell
	{
		enum class State
			: uint8_t
		{
			Empty,
			Start,
			Tower,
			Path,
			Target,
			Invalid,
		};

		Cell( uint32_t x = 0u, uint32_t y = 0u, State state = State::Empty )
			: m_x{ x }
			, m_y{ y }
			, m_state{ state }
		{
		}

		uint32_t m_x{ 0u };
		uint32_t m_y{ 0u };
		State m_state{ State::Empty };
	};

	class Grid
	{
	public:
		Grid();

		auto begin()const
		{
			return m_cells.begin();
		}

		auto begin()
		{
			return m_cells.begin();
		}

		auto end()const
		{
			return m_cells.end();
		}

		auto end()
		{
			return m_cells.end();
		}

		auto getWidth()const
		{
			return m_width;
		}

		auto getHeight()const
		{
			return m_height;
		}

		auto & operator()( uint32_t x, uint32_t y )const
		{
			return m_cells[size_t( x ) * m_height + y];
		}

		auto & operator()( uint32_t x, uint32_t y )
		{
			return m_cells[size_t( x ) * m_height + y];
		}

	private:
		constexpr static uint32_t m_width{ 32u };
		constexpr static uint32_t m_height{ 32u };
		using CellArray = castor::Array< Cell, m_width * m_height >;
		CellArray m_cells;
	};
}
