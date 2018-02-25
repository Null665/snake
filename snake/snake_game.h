#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <vector>
#include <string>

class SnakeGame
{
public:
	// Direction in which the snake currently moves and to relate body pieces together
	enum class Direction { Up, Down, Left, Right };
	enum class CellType { Empty, SnakeBody, Wall, Snack, Dessert };

	struct coord_t
	{
		size_t x;
		size_t y;

		bool operator ==(const coord_t & c) { return x == c.x && y == c.y; }
		bool operator !=(const coord_t & c) { return !this->operator==(c); }
	};

private:
	// Direction the head is currently moving
	Direction direction;

	struct GameCell
	{
		CellType type = CellType::Empty;
		Direction related;

		GameCell() {}
		GameCell(CellType type) : type(type) {}
		GameCell(CellType type, Direction dir) : type(type), related(dir) {}

	};

	// Keeping track of head and tail positions
	coord_t head, tail;

	std::vector<std::vector<GameCell>> board;

	unsigned score;
	static const unsigned reward_snack = 10;
	static const unsigned reward_dessert = 50;
	unsigned snack_counter;

public:
	SnakeGame();

	void reset();
	void step();
	auto get_board() const -> const decltype(SnakeGame::board) &;
	void set_direction(SnakeGame::Direction dir);
	auto get_score() const -> const decltype(score) &;
	void load_map(const std::string &s);


private:

	// Checkis if the head will collide if moved in given direction
	bool is_collision(Direction dir);

	GameCell &cell_at(const coord_t &c);

	// Next coordinate when moving in given direction. 
	// Coordinates wrap around when going out of bounds
	coord_t next_coord(coord_t c, Direction dir);
	// Snack or a dessert, to be exact
	void place_snack();
	// If there i any empty cell in map. Used to check if it is possible to place a snack
	bool any_empty_cell();

	GameCell map_symbol_to_cell(char c);
	bool map_symbol_is_tail(char c);
	bool map_symbol_is_head(char c);
};


#endif