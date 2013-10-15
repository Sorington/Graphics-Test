#include "tools.h"

#define RESOLUTION_X 920
#define RESOLUTION_Y 690

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

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> texCoords;
    loadOBJ("resources/models/casa.obj", vertices, texCoords, normals);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    GLuint shader = LoadShaders("shaders/vert.glsl", "shaders/frag.glsl");

    sf::Texture crateTexture;
    crateTexture.loadFromFile("resources/textures/crate.jpg");

    sf::Texture houseTexture;
    houseTexture.loadFromFile("resources/textures/casa.png");

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

        cout << xpos << " " << ypos << endl;

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

        glm::mat4 MVP = Proj*View*Model;

        glUseProgram(shader);

        GLuint matMVPID = glGetUniformLocation(shader, "MVP");
        GLuint matMID = glGetUniformLocation(shader, "M");
        GLuint matVID = glGetUniformLocation(shader, "V");

        glUniformMatrix4fv(matMVPID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(matMID, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(matVID, 1, GL_FALSE, &View[0][0]);

        GLuint textureID = glGetUniformLocation(shader, "textureSampler");
        glActiveTexture(GL_TEXTURE0);
        sf::Texture::bind(&houseTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glm::vec3 dirLight(3.0, -1.0, -1.0);
        glm::vec3 ptLight(3.5, 2.0, 4);

        glUniform1i(textureID, 0);

        GLuint dirLightID = glGetUniformLocation(shader, "dirLight_in");
        glUniform3fv(dirLightID, 1, (float*) &dirLight);

        GLuint ptLightID = glGetUniformLocation(shader, "ptLightPos");
        glUniform3fv(ptLightID, 1, (float*) &ptLight);

        GLuint eyePosID = glGetUniformLocation(shader, "eyePos");
        glUniform3fv(eyePosID, 1, (float*) &eyePos);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
           1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
           2,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size()*3);


        glDisableVertexAttribArray(2);

        glDisableVertexAttribArray(1);

        glDisableVertexAttribArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        window->display();
    }

    return 0;
}

