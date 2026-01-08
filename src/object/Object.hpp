// DESCRIPTION
/*
SETTING UP:
As of right now, an object must be created with a mesh. After, textures can be attached and will
bind automatically on the object render call. Apply starting translations outside of while loop,
they are saved to the object and will automatically calculate the model matrix when calling
getModelM().


TRANSLATIONS:
object translation calls will REPLACE previous translation data. if a translation along the x-axis
is called and then another seperate call on only the y-axis, the x translation will be lost.
This is true for all translation calls currently.


TEXTURES:
To apply textures, a texture object must be made. To link the texture, supply setTexture() with
the texture object, its bind unit, and the uniform name it should be bound to. 


MISC:
Currently materials can store a program name, and one uniform name per texture. These do not do
anything inside of this class.
*/


#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "Mesh.hpp"
#include "Material.hpp"


class Object {
    private:
        struct Renderling {
            std::unique_ptr<Mesh> mesh;
            Material mat;
        };

    public:
        Renderling renderable;

        Object(std::vector<float> verts, std::vector<int> indicies, bool hasNorms, bool hasUVs);
        void render();
        void translate(glm::vec3 vector);
        void scale(glm::vec3 vector);
        void rotate(float angle, glm::vec3 axis);
        GLuint getProgramID();
        ShaderProgram* getProgram();
        glm::mat4 getModelM();
        void setTexture(Texture &tex, int unit, std::string uniformName);
        void setProgram(ShaderProgram &program);
        std::string name;
        glm::vec3 objPosition = glm::vec3(0.0f);
        glm::vec3 objScale = glm::vec3(1.0f);
        glm::quat objOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); 

    private:
        int objectID;
};

#endif
