#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <deque>
#include <algorithm>
#include <fstream>

// Kid 类
class Kid {
private:
    const int KID_WIDTH = 128;
    const int KID_HEIGHT = 128;

    const int X_POS = 300;
    float posY;
    float velocityY;

    // 跳跃参数
    const float GRAVITY_JUMP_HOLD = 400.f; // 按跳键时的重力
    const float GRAVITY_NATURAL = 3200.f; // 不按跳键时的重力
    const float INITIAL_VELOCITY = -1150.f;
    const float MAX_JUMP_TIME = 0.24f;
    float jumpTimer;
    bool wasJumpPressed; // 记录上一帧是否按跳，使得必须在落地后按跳才能跳起来

public:
    sf::Sprite kidSprite;
    int groundPos;

    enum class KidState {
        RUNNING,
        JUMPING,
        RISING, // 松开跳后仍然上升的阶段
        FALLING
    };
    KidState kidState;

    void reset() {
        posY = groundPos - KID_HEIGHT;
        velocityY = 0.f;
        kidState = KidState::RUNNING;
        wasJumpPressed = false;
        kidSprite.setPosition(X_POS, groundPos - KID_HEIGHT);
    }

    // 加载纹理，适应大小
    void setTexture(const sf::Texture& texture) {
        kidSprite.setTexture(texture);
        float scaleX = (float)KID_WIDTH / texture.getSize().x;
        float scaleY = (float)KID_HEIGHT / texture.getSize().y;
        kidSprite.setScale(scaleX, scaleY);
    }

    // 处理跳跃，每一帧进行
    void move(float dt) {
        bool isJumpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

        switch (kidState) {
            case KidState::RUNNING:
                if (isJumpPressed && !wasJumpPressed) {
                    startJump();
                }
                break;
            case KidState::JUMPING:
                // 跳跃高度由按住跳的时长决定，有上限
                if (isJumpPressed && jumpTimer < MAX_JUMP_TIME) {
                    velocityY += GRAVITY_JUMP_HOLD * dt;
                    posY += velocityY * dt;
                    jumpTimer += dt;
                } else {
                    velocityY += GRAVITY_NATURAL * dt;
                    posY += velocityY * dt;
                    kidState = KidState::RISING;
                }
                break;
            case KidState::RISING:
                velocityY += GRAVITY_NATURAL * dt;
                posY += velocityY * dt;
                if (velocityY >= 0.f) {
                    kidState = KidState::FALLING;
                }
                break;
            case KidState::FALLING:
                velocityY += GRAVITY_NATURAL * dt;
                posY += velocityY * dt;
                // 检测落地
                if (posY >= groundPos - KID_HEIGHT) {
                    posY = groundPos - KID_HEIGHT;
                    velocityY = 0.f;
                    kidState = KidState::RUNNING;
                }
                break;
        }

        wasJumpPressed = isJumpPressed;
        kidSprite.setPosition(X_POS, posY);
    }

private:
    void startJump() {
        velocityY = INITIAL_VELOCITY;
        kidState = KidState::JUMPING;
        jumpTimer = 0.f;
    }
};

// 刺 类
class Spike {
public:
    int spikeWidth;
    int spikeHeight;
    int velocityX;
    bool passed; // 判断Kid是否经过刺，用于加分

    sf::Sprite spikeSprite;

    // 加载纹理并生成刺
    void spawn(const sf::Texture& texture, int initialPosX, int groundPos) {
        spikeSprite.setTexture(texture);
        float scaleX = (float)spikeWidth / texture.getSize().x;
        float scaleY = (float)spikeHeight / texture.getSize().y;
        spikeSprite.setScale(scaleX, scaleY);
        spikeSprite.setPosition(initialPosX, groundPos - spikeHeight);
    }

    // 移动，每一帧进行
    void move(float dt) {
        spikeSprite.move(-velocityX * dt, 0.f);
    }
};

// 雪花 类
class Snow {
private:
    const int LEFT_BORDER = 300;
    const int LOW_BORDER = 600;

public:
    int snowWidth;
    int snowHeight;
    int velocityX;
    int velocityY;
    int angleVelocity;

    sf::Sprite snowSprite;

    // 加载纹理并生成雪花
    void spawn(const sf::Texture& texture, int windowWidth) {
        snowSprite.setTexture(texture);
        float scaleX = (float)snowWidth / texture.getSize().x;
        float scaleY = (float)snowHeight / texture.getSize().y;
        snowSprite.setScale(scaleX, scaleY);

        int posX, posY;
        int choice = rand() % 10;
        if (choice < 7) { // 雪花生成在上方
            posY = -snowHeight;
            posX = LEFT_BORDER + rand() % (windowWidth - LEFT_BORDER);
        } else { // 雪花生成在右方
            posX = windowWidth;
            posY = rand() % LOW_BORDER - snowHeight;
        }

        snowSprite.setPosition(posX, posY);
        snowSprite.setOrigin(snowWidth / 2.f, snowHeight / 2.f);
    }

    // 平移与旋转
    void move(float dt) {
        snowSprite.move(-velocityX * dt, velocityY * dt);
        snowSprite.rotate(angleVelocity * dt);
    }
};

// 游戏主控 类
class Game {
private:
    const int WINDOW_WIDTH = 1920;
    const int WINDOW_HEIGHT = 1080;
    sf::RenderWindow window;
    const int FRAME_RATE = 50; // I Wanna通常是50帧
    const int GROUND_POS = 913; // 地面高度，根据背景图片得出

    // 贴图材质，包括Kid的一系列材质
    std::vector<sf::Texture> runTextures;
    std::vector<sf::Texture> riseTextures;
    std::vector<sf::Texture> fallTextures;
    Kid kid;
    sf::Sprite background; // 上方背景
    sf::Texture backgroundTexture;
    sf::Sprite land; // 下方地面
    sf::Texture landTexture;
    sf::Texture spikeTexture;
    std::deque<Spike> spikes; // 其实queue够用了
    sf::Texture snowTexture;
    std::deque<Snow> snowflakes;

    enum class GameState {
        MENU,
        PLAYING,
        GAME_OVER,
        PAUSED
    };
    GameState state;
    // 用于计时的系统
    sf::Clock clock;
    float kidTimer;
    float spikeTimer;
    float snowTimer;
    float instTimer;
    const float KID_UPDATE_DELAY = 0.1f; // Kid贴图更新间隔
    float spikeDelay; // 刺生成间隔
    float snowDelay; // 雪花生成间隔
    int score;
    int highScore;
    bool instShow; // 指示文字，只会出现一次

    // 用于更新Kid的贴图
    int runFrame;
    int riseFrame;
    int fallFrame;

    // 刺的随机相关参数，给定范围与下限
    const float INITIAL_SPIKE_DELAY = 1.5f;
    const int SPIKE_DELAY_RANGE = 600;
    const float MIN_SPIKE_DELAY = 0.7f;
    const int MIN_SPIKE_WIDTH = 40;
    const int SPIKE_WIDTH_RANGE = 200;
    const int MIN_SPIKE_HEIGHT = 50;
    const int SPIKE_HEIGHT_RANGE = 250;
    const int MIN_SPIKE_SPEED = 400;
    const int SPIKE_SPEED_RANGE = 150;
    const int MAX_SPIKE_SPEED = 1200;

    // 雪花的随机相关参数，给定范围与下限
    const float MIN_SNOW_DELAY = 0.1f;
    const int SNOW_DELAY_RANGE = 300;
    const int MIN_SNOW_SIZE = 20;
    const int SNOW_SIZE_RANGE = 60;
    const int MIN_SNOW_XSPEED = 300;
    const int SNOW_XSPEED_RANGE = 900;
    const int MIN_SNOW_YSPEED = 200;
    const int SNOW_YSPEED_RANGE = 600;
    const int MIN_SNOW_ANGLE_SPEED = -120;
    const int SNOW_ANGLE_SPEED_RANGE = 240;

    sf::Font font;
    sf::Text scoreText;
    sf::Text titleText;
    sf::Text subText;

    sf::Music bgmMenu;
    sf::Music bgmGaming;
    float currentVolume; // 游戏bgm而非标题bgm的音量
    const float MAX_VOLUME = 50.f;
    const float TARGET_PAUSED_VOLUME = 15.f;
    const float VOLUME_CHANGE_SPEED = 20.f;

public:
    Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "I Wanna Celeste"), instShow(true), state(GameState::MENU) {
        window.setFramerateLimit(FRAME_RATE);
        srand(static_cast<unsigned int>(time(nullptr)));

        // 加载全部所需资源
        if (!font.loadFromFile("../resources/arial.ttf")) {
            exit(EXIT_FAILURE);
        }

        if (!bgmMenu.openFromFile("../resources/Prologue.ogg") || !bgmGaming.openFromFile("../resources/First Steps.ogg")) {
            exit(EXIT_FAILURE);
        } else {
            bgmMenu.setLoop(true);
            bgmGaming.setLoop(true);
            bgmMenu.play();
            bgmMenu.setVolume(MAX_VOLUME);
        }
        
        if (!backgroundTexture.loadFromFile("../resources/background.png") || !landTexture.loadFromFile("../resources/land.png")) {
            exit(EXIT_FAILURE);
        } else {
            background.setTexture(backgroundTexture);
            land.setTexture(landTexture);
        }

        sf::Texture tmpTexture;

        // Kid的贴图加载，并放入对应的vector
        for (int i = 0; i < 4; ++i) {
            if (!tmpTexture.loadFromFile("../resources/run" + std::to_string(i) + ".png")) {
                exit(EXIT_FAILURE);
            } else {
                runTextures.push_back(tmpTexture);
            }
        }

        for (int i = 0; i < 2; ++i) {
            if (!tmpTexture.loadFromFile("../resources/jump" + std::to_string(i) + ".png")) {
                exit(EXIT_FAILURE);
            } else {
                riseTextures.push_back(tmpTexture);
            }
        }

        for (int i = 0; i < 2; ++i) {
            if (!tmpTexture.loadFromFile("../resources/fall" + std::to_string(i) + ".png")) {
                exit(EXIT_FAILURE);
            } else {
                fallTextures.push_back(tmpTexture);
            }
        }

        if(!spikeTexture.loadFromFile("../resources/spike.png") || !snowTexture.loadFromFile("../resources/snowflake.png")) {
            exit(EXIT_FAILURE);
        }

        kid.setTexture(runTextures[0]);
        kid.groundPos = GROUND_POS;

        // 文字初始化
        scoreText.setFont(font);
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(20, 20);

        titleText.setFont(font);
        titleText.setCharacterSize(200);

        subText.setFont(font);
        subText.setCharacterSize(60);

        // 尝试读取最高分
        std::ifstream inputFile("highscore.txt");
        if (inputFile.is_open()) {
            inputFile >> highScore;
            inputFile.close();
        } else {
            highScore = 0; // 如果没有文件，默认为0
        }
    }

    void run() {
        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }

private:
    // 处理游戏状态变化
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // 主菜单，按Enter开始游戏
            if (state == GameState::MENU) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    resetGame();
                    bgmMenu.stop();
                    bgmGaming.play();
                    bgmGaming.setVolume(MAX_VOLUME);
                    currentVolume = MAX_VOLUME;
                }
            }

            // 死亡后按R重开
            if (state == GameState::GAME_OVER) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
            }

            // 暂停功能
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                if (state == GameState::PLAYING) {
                    state = GameState::PAUSED;
                }
                else if (state == GameState::PAUSED) {
                    state = GameState::PLAYING;
                }
            }

            // Esc回到主菜单，或退出游戏（正常游戏时无效，防误触）
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (state == GameState::PAUSED || state == GameState::GAME_OVER) {
                    state = GameState::MENU;
                    bgmGaming.stop();
                    bgmMenu.play();
                    bgmMenu.setVolume(MAX_VOLUME);
                }
                else if (state == GameState::MENU) {
                    window.close();
                }
            }
        }
    }

    // 各种游戏内部事件处理
    void update() {
        // 一帧的时间
        float dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        // 对于游戏bgm，死亡后bgm渐弱，重开后bgm渐强，暂停后bgm变化至较低音量
        if (state == GameState::PLAYING) {
            if (currentVolume < MAX_VOLUME) {
                currentVolume += VOLUME_CHANGE_SPEED * dt;
                if (currentVolume > MAX_VOLUME) currentVolume = MAX_VOLUME; // 音量上限
                bgmGaming.setVolume(currentVolume);
            }
        }
        else if (state == GameState::GAME_OVER) {
            if (currentVolume > 0.f) {
                currentVolume -= VOLUME_CHANGE_SPEED * dt;
                if (currentVolume < 0.f) currentVolume = 0.f; // 死亡音量下限
                bgmGaming.setVolume(currentVolume);
            }
        }
        // 暂停后音量变化至目标音量
        else if (state == GameState::PAUSED) {
            if (currentVolume > TARGET_PAUSED_VOLUME) {
                currentVolume -= VOLUME_CHANGE_SPEED * dt;
                if (currentVolume < TARGET_PAUSED_VOLUME) currentVolume = TARGET_PAUSED_VOLUME;
                bgmGaming.setVolume(currentVolume);
            }
            else if (currentVolume < TARGET_PAUSED_VOLUME) {
                currentVolume += VOLUME_CHANGE_SPEED * dt;
                if (currentVolume > TARGET_PAUSED_VOLUME) currentVolume = TARGET_PAUSED_VOLUME;
                bgmGaming.setVolume(currentVolume);
            }
        }

        // 以下逻辑仅在游戏进行时更新
        if (state != GameState::PLAYING) return;

        // 跳跃与暂停提示只持续固定时长
        if (instShow) {
            instTimer += dt;
            if (instTimer > 5.f) instShow = false;
        }

        // 更新Kid贴图
        kidTimer += dt;
        if (kidTimer >= KID_UPDATE_DELAY) {
            kidTimer -= KID_UPDATE_DELAY;

            // 使用贴图指针，避免复制贴图
            sf::Texture* currentTexture = nullptr;

            switch (kid.kidState) {
                case Kid::KidState::RUNNING:
                    currentTexture = &runTextures[runFrame];
                    runFrame = (runFrame + 1) % runTextures.size(); // 地面贴图循环
                    break;
                case Kid::KidState::JUMPING:
                case Kid::KidState::RISING:
                    currentTexture = &riseTextures[riseFrame];
                    riseFrame = (riseFrame + 1) % riseTextures.size(); // 上升贴图循环
                    break;
                case Kid::KidState::FALLING:
                    currentTexture = &fallTextures[fallFrame];
                    fallFrame = (fallFrame + 1) % fallTextures.size(); // 下降贴图循环
                    break;
            }
            
            if (currentTexture) {
                kid.setTexture(*currentTexture);
            }
        }

        kid.move(dt);

        // 刺的生成，参数随机
        spikeTimer += dt;
        // 生成间隔也是随机的
        if (spikeTimer >= spikeDelay) {
            spikeTimer -= spikeDelay;
            // 动态难度，分数越高，刺频率越高，但有上限
            spikeDelay = std::max(INITIAL_SPIKE_DELAY - score / 1000.f + (rand() % SPIKE_DELAY_RANGE) / 1000.f, MIN_SPIKE_DELAY);

            Spike newSpike;
            newSpike.spikeWidth = MIN_SPIKE_WIDTH + rand() % SPIKE_WIDTH_RANGE;
            newSpike.spikeHeight = MIN_SPIKE_HEIGHT + rand() % SPIKE_HEIGHT_RANGE;
            // 动态难度，分数越高，刺越快，但有上限
            newSpike.velocityX = std::min(MIN_SPIKE_SPEED + score / 2 + rand() % SPIKE_SPEED_RANGE, MAX_SPIKE_SPEED);
            newSpike.passed = false;
            newSpike.spawn(spikeTexture, WINDOW_WIDTH, GROUND_POS);

            // 新的刺入队
            spikes.push_back(newSpike);
        }

        // 离开屏幕的刺出队
        if (!spikes.empty()) {
            if (spikes[0].spikeSprite.getPosition().x < -spikes[0].spikeWidth) {
                spikes.pop_front();
            }
        }

        // 全体刺移动
        for (auto& spike : spikes) {
            spike.move(dt);

            // 判断Kid是否通过刺，通过则加分
            float spikeRight = spike.spikeSprite.getPosition().x + spike.spikeWidth;
            float kidLeft = kid.kidSprite.getPosition().x;

            if (spikeRight < kidLeft && !spike.passed) {
                spike.passed = true;
                score += 10;
                // 若破纪录，实时更新最高分
                if (score > highScore) {
                    highScore = score;
                }
            }
        }

        sf::FloatRect kidBounds = kid.kidSprite.getGlobalBounds();
        // Kid的碰撞箱参考I Wanna，较小
        kidBounds.width *= 0.35f;
        kidBounds.height *= 0.66f;
        kidBounds.left += kid.kidSprite.getGlobalBounds().width * 0.34f;
        kidBounds.top += kid.kidSprite.getGlobalBounds().height * 0.34f;

        // 刺的实际碰撞箱应该是三角形，这里近似实现
        for (auto& spike : spikes) {
            // 粗略检测：只检测矩形边界
            if (!kidBounds.intersects(spike.spikeSprite.getGlobalBounds())) {
                continue;
            }

            // 精细检测：三角形碰撞
            // 获取刺的位置和尺寸
            sf::Vector2f spikePos = spike.spikeSprite.getPosition();
            float spikeW = (float)spike.spikeWidth;
            float spikeH = (float)spike.spikeHeight;

            // 定义刺的三个顶点（三角形）          
            sf::Vector2f top(spikePos.x + spikeW / 2.f, spikePos.y);
            sf::Vector2f botLeft(spikePos.x, spikePos.y + spikeH);
            sf::Vector2f botRight(spikePos.x + spikeW, spikePos.y + spikeH);

            // 定义主角身上需要检测的关键点
            // 检测主角碰撞箱的：左下角、右下角、底边中点(不够严谨，但基本够用)
            std::vector<sf::Vector2f> kidPoints;
            kidPoints.push_back(sf::Vector2f(kidBounds.left, kidBounds.top + kidBounds.height)); // 左下
            kidPoints.push_back(sf::Vector2f(kidBounds.left + kidBounds.width, kidBounds.top + kidBounds.height)); // 右下
            kidPoints.push_back(sf::Vector2f(kidBounds.left + kidBounds.width / 2.f, kidBounds.top + kidBounds.height)); // 底中

            // 检查这些点是否在刺的三角形内
            bool isDead = false;
            for (const auto& point : kidPoints) {
                if (isPointInTriangle(point, top, botLeft, botRight)) {
                    isDead = true;
                    break;
                }
            }

            if (isDead) {
                state = GameState::GAME_OVER;
                // 死亡时保存最高分到文件
                std::ofstream outputFile("highscore.txt");
                if (outputFile.is_open()) {
                    outputFile << highScore;
                    outputFile.close();
                }
            }
        }

        // 雪花的生成，参数随机
        snowTimer += dt;
        if (snowTimer >= snowDelay) {
            snowTimer -= snowDelay;
            snowDelay = MIN_SNOW_DELAY + (rand() % SNOW_DELAY_RANGE) / 1000.f;

            Snow newSnowflake;
            newSnowflake.snowWidth = MIN_SNOW_SIZE + rand() % SNOW_SIZE_RANGE;
            newSnowflake.snowHeight = newSnowflake.snowWidth;
            newSnowflake.velocityX = MIN_SNOW_XSPEED + rand() % SNOW_XSPEED_RANGE;
            newSnowflake.velocityY = MIN_SNOW_YSPEED + rand() % SNOW_YSPEED_RANGE;
            newSnowflake.angleVelocity = MIN_SNOW_ANGLE_SPEED + rand() % SNOW_ANGLE_SPEED_RANGE;
            newSnowflake.spawn(snowTexture, WINDOW_WIDTH);

            // 新的雪花入队
            snowflakes.push_back(newSnowflake);
        }

        // 离开屏幕的雪花出队
        if (!snowflakes.empty()) {
            if (snowflakes[0].snowSprite.getPosition().x < -snowflakes[0].snowWidth / 2.f || snowflakes[0].snowSprite.getPosition().y > GROUND_POS + snowflakes[0].snowHeight / 2.f) {
                snowflakes.pop_front();
            }
        }

        // 全体雪花平移和旋转
        for (auto& snow : snowflakes) {
            snow.move(dt);
        }
    }

    // 绘制窗口
    void render() {
        window.clear(); // 清除上一帧

        // 先绘制的在下层，所以先绘制背景
        switch (state) {
            case GameState::MENU:
                window.draw(background);
                window.draw(land);

                drawTitle("I Wanna Celeste", sf::Color(0, 192, 255));
                drawSubtext("High Score: " + std::to_string(highScore) + "\nPress ENTER to Start\nPress ESC to Exit", sf::Color::White);
                break;
            case GameState::PLAYING:
                window.draw(background);
                drawSnow();
                window.draw(land);
                window.draw(kid.kidSprite);
                drawSpikes();

                scoreText.setString("High Score: " + std::to_string(highScore) + "\nScore: " + std::to_string(score));
                window.draw(scoreText);
                if (instShow) drawSubtext("Press SPACE to Jump\nPress P to Pause", sf::Color::White);
                break;
            case GameState::GAME_OVER:
                window.draw(background);
                window.draw(land);

                drawTitle("Game Over", sf::Color(128, 0, 0));
                drawSubtext("Your Score: " + std::to_string(score) + "\nHigh Score: " + std::to_string(highScore) + "\nPress R to Restart\nPress ESC to Menu", sf::Color::White);
                break;
            case GameState::PAUSED:
                window.draw(background);
                drawSnow();
                window.draw(land);
                window.draw(kid.kidSprite);
                drawSpikes();
                // 绘制半透明黑色遮罩，增加视觉层次
                sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                overlay.setFillColor(sf::Color(0, 0, 0, 150));
                window.draw(overlay);

                drawTitle("Paused", sf::Color(0, 192, 255));
                drawSubtext("Press P to Resume\nPress ESC to Menu", sf::Color::White);
                break;
        }

        window.display(); // 显示当前帧
    }

    // 开始新游戏，参数重置
    void resetGame() {
        kid.reset();
        score = 0;
        state = GameState::PLAYING;
        kidTimer = 0.f;
        spikeTimer = 0.f;
        snowTimer = 0.f;
        instTimer = 0.f;
        clock.restart();
        spikeDelay = INITIAL_SPIKE_DELAY + (rand() % SPIKE_DELAY_RANGE) / 1000.f;
        spikes.clear();
        snowDelay = MIN_SNOW_DELAY + (rand() % SNOW_DELAY_RANGE) / 1000.f;
        snowflakes.clear();
        runFrame = 0;
        riseFrame = 0;
        fallFrame = 0;
    }

    // 绘制标题文字
    void drawTitle(std::string title, sf::Color color) {
        titleText.setString(title);
        titleText.setFillColor(color);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
        titleText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 3.6f);

        window.draw(titleText);
    }

    // 绘制辅助文字
    void drawSubtext(std::string subtext, sf::Color color) {
        subText.setString(subtext);
        subText.setFillColor(color);
        sf::FloatRect subTextBounds = subText.getLocalBounds();
        subText.setOrigin(subTextBounds.width / 2.f, subTextBounds.height / 2.f);
        subText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 1.8f);

        window.draw(subText);
    }

    // 绘制全体刺
    void drawSpikes() {
        for (auto& spike : spikes) {
            window.draw(spike.spikeSprite);
        }
    }

    // 计算叉乘符号，用于判断点与线的相对位置
    float sign(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    // 判断点 pt 是否在三角形 (v1, v2, v3) 内部
    bool isPointInTriangle(sf::Vector2f pt, sf::Vector2f v1, sf::Vector2f v2, sf::Vector2f v3) {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    // 绘制全体雪花
    void drawSnow() {
        for (auto& snow : snowflakes) {
            window.draw(snow.snowSprite);
        }
    }
};

// 主程序
int main() {
    Game game;
    game.run();
    return 0;
}