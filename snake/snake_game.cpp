#include "snake_game.h"
#include <fstream>
#include <map>
#include <functional>
#include <random>

SnakeGame::SnakeGame()
{
	// init default board
	for (auto i = 0; i < 30; i++)
	{
		board.emplace_back(std::vector<GameCell>());
		for (int j = 0; j < 30; j++)
			board[i].emplace_back(GameCell());
	}
	reset();
}


void SnakeGame::reset()
{
	direction = Direction::Right;

	for (auto &row : board)
		for (auto & cell : row)
			cell.type = CellType::Empty;

	for (auto i = 0; i < 5; i++)
	{
		board[15][i].type = CellType::SnakeBody;
		board[15][i].related = Direction::Right;
	}

	for (size_t i = 0; i < board.size(); i++)
		board[i][0].type = CellType::Wall;
	for (size_t i = 0; i < board[0].size(); i++)
		board[0][i].type = CellType::Wall;
	for (size_t i = 0; i < board.size(); i++)
		board[i][board.size() - 1].type = CellType::Wall;
	for (size_t i = 0; i < board.size(); i++)
		board[board.size() - 1][i].type = CellType::Wall;

	head.x = 15; head.y = 4;
	tail.x = 15; tail.y = 1;

	score = 0;
	snack_counter = 0;

	place_snack();
}

void SnakeGame::step()
{
	if (is_collision(direction))
		return;

	auto next = cell_at(next_coord(head, direction));
	if (next.type == CellType::Snack)
	{
		score += reward_snack;
		snack_counter++;
		place_snack();
	}
	else if (next.type == CellType::Dessert)
	{
		score += reward_dessert;
		snack_counter++;
		place_snack();
	}
	else
	{
		board[tail.x][tail.y].type = CellType::Empty;
		tail = next_coord(tail, board[tail.x][tail.y].related);
	}

	board[head.x][head.y].related = direction;
	head = next_coord(head, board[head.x][head.y].related);
	board[head.x][head.y].type = CellType::SnakeBody;
}

auto SnakeGame::get_board() const -> const decltype(SnakeGame::board) &
{
	return board;
}

void SnakeGame::set_direction(SnakeGame::Direction dir)
{
	if (!is_collision(dir))
		direction = dir;
}

auto SnakeGame::get_score() const -> const decltype(score) &
{
	return score;
}

void SnakeGame::load_map(const std::string &s)
{
	std::ifstream file(s);
	if (!file)
		return;

	reset();
	board.clear();

	std::string line;
	while (getline(file, line))
	{
		board.emplace_back(std::vector<GameCell>());
		for (auto symbol : line)
		{
			board.back().push_back(map_symbol_to_cell(symbol));
			if (map_symbol_is_head(symbol))
				head = { board.size() - 1, board.back().size() - 1 };
			else if (map_symbol_is_tail(symbol))
				tail = { board.size() - 1, board.back().size() - 1 };
		}
	}
	place_snack();
}


//
// Privates
//

bool SnakeGame::is_collision(Direction dir)
{
	auto next_pos = next_coord(head, dir);
	auto &next = cell_at(next_pos);
	if (next.type == CellType::SnakeBody || next.type == CellType::Wall)
		if (next_pos != tail) // by the next step the tail will have moved away
			return true;
	return false;
}

SnakeGame::GameCell &SnakeGame::cell_at(const coord_t &c)
{
	return board[c.x][c.y];
}

SnakeGame::coord_t SnakeGame::next_coord(coord_t c, Direction dir)
{
	size_t size_x = board.size(), size_y = board[c.x].size();
	return std::map<Direction, std::function<coord_t()>> {
		{ Direction::Up, [&]() { c.x = (c.x > 0 ? --c.x : size_x - 1); return c; } },
		{ Direction::Down,  [&]() { c.x = (++c.x) % size_x; return c; } },
		{ Direction::Left,  [&]() { c.y = (c.y > 0 ? --c.y : size_y - 1); return c; } },
		{ Direction::Right, [&]() { c.y = (++c.y) % size_y; return c;  } },
	}[dir]();
}

void SnakeGame::place_snack()
{
	if (!any_empty_cell())
		return;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> dist_x(0, board.size() - 1);
	std::uniform_int_distribution<size_t> dist_y(0, board[0].size() - 1);

	do {
		auto &cell = cell_at({ dist_x(mt), dist_y(mt) });
		if (cell.type != CellType::Empty)
			continue;
		else
		{
			cell.type = snack_counter % 10 == 0 && snack_counter > 0 ? CellType::Dessert : CellType::Snack;
			break;
		}

	} while (true);
}

bool SnakeGame::any_empty_cell()
{
	for (size_t i = 0; i < board.size(); i++)
		for (size_t j = 0; j < board[0].size(); j++)
			if (board[i][j].type == CellType::Empty)
				return true;
	return false;
}

SnakeGame::GameCell SnakeGame::map_symbol_to_cell(char c)
{
	c = toupper(c);
	switch (c)
	{
	case ' ':
		return GameCell(CellType::Empty);
	case '#':
		return GameCell(CellType::Wall);
		// Additional case because snake head has no predefined direction
	case 'E':
		return GameCell(CellType::SnakeBody);
		// Body + direction. 
		// WASD for body direction. HJKL for tail
	case 'W': case 'K':
		return GameCell(CellType::SnakeBody, Direction::Up);
	case 'A': case 'H':
		return GameCell(CellType::SnakeBody, Direction::Left);
	case 'S': case 'J':
		return GameCell(CellType::SnakeBody, Direction::Down);
	case 'D': case 'L':
		return GameCell(CellType::SnakeBody, Direction::Right);
	default:
		throw std::runtime_error("Unknown symbol in the game map");
	}
}

bool SnakeGame::map_symbol_is_tail(char c)
{
	auto tails = { 'H', 'J', 'K', 'L' };
	return std::find(tails.begin(), tails.end(), toupper(c)) != tails.end();
}

bool SnakeGame::map_symbol_is_head(char c)
{
	auto heads = { 'E' };
	return std::find(heads.begin(), heads.end(), toupper(c)) != heads.end();
}