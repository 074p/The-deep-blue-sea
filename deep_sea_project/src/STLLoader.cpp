#include "STLLoader.h"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

bool STLLoader::loadBinarySTL(const std::string& path)
{
    std::ifstream file(path,std::ios::binary);

    if(!file.is_open())
    {
        std::cout<<"Cannot open STL file\n";
        return false;
    }

    char header[80];
    file.read(header,80);

    uint32_t triangleCount;
    file.read(reinterpret_cast<char*>(&triangleCount),4);

    vertices.clear();
    collisionTriangles.clear();

    const float PI=3.1415926f;

    float minX=-100.0f;
    float maxX=100.0f;

    float minY=-100.0f;
    float maxY=100.0f;

    float minZ=0.0f;
    float maxZ=80.0f;

    auto calcUV=[&](glm::vec3 p,glm::vec3 n)
    {
        n=glm::normalize(n);

        // SÀN + TRẦN
        if(abs(n.z)>0.7f)
        {
            float u=
            (p.x-minX)/
            (maxX-minX);

            float v=
            (p.y-minY)/
            (maxY-minY);

            return glm::vec2(
                glm::clamp(u,0.0f,1.0f),
                glm::clamp(v,0.0f,1.0f)
            );
        }

        // TƯỜNG HÌNH TRỤ
        float angle=
        atan2(
            p.y,
            p.x
        );

        float u=
        1.0f-
        (
            (angle+PI)/
            (2.0f*PI)
        );

        float v=
        glm::clamp(
            (p.z-minZ)/
            (maxZ-minZ),
            0.0f,
            1.0f
        );

        return glm::vec2(u,v);
    };

    for(uint32_t i=0;i<triangleCount;i++)
    {
        glm::vec3 normal,v1,v2,v3;

        file.read(reinterpret_cast<char*>(&normal),12);

        file.read(reinterpret_cast<char*>(&v1),12);
        file.read(reinterpret_cast<char*>(&v2),12);
        file.read(reinterpret_cast<char*>(&v3),12);

        uint16_t attributeByteCount;

        file.read(reinterpret_cast<char*>(&attributeByteCount),2);

        normal=glm::normalize(normal);

        glm::vec2 uv1=calcUV(v1,normal);
        glm::vec2 uv2=calcUV(v2,normal);
        glm::vec2 uv3=calcUV(v3,normal);

        glm::vec3 center=
        (v1+v2+v3)/3.0f;

        bool insideDoor=
        (
            center.x>-25.0f &&
            center.x<25.0f &&
            center.y>-55.0f &&
            center.y<-20.0f &&
            center.z>0.0f &&
            center.z<80.0f
        );

        if(insideDoor)
            continue;

        vertices.push_back({v1,normal,uv1});
        vertices.push_back({v2,normal,uv2});
        vertices.push_back({v3,normal,uv3});

        Triangle tri;

        tri.a=v1;
        tri.b=v2;
        tri.c=v3;

        collisionTriangles.push_back(tri);
    }

    file.close();

    std::cout<<"Loaded STL: "<<triangleCount<<" triangles\n";
    std::cout<<"Render vertices: "<<vertices.size()<<"\n";
    std::cout<<"Collision triangles: "<<collisionTriangles.size()<<"\n";

    return true;
}