#pragma once
#include "../InitShader.h"
#include <algorithm>
#include <unordered_map>




enum class eShading
{
    flat,
    gouraud,
    phong
};


struct KeyFuncs
{
    size_t operator()(const VEC3& k)const
    {
        return std::hash<int>()(k.x) ^ std::hash<int>()(k.y) ^ std::hash<int>()(k.z);
    }

    bool operator()(const VEC3& a, const VEC3& b)const
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};




/// <summary>
/// caculate normal vector for gouraud shading
/// vertex's normal vector is average of adjact plain's normal vector
/// normalVector has sum of normal vector of adjact plain 
/// just normalize last 
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// 
void gouraudShading(VEC4 a, VEC4 b, VEC4 c,
    std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs>& normalMap,bool);

void phongShading(VEC4 a, VEC4 b, VEC4 c,
    std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs>& normalVector,bool);