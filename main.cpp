#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "graphics/Material.h"
#include "graphics/Light.h"
#include "graphics/ShadowMap.h"
#include "postprocess/PostProcess.h"
#include "physics/Collision.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include "STLLoader.h"

struct Furniture
{
    STLLoader stl;
    unsigned int VAO;
    unsigned int VBO;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(
    glm::vec3(
        0.0f,
        -3.0f,
        2.0f
    )
);

float lastX = SCR_WIDTH * 0.5f;
float lastY = SCR_HEIGHT * 0.5f;

bool firstMouse = true;
bool rotatingCamera = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float fov = 70.0f;
bool firstPerson=true;
float cctvRadius=5.0f;
float cctvTheta=45.0f;
float cctvPhi=45.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!rotatingCamera)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;

    if (fov < 30.0f) fov = 30.0f;
    if (fov > 90.0f) fov = 90.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) rotatingCamera = true;
        if (action == GLFW_RELEASE) rotatingCamera = false;
    }
}

bool sofaCollision(glm::vec3 pos,Furniture& sofa,float radius)
{
    glm::vec3 half=glm::vec3(15.0f,25.0f,10.0f)*sofa.scale.x;

    glm::vec3 minB=sofa.position-half;
    glm::vec3 maxB=sofa.position+half;

    if(
        pos.x+radius<minB.x ||
        pos.x-radius>maxB.x ||
        pos.y+radius<minB.y ||
        pos.y-radius>maxB.y ||
        pos.z+radius<minB.z ||
        pos.z-radius>maxB.z
    )
        return false;

    return true;
}

void processInput(GLFWwindow* window,STLLoader& stl,Furniture& sofa)
{
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);

    bool forward=glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS;
    bool backward=glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS;
    bool left=glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS;
    bool right=glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS;

    static bool vPressed=false;
    bool vNow=glfwGetKey(window,GLFW_KEY_V)==GLFW_PRESS;

    if(vNow&&!vPressed)
        firstPerson=!firstPerson;

    vPressed=vNow;

    glm::vec3 nextPos=
        camera.GetNextPosition(
            forward,
            backward,
            left,
            right,
            deltaTime
        );

    bool collided=false;

    for(const Triangle& tri:stl.collisionTriangles)
    {
        Triangle t;

        t.a=tri.a*0.01f;
        t.b=tri.b*0.01f;
        t.c=tri.c*0.01f;

        if(Collision::SphereTriangleCollision(
            nextPos,
            camera.Radius,
            t
        ))
        {
            collided=true;
            break;
        }
    }

    if(!collided &&
       sofaCollision(
            nextPos,
            sofa,
            camera.Radius
       ))
    {
        collided=true;
        std::cout<<"SOFA HIT\n";
    }

    if(!collided)
        camera.SetPosition(nextPos);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "The Deep Sea",
        NULL,
        NULL
    );

    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    std::cout<<"Shader OK\n";

    Shader depthShader(
        "shaders/shadow_depth.vert",
        "shaders/shadow_depth.frag"
    );

    Material material;
    Light light;
    ShadowMap shadowMap;

    PostProcess postProcess(
        SCR_WIDTH,
        SCR_HEIGHT
    );

    Texture wallTexture("assets/textures/wall_texture.jpg");
    Texture sofaTexture("assets/textures/sofa.jpg");
    Texture floorTexture("assets/textures/underfloor.jpg");

    STLLoader stl;
    std::vector<Furniture> furnitures;
    if (!stl.loadBinarySTL("assets/1.stl"))
        return -1;


    Furniture sofa;

    sofa.stl.loadBinarySTL("assets/sofa1.stl");
    if(sofa.stl.vertices.empty())
    {
        std::cout<<"SOFA LOAD FAILED\n";
        return -1;
    }
    glGenVertexArrays(1,&sofa.VAO);
    glGenBuffers(1,&sofa.VBO);

    glBindVertexArray(sofa.VAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        sofa.VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sofa.stl.vertices.size()*sizeof(Vertex),
        sofa.stl.vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,normal)
    );

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex,texCoord)
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    sofa.position=
    glm::vec3(
        0.0f,
        0.0f,
        0.2f
    );
    sofa.rotation=glm::vec3(0.0f,90.0f,0.0f);
    sofa.scale=glm::vec3(0.02f);
    //furnitures.push_back(sofa);


    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        stl.vertices.size() * sizeof(Vertex),
        stl.vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, texCoord)
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    light.position = glm::vec3(0.0f, -8.0f, 10.0f);
    light.ambient=glm::vec3(0.4f);
    light.diffuse=glm::vec3(1.0f);
    light.specular=glm::vec3(1.0f);
    material.ambient=glm::vec3(1.0f);
    material.diffuse=glm::vec3(1.0f);
    material.specular=glm::vec3(0.5f);
    material.shininess=32.0f;

    glm::mat4 lightProjection = glm::ortho(
        -30.0f,
        30.0f,
        -30.0f,
        30.0f,
        1.0f,
        50.0f
    );

    glm::mat4 lightView = glm::lookAt(
        light.position,
        glm::vec3(0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::mat4 lightSpaceMatrix =
        lightProjection * lightView;


    while(!glfwWindowShouldClose(window))
    {
        float currentFrame=(float)glfwGetTime();

        deltaTime=currentFrame-lastFrame;
        lastFrame=currentFrame;

        processInput(window,stl,sofa);

        glm::mat4 model=glm::mat4(1.0f);
        model=glm::scale(model,glm::vec3(0.01f));

        glm::mat4 view;

        if(firstPerson)
        {
            view=camera.GetViewMatrix();
        }
        else
        {
            glm::vec3 target=camera.Position;
            glm::vec3 camPos;

            camPos.x=
            target.x+
            cctvRadius*
            sin(glm::radians(cctvTheta))*
            cos(glm::radians(cctvPhi));

            camPos.y=
            target.y+
            cctvRadius*
            sin(glm::radians(cctvPhi));

            camPos.z=
            target.z+
            cctvRadius*
            cos(glm::radians(cctvTheta))*
            cos(glm::radians(cctvPhi));

            view=
            glm::lookAt(
                camPos,
                target,
                glm::vec3(0,0,1)
            );
        }

        glm::mat4 projection=
        glm::perspective(
            glm::radians(fov),
            (float)SCR_WIDTH/
            (float)SCR_HEIGHT,
            0.01f,
            500.0f
        );

        glm::mat4 sofaModel=
        glm::mat4(1.0f);

        sofaModel=
        glm::translate(
            sofaModel,
            sofa.position
        );

        sofaModel=
        glm::rotate(
            sofaModel,
            glm::radians(
                sofa.rotation.y
            ),
            glm::vec3(0,0,1)
        );

        sofaModel=
        glm::scale(
            sofaModel,
            sofa.scale
        );



        shadowMap.Bind();

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f,4.0f);

        depthShader.use();

        glUniformMatrix4fv(
            glGetUniformLocation(
                depthShader.ID,
                "lightSpaceMatrix"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                lightSpaceMatrix
            )
        );

        // ===== ROOM =====

        glUniformMatrix4fv(
            glGetUniformLocation(
                depthShader.ID,
                "model"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            (GLsizei)
            stl.vertices.size()
        );

        // ===== SOFA =====

        glUniformMatrix4fv(
            glGetUniformLocation(
                depthShader.ID,
                "model"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                sofaModel
            )
        );

        glBindVertexArray(
            sofa.VAO
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            (GLsizei)
            sofa.stl.vertices.size()
        );

        glBindVertexArray(0);

        glDisable(GL_POLYGON_OFFSET_FILL);

        shadowMap.Unbind();

        //=====================
        // MAIN PASS
        //=====================

        postProcess.BeginRender();

        glViewport(
            0,
            0,
            SCR_WIDTH,
            SCR_HEIGHT
        );

        glClearColor(
            0.05f,
            0.1f,
            0.2f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT|
            GL_DEPTH_BUFFER_BIT
        );

        shader.use();

        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.ID,
                "lightSpaceMatrix"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                lightSpaceMatrix
            )
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.ID,
                "view"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.ID,
                "projection"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "viewPos"
            ),
            1,
            glm::value_ptr(
                camera.Position
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "light.position"
            ),
            1,
            glm::value_ptr(
                light.position
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "light.ambient"
            ),
            1,
            glm::value_ptr(
                light.ambient
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "light.diffuse"
            ),
            1,
            glm::value_ptr(
                light.diffuse
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "light.specular"
            ),
            1,
            glm::value_ptr(
                light.specular
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "material.ambient"
            ),
            1,
            glm::value_ptr(
                material.ambient
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "material.diffuse"
            ),
            1,
            glm::value_ptr(
                material.diffuse
            )
        );

        glUniform3fv(
            glGetUniformLocation(
                shader.ID,
                "material.specular"
            ),
            1,
            glm::value_ptr(
                material.specular
            )
        );

        glUniform1f(
            glGetUniformLocation(
                shader.ID,
                "material.shininess"
            ),
            material.shininess
        );

        glActiveTexture(GL_TEXTURE1);

        glBindTexture(
            GL_TEXTURE_2D,
            shadowMap.depthMap
        );

        glUniform1i(
            glGetUniformLocation(
                shader.ID,
                "shadowMap"
            ),
            1
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,wallTexture.ID);
        glUniform1i(glGetUniformLocation(shader.ID,"wallTexture"),0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,floorTexture.ID);
        glUniform1i(glGetUniformLocation(shader.ID,"floorTexture"),2);

        // ROOM

        glUniformMatrix4fv(
        glGetUniformLocation(shader.ID,"model"),
        1,
        GL_FALSE,
        glm::value_ptr(model)
        );

        glBindVertexArray(VAO);

        glDrawArrays(
        GL_TRIANGLES,
        0,
        (GLsizei)stl.vertices.size()
        );

        // SOFA

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,sofaTexture.ID);

        glUniformMatrix4fv(
        glGetUniformLocation(shader.ID,"model"),
        1,
        GL_FALSE,
        glm::value_ptr(sofaModel)
        );

        glBindVertexArray(sofa.VAO);

        glDrawArrays(
        GL_TRIANGLES,
        0,
        (GLsizei)sofa.stl.vertices.size()
        );

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);

        postProcess.EndRender();

        postProcess.Render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&sofa.VAO);
    glDeleteBuffers(1,&sofa.VBO);
    glfwTerminate();
    return 0;
}
