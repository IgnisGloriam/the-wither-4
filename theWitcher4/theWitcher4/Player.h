#pragma once
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>


#include <SFML/Graphics.hpp>


class player
{
public:
	player();
	// направления движения игрока
	enum class playermove {
		uppressed, uprg, uplf, downpressed,
		downrg, downlf, leftpressed, rightpressed, stop
	};
	// метод появление игрока на игровом поле
	//void spawn(sf::intrect planet, sf::vector2f resolution, int tilesize);
	// метод рестарт параметров игрока
	void resetplayerstats();
	// возвращает состояние жизни игрока
	bool getlive() const;
	// метод получения урона игроком
	//bool hit(sf::time timehit);
	// возвращает время как давно был последний удар по игроку
	//sf::time getlasthittime() const;
	// возвращает координаты игрока
	//sf::floatrect getposition() const;
	// возвращает центральные координаты игрока
	//sf::vector2f getcenter() const;
	// возвращает угол поворота игрока
	float getrotation() const;
	// возвращает копию спрайта игрока
	//sf::sprite getsprite() const;
	// рисуем игрока
	//void draw(sf::renderwindow& win) const;
	// перемещаем игрока
	void move(playermove mov);
	// обновление игровой логики 
	//void update(sf::time deltatime, sf::vector2i mouseposition);
	// увеличиваем максимальное количество здоровья 
	void upgradehealth(float heal);
	// пополняем здоровье игрока
	void increasehealthlevel(float amount);
	// возвращает сколько здоровья у игрока на данный момент?
	float gethealth() const;
	// возвращает максимальное здоровье игрока
	float getmaxhealth() const;

private:
	// стартовая жизнь игрока
	const float start_health = 200;
	// позиция игрока
	//sf::vector2f m_position;
	// отображение игрока спрайт и  объект анимации
	//sf::sprite m_sprite;
	
	// разрешение анимировать игрока
	bool m_animmove = false;
	// разрешение экрана
	//sf::vector2f m_resolution;
	// размер игрового поля 
	//sf::intrect m_planet;
	// размер текстур игрового поля 
	int m_tilesize;
	// в каком направлении(ях) движется игрок в данный момент
	playermove m_move;
	// живой ли игрок
	bool m_live = true;
	// здоровье игрока
	float m_health;
	// максимальное здоровье игрока
	float m_maxhealth;
	// время нанесения последнего удара по игроку 
	//sf::time m_lasthit;
	// частота перемещения игрока
	//sf::time m_time_moving;
	// скорость перемещения игрока в пикселях в секунду
	float m_speed;

};
