#include "Player.h"

Player::Player() {
	// ����� ��������� �������� ��������� ������
	m_Speed = 2;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;
	m_move = playermove::Stop;
	m_TileSize = 0;
	// �������� ������
	auto& idleForward = m_AnimPlayer.CreateAnimation("idleForward", "graphics/player.png", sf::seconds(0.5), true);
	idleForward.AddFrames(sf::Vector2i(0, 0), sf::Vector2i(135, 105), 4, 1);
	auto& dead = m_AnimPlayer.CreateAnimation("dead", "graphics/player.png", sf::seconds(0.5), false);
	dead.AddFrames(sf::Vector2i(405, 0), sf::Vector2i(135, 105), 4, 1);
	m_AnimPlayer.SwitchAnimation("idleForward");
	m_AnimPlayer.Update(sf::seconds(0));
	// ������������� ���������� ������� � ����� 
	m_Sprite.setOrigin(m_Sprite.getGlobalBounds().width / 2, m_Sprite.getGlobalBounds().height / 2);
}