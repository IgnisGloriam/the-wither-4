#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// Константы игры
const float HeroSpeed = 200.0f;
const float EnemySpeed = 100.0f;
const float SwordCooldown = 0.5f;
const float SwordDuration = 0.3f;
const float SwordRotationSpeed = 360.0f; // Скорость вращения меча (градусы в секунду)

// Функция для вычисления дистанции между двумя точками
float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return std::sqrt((a.x - b.x) * (a.y - b.y));
}

// Функция для вычисления угла между двумя точками
float angleToTarget(const sf::Vector2f& source, const sf::Vector2f& target) {
    return std::atan2(target.y - source.y, target.x - source.x) * 180 / 3.14159265;
}

int main() {
    // Создаем окно приложения
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hero vs Enemies");

    // Загружаем текстуру героя
    sf::Texture heroTexture;
    if (!heroTexture.loadFromFile("images/hero.png")) {
        std::cerr << "Error: Could not load hero texture" << std::endl;
        return -1;
    }
    sf::Sprite hero(heroTexture);
    hero.setOrigin(heroTexture.getSize().x / 2, heroTexture.getSize().y / 2);
    hero.setPosition(400, 300);

    // Загружаем текстуру врага
    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("images/enemy.png")) {
        std::cerr << "Error: Could not load enemy texture" << std::endl;
        return -1;
    }

    // Загружаем текстуру меча
    sf::Texture swordTexture;
    if (!swordTexture.loadFromFile("images/sword.png")) {
        std::cerr << "Error: Could not load sword texture" << std::endl;
        return -1;
    }
    sf::Sprite sword(swordTexture);
    sword.setOrigin(swordTexture.getSize().x / 2, swordTexture.getSize().y / 2);

    // Вектор для хранения врагов
    std::vector<sf::Sprite> enemies;

    // Таймер для создания врагов и анимации атаки
    sf::Clock clock;
    float timeSinceLastSwordAttack = 0.0f;
    float swordElapsedTime = 0.0f;
    bool swordSwinging = false;

    // Параметры для увеличения числа врагов
    int initialEnemySpawnRate = 1; // Начальная вероятность спавна врага
    int enemySpawnRate = initialEnemySpawnRate;
    float timeSinceStart = 0.0f;
    float spawnIncreaseInterval = 2000.0f; // Интервал времени, через который увеличивается количество врагов

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        timeSinceLastSwordAttack += deltaTime;
        timeSinceStart += deltaTime;

        if (swordSwinging) {
            swordElapsedTime += deltaTime;
            if (swordElapsedTime >= SwordDuration) {
                swordSwinging = false;
            }
            else {
                sword.rotate(SwordRotationSpeed * deltaTime);
            }
        }

        // Управление героем
        sf::Vector2f movement(0.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            movement.y -= HeroSpeed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            movement.y += HeroSpeed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            movement.x -= HeroSpeed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            movement.x += HeroSpeed * deltaTime;
        hero.move(movement);

        // Атака мечом по нажатию левой кнопки мыши
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && timeSinceLastSwordAttack >= SwordCooldown) {
            timeSinceLastSwordAttack = 0.0f;
            swordElapsedTime = 0.0f;
            swordSwinging = true;

            // Поиск ближайшего врага для атаки
            if (!enemies.empty()) {
                auto closestEnemy = std::min_element(enemies.begin(), enemies.end(), [&](const sf::Sprite& a, const sf::Sprite& b) {
                    return distance(hero.getPosition(), a.getPosition()) < distance(hero.getPosition(), b.getPosition());
                    });

                // Устанавливаем меч в позицию героя и поворачиваем к ближайшему врагу
                if (closestEnemy != enemies.end()) {
                    sword.setPosition(hero.getPosition());
                    sword.setRotation(angleToTarget(hero.getPosition(), closestEnemy->getPosition()));
                }
            }
        }

        // Создание врагов с увеличением частоты спавна
        if (std::rand() % 100 < enemySpawnRate) {
            sf::Sprite enemy(enemyTexture);
            float angle = std::rand() % 360 * 3.14f / 180;
            enemy.setPosition(hero.getPosition().x + std::cos(angle) * 500, hero.getPosition().y + std::sin(angle) * 500);
            enemies.push_back(enemy);
        }

        // Увеличение частоты спавна врагов
        if (timeSinceStart >= spawnIncreaseInterval) {
            enemySpawnRate += 10;
            timeSinceStart = 0.0f;
        }

        // Обновление врагов
        for (auto& enemy : enemies) {
            sf::Vector2f direction = hero.getPosition() - enemy.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length;
            enemy.move(direction * EnemySpeed * deltaTime);
        }

        // Проверка коллизий между мечом и врагами
        if (swordSwinging) {
            auto it = enemies.begin();
            while (it != enemies.end()) {
                if (distance(sword.getPosition(), it->getPosition()) < 50) {
                    it = enemies.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        // Рендеринг
        window.clear();
        window.draw(hero);
        for (const auto& enemy : enemies) {
            window.draw(enemy);
        }
        if (swordSwinging) {
            window.draw(sword);
        }
        window.display();
    }

    return 0;
}
















//#include <iostream>
//
//#include <SFML/Graphics.hpp>
//
//#include "Player.h"
//
//using namespace sf;
//
//
//int main()
//{
//	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Sprite Example");
//
//
//	sf::Texture texture;
//	if (!texture.loadFromFile("images/hero.png")) {
//		std::cerr << "Error: Could not load texture" << std::endl;
//		return -1;
//	}
//
//
//	sf::Sprite herosprite;
//	herosprite.setTexture(texture);
//
//	herosprite.setPosition(250, 250);
//
//	double plSpeed = 0.1;
//
//	
//	while (window.isOpen()) {
//		// Обрабатываем события
//		sf::Event event;
//		while (window.pollEvent(event)) {
//			if (event.type == sf::Event::Closed) {
//				window.close();
//			}
//		}
//		
//		if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) { herosprite.move(-plSpeed, 0);  } //добавили управление на клавиши W,S,A,D
//		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) { herosprite.move(plSpeed, 0);  }
//		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) { herosprite.move(0, -plSpeed);  }
//		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { herosprite.move(0, plSpeed);  }
//
//		// Очищаем экран
//		window.clear();
//
//		// Рисуем спрайт
//		window.draw(herosprite);
//
//		// Отображаем содержимое окна
//		window.display();
//	}
//	return 0;
//}
//
//
//int main1()
//{
//	RenderWindow window(sf::VideoMode(640, 480), "Lesson 5. kychka-pc.ru");
//
//
//
//	Texture herotexture;
//	herotexture.loadFromFile("images/hero.png");
//
//	Sprite herosprite;
//	herosprite.setTexture(herotexture);
//	herosprite.setTextureRect(IntRect(0, 192, 96, 96));//получили нужный нам прямоугольник с котом
//	herosprite.setPosition(250, 250); //выводим спрайт в позицию x y 
//
//
//
//	double plSpeed = 0.1;
//	
//	while (window.isOpen())
//	{
//
//
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//
//		if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) { herosprite.move(plSpeed, 0); herosprite.setTextureRect(IntRect(0, 96, 96, 96)); } //добавили управление на клавиши W,S,A,D
//		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) { herosprite.move(plSpeed, 0); herosprite.setTextureRect(IntRect(0, 192, 96, 96)); }
//		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) { herosprite.move(0, -plSpeed); herosprite.setTextureRect(IntRect(0, 288, 96, 96)); }
//		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { herosprite.move(0, plSpeed); herosprite.setTextureRect(IntRect(0, 0, 96, 96)); }
//		window.clear();
//		window.draw(herosprite);
//		window.display();
//	}
//
//	return 0;
//}