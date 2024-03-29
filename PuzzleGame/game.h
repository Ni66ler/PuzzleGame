#pragma once

#include <algorithm>
#include <cstdlib>

template<int BOARD_SIZE> class Board;

extern int g_boardSize;

extern Board<4> g_board4;

enum class MoveInfo
{
	MOVE_UP = 0,
	MOVE_DOWN = 1,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 3
};

struct PosInfo
{
	int row;
	int col;
};

template <class BoardType> struct Status;

template <int BOARD_SIZE>
class Board
{
	int m_board[BOARD_SIZE * BOARD_SIZE];
	int m_empty;

	template <class BoardType>
	friend void Status<BoardType>::calcF();

public:

	Board()
	{
		clear();
	}

	void clear()
	{
		m_empty = BOARD_SIZE * BOARD_SIZE - 1;
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
		{
			m_board[i] = i;
		}
	}

	bool click(PosInfo pos)
	{
		if (pos.row < 0 || pos.row >= BOARD_SIZE || pos.col < 0 || pos.col >= BOARD_SIZE) return false;
		int emptrow = m_empty / BOARD_SIZE;
		int emptcol = m_empty % BOARD_SIZE;
		if (emptrow == pos.row)
		{
			if (emptcol == pos.col) return false;
			const int base = emptrow * BOARD_SIZE;
			if (emptcol < pos.col)
			{
				for (int i = emptcol; i < pos.col; ++i)
				{
					std::swap(m_board[base + i], m_board[base + i + 1]);
				}
			}
			else
			{
				for (int i = emptcol; i > pos.col; --i)
				{
					std::swap(m_board[base + i], m_board[base + i - 1]);
				}
			}
			m_empty = base + pos.col;
			return true;
		}
		else if (emptcol == pos.col)
		{
			if (emptrow == pos.row) return false;
			if (emptrow < pos.row)
			{
				for (int i = emptrow; i < pos.row; ++i)
				{
					std::swap(m_board[i * BOARD_SIZE + emptcol], m_board[(i + 1) * BOARD_SIZE + emptcol]);
				}
			}
			else
			{
				for (int i = emptrow; i > pos.row; --i)
				{
					std::swap(m_board[i * BOARD_SIZE + emptcol], m_board[(i - 1) * BOARD_SIZE + emptcol]);
				}
			}
			m_empty = pos.row * BOARD_SIZE + emptcol;
			return true;
		}
		return false;
	}

	int getEmpty() const
	{
		return m_empty;
	}

	PosInfo getPiecePos(PosInfo pos) const
	{
		const int num = m_board[pos.row * BOARD_SIZE + pos.col];
		return { num / BOARD_SIZE, num % BOARD_SIZE };
	}

	unsigned long long hash() const
	{
		unsigned long long res = 0;
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
		{
			res = res * 33 + m_board[i];
		}
		return res;
	}

	bool isFinished() const
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
		{
			if (m_board[i] != i) return false;
		}
		return true;
	}

	bool move(MoveInfo mov)
	{
		int nextEmpty;
		bool res;
		switch (mov)
		{
		case MoveInfo::MOVE_UP:
			nextEmpty = m_empty + BOARD_SIZE;
			res = nextEmpty < BOARD_SIZE * BOARD_SIZE;
			break;

		case MoveInfo::MOVE_DOWN:
			nextEmpty = m_empty - BOARD_SIZE;
			res = nextEmpty >= 0;
			break;

		case MoveInfo::MOVE_LEFT:
			nextEmpty = m_empty + 1;
			res = nextEmpty % BOARD_SIZE != 0;
			break;

		case MoveInfo::MOVE_RIGHT:
			nextEmpty = m_empty - 1;
			res = m_empty % BOARD_SIZE != 0;
			break;
		}
		if (res)
		{
			std::swap(m_board[m_empty], m_board[nextEmpty]);
			m_empty = nextEmpty;
		}
		return res;
	}

	void random_shuffle()
	{
		int inversions;
		do
		{
			inversions = 0;
			clear();
			shuffle();
			for (int i = 0; i < BOARD_SIZE * BOARD_SIZE - 2; i++)
			{
				for (int j = i + 1; j < BOARD_SIZE * BOARD_SIZE - 1; j++)
				{
					if (m_board[i] > m_board[j]) inversions++;
				}
			}
			inversions += BOARD_SIZE;
		} while (inversions % 2 == 1);
	}

	void shuffle()
	{
		int n = BOARD_SIZE * BOARD_SIZE - 1;
		while (n > 1)
		{
			int r = rand() % n;
			n--;
			std::swap(m_board[r], m_board[n]);
		}
	}
};