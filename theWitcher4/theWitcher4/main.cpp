#include <SFML/Graphics.hpp>

#include "Player.h"

using namespace sf;


int main()
{
	RenderWindow window(sf::VideoMode(640, 480), "Lesson 5. kychka-pc.ru");



	Texture herotexture;
	herotexture.loadFromFile("images/hero.png");

	Sprite herosprite;
	herosprite.setTexture(herotexture);
	herosprite.setTextureRect(IntRect(0, 192, 96, 96));//получили нужный нам прямоугольник с котом
	herosprite.setPosition(250, 250); //выводим спрайт в позицию x y 



	double plSpeed = 0.1;
	
	while (window.isOpen())
	{


		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) { herosprite.move(-Player.speed, 0); herosprite.setTextureRect(IntRect(0, 96, 96, 96)); } //добавили управление на клавиши W,S,A,D
		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) { herosprite.move(plSpeed, 0); herosprite.setTextureRect(IntRect(0, 192, 96, 96)); }
		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) { herosprite.move(0, -plSpeed); herosprite.setTextureRect(IntRect(0, 288, 96, 96)); }
		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { herosprite.move(0, plSpeed); herosprite.setTextureRect(IntRect(0, 0, 96, 96)); }
		window.clear();
		window.draw(herosprite);
		window.display();
	}

	return 0;
}