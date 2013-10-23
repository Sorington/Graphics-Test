#include "tools.h"

#define RESOLUTION_X 920
#define RESOLUTION_Y 690

GLuint normalbuffer;
GLuint uvbuffer;
GLuint vertexbuffer;
GLuint tangentbuffer;
GLuint bitangentbuffer;

int main()
{
    sf::Window *window = new sf::Window(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y, 32), "Graphics Test", sf::Style::Default, sf::ContextSettings(32, 8, 4, 4, 2));

    // Initialize GLEW
    glewExperimental= true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glm::vec3 eyePos(7, 4, 15);

    float horizontalAngle = 3.14f;

    float verticalAngle = 0.0f;

    float initialFOV = 45.0f;

    float speed = 3.0f;
    float mouseSpeed = 0.02f;

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    window->setVerticalSyncEnabled(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &uvbuffer);
    glGenBuffers(1, &normalbuffer);
    glGenBuffers(1, &tangentbuffer);
    glGenBuffers(1, &bitangentbuffer);

    Model house, cylinder;

    house = loadModel("resources/models/casa.obj", "resources/textures/casa.png", "resources/normalMaps/casaNormal.jpg");
    cylinder = loadModel("resources/models/cylinder.obj", "resources/textures/cylinderDiffuse.png", "resources/normalMaps/cylinderNormal.tga");

    GLuint shader = loadShaders("shaders/vert.glsl", "shaders/frag.glsl");
    GLuint normalShader = loadShaders("shaders/vertNormal.glsl", "shaders/fragNormal.glsl");

    sf::Clock c;
    c.restart();

    sf::Time deltaTime;
    sf::Time currentTime;
    sf::Time lastTime = c.getElapsedTime();

    while (window->isOpen())
    {
        sf::Event event;
        if (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
                break;
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window->close();
            break;
        }

        //get current elapsed time of frame
        currentTime = c.getElapsedTime();
        deltaTime = currentTime-lastTime;
        lastTime = currentTime;

        float dt = deltaTime.asSeconds();

//        cout << dt << endl;

        int xpos, ypos;
        xpos = sf::Mouse::getPosition().x-3;
        ypos = sf::Mouse::getPosition().y-136;

//        cout << xpos << " " << ypos << " " << RESOLUTION_X/2 << " " << RESOLUTION_Y/2 << endl;

        sf::Vector2i mousePos = sf::Vector2i(RESOLUTION_X/2, RESOLUTION_Y/2);
        sf::Mouse::setPosition(mousePos, *window);

        horizontalAngle += mouseSpeed * dt * float(RESOLUTION_X/2 - xpos);
        verticalAngle   += mouseSpeed * dt * float(RESOLUTION_Y/2 - ypos);

        glm::vec3 eyeDirection(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );

        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f/2.0f),
            0,
            cos(horizontalAngle - 3.14f/2.0f)
        );

        glm::vec3 upVector = glm::cross(right, eyeDirection);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) eyePos += eyeDirection*dt*speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) eyePos -= eyeDirection*dt*speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) eyePos += right*dt*speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) eyePos -= right*dt*speed;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projMat = glm::perspective(initialFOV, 4.0f / 3.0f, 0.1f, 100.0f);

        glm::mat4 viewMat = glm::lookAt(eyePos, eyePos+eyeDirection, upVector);

        glm::mat4 modelMat = glm::mat4(1.0f);

        setBuffers(house);

        drawModel(modelMat, viewMat, projMat, shader, eyePos, house, false);

        modelMat = glm::translate(20.0f, 0.0f, 0.0f)*glm::mat4(1.0f);
        drawModel(modelMat, viewMat, projMat, shader, eyePos, house, false);

        setBuffers(cylinder);

        modelMat = glm::translate(4.0f, 2.0f, 5.0f)*glm::mat4(1.0f);
        drawModel(modelMat, viewMat, projMat, normalShader, eyePos, cylinder, true);

        window->display();
    }

    return 0;
}

