#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include<Object.hpp>

Model::Model(const char *model_path) {
    scene = Model::importer.ReadFile(model_path,
                                     aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
    load_model();
}

void Model::load_model()
{
    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        std::cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
    else
    {
        num_meshes = scene->mNumMeshes;
        mesh_list.resize(num_meshes);

        aiMesh* mesh{};
        int indices_offset = 0; // Not being used yet... i.e. indices_offset += mesh->mNumVertices; is commented further down.

        // (1) Loop through all the model's meshes
        // -----------------------------------------------------
        for (unsigned int i = 0; i < num_meshes; ++i)
        {
            mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html

            // This loop will only run once (i.e. there's only 1 texture per mesh)
            for (unsigned int tex_count = 0; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // Also, only using: aiTextureType_DIFFUSE.
            {
                aiString string;
                material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string); // Acquire the name of the image file to be loaded.

                // (2) Load mesh [i]'s texture if not already loaded
                // ---------------------------------------------------------------
                int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.

                if (already_loaded == -1) // Image not yet loaded so now attempt to load it.
                {
                    bool load_success = false;
                    unsigned int texture_handle = load_texture_image(string.C_Str(), load_success);

                    //fixme load_success function
                    if (load_success) // Although do nothing if the image fails to load.
                    {
                        Texture texture;
                        texture.image_name = string.C_Str();
                        texture.textureID = texture_handle;

                        texture_list.push_back(texture);
                        mesh_list[i].tex_handle = texture_handle;
                    }
                }
                else
                    mesh_list[i].tex_handle = already_loaded; // Assign existing texture handle.
            }
            // (3) Loop through all mesh [i]'s vertices
            // ---------------------------------------------------
            for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
            {
                glm::vec3 position{};
                position.x = mesh->mVertices[i2].x;
                position.y = mesh->mVertices[i2].y;
                position.z = mesh->mVertices[i2].z;
                mesh_list[i].vert_positions.push_back(position);

                if (mesh->HasNormals())
                {
                    glm::vec3 normal{};
                    normal.x = mesh->mNormals[i2].x;
                    normal.y = mesh->mNormals[i2].y;
                    normal.z = mesh->mNormals[i2].z;
                    mesh_list[i].vert_normals.push_back(normal);
                }
                else
                    mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

                if (mesh->HasTextureCoords(0)) // Only slot [0] is in question.
                {
                    glm::vec2 tex_coords{};
                    tex_coords.x = mesh->mTextureCoords[0][i2].x;
                    tex_coords.y = mesh->mTextureCoords[0][i2].y;
                    mesh_list[i].tex_coords.push_back(tex_coords);
                    std::cout << "Texture Coordinates: (" << tex_coords.x << ", " << tex_coords.y << ")\n";
                }
                else
                    mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
            }
            // (4) Loop through all mesh [i]'s Indices
            // --------------------------------------------------
            for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
                for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
                    mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);

            // indices_offset += mesh->mNumVertices; // Disabled for tutorial: Model Loading (Part 1 of 3)

            set_buffer_data(i); // Set up: VAO, VBO and EBO.
        }
    }
}

int Model::is_image_loaded(std::string file_name) {
    for (unsigned int i = 0; i < texture_list.size(); ++i)
        if (file_name.compare(texture_list[i].image_name) == 0)
            return texture_list[i].textureID;
    return -1;
}


unsigned int Model::load_texture_image(std::string file_name, bool &load_complete) {
    // stbi_set_flip_vertically_on_load(1); // Call this function if the image is upside-down.

    std::size_t position = file_name.find_last_of("\\");
    file_name = "../res/textures/" + file_name.substr(position + 1);

    int width, height, num_components;
    unsigned char *image_data = stbi_load(file_name.c_str(), &width, &height, &num_components, 0);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    if (image_data) {
        GLenum format{};

        if (num_components == 1)
            format = GL_RED;
        else if (num_components == 3)
            format = GL_RGB;
        else if (num_components == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT,
                      1); // Recommended by NVIDIA Rep: https://devtalk.nvidia.com/default/topic/875205/opengl/how-does-gl_unpack_alignment-work-/

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
        // ----------------------------------------------------------------------------------------------------------------
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_MIRRORED_REPEAT); // GL_REPEAT... GL_MIRRORED_REPEAT... GL_CLAMP_TO_EDGE... GL_CLAMP_TO_BORDER.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        // float border_colour[] = {0.45, 0.55, 0.95};
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour); // For above when using: GL_CLAMP_TO_BORDER

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST... GL_LINEAR... GL_NEAREST_MIPMAP_NEAREST (See above link for full list)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.

        load_complete = true;
        stbi_image_free(image_data);
        std::cout << "   Image loaded OK: " << file_name << "\n";
    } else {
        load_complete = false;
        stbi_image_free(image_data);
        std::cout << "   Image failed to load: " << file_name << "\n";
    }
    return textureID;
}

void Model::set_buffer_data(unsigned int index)
{
    glGenVertexArrays(1, &mesh_list[index].VAO);
    glGenBuffers(1, &mesh_list[index].VBO1); // Alternative to using 3 separate VBOs, instead use only 1 VBO and set glVertexAttribPointer's offset...
    glGenBuffers(1, &mesh_list[index].VBO2); // like was done in tutorial 3... Orbiting spinning cubes.
    glGenBuffers(1, &mesh_list[index].VBO3);
    glGenBuffers(1, &mesh_list[index].EBO);

    glBindVertexArray(mesh_list[index].VAO);

    // Vertex Positions
    // ---------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Vertex Normals
    // --------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_normals.size(), &mesh_list[index].vert_normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Texture Coordinates
    // ---------------------------
    glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh_list[index].tex_coords.size(), &mesh_list[index].tex_coords[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    // Indices for: glDrawElements()
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); 	// Unbind VAO
}