#include <SFML/Graphics.hpp>
#include "snake_game.h"

class SnakeRender : public sf::Drawable, public sf::Transformable
{
	mutable sf::RectangleShape piece = sf::RectangleShape(sf::Vector2f(20, 20));
	mutable sf::CircleShape snack = sf::CircleShape(5);
	mutable sf::RectangleShape dessert = sf::RectangleShape(sf::Vector2f(10, 10));
	mutable sf::RectangleShape wall = sf::RectangleShape(sf::Vector2f(21, 21));

	SnakeGame game;
	
public:

	SnakeRender() 
	{
		dessert.rotate(45); 
		wall.setFillColor(sf::Color(255, 0, 0));
		game.load_map("level01.txt");
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = NULL;

		for (size_t i = 0; i < game.get_board().size(); i++)
		{
			const auto &row = game.get_board()[i];
			for (size_t j = 0; j < row.size(); j++)
			{
				if (row[j].type == SnakeGame::CellType::SnakeBody)
				{
					piece.setPosition(sf::Vector2f(j*21.f, i * 21.f));
					target.draw(piece, states);
				}
				else if (row[j].type == SnakeGame::CellType::Snack)
				{
					snack.setPosition(sf::Vector2f(j * 21.f, i * 21.f));
					target.draw(snack, states);
				}
				else if (row[j].type == SnakeGame::CellType::Dessert)
				{
					dessert.setPosition(sf::Vector2f(j * 21.f, i * 21.f));
					target.draw(dessert, states);
				}
				else if (row[j].type == SnakeGame::CellType::Wall)
				{
					wall.setPosition(sf::Vector2f(j * 21.f, i * 21.f));
					target.draw(wall, states);
				}

			}
		}
	}

	void step()
	{
		game.step();	
	}

	void setDirection(SnakeGame::Direction dir)
	{
		game.set_direction(dir);
	}

};



int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(650, 650), "Snek");

	// create a clock to track the elapsed time
	sf::Clock clock;
	SnakeRender renderer;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				renderer.setDirection(SnakeGame::Direction::Right);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				renderer.setDirection(SnakeGame::Direction::Left);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				renderer.setDirection(SnakeGame::Direction::Up);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				renderer.setDirection(SnakeGame::Direction::Down);
		}

		// make the particle system emitter follow the mouse
		//sf::Vector2i mouse = sf::Mouse::getPosition(window);
		if (clock.getElapsedTime().asMilliseconds() >= 100)
		{
			clock.restart();
			renderer.step();
		}
			
		// draw it
		window.clear();
		//window.draw(sinuser);
		window.draw(renderer);
		window.display();
	}


	return 0;
}

#ifdef _WIN32

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, int nCmdShow)
{
	return main();
}

#endif