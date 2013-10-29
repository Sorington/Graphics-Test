#include "tools.h"

#define RESOLUTION_X 1024
#define RESOLUTION_Y 768

GLuint normalbuffer;
GLuint uvbuffer;
GLuint vertexbuffer;
GLuint tangentbuffer;
GLuint bitangentbuffer;

glm::vec3 dirLight;
glm::vec3 ptLight;

int main()
{
    sf::Window *window = new sf::Window(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y, 32), "Graphics Test", sf::Style::Default, sf::ContextSettings(32, 24, 4, 4, 2));

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

    glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    // SETTINGS

    window->setVerticalSyncEnabled(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);


    // GENERATE MODEL BUFFERS

    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &uvbuffer);
    glGenBuffers(1, &normalbuffer);
    glGenBuffers(1, &tangentbuffer);
    glGenBuffers(1, &bitangentbuffer);


    //INIT FRAMEBUFFERS

    // frambuffer for the screen quad
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint renderTexture;
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RESOLUTION_X, RESOLUTION_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RESOLUTION_X, RESOLUTION_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    // depth fbo

    GLuint depthFramebuffer = 0;
    glGenFramebuffers(1, &depthFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;


    // SCREEN QUAD VAO

    GLuint quad_vaoID;
    glGenVertexArrays(1, &quad_vaoID);
    glBindVertexArray(quad_vaoID);

    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);


    // LOADING THE MODELS
    Model house, cylinder, room, room_thickwalls;

    room = loadModel("resources/models/room.obj", DEFAULT_DIFFUSE, DEFAULT_SPECULAR, "resources/normalMaps/casaNormal.jpg");
    room_thickwalls = loadModel("resources/models/room_thickwalls.obj", DEFAULT_DIFFUSE, DEFAULT_SPECULAR, "resources/normalMaps/casaNormal.jpg");
    house = loadModel("resources/models/casa.obj", "resources/textures/casa.png", DEFAULT_SPECULAR, "resources/normalMaps/casaNormal.jpg");
    cylinder = loadModel("resources/models/cylinder.obj", "resources/textures/cylinderDiffuse.png", "resources/textures/cylinderSpecular.png", "resources/normalMaps/cylinderNormal.tga");


    // LOADING THE SHADERS
    GLuint shader = loadShaders("shaders/vert.glsl", "shaders/frag.glsl");
    GLuint normalShader = loadShaders("shaders/vertNormal.glsl", "shaders/fragNormal.glsl");
    GLuint quad_shader = loadShaders("shaders/vertFBO.glsl", "shaders/fragFBO.glsl");
    GLuint lightShader = loadShaders("shaders/lightVert.glsl", "shaders/lightFrag.glsl");


    // CLOCK

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
        float t = c.getElapsedTime().asSeconds();

//        cout << dt << endl;

        int xpos, ypos;
        xpos = sf::Mouse::getPosition(*window).x;
        ypos = sf::Mouse::getPosition(*window).y;

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

        dirLight = glm::vec3(sin(t/2), 0.5, cos(t/2));
        ptLight = glm::vec3(4.0, 3.0, 7);


        glBindVertexArray(vao);

        // COMPUTE THE SHADOW MAP FOR THE DIRECTIONAL LIGHT
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
        glm::mat4 depthViewMatrix = glm::lookAt(dirLight, glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        glm::mat4 depthMVP = depthProjectionMatrix*depthViewMatrix*depthModelMatrix*glm::mat4(1.0);
        glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

        glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
        glViewport(0, 0, 1024, 1024);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(lightShader);
        GLuint depthMVPID = glGetUniformLocation(lightShader, "depthMVP");
        glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);

        setBuffers(house);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, house.vertices.size()*3);

        glDisableVertexAttribArray(0);


        // COMPUTE THE GEOMETRY
        glm::mat4 projMat = glm::perspective(initialFOV, 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 viewMat = glm::lookAt(eyePos, eyePos+eyeDirection, upVector);
        glm::mat4 modelMat = glm::mat4(1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, RESOLUTION_X, RESOLUTION_Y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setBuffers(house);

        drawModel(modelMat, viewMat, projMat, depthBiasMVP, shader, depthTexture, eyePos, house, false);

        modelMat = glm::translate(20.0f, 0.0f, 0.0f)*glm::mat4(1.0f);
  //      drawModel(modelMat, viewMat, projMat, depthBiasMVP, shader, depthTexture, eyePos, house, false);

        setBuffers(cylinder);

        modelMat = glm::translate(4.0f, 1.0f, 5.0f)*glm::mat4(1.0f);
  //      drawModel(modelMat, viewMat, projMat, depthBiasMVP, normalShader, depthTexture, eyePos, cylinder, true);



        // DRAW THE SCREEN QUAD FOR RENDERING THE FBO

        glBindVertexArray(quad_vaoID);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, RESOLUTION_X, RESOLUTION_Y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(quad_shader);

        GLuint renderTexID = glGetUniformLocation(quad_shader, "renderTexture");

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, renderTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glUniform1i(renderTexID, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(0);

        window->display();
    }

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);

    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &depthFramebuffer);

    glDeleteProgram(shader);
    glDeleteProgram(normalShader);
    glDeleteProgram(quad_shader);
    glDeleteProgram(lightShader);

    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &quad_vaoID);

    return 0;
}

