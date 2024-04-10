#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

enum class GameState { Menu, Game };

void transitionEffect(sf::RenderWindow& window, sf::RenderTexture& texture, sf::Sprite& sprite) {
    int alpha = 255;
    while (alpha >= 0) {
        sf::Sprite transitionSprite(texture.getTexture());
        transitionSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.clear();
        window.draw(sprite);
        window.draw(transitionSprite);
        window.display();
        alpha -= 5;
        sf::sleep(sf::milliseconds(50));
    }
}

void changeHue(sf::Sprite& sprite, float hue) {
    sf::Color color;
    float c = 1.0f;
    float x = (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f)) * c;
    float m = 0.0f;
    if (hue >= 0 && hue < 60) {
        color = sf::Color(255 * c, 255 * x, 0);
    } else if (hue >= 60 && hue < 120) {
        color = sf::Color(255 * x, 255 * c, 0);
    } else if (hue >= 120 && hue < 180) {
        color = sf::Color(0, 255 * c, 255 * x);
    } else if (hue >= 180 && hue < 240) {
        color = sf::Color(0, 255 * x, 255 * c);
    } else if (hue >= 240 && hue < 300) {
        color = sf::Color(255 * x, 0, 255 * c);
    } else {
        color = sf::Color(255 * c, 0, 255 * x);
    }
    sprite.setColor(color);
}

   sf::Color Grey(128, 128, 128); 

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "DVD Simulator (FIRST VERSION!)", sf::Style::Close);
    window.setFramerateLimit(120); // Limit the frame rate to 120 fps

    const int tileSize = 25;
    const int numTilesX = window.getSize().x / tileSize;
    const int numTilesY = window.getSize().y / tileSize;

    sf::RenderTexture checkerboardTexture;
    if (!checkerboardTexture.create(window.getSize().x, window.getSize().y)) {
        return -1;
    }
    for (int i = 0; i < numTilesX; ++i) {
        for (int j = 0; j < numTilesY; ++j) {
            sf::Color color = (i + j) % 2 == 0 ? sf::Color::White : Grey;
            sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(i * tileSize, j * tileSize);
            tile.setFillColor(color);
            checkerboardTexture.draw(tile);
        }
    }
    checkerboardTexture.display();
    sf::Sprite checkerboardSprite(checkerboardTexture.getTexture());

    sf::Texture dvdTexture;
    if (!dvdTexture.loadFromFile("Resources/dvd-logo.png")) {
        std::cerr << "Failed to load DVD texture!" << std::endl;
        return -1;
    }

    sf::Sprite dvdSprite(dvdTexture);
    dvdSprite.setScale(0.09f, 0.09f);
    dvdTexture.setSmooth(true);
    float dx = 1.0f;
    float dy = 1.0f;
    float hue = 1.0f;

    GameState gameState = GameState::Menu;

    sf::Texture startImageTexture;
    if (!startImageTexture.loadFromFile("Resources/start-image.png")) {
        std::cerr << "Failed to load start image texture!" << std::endl;
        return -1;
    }
    sf::Sprite startImageSprite(startImageTexture);
    float xPos = (window.getSize().x - startImageSprite.getLocalBounds().width) / 2;
    float yPos = (window.getSize().y - startImageSprite.getLocalBounds().height) / 2;
    startImageSprite.setPosition(xPos, yPos);

    sf::Texture playButtonTexture;
    if (!playButtonTexture.loadFromFile("Resources/play.png")) {
        std::cout << "Failed to load play button image." << std::endl;
        return -1;
    }
    sf::Sprite playButtonSprite(playButtonTexture);
    playButtonSprite.setScale(0.4f, 0.4f);
    playButtonSprite.setPosition(150.f, 350.f);

    sf::Music music;
    if (!music.openFromFile("Resources/music.mp3")) {
        std::cerr << "Failed to load music!" << std::endl;
        return -1;
    }
    music.setLoop(true); // Make the music loop
    music.play(); // Start playing the music.

    // Create a sound buffer and sound to play the tone
    sf::SoundBuffer toneBuffer;
    if (!toneBuffer.loadFromFile("Resources/tone.wav")) {
        std::cerr << "Failed to load tone sound!" << std::endl;
        return -1;
    }
    sf::Sound toneSound;
    toneSound.setBuffer(toneBuffer);

    sf::SoundBuffer touchBuffer;
    if (!touchBuffer.loadFromFile("Resources/tone.mp3")) {
        std::cerr << "Failed to load touch sound!" << std::endl;
        return -1;
    }
    sf::Sound touchSound;
    touchSound.setBuffer(touchBuffer);

    // Sound buffer for the music
    sf::SoundBuffer musicBuffer;
    if (!musicBuffer.loadFromFile("Resources/music.mp3")) {
        std::cerr << "Failed to load music sound buffer!" << std::endl;
        return -1;
    }

    // Create a sound to analyze the music in real-time
    sf::Sound musicAnalyzer;
    musicAnalyzer.setBuffer(musicBuffer);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (gameState == GameState::Menu) {
                        transitionEffect(window, checkerboardTexture, checkerboardSprite);
                        gameState = GameState::Game;

                        // Play the tone and lower the volume of the music
                        toneSound.play();
                        music.setVolume(10000); // Adjust the volume as needed
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (gameState == GameState::Menu) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    if (playButtonSprite.getGlobalBounds().contains(mousePos)) {
                        transitionEffect(window, checkerboardTexture, checkerboardSprite);
                        gameState = GameState::Game;

                        // Play the tone and lower the volume of the music
                        toneSound.play();
                        music.setVolume(20); // Adjust the volume as needed
                        touchSound.setVolume(1000);
                    }
                }
            }
        }

        window.clear();

        if (gameState == GameState::Menu) {
            window.draw(checkerboardSprite);
            window.draw(startImageSprite);
            window.draw(playButtonSprite);
        }

        if (gameState == GameState::Game) {
            dvdSprite.move(dx, dy);
            if (dvdSprite.getPosition().x <= 0 || dvdSprite.getPosition().x >= window.getSize().x - dvdSprite.getGlobalBounds().width) {
                dx = -dx;
                touchSound.play(); // Play tone when touching the border
            }
            if (dvdSprite.getPosition().y <= 0 || dvdSprite.getPosition().y >= window.getSize().y - dvdSprite.getGlobalBounds().height) {
                dy = -dy;
                touchSound.play(); // Play tone when touching the border
            }
            changeHue(dvdSprite, hue);
            hue += 1.0f; // Adjust hue change speed



            window.draw(dvdSprite);
        }

        window.display();
    }

    return 0;
}
