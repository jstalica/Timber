#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// Functioin declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];


int main()
{
    // Create a video mode object
    VideoMode vm(1920, 1080);

    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", Style::Default);

    // Create a texture to hold a graphic on the GPU
    Texture textureBackground;

    // Load graphic into the texture
    textureBackground.loadFromFile("graphics/background.png");

    // Create a sprite
    Sprite spriteBackground;

    // Attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);

    // Attach the texture to the sprite
    spriteBackground.setPosition(0, 0);

    // Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    Vector2f treePositionInit = { (window.getSize().x  - textureTree.getSize().x) / 2.0f, 0.0f };
    spriteTree.setPosition(treePositionInit);

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    Vector2f beePositionInit = { (window.getSize().x - textureBee.getSize().x) / 2.0f, 0.0f };
    spriteBee.setPosition(beePositionInit);
    // Is the bee currently moving?
    bool beeActive = false;
    // How fast can the bee fly?
    float beeSpeed = 0.0f;

    // make 3 cloud sprites from 1 texture
    Texture textureCloud;
    textureCloud.loadFromFile("graphics/cloud.png");
    // 3 new sprites with the same texture
    Sprite spriteClouds[3];
    spriteClouds[0].setTexture(textureCloud);
    spriteClouds[1].setTexture(textureCloud);
    spriteClouds[2].setTexture(textureCloud);
    // Position the clouds off the screen
    spriteClouds[0].setPosition(0, 0);
    spriteClouds[1].setPosition(0, 250);
    spriteClouds[2].setPosition(0, 500);
    // Are the clouds currently on screen?
    bool cloudsActive[3] = { false,false,false };
    // How fast is each cloud?
    float cloundsSpeed[3] = { 0.f, 0.f, 0.f };

    // Draw some text
    int score = 0;
    sf::Text messageText;
    sf::Text scoreText;

    //We need a font
    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    // set the font to our message
    messageText.setFont(font);
    scoreText.setFont(font);

    // Assign the actual message
    messageText.setString("Press Enter to Start!");
    scoreText.setString("Score = 0");

    // Make it really big
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position the text
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.f);
    messageText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    scoreText.setPosition(20, 20);

    // Variables to control time itself
    Clock clock;

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition(window.getSize().x / 2.f - timeBarStartWidth / 2, window.getSize().y - 100);

    Time gameTimeTotal;
    float timeRemaining = 6.f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Prepare our branches
    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");
    // Set the textures of each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // Set the sprite's origin to dead center so we can spin without changing its position
        branches[i].setOrigin(textureBranch.getSize().x / 2.f, textureBranch.getSize().y / 2.f);
    }

    for (int i = 1; i < 6; i++)
    {
        updateBranches(i);
    }

    // Track whether the game is running
    bool paused = true;

    // FPS
    bool isDebug = false;
    
    Text text("0.0", font);
    text.setCharacterSize(30);
    text.setFillColor(Color::Green);
    Clock fpsClock;


    while (window.isOpen())
    {
        /*
        ********************************************
        Handle player input
        ********************************************
        */

        // Exit the game
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        // Start the game
        if (Keyboard::isKeyPressed(Keyboard::Return))
        {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 6.f;
        }

        if (Keyboard::isKeyPressed(Keyboard::F1))
        {
            isDebug = !isDebug;
        }


        /*
        ********************************************
        Update the scene
        ********************************************
        */

        if (!paused)
        {
            // Meaure time
            Time dt = clock.restart();

            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // size up the time bar
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f)
            {
                // Pause the game
                paused = true;

                // Change the message shown to the player
                messageText.setString("Out of time!");

                // Reposition the text based on the new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.f);
                messageText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

            }

            // Calculate FPS
            Time dtFPS = fpsClock.getElapsedTime();
            if (dtFPS.asSeconds() > 0.1f)
            {
                text.setString(std::to_string((int)(1.f / dt.asSeconds())));
                fpsClock.restart();
            }


            // Setup the bee
            if (!beeActive)
            {
                // How fast is the bee
                srand((int)time(0) * 10);
                beeSpeed = (rand() % 200) + 200;

                // How high is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;
            }
            else
                // Move the bee
            {
                spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

                // Has the bee reached the right hand edge of the screen?
                if (spriteBee.getPosition().x < -100)
                {
                    // Set it up to be a while new bee next frame
                    beeActive = false;
                }
            }

            // Manage the clouds
            for (int cloudIndex = 0; cloudIndex < 3; cloudIndex++)
            {


                if (!cloudsActive[cloudIndex])
                {
                    // How fast is the cloud
                    srand((int)time(0) * (10 + cloudIndex));
                    cloundsSpeed[cloudIndex] = (rand() % 200) + 200;


                    // How high is the bee
                    srand((int)time(0) * (10 + cloudIndex));
                    float height = (rand() % 150);
                    spriteClouds[cloudIndex].setPosition(-200, height);
                    cloudsActive[cloudIndex] = true;
                }
                else
                    // Move clouds
                {
                    spriteClouds[cloudIndex].setPosition(spriteClouds[cloudIndex].getPosition().x + (cloundsSpeed[cloudIndex] * dt.asSeconds()), spriteClouds[cloudIndex].getPosition().y);
                    //std::cout << "Cloud: " << cloudIndex << " speed: " << cloundsSpeed[cloudIndex] << " pos: " << spriteClouds[cloudIndex].getPosition().x << '\n';

                    // Has the cloud reached the right hand edge of the screen
                    if (spriteClouds[cloudIndex].getPosition().x > 1920)
                    {
                        cloudsActive[cloudIndex] = false;
                    }
                }
            }

            // Update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;
                if (branchPositions[i] == side::LEFT)
                {
                    // Move the sprite to the left side
                    branches[i].setPosition(610, height);
                    // Flip the sprite vertically
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i] == side::RIGHT)
                {
                    // Mvoe the sprite to the right side
                    branches[i].setPosition(1330, height);
                    // Reset rotation
                    branches[i].setRotation(0);
                }
                else
                {
                    // Hide the branch
                    branches[i].setPosition(3000, height);
                }
            }
        }

        /*
        ********************************************
        Draw the scene
        ********************************************
        */

        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw clouds
        for (int i = 0; i < 3; i++)
            window.draw(spriteClouds[i]);

        // Draw branches
        for (int i = 0; i < NUM_BRANCHES; i++)
            window.draw(branches[i]);


        // Draw the tree
        window.draw(spriteTree);

        // Draw the bee
        window.draw(spriteBee);

        // Draw the score
        window.draw(scoreText);

        // Draw the timebar
        window.draw(timeBar);

        if (paused)
        {
            // Draw our message
            window.draw(messageText);
        }

        // Debug
        if (isDebug)
        {
            window.draw(text);
        }

        // Show everything we just drew
        window.display();

    }

    return 0;
}

void updateBranches(int seed)
{
    // Move all the branches down one place
    for (int j = sizeof(&branches) - 1; j > 0; j--)
    {
        branchPositions[j] = branchPositions[j - 1];
    }

    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    srand((int)time(0) + seed);
    int r = (rand() % 5);
    switch (r)
    {
    case 0:
        branchPositions[0] = side::LEFT;
        break;
    case 1:
        branchPositions[0] = side::RIGHT;
        break;
    default:
        branchPositions[0] = side::NONE;
        break;
    }
}

