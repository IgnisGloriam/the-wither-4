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
	// ����������� �������� ������
	enum class playermove {
		uppressed, uprg, uplf, downpressed,
		downrg, downlf, leftpressed, rightpressed, stop
	};
	// ����� ��������� ������ �� ������� ����
	//void spawn(sf::intrect planet, sf::vector2f resolution, int tilesize);
	// ����� ������� ���������� ������
	void resetplayerstats();
	// ���������� ��������� ����� ������
	bool getlive() const;
	// ����� ��������� ����� �������
	//bool hit(sf::time timehit);
	// ���������� ����� ��� ����� ��� ��������� ���� �� ������
	//sf::time getlasthittime() const;
	// ���������� ���������� ������
	//sf::floatrect getposition() const;
	// ���������� ����������� ���������� ������
	//sf::vector2f getcenter() const;
	// ���������� ���� �������� ������
	float getrotation() const;
	// ���������� ����� ������� ������
	//sf::sprite getsprite() const;
	// ������ ������
	//void draw(sf::renderwindow& win) const;
	// ���������� ������
	void move(playermove mov);
	// ���������� ������� ������ 
	//void update(sf::time deltatime, sf::vector2i mouseposition);
	// ����������� ������������ ���������� �������� 
	void upgradehealth(float heal);
	// ��������� �������� ������
	void increasehealthlevel(float amount);
	// ���������� ������� �������� � ������ �� ������ ������?
	float gethealth() const;
	// ���������� ������������ �������� ������
	float getmaxhealth() const;

private:
	// ��������� ����� ������
	const float start_health = 200;
	// ������� ������
	//sf::vector2f m_position;
	// ����������� ������ ������ �  ������ ��������
	//sf::sprite m_sprite;
	
	// ���������� ����������� ������
	bool m_animmove = false;
	// ���������� ������
	//sf::vector2f m_resolution;
	// ������ �������� ���� 
	//sf::intrect m_planet;
	// ������ ������� �������� ���� 
	int m_tilesize;
	// � ����� �����������(��) �������� ����� � ������ ������
	playermove m_move;
	// ����� �� �����
	bool m_live = true;
	// �������� ������
	float m_health;
	// ������������ �������� ������
	float m_maxhealth;
	// ����� ��������� ���������� ����� �� ������ 
	//sf::time m_lasthit;
	// ������� ����������� ������
	//sf::time m_time_moving;
	// �������� ����������� ������ � �������� � �������
	float m_speed;

};
