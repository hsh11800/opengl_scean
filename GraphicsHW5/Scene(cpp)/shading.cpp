#include "shading.h"


void gouraudShading(VEC4 a, VEC4 b, VEC4 c,
    std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs>& normalMap,bool isTeapot)
{
    VEC3 ba = b - a;
    VEC3 ca = c - a;
    VEC3 n = glm::cross(ba, ca);
    
    if (isTeapot&&n.z < 0)
    {
        n.x = n.x * -1;
        n.y = n.y * -1;
        n.z = n.z * -1;
    }
        


    if (normalMap.find(a) != normalMap.end())
    {
        VEC3 lastNormal = normalMap[a];
        VEC3 curNormal = lastNormal + n;
        normalMap[a] = curNormal;
    }
    else
    {
        normalMap.insert({ a, n });
    }
    if (normalMap.find(b) != normalMap.end())
    {
        VEC3 lastNormal = normalMap[b];
        VEC3 curNormal = lastNormal + n;
        normalMap[b] = curNormal;
    }
    else
    {
        normalMap.insert({ b, n });
    }
    if (normalMap.find(c) != normalMap.end())
    {
        VEC3 lastNormal = normalMap[c];
        VEC3 curNormal = lastNormal + n;
        normalMap[c] = curNormal;
    }
    else
    {
        normalMap.insert({ c, n });
    }
}

void phongShading(VEC4 a, VEC4 b, VEC4 c,
    std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs>& normalVector,bool isTeapot)
{
    gouraudShading(a, b, c, normalVector, isTeapot);
}