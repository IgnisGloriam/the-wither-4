#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

// ��������� ����
const int h = 800, w = 600;
const float SwordDuration = 0.3f;
const float SwordRotationSpeed = 120.0f; // �������� �������� ���� (������� � �������)
const float CameraSmoothness = 0.1f; // ����������� ��������� ������
const sf::Vector2f MapSize(2000, 2000); // ������ �����


const float FireDuration = 3.0f; // ����������������� ����� ����

// ��������� ��� �������� �������� ���������
struct Fireball {
	sf::Sprite sprite;
	sf::Vector2f direction;
	float elapsedTime;
};

// ������� ��� ���������� ��������� ����� ����� �������
float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
	return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// ������� ��� ���������� ���� ����� ����� �������
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
	// ������� ���� ����������
	sf::RenderWindow window(sf::VideoMode(h, w), "Hero vs Enemies");

	// ��������� �������� ��� ����
	sf::Texture witcherTexture;
	if (!witcherTexture.loadFromFile("images/witcher.png")) {
		std::cerr << "Error: Could not load witcher texture" << std::endl;
		return -1;
	}
	sf::Sprite witcher(witcherTexture);
	witcher.setOrigin(witcherTexture.getSize().x / 2, witcherTexture.getSize().y / 2);
	witcher.setPosition(h / 2, w / 4);

	// ��������� �������� ��� ��������
	sf::Texture witcherDTexture;
	if (!witcherDTexture.loadFromFile("images/witcherDead.png")) {
		std::cerr << "Error: Could not load witcher texture" << std::endl;
		return -1;
	}
	sf::Sprite witcherD(witcherDTexture);
	witcherD.setOrigin(witcherDTexture.getSize().x / 2, witcherDTexture.getSize().y / 2);
	witcherD.setPosition(h / 2, w / 4);

	// ��������� �������� �����
	sf::Texture heroTexture;
	if (!heroTexture.loadFromFile("images/hero.png")) {
		std::cerr << "Error: Could not load hero texture" << std::endl;
		return -1;
	}
	sf::Sprite hero(heroTexture);
	hero.setOrigin(heroTexture.getSize().x / 2, heroTexture.getSize().y / 2);
	hero.setPosition(MapSize.x / 2, MapSize.y / 2);

	// ��������� �������� �����
	sf::Texture enemyTexture;
	if (!enemyTexture.loadFromFile("images/enemy.png")) {
		std::cerr << "Error: Could not load enemy texture" << std::endl;
		return -1;
	}

	// ��������� �������� ����
	sf::Texture swordTexture;
	if (!swordTexture.loadFromFile("images/sword.png")) {
		std::cerr << "Error: Could not load sword texture" << std::endl;
		return -1;
	}
	sf::Sprite sword(swordTexture);
	sword.setOrigin(0, swordTexture.getSize().y / 2);

	// ��������� �������� ��������� ����
	sf::Texture fireballTexture;
	if (!fireballTexture.loadFromFile("images/fireball.png")) {
		std::cerr << "Error: Could not load fireball texture" << std::endl;
		return -1;
	}

	// ��������� �������� ���� ��� ����
	sf::Texture fieldTexture;
	if (!fieldTexture.loadFromFile("images/field.png")) {
		std::cerr << "Error: Could not load field texture" << std::endl;
		return -1;
	}
	sf::Sprite field(fieldTexture);
	field.setScale(MapSize.x / fieldTexture.getSize().x, MapSize.y / fieldTexture.getSize().y);

	// ����� ��� ����������������� ����������
	sf::Font font;
	if (!font.loadFromFile("images/FFFFORWA.TTF")) {
		std::cerr << "Error: Could not load font" << std::endl;
		return -1;
	}

	// ����� ��� ���������� ����
	sf::Text mainMenuText("Press Enter to Start", font, 48);
	mainMenuText.setPosition(100, 300);

	// ����� ��� ���� ��� ������
	sf::Text gameOverText("", font, 48);
	gameOverText.setPosition(50, 300);

	// ����� ��� ����������� ���������� ������ � ������ ������
	sf::Text livesText("Lives: 3", font, 24);
	sf::Text killsText("Kills: 0", font, 24);
	livesText.setPosition(10, 10);
	killsText.setPosition(10, 40);

	// ����� ��� ���� ���������
	sf::Text upgradeMenuText("Choose an upgrade:\n1. Increase Speed\n2. Decrease Sword Cooldown\n3. Decrease Fire Cooldown\n4. Decrease Enemy Speed\n5. Increase Fireball Speed", 
		font, 20);
	upgradeMenuText.setPosition(100, 200);

	// ������ ��� �������� ������
	std::vector<sf::Sprite> enemies;

	// ������ ��� �������� �������� ���������
	std::vector<Fireball> fireballs;

	// ������ ��� �������� ������ � �������� �����
	sf::Clock clock;
	float timeSinceLastSwordAttack = 0.0f;
	float swordElapsedTime = 0.0f;
	bool swordSwinging = false;

	// ������ ��� ��������� �����
	float timeSinceLastFire = 0.0f;

	// ��������� ��� ���������� ����� ������
	int initialEnemySpawnRate = 100; // ��������� ����������� ������ �����
	int enemySpawnRate = initialEnemySpawnRate;
	float timeSinceStart = 0.0f;
	float spawnIncreaseInterval = 5.0f; // �������� �������, ����� ������� ������������� ���������� ������

	// ��������� ����
	int lives = 3;
	int kills = 0;
	int tempKills = 0;

	// ������������� ��������� ������� ������
	sf::View camera(sf::Vector2f(MapSize.x / 2, MapSize.y / 2), sf::Vector2f(h, w));

	// ������������� ��������� ��������� ����
	GameState gameState = GameState::MainMenu;


	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float deltaTime = clock.restart().asSeconds();

		if (gameState == GameState::MainMenu) {
			// ��������� ����
			window.clear();
			window.draw(witcher);
			window.draw(mainMenuText);
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				gameState = GameState::Playing;
				lives = 3;
				kills = 0;
				tempKills = 0;
				enemies.clear();
				fireballs.clear();
				hero.setPosition(MapSize.x / 2, MapSize.y / 2);
			}
		}
		else if (gameState == GameState::Playing) {

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

			// ���������� ������
			{
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();
				mousePosition.x -= h / 2;//�������� �� �����
				mousePosition.y -= w / 2;
				hero.setRotation(angleToTarget(hero.getPosition(), mousePosition));
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

			// ������������ �������� ����� ��������� �����
			sf::Vector2f newHeroPosition = hero.getPosition() + movement;
			if (newHeroPosition.x > 0 && newHeroPosition.x < MapSize.x && newHeroPosition.y > 0 && newHeroPosition.y < MapSize.y) {
				hero.move(movement);
			}

			// ����� ����� �� ������� ����� ������ ����
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && timeSinceLastSwordAttack >= SwordCooldown) {
				timeSinceLastSwordAttack = 0.0f;
				swordElapsedTime = 0.0f;
				swordSwinging = true;

				// ������������� ��� � ������� ����� � ������������ � ����������� �������
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();
				mousePosition.x -= h / 2;//�������� �� �����
				mousePosition.y -= w / 2;
				sword.setPosition(hero.getPosition());
				sword.setRotation(angleToTarget(hero.getPosition(), mousePosition) - 20); // ��� �� ������ ����, -20 ��������� ������� ����������, �����, � �� ��������
			}

			// ������� ����� �� ������� ������ ������ ����
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

				fireballs.push_back(fireball);
			}

			// �������� ������ � ����������� ������� ������
			if (std::rand() % 10000 < enemySpawnRate) {
				sf::Sprite enemy(enemyTexture);
				float angle = std::rand() % 360 * 3.14f / 180;
				enemy.setPosition(hero.getPosition().x + std::cos(angle) * 500, hero.getPosition().y + std::sin(angle) * 500);
				enemies.push_back(enemy);
			}

			// ���������� ������� ������ ������
			if (timeSinceStart >= spawnIncreaseInterval) {
				enemySpawnRate += 100;
				timeSinceStart = 0.0f;
			}

			// ���������� ������
			for (auto& enemy : enemies) {
				sf::Vector2f direction = hero.getPosition() - enemy.getPosition();
				float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
				direction /= length;
				enemy.move(direction * EnemySpeed * deltaTime);
			}

			// �������� �������� ����� ����� � �������
			if (swordSwinging) {
				//std::cout << sword.getPosition().x << ' ' << sword.getPosition().y << '\n' << '\n' << '\n';
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				mousePosition += hero.getPosition();//�������� �� �����
				mousePosition.x -= h / 2;//�������� �� �����
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

			// ���������� �������� �����
			auto fireIt = fireballs.begin();
			while (fireIt != fireballs.end()) {
				fireIt->elapsedTime += deltaTime;
				if (fireIt->elapsedTime >= FireDuration) {
					fireIt = fireballs.erase(fireIt);
				}
				else {
					fireIt->sprite.move(fireIt->direction * FireSpeed * deltaTime);
					++fireIt;
				}
			}

			// �������� �������� ����� ��������� ������ � �������
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

			// �������� �������� ����� ������� � ������
			for (const auto& enemy : enemies) {
				if (distance(hero.getPosition(), enemy.getPosition()) < 50) {
					lives--;
					enemies.clear();
					if (lives <= 0) {
						gameState = GameState::GameOver;
					}
					break;
				}
			}

			// ���������� ������ ����������������� ����������
			livesText.setString("Lives: " + std::to_string(lives));
			killsText.setString("Kills: " + std::to_string(kills));

			// ������� �������� ������ �� ������
			sf::Vector2f cameraTarget = hero.getPosition();
			sf::Vector2f cameraPosition = camera.getCenter();
			sf::Vector2f cameraMovement = CameraSmoothness * (cameraTarget - cameraPosition);
			camera.setCenter(cameraPosition + cameraMovement);

			// ������������� ������
			window.setView(camera);

			// ���������
			window.clear();
			window.draw(field); // ������ ���� �� ������ �����
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

			// ������ ���������������� ���������
			window.setView(window.getDefaultView());
			window.draw(livesText);
			window.draw(killsText);
			window.display();



			// ������� � ���� ��������� ��� ���������� 100 �������
			if (tempKills >= UpgradeRequided) {
				tempKills -= UpgradeRequided;
				UpgradeRequided += 50;
				gameState = GameState::UpgradeMenu;
			}
		}


		else if (gameState == GameState::GameOver) {
			// ��������� ����
			window.clear();
			window.draw(witcherD);
			gameOverText.setString("Game Over! Kills: " + std::to_string(kills) + "\nPress Enter to Restart");
			window.draw(gameOverText);
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				gameState = GameState::Playing;
				lives = 3;
				kills = 0;
				tempKills = 0;
				enemies.clear();
				fireballs.clear();
				hero.setPosition(MapSize.x / 2, MapSize.y / 2);
			}
		}

		else if (gameState == GameState::UpgradeMenu) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
				HeroSpeed += 50.0f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
				SwordCooldown -= 0.03f;
				if (SwordCooldown < 0.3f) SwordCooldown = 0.3f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
				FireCooldown -= 0.2f;
				if (FireCooldown < 0.1f) FireCooldown = 0.1f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
				EnemySpeed -= 0.1f;
				if (EnemySpeed < 0.1f) EnemySpeed = 0.1f;
				gameState = GameState::Playing;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
				FireSpeed += 0.1f;
				gameState = GameState::Playing;
			}

			// ��������� ���� ���������
			window.clear();
			window.draw(upgradeMenuText);
			window.display();
		}
		
	}

	return 0;
}
