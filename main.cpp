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

    std::vector<glm::vec3> vertices, normals, tangents, bitangents;
    std::vector<glm::vec2> texCoords;
    loadOBJ("resources/models/casa.obj", vertices, texCoords, normals);
    computeTangentBasis(vertices, texCoords, normals, tangents, bitangents);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size()*sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

    GLuint shader = loadShaders("shaders/vert.glsl", "shaders/frag.glsl");
    GLuint normalShader = loadShaders("shaders/vertNormal.glsl", "shaders/fragNormal.glsl");

    sf::Texture crateTexture;
    crateTexture.loadFromFile("resources/textures/crate.jpg");

    sf::Texture houseTexture;
    houseTexture.loadFromFile("resources/textures/casa.png");

    sf::Texture houseNormalMap;
    houseNormalMap.loadFromFile("resources/normalMaps/casaNormal.jpg");

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
        xpos = sf::Mouse::getPosition().x-1;
        ypos = sf::Mouse::getPosition().y-80;

//        cout << xpos << " " << ypos << endl;

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

        glm::mat4 Proj = glm::perspective(initialFOV, 4.0f / 3.0f, 0.1f, 100.0f);

        glm::mat4 View = glm::lookAt(eyePos, eyePos+eyeDirection, upVector);

        glm::mat4 Model = glm::mat4(1.0f);

        drawModel(Model, View, Proj, normalShader, houseTexture, eyePos, vertices.size(), true, houseNormalMap);

        Model = glm::translate(20.0f, 0.0f, 0.0f)*glm::mat4(1.0f);

        drawModel(Model, View, Proj, shader, houseTexture, eyePos, vertices.size(), false, houseNormalMap);

        window->display();
    }

    return 0;
}

