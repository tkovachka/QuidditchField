#ifndef OPENGLPRJ_OBJECT_HPP
#define OPENGLPRJ_OBJECT_HPP


#include<glm/glm.hpp>

#include<assimp/postprocess.h>
#include<assimp/scene.h>
#include<assimp/Importer.hpp>
#include<iostream>
#include<glad/glad.h>
#include<vector>

class Object {
private:
    Assimp::Importer importer;
    const aiScene *scene = nullptr;
    aiNode *root_node = nullptr;

    struct Mesh {
        unsigned int VAO, VBO1, VBO2, VBO3, EBO;

        std::vector<glm::vec3> vert_positions;
        std::vector<glm::vec3> vert_normals;
        std::vector<glm::vec2> tex_coords;
        std::vector<unsigned int> vert_indices;
        unsigned int tex_handle;
    };

    struct Texture {
        unsigned int textureID;
        std::string image_name;
    };

    void load_model();

    void set_buffer_data(unsigned int index);

public:
    unsigned int num_meshes;
    std::vector<Mesh> mesh_list;
    std::vector<Texture> texture_list;

    Object(const char *model_path);

    int is_image_loaded(std::string file_name);

    unsigned int load_texture_image(std::string file_name, bool &load_complete);
};

#endif //OPENGLPRJ_OBJECT_HPP
