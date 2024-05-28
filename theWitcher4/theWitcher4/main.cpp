#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <fstream>

// Константы игры
const int h = 800, w = 600;
const float SwordDuration = 0.3f;
const float SwordRotationSpeed = 200.0f; // Скорость вращения меча (градусы в секунду)
const float CameraSmoothness = 0.1f; // Коэффициент плавности камеры
const sf::Vector2f MapSize(5000, 5000); // Размер карты


const float FireDuration = 3.0f; // Продолжительность полёта огня

// Структура для хранения огненных выстрелов
struct Fireball {
	sf::Sprite sprite;
	sf::Vector2f direction;
	float elapsedTime;
};

// Функция для вычисления дистанции между двумя точками
float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
	return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Функция для вычисления угла между двумя точками
float angleToTarget(const sf::Vector2f& source, const sf::Vector2f& target) {
	return std::atan2(target.y - source.y, target.x - source.x) * 180 / 3.14159265;
}

enum class GameState {
	MainMenu,
	Playing,
	GameOver,
	UpgradeMenu
};

int main() {

	float HeroSpeed = 200.0f;
	float EnemySpeed = 100.0f; /////
	float SwordCooldown = 1.0f;
	float FireSpeed = 500.0f; ////
	float FireCooldown = 1.0f;
	int UpgradeRequided = 100;
	// Создаем окно приложения
	sf::RenderWindow window(sf::VideoMode(h, w), "the Witcher 4");

	// Загружаем текстуру для меню
	sf::Texture witcherTexture;
	if (!witcherTexture.loadFromFile("images/witcher.png")) {
		std::cerr << "Error: Could not load witcher texture" << std::endl;
		return -1;
	}
	sf::Sprite witcher(witcherTexture);
	witcher.setOrigin(witcherTexture.getSize().x / 2, witcherTexture.getSize().y / 2);
	witcher.setPosition(h / 2, w / 4);

	// Загружаем текстуру для рестарта
	sf::Texture witcherDTexture;
	if (!witcherDTexture.loadFromFile("images/witcherDead.png")) {
		std::cerr << "Error: Could not load witcherDead texture" << std::endl;
		return -1;
	}
	sf::Sprite witcherD(witcherDTexture);
	witcherD.setOrigin(witcherDTexture.getSize().x / 2, witcherDTexture.getSize().y / 2);
	witcherD.setPosition(h / 2, w / 4);

	// Загружаем текстуру героя
	sf::Texture heroTexture;
	if (!heroTexture.loadFromFile("images/hero.png")) {
		std::cerr << "Error: Could not load hero texture" << std::endl;
		return -1;
	}
	sf::Sprite hero(heroTexture);
	hero.setOrigin(heroTexture.getSize().x / 2, heroTexture.getSize().y / 2);
	hero.setPosition(MapSize.x / 2, MapSize.y / 2);

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
	sword.setOrigin(-14, swordTexture.getSize().y / 2);

	// Сердечко и score
	sf::Texture tHeart;
	if (!tHeart.loadFromFile("images/Heart.png")) {
		std::cerr << "Error: Could not load Heart texture" << std::endl;
		return -1;
	}
	sf::Sprite Heart(tHeart);
	Heart.setOrigin(tHeart.getSize().x / 2, tHeart.getSize().y / 2);
	Heart.setPosition(30, 30);

	sf::Texture tStar;
	if (!tStar.loadFromFile("images/Star.png")) {
		std::cerr << "Error: Could not load Star texture" << std::endl;
		return -1;
	}
	sf::Sprite Star(tStar);
	Star.setOrigin(tStar.getSize().x / 2, tStar.getSize().y / 2);
	Star.setPosition(30, 70);

	// Загружаем текстуру огненного шара
	sf::Texture fireballTexture;
	if (!fireballTexture.loadFromFile("images/fireball.png")) {
		std::cerr << "Error: Could not load fireball texture" << std::endl;
		return -1;
	}

	// Загружаем текстуру поля для фона
	sf::Texture fieldTexture;
	if (!fieldTexture.loadFromFile("images/field.png")) {
		std::cerr << "Error: Could not load field texture" << std::endl;
		return -1;
	}
	sf::Sprite field(fieldTexture);
	field.setScale(MapSize.x / fieldTexture.getSize().x, MapSize.y / fieldTexture.getSize().y);

	// Шрифт для пользовательского интерфейса
	sf::Font font;
	if (!font.loadFromFile("images/FFFFORWA.TTF")) {
		std::cerr << "Error: Could not load font" << std::endl;
		return -1;
	}

	// Текст для начального меню
	sf::Text mainMenuText("Press Enter to Start", font, 48);
	mainMenuText.setPosition(100, 300);

	// Текст для меню при смерти
	sf::Text gameOverText("", font, 48);
	gameOverText.setPosition(50, 300);

	// Текст для отображения количества жизней и убитых врагов
	sf::Text livesText("Lives: 3", font, 24);
	sf::Text killsText("Kills: 0", font, 24);
	livesText.setPosition(60, 20);
	killsText.setPosition(60, 60);

	// Текст для меню улучшений
	sf::Text upgradeMenuText("Choose an upgrade:\n1. Increase Speed\n2. Decrease Sword Cooldown\n3. Decrease Fire Cooldown\n4. Decrease Enemy Speed\n5. Increase Fireball Speed\n6. Increase Lives", 
		font, 20);
	upgradeMenuText.setPosition(100, 200);

	// Вектор для хранения врагов
	std::vector<sf::Sprite> enemies;

	// Вектор для хранения огненных выстрелов
	std::vector<Fireball> fireballs;

	// Таймер для создания врагов и анимации атаки
	sf::Clock clock;
	float timeSinceLastSwordAttack = 0.0f;
	float swordElapsedTime = 0.0f;
	bool swordSwinging = false;

	// Таймер для выстрелов огнём
	float timeSinceLastFire = 0.0f;

	// Параметры для увеличения числа врагов
	int initialEnemySpawnRate = 600; // Задержка спавна врага
	int enemySpawnRate = initialEnemySpawnRate;
	int enemySpawn = 0;
	float timeSinceStart = 0.0f;
	float spawnIncreaseInterval = 10.0f; // Интервал времени, через который увеличивается количество врагов

	// Параметры игры
	int lives = 3;
	int kills = 0;
	int tempKills = 0;

	// Устанавливаем начальную позицию камеры
	sf::View camera(sf::Vector2f(MapSize.x / 2, MapSize.y / 2), sf::Vector2f(h, w));

	// Устанавливаем начальное состояние игры
	GameState gameState = GameState::MainMenu;

	// Загружаем музыку для меню и геймплея
	sf::Music menuMusic;
	if (!menuMusic.openFromFile("audio/witcherTheme.ogg")) {
		std::cerr << "Error: Could not load menu music" << std::endl;
		return -1;
	}

	sf::Music gameMusic;
	if (!gameMusic.openFromFile("audio/witcherFight.ogg")) {
		std::cerr << "Error: Could not load game music" << std::endl;
		return -1;
	}


	sf::SoundBuffer deathSoundBuffer;
	if (!deathSoundBuffer.loadFromFile("audio/deathSound.ogg")) {
		std::cerr << "Error: Could not load death sound" << std::endl;
		return -1;
	}
	sf::Sound deathSound(deathSoundBuffer);

	sf::SoundBuffer winSoundBuffer;
	if (!winSoundBuffer.loadFromFile("audio/winSound.ogg")) {
		std::cerr << "Error: Could not load win sound" << std::endl;
		return -1;
	}
	sf::Sound winSound(winSoundBuffer);

	std::set <int> scores;
	int s;
	std::ifstream fin("score.txt");
	if (fin.is_open())
	{
		while (true) {
			if (fin.eof())
				break;
			fin >> s;
			scores.insert(s);
			if (fin.eof())
				break;

		}
		
	}
	fin.close();

	bool youWin = false;

	menuMusic.play();
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float deltaTime = clock.restart().asSeconds();

		if (gameState == GameState::MainMenu) {
			if (menuMusic.getStatus() != sf::Music::Playing) {
				menuMusic.play();
			}
			// Рендеринг меню
			window.clear();
			window.draw(witcher);
			window.draw(mainMenuText);
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				gameState = GameState::Playing;
				lives = 3;
				kills = 0;
				tempKills = 0;
				enemies.clear();
				fireballs.clear();
				hero.setPosition(MapSize.x / 2, MapSize.y / 2);
				menuMusic.stop();
				gameMusic.play();
			}
		}
		else if (gameState == GameState::Playing) {
			if (gameMusic.getStatus() != sf::Music::Playing) {
				gameMusic.play();
			}
			timeSinceLastSwordAttack += deltaTime;
			timeSinceStart += deltaTime;
			timeSinceLastFire += deltaTime;

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
			{
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();
				mousePosition.x -= h / 2;//поправка на ветер
				mousePosition.y -= w / 2;
				hero.setRotation(angleToTarget(hero.getPosition(), mousePosition) + 90);
			}
			sf::Vector2f movement(0.0f, 0.0f);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				movement.y -= HeroSpeed * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				movement.y += HeroSpeed * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				movement.x -= HeroSpeed * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				movement.x += HeroSpeed * deltaTime;

			// Ограничиваем движение героя границами карты
			sf::Vector2f newHeroPosition = hero.getPosition() + movement;
			if (newHeroPosition.x > 0 + h / 2 && newHeroPosition.x < MapSize.x - h / 2 && newHeroPosition.y > 0 + h / 2 && newHeroPosition.y < MapSize.y - h / 2) {
				hero.move(movement);
			}

			// Атака мечом по нажатию левой кнопки мыши
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && timeSinceLastSwordAttack >= SwordCooldown) {
				timeSinceLastSwordAttack = 0.0f;
				swordElapsedTime = 0.0f;
				swordSwinging = true;

				// Устанавливаем меч в позицию героя и поворачиваем в направлении курсора
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();
				mousePosition.x -= h / 2;//поправка на ветер
				mousePosition.y -= w / 2;
				sword.setPosition(hero.getPosition());
				sword.setRotation(angleToTarget(hero.getPosition(), mousePosition) - 20); // Как бы размах меча, -20 начальное угловое отклонение, ПОМНИ, А ТО ЗАБУДЕШЬ
			}

			// Выстрел огнём по нажатию правой кнопки мыши
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && timeSinceLastFire >= FireCooldown) {
				timeSinceLastFire = 0.0f;
				Fireball fireball;
				fireball.sprite.setTexture(fireballTexture);
				fireball.sprite.setOrigin(fireballTexture.getSize().x / 2, fireballTexture.getSize().y / 2);
				fireball.sprite.setPosition(hero.getPosition());

				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				//std::cout << hero.getPosition().x << ' ' << hero.getPosition().y << '\n' << mousePosition.x << ' ' << mousePosition.y << '\n' << '\n' << '\n';
				mousePosition.x -= h / 2;
				mousePosition.y -= w / 2;
				fireball.direction = mousePosition;
				float length = std::sqrt(fireball.direction.x * fireball.direction.x + fireball.direction.y * fireball.direction.y);
				fireball.direction /= length;
				fireball.elapsedTime = 0.0f;
				sf::Vector2f mousePosition2 = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition2 += hero.getPosition();
				mousePosition2.x -= h / 2;//поправка на ветер
				mousePosition2.y -= w / 2;
				
				fireball.sprite.setRotation(angleToTarget(hero.getPosition(), mousePosition2)+90);
				fireballs.push_back(fireball);
			}

			// Создание врагов с увеличением частоты спавна
			if (enemySpawn > enemySpawnRate) {
				enemySpawn = 0;
				sf::Sprite enemy(enemyTexture);
				float angle = std::rand() % 360 * 3.14f / 180;
				enemy.setOrigin(enemyTexture.getSize().x / 2, enemyTexture.getSize().y / 2);
				enemy.setPosition(hero.getPosition().x + std::cos(angle) * 500, hero.getPosition().y + std::sin(angle) * 500);
				enemies.push_back(enemy);
			}
			else {
				++enemySpawn;
			}

			// Увеличение частоты спавна врагов
			if (timeSinceStart >= spawnIncreaseInterval) {
				enemySpawnRate /= 2;
				enemySpawnRate += 40;
				timeSinceStart = 0.0f;
			}

			// Обновление врагов
			for (auto& enemy : enemies) {
				
				enemy.setRotation(angleToTarget(enemy.getPosition(), hero.getPosition()) + 90);

				sf::Vector2f direction = hero.getPosition() - enemy.getPosition();
				float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
				direction /= length;
				enemy.move(direction * EnemySpeed * deltaTime);
			}

			// Проверка коллизий между мечом и врагами
			if (swordSwinging) {
				//std::cout << sword.getPosition().x << ' ' << sword.getPosition().y << '\n' << '\n' << '\n';
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();//поправка на ветер
				mousePosition.x -= h / 2;//поправка на ветер
				mousePosition.y -= w / 2;
				float f = distance(mousePosition, hero.getPosition());
				sf::Vector2f a = mousePosition - hero.getPosition();
				a /= f;
				float t = sword.getTextureRect().width;
				a *= t;
				sf::Vector2f b = a / 2.0f;

				a += hero.getPosition();
				b += hero.getPosition();
				;// +sword.getTextureRect().width;
				sword.getTextureRect().left + sword.getTextureRect().width;
				auto it = enemies.begin();
				int maxKills = 10;
				while (it != enemies.end() && maxKills > 0) {
					sword.setPosition(hero.getPosition());
					if (distance(sword.getPosition(), it->getPosition()) < 80 || distance(a, it->getPosition()) < 80 || distance(b, it->getPosition()) < 80) {
						it = enemies.erase(it);
						++kills;
						++tempKills;
						--maxKills;
					}
					else {
						++it;
					}
				}
			}

			// Обновление огненных шаров
			auto fireIt = fireballs.begin();
			while (fireIt != fireballs.end()) {
				fireIt->elapsedTime += deltaTime;
				//fireIt->setRotation(fireIt->angle)
				if (fireIt->elapsedTime >= FireDuration) {
					fireIt = fireballs.erase(fireIt);
				}
				else {
					fireIt->sprite.move(fireIt->direction * FireSpeed * deltaTime);
					++fireIt;
				}
			}

			// Проверка коллизий между огненными шарами и врагами
			for (auto fireballIt = fireballs.begin(); fireballIt != fireballs.end(); ) {
				bool hit = false;
				for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); ) {
					if (distance(fireballIt->sprite.getPosition(), enemyIt->getPosition()) < 50) {
						enemyIt = enemies.erase(enemyIt);
						++kills;
						++tempKills;
					}
					else {
						++enemyIt;
					}
				}
				if (hit) {
					fireballIt = fireballs.erase(fireballIt);
				}
				else {
					++fireballIt;
				}
			}

			// Проверка коллизий между врагами и героем
			for (const auto& enemy : enemies) {
				if (distance(hero.getPosition(), enemy.getPosition()) < 50) {
					lives--;
					enemies.clear();
					if (lives <= 0) {
						youWin = 0;
						auto t = scores.crbegin();
						if (kills > *t) {
							youWin = 1;
						}
						scores.insert(kills);
						gameState = GameState::GameOver;
						gameMusic.stop();
						//menuMusic.play();
						if(youWin)
							winSound.play();
						else
							deathSound.play();
					}
					break;
				}
			}

			// Обновление текста пользовательского интерфейса
			livesText.setString("Lives: " + std::to_string(lives));
			killsText.setString("Kills: " + std::to_string(kills));

			// Плавное движение камеры за героем
			sf::Vector2f cameraTarget = hero.getPosition();
			sf::Vector2f cameraPosition = camera.getCenter();
			sf::Vector2f cameraMovement = CameraSmoothness * (cameraTarget - cameraPosition);
			camera.setCenter(cameraPosition + cameraMovement);

			// Устанавливаем камеру
			window.setView(camera);

			// Рендеринг
			window.clear();
			window.draw(field); // Рисуем поле на заднем плане
			window.draw(hero);
			
			for (const auto& enemy : enemies) {
				window.draw(enemy);
			}
			for (const auto& fireball : fireballs) {
				window.draw(fireball.sprite);
			}
			if (swordSwinging) {
				window.draw(sword);
			}

			// Рисуем пользовательский интерфейс
			window.setView(window.getDefaultView());
			window.draw(livesText);
			window.draw(killsText);
			window.draw(Heart);
			window.draw(Star);
			window.display();



			// Переход в меню улучшений при достижении 100 убийств
			if (tempKills >= UpgradeRequided) {
				tempKills -= UpgradeRequided;
				UpgradeRequided += 50;
				gameState = GameState::UpgradeMenu;
			}
		}


		else if (gameState == GameState::GameOver) {

			if (menuMusic.getStatus() != sf::Music::Playing && deathSound.getStatus() != sf::Music::Playing && winSound.getStatus() != sf::Music::Playing) {
				menuMusic.play();
			}
			// Рендеринг меню
			window.clear();
			window.draw(witcherD);
			if (youWin)
				gameOverText.setString("New Record! Kills: " + std::to_string(kills) + "\nPress Enter to Restart");
			else
				gameOverText.setString("Game Over! Kills: " + std::to_string(kills) + "\nPress Enter to Restart");
			window.draw(gameOverText);
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				gameState = GameState::Playing;
				lives = 3;
				kills = 0;
				tempKills = 0;
				enemies.clear();
				fireballs.clear();
				hero.setPosition(MapSize.x / 2, MapSize.y / 2);
				menuMusic.stop();
				gameMusic.play();
			}
		}

		else if (gameState == GameState::UpgradeMenu) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
				HeroSpeed += 50.0f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {

				SwordCooldown = 0.3f + SwordCooldown / 1.5;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
				FireCooldown = 0.3f + FireCooldown / 1.5;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
				EnemySpeed = 0.2f + EnemySpeed / 1.5;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
				FireSpeed += 0.2f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
				++lives;
				gameState = GameState::Playing;
			}

			if (gameMusic.getStatus() != sf::Music::Playing) {
				gameMusic.play();
			}
			// Рендеринг меню улучшений
			window.clear();
			window.draw(upgradeMenuText);
			window.display();
		}
		
	}


	std::ofstream fout("score.txt");
	if (fout.is_open())
	{
		for (const auto& kill : scores) {
			fout << kill << '\n';
		}

	}
	fout.close();
	return 0;
}
