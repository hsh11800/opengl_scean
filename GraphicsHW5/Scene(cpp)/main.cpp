#include <iostream>
#include "../InitShader.h"
#include <stdlib.h>
#include "patches.h"
#include "vertices.h"
#include "TrackBall.h"
#include "shading.h"
#include "texture.h"


GLubyte checkboardTexture[64][64][3]; // Create a 64x64 checkerboard pattern 


const int NumTriangles = (NumTeapotPatches * 4)*4*4*4;
const int NumVertics = (NumTriangles * 3);
int index = 0;
int winWidth = 512, winHeight = 512;

VEC4 points[NumVertics];
VEC3 normals[NumVertics];


const int NumCubeTriangles = NumCubePatches * 4;
const int NumCubeVertics = (NumCubeTriangles * 3);
int cubeIndex = 0;

VEC4 cubePoints[NumCubeVertics];
VEC3 cubeNormals[NumCubeVertics];
VEC2 cubeTexCoord[NumCubeVertics];
const int NumSphereDivide = 50;
const int NumSphereTriangles = NumSphereDivide * NumSphereDivide * 3;
const int NumSphereVertics = (NumSphereTriangles * 3);
int sphereIndex = 0;

VEC4 spherePoints[NumSphereVertics];
VEC3 sphereNormals[NumSphereVertics];
VEC2 sphereTexCoord[NumSphereVertics];


GLuint Projection;
glm::mat4 model_teapotM = glm::mat4(1.0f);
glm::mat4 model_cubeM = glm::mat4(1.0f);
glm::mat4 model_sphereM = glm::mat4(1.0f);

glm::mat4 ViewM = glm::mat4(1.0f);
glm::mat4 ctm = glm::mat4(1.0f);

glm::mat4 teapotRotateM = glm::mat4(1.0f);
glm::mat4 cubeRotateM = glm::mat4(1.0f);
glm::mat4 sphereRotateM = glm::mat4(1.0f);

GLuint modelLoc;
GLuint ViewLoc;

VEC4 lightPos = VEC4(0.0, 0.0, 2.0, 1.0);
GLuint light_loc;
GLfloat shininess = 30.0;
GLuint shin_loc;
/// <summary>
/// light term for shading
/// </summary>
VEC4 ambientLight = VEC4(0.3, 0.3, 0.3, 1.0);
VEC4 diffuseLight = VEC4(0.8, 0.8, 0.8, 1.0);
VEC4 specularLight = VEC4(1.0, 1.0, 1.0, 1.0);
/// <summary>
/// mesh term for shading
/// </summary>
VEC4 ambientSurface[3] =
{
    VEC4(0.3, 0.7, 0.2, 1.0),
    VEC4(0.8,0.3,0.3,1.0),
    VEC4(0.4,0.4,0.4,1.0)

};
VEC4 diffuseSurface[3] = 
{
    VEC4(0.3, 0.7, 0.2, 1.0),
    VEC4(0.8,0.3,0.3,1.0),
    VEC4(0.4,0.4,0.8,1.0)

};
VEC4 specularSurface[3] = {
    VEC4(0.6, 0.6, 0.6, 0.3),
    VEC4(0.6, 0.6, 0.6, 0.3),
    VEC4(0.6, 0.6, 0.6, 0.3)
};

VEC4 ambientProduct[3];
VEC4 diffuseProduct[3];
VEC4 specularProduct[3];
GLuint ambient_loc;
GLuint diffuse_loc;
GLuint specular_loc;

GLuint vao[3];

enum
{
    X = 0, Y, Z
};

enum class eModelType
{
    teapot = 0, cube, sphere,
};

/////////////////////////////////////////////////
///////////////spin cube/////////////////////////
/////////////////////////////////////////////////
enum class eAxis
{
    X = 0, Y, Z
};
eAxis spin_axis = eAxis::X;
GLfloat teapotTheta[3] = { 0.0,0.0,0.0 };
GLfloat cubeTheta[3] = { 0.0,0.0,0.0 };
GLfloat sphereTheta[3] = { 0.0,0.0,0.0 };
GLfloat degree = 0.001f;
bool isSpin = false;

VEC3 teapotTrans = VEC3(0.0, 0.0, 0.0);

VEC3 cubeTrans = VEC3(0.0, 0.0, 0.0);

VEC3 sphereTrans = VEC3(0.0, 0.0, 0.0);
eShading teapotShading;
eShading cubeShading;

/// <summary>
/// key: the vertex of Teapot
/// value: rescently caculated normal vector
/// </summary>
std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs> teapotNormalVectorMap;

/// <summary>
/// key: the vertex of cube
/// value: rescently caculated normal vector
/// </summary>
std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs> cubeNormalVectorMap;


bool teapotMove = false;
bool cubeMove = false;
bool sphereMove = false;


/// <summary>
/// caculate normal vector for flat shading
/// just plane's normal vector
/// </summary>
/// <param name="a">vertex a</param>
/// <param name="b">vertex b</param>
/// <param name="c">vertex c</param>
void flatShading(VEC4 a, VEC4 b, VEC4 c)
{
    VEC3 ba = b-a;
    VEC3 ca = c - a;
    VEC3 n = glm::cross( ca,ba);
    n = glm::normalize(n);
    normals[index] = n;
    normals[index+1] = n;
    normals[index+2] = n;
}


glm::mat4 spin_matrix(VEC3 translation,GLfloat theta[3],glm::mat4& RotateM)
{
    VEC3 modelTrans;
    glm::mat4 transForRotate = glm::translate(glm::mat4(1.0f), VEC3(translation[0], translation[1], translation[2]));
    glm::mat4 transForRotateInverse = glm::translate(glm::mat4(1.0f), VEC3(-translation[0], -translation[1], -translation[2]));


    RotateM = glm::rotate(glm::mat4(1.0), theta[static_cast<int>(eAxis::X)], VEC3(1.0, 0.0, 0.0)) *
        glm::rotate(glm::mat4(1.0), theta[static_cast<int>(eAxis::Y)], VEC3(0.0, 1.0, 0.0)) *
        glm::rotate(glm::mat4(1.0), theta[static_cast<int>(eAxis::Z)], VEC3(0.0, 0.0, 1.0));
    return transForRotate * RotateM * transForRotateInverse;//arbitary axis rotation
}

void transpose(VEC4 a[4][4])
{
    VEC4 temp[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][i] = a[i][j];
        }
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            a[i][j] = temp[i][j];
        }
    }
}

/// <summary>
/// make bazier curve
/// </summary>
/// <param name="c">input cunvex hull</param>
/// <param name="r">right convex hull</param>
/// <param name="l">left convex hull</param>
void divide_curve(VEC4 c[4], VEC4 r[4], VEC4 l[4])
{
    /* division of convex hull of Bezier curve */
    int i;
    VEC4 t;
    for (i = 0; i < 3; i++) {
        l[0][i] = c[0][i];
        r[3][i] = c[3][i];
        l[1][i] = (c[1][i] + c[0][i]) / 2;
        r[2][i] = (c[2][i] + c[3][i]) / 2;
        t[i] = (c[1][i] + c[2][i]) / 2;
        l[2][i] = (t[i] + l[1][i]) / 2;
        r[1][i] = (t[i] + r[2][i]) / 2;
        l[3][i] = r[0][i] = (l[2][i] + r[1][i]) / 2;
    }
    for (i = 0; i < 4; i++)
        l[i][3] = r[i][3] = 1.0;
}

/// <summary>
/// make bezier patch
/// </summary>
/// <param name="p"></param>
/// <param name="n"></param>
/// <param name="es"></param>
/// <param name="normalMap"></param>
void divide_patch(VEC4 p[4][4], int n,eShading es,
    std::unordered_map<VEC3, VEC3, KeyFuncs, KeyFuncs>& normalMap)
{
    VEC4 q[4][4], r[4][4], s[4][4], t[4][4];
    VEC4 a[4][4], b[4][4];
    int k;
    if (n == 0)
    {

        VEC4 patch[4];
        patch[0] = p[0][0], patch[1] = p[3][0], patch[2] = p[3][3], patch[3] = p[0][3];
        //save vertex of triangle
        points[index] = patch[0];
        points[index + 1] = patch[1];
        points[index + 2] = patch[2];
        //shading

        phongShading(patch[0], patch[1], patch[2], normalMap,true);
        index += 3;
        
        points[index] = patch[0];
        points[index + 1] = patch[2];
        points[index + 2] = patch[3];
        //shading

        phongShading(patch[0], patch[2], patch[3], normalMap,true);
        index += 3;

       

    }
    else
    {
        for (k = 0; k < 4; k++)
            divide_curve(p[k], a[k], b[k]);
        transpose(a);
        transpose(b);
        for (k = 0; k < 4; k++) {
            divide_curve(a[k], q[k], r[k]);
            divide_curve(b[k], s[k], t[k]);
        }
        /* recursive division of 4 resulting patches */
        divide_patch(q, n - 1, es, normalMap);
        divide_patch(r, n - 1, es, normalMap);
        divide_patch(s, n - 1, es, normalMap);
        divide_patch(t, n - 1, es, normalMap);
    }
}



void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    GLfloat left = -8.0, right = 8.0;
    GLfloat bottom = -8.0, top = 8.0;
    GLfloat zNear = -13.0, zFar = 13.0;
    GLfloat aspect = GLfloat(width) / height;

    if (aspect > 0)
    {
        left *= aspect;
        right *= aspect;
    }
    else
    {
        bottom /= aspect;
        top /= aspect;
    }
    
    glm::mat4 projection = glm::ortho(left, right, bottom, top, zNear, zFar);
    glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
    
    glUniform4fv(light_loc, 1, glm::value_ptr(lightPos));
    glUniform1f(shin_loc, shininess);
    

}

GLuint program;
GLuint textures[2];
void init()
{
    makeTexture(checkboardTexture);

    GLubyte fakeTexture[64][64][3];
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                fakeTexture[i][j][k] = 255;
            }
        }
    }

    /// <summary>
    /// shading poroduct caculate
    /// </summary>

    for (int i = 0; i < 3; i++)
    {
        ambientProduct[i] = VEC4(ambientLight[0] * ambientSurface[i][0],
            ambientLight[1] * ambientSurface[i][1],
            ambientLight[2] * ambientSurface[i][2],
            1.0);
        diffuseProduct[i] = VEC4(diffuseLight[0] * diffuseSurface[i][0],
            diffuseLight[1] * diffuseSurface[i][1],
            diffuseLight[2] * diffuseSurface[i][2],
            1.0);
        specularProduct[i] = VEC4(specularLight[0] * specularSurface[i][0],
            specularLight[1] * specularSurface[i][1],
            specularLight[2] * specularSurface[i][2],
            1.0);
    }
    

    teapotShading = eShading::phong;
    cubeShading = eShading::phong;
    for (int n = 0; n < NumTeapotPatches; n++)
    {
        VEC4 patch[4][4];

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                VEC3& v = vertices[indices[n][i][j]];
                patch[i][j] = VEC4(v[X], v[Y], v[Z], 1.0);
            }
        }
        
        
        
        divide_patch(patch, 3, teapotShading,teapotNormalVectorMap);
    }//end for n<NumTeapotPatches

    if (teapotShading != eShading::flat)
    {

        for (int i = 0; i < index; i++) {
            normals[i] = glm::normalize(teapotNormalVectorMap[points[i]]);
            
        }
    }
    
    /// <summary>
    /// make cube 
    /// </summary>
    /// 
    
    for (int n = 0; n < NumCubePatches; n++)
    {

        VEC4 patch[4];
        for (int i = 0; i < 4; i++)
        {
            patch[i] = cubeVertics[cubeIndices[n][i]];

        }

        VEC2 texcor[4] =
        {
            VEC2(0.0,0.0), VEC2(0.0, 1.0), VEC2(1.0, 1.0),VEC2(1.0, 0.0)
        };
       for (int i = 0; i < 4; i++)
        {
            cubePoints[cubeIndex] = patch[i];
            cubePoints[cubeIndex + 1] = patch[(i + 1) % 4];
            cubePoints[cubeIndex + 2] = patch[(i + 2) % 4];
            phongShading(patch[i], patch[(i + 1) % 4], patch[(i + 2) % 4], cubeNormalVectorMap,false);


            cubeTexCoord[cubeIndex] = texcor[i];
            cubeTexCoord[cubeIndex + 1] = texcor[(i + 1) % 4];
            cubeTexCoord[cubeIndex + 2] = texcor[(i + 2) % 4];
            

            cubeIndex += 3;
        }

        

    }

    if (cubeShading != eShading::flat)
    {

        for (int i = 0; i < cubeIndex; i++) {
            cubeNormals[i] = glm::normalize(cubeNormalVectorMap[cubePoints[i]]);
        }
    }
    
    /// <summary>
    /// make sphere
    /// </summary>
    
    for (float u = 0; u < NumSphereDivide; u++)
    {
        for (float v = 0; v < NumSphereDivide; v++)
        {
            float nowTheta = 2 * M_PI * u / NumSphereDivide;
            float nextTheta = 2 * M_PI * ((int)(u + 1) % NumSphereDivide) / NumSphereDivide;
            float nowPi = 2 * M_PI * v / NumSphereDivide;
            float nextPi = 2 * M_PI * ((int)(v + 1) % NumSphereDivide) / NumSphereDivide;
            VEC4 patch[4] =
            {
                VEC4(sin(nowTheta) * cos(nowPi),sin(nowTheta) * sin(nowPi),cos(nowTheta),1.0),
                VEC4(sin(nextTheta) * cos(nowPi),sin(nextTheta) * sin(nowPi),cos(nextTheta),1.0),
                VEC4(sin(nextTheta) * cos(nextPi),sin(nextTheta) * sin(nextPi),cos(nextTheta),1.0),
                VEC4(sin(nowTheta) * cos(nextPi),sin(nowTheta) * sin(nextPi),cos(nowTheta),1.0),
            };
            spherePoints[sphereIndex] = patch[0];
            spherePoints[sphereIndex+1] = patch[1];
            spherePoints[sphereIndex+2] = patch[2];
            sphereNormals[sphereIndex] = patch[0];
            sphereNormals[sphereIndex + 1] = patch[1];
            sphereNormals[sphereIndex + 2] = patch[2];
            
            sphereTexCoord[sphereIndex] = VEC2((atan2f(patch[0].y, patch[0].x) / M_PI + 1.0) * 0.5, (asin(patch[0].z) / M_PI + 0.5));
            sphereTexCoord[sphereIndex + 1] = VEC2((atan2f(patch[1].y, patch[1].x) / M_PI + 1.0) * 0.5, (asin(patch[1].z) / M_PI + 0.5));
            sphereTexCoord[sphereIndex + 2] = VEC2((atan2f(patch[2].y, patch[2].x) / M_PI + 1.0) * 0.5, (asin(patch[2].z) / M_PI + 0.5));
            sphereIndex += 3;


            spherePoints[sphereIndex] = patch[1];
            spherePoints[sphereIndex + 1] = patch[2];
            spherePoints[sphereIndex + 2] = patch[3];
            sphereNormals[sphereIndex] = patch[1];
            sphereNormals[sphereIndex + 1] = patch[2];
            sphereNormals[sphereIndex + 2] = patch[3];
            sphereTexCoord[sphereIndex] = VEC2((atan2f(patch[1].y, patch[1].x) / M_PI + 1.0) * 0.5, (asin(patch[1].z) / M_PI + 0.5));
            sphereTexCoord[sphereIndex + 1] = VEC2((atan2f(patch[2].y, patch[2].x) / M_PI + 1.0) * 0.5, (asin(patch[2].z) / M_PI + 0.5));
            sphereTexCoord[sphereIndex + 2] = VEC2((atan2f(patch[3].y, patch[3].x) / M_PI + 1.0) * 0.5, (asin(patch[3].z) / M_PI + 0.5));
            sphereIndex += 3;
            
        }
    }



 

    program = InitShader("vshaderPhong.glsl", "fshaderPhong.glsl");
    glEnable(program);

    glGenVertexArrays(3, vao);
    glBindVertexArray(vao[0]);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);

    glEnable(program);
    unsigned int offset = 0;
    GLuint loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

    offset += sizeof(points);
    GLuint loc2 = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
    
    GLuint TextureSize = 64;
    
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, fakeTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(program, "texture"), 0);


    /// <summary>
    /// set cube vertex 
    /// </summary>
    //glGenVertexArrays(2, vao);
    glBindVertexArray(vao[1]);

    GLuint buffer2;
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePoints) + sizeof(cubeNormals) + sizeof(cubeTexCoord), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubePoints), cubePoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubePoints), sizeof(cubeNormals), cubeNormals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubePoints)+ sizeof(cubeNormals), sizeof(cubeTexCoord), cubeTexCoord);


    glEnable(program);

    offset = 0;
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
    offset += sizeof(cubePoints);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
    offset+= sizeof(cubeNormals);

    
    
    glBindTexture(GL_TEXTURE_2D, textures[1]); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, checkboardTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glActiveTexture(GL_TEXTURE1); 
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glEnable(GL_TEXTURE_2D);

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord"); 
    glEnableVertexAttribArray(vTexCoord); 
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
   
    glUniform1i(glGetUniformLocation(program, "texture"), 0);


    /// <summary>
    /// set sphere vertex
    /// </summary>
    //glGenVertexArrays(3, vao);
    glBindVertexArray(vao[2]);
    GLuint buffer3;
    glGenBuffers(1, &buffer3);
    glBindBuffer(GL_ARRAY_BUFFER, buffer3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spherePoints) + sizeof(sphereNormals)+sizeof(sphereTexCoord), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spherePoints), spherePoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(spherePoints), sizeof(sphereNormals), sphereNormals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(spherePoints)+ sizeof(sphereNormals), sizeof(sphereTexCoord), sphereTexCoord);
    
    glEnable(program);

    offset = 0;

    GLuint loc5 = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc5);
    glVertexAttribPointer(loc5, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
    offset += sizeof(spherePoints);

    GLuint loc6 = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(loc6);
    glVertexAttribPointer(loc6, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
    offset += sizeof(sphereNormals);


    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));


    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, checkboardTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(program, "texture"), 0);
    
    {
        /// <summary>
        /// uniform variable location for shading
        /// </summary>
        glEnable(program);
        
        ambient_loc = glGetUniformLocation(program, "AmbientProduct");
        diffuse_loc = glGetUniformLocation(program, "DiffuseProduct");
        specular_loc = glGetUniformLocation(program, "SpecularProduct");
        light_loc = glGetUniformLocation(program, "LightPosition");
        shin_loc = glGetUniformLocation(program, "Shininess");
        
    }

    {
        /// <summary>
        /// uniform variable location for rotate quatanion
        /// </summary>
        rquat_loc = glGetUniformLocation(program, "rquat");
        
        
    }
    {

        modelLoc = glGetUniformLocation(program, "Model");
        ViewLoc = glGetUniformLocation(program, "View");
        Projection = glGetUniformLocation(program, "Projection");
    }

   
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
    /// <summary>
    /// lookAt(eye,center,up)
    /// </summary>
    glm::mat4 view = glm::lookAt(VEC3(0.0,0.0,3.0),
        VEC3(0.0, 0.0, 0.0), VEC3(0.0, 1.0, 0.0));
    ViewM = view * ViewM;


    /// <summary>
    /// model initial point
    /// </summary>
    model_cubeM = glm::translate(glm::mat4(1.0f), VEC3(4.0, 0.0, 0.0)) * model_cubeM;
    cubeTrans[0] += 4.0;

    model_sphereM= glm::translate(glm::mat4(1.0f), VEC3(-4.0, 0.0, 0.0)) * model_sphereM;
    sphereTrans[0] -= 4.0;


    glEnable(GL_DEPTH_TEST);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// <summary>
    /// track ball quatanion
    /// </summary>
    if (trackballMove)
    {
        rquat = mulq(VEC4(cos(angle / 2.0), sin(angle / 2.0) * axis[0], sin(angle / 2.0) * axis[1],
            sin(angle / 2.0) * axis[2]), rquat);
        std::cout << "rquat = (" << rquat[0] << ", " << rquat[1] << ", " << rquat[2] << ", " << rquat[3] << ")\n";
    }


    

    /// <summary>
    /// spin model matrix
    /// </summary>
    {
        //teapot
        glm::mat4 transForRotate = glm::translate(glm::mat4(1.0f), VEC3(teapotTrans[0], teapotTrans[1], teapotTrans[2]));
        glm::mat4 transForRotateInverse = glm::translate(glm::mat4(1.0f), VEC3(-teapotTrans[0], -teapotTrans[1], -teapotTrans[2]));

        model_teapotM = glm::inverse(transForRotate * teapotRotateM * transForRotateInverse) * model_teapotM;
        ctm = spin_matrix(teapotTrans, teapotTheta, teapotRotateM);
        model_teapotM = ctm * model_teapotM;

    }
    {
        //cube
        glm::mat4 transForRotate = glm::translate(glm::mat4(1.0f), cubeTrans);
        glm::mat4 transForRotateInverse = glm::translate(glm::mat4(1.0f), VEC3(-cubeTrans[0],-cubeTrans[1],-cubeTrans[2]));
        model_cubeM = glm::inverse(transForRotate * cubeRotateM * transForRotateInverse) * model_cubeM;
        ctm = spin_matrix(cubeTrans, cubeTheta, cubeRotateM);
        model_cubeM = ctm * model_cubeM;
    }
    {
        //sphere
        glm::mat4 transForRotate = glm::translate(glm::mat4(1.0f), sphereTrans);
        glm::mat4 transForRotateInverse = glm::translate(glm::mat4(1.0f), VEC3(-sphereTrans[0], -sphereTrans[1], -sphereTrans[2]));
        model_sphereM = glm::inverse(transForRotate * sphereRotateM * transForRotateInverse) * model_sphereM;
        ctm = spin_matrix(sphereTrans, sphereTheta, sphereRotateM);
        model_sphereM = ctm * model_sphereM;
    }



    /// <summary>
    /// teapot draw
    /// </summary>
    glBindVertexArray(vao[0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_teapotM));
    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(ViewM));
    glUniform4fv(rquat_loc, 1, glm::value_ptr(rquat));
    glUniform4fv(ambient_loc, 1, glm::value_ptr(ambientProduct[0]));
    glUniform4fv(diffuse_loc, 1, glm::value_ptr(diffuseProduct[0]));
    glUniform4fv(specular_loc, 1, glm::value_ptr(specularProduct[0]));
    
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertics);
    
    /// <summary>
    /// cube draw
    /// </summary>
    
    glBindVertexArray(vao[1]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_cubeM));
    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(ViewM));
    glUniform4fv(rquat_loc, 1, glm::value_ptr(rquat));
    glUniform4fv(ambient_loc, 1, glm::value_ptr(ambientProduct[1]));
    glUniform4fv(diffuse_loc, 1, glm::value_ptr(diffuseProduct[1]));
    glUniform4fv(specular_loc, 1, glm::value_ptr(specularProduct[1]));
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    
    glDrawArrays(GL_TRIANGLES, 0, NumCubeVertics);
    
    /// <summary>
    /// sphere draw
    /// </summary>
    glBindVertexArray(vao[2]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_sphereM));
    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(ViewM));
    glUniform4fv(rquat_loc, 1, glm::value_ptr(rquat));
    glUniform4fv(ambient_loc, 1, glm::value_ptr(ambientProduct[2]));
    glUniform4fv(diffuse_loc, 1, glm::value_ptr(diffuseProduct[2]));
    glUniform4fv(specular_loc, 1, glm::value_ptr(specularProduct[2]));
    glDrawArrays(GL_TRIANGLES, 0, NumSphereVertics);
    
    glutSwapBuffers();
}


void mouseMotion(int x, int y)
{
    VEC3 curPos;
    VEC3 d;
    curPos = trackball_ptov(x, y, winWidth, winHeight);
    if (trackingMouse)
    {
        d = curPos - lastPos;
        float a = glm::dot(d, d);
        if (a) {
            float speed = 1.0;
            angle = speed * (M_PI / 2.0f) * sqrt(a);
            axis = glm::cross(lastPos, curPos);
            a = 1.0f / glm::sqrt(glm::dot(axis, axis));
            axis *= a;

            lastPos = curPos;
        }
    }
    glutPostRedisplay();
}

void startMotion(int x, int y)
{
    trackingMouse = true;
    redrawContinue = false;
    startX = x, startY = y;
    curx = x, cury = y;
    lastPos = trackball_ptov(x, y, winWidth, winHeight);
    trackballMove = true;
}
void spinCube()
{

    if (isSpin)
    {
        if (teapotMove)
        {
            teapotTheta[static_cast<int>(spin_axis)] += degree;
            if (teapotTheta[static_cast<int>(spin_axis)] > 360.0)
            {
                teapotTheta[static_cast<int>(spin_axis)] -= 360.0;
            }

        }
        if (cubeMove)
        {
            cubeTheta[static_cast<int>(spin_axis)] += degree;
            if (cubeTheta[static_cast<int>(spin_axis)] > 360.0)
            {
                cubeTheta[static_cast<int>(spin_axis)] -= 360.0;
            }

        }

        if (sphereMove)
        {
            sphereTheta[static_cast<int>(spin_axis)] += degree;
            if (sphereTheta[static_cast<int>(spin_axis)] > 360.0)
            {
                sphereTheta[static_cast<int>(spin_axis)] -= 360.0;
            }

        }
    }
    
    
    glutPostRedisplay();

}
void stopMotion(int x, int y)
{

    trackingMouse = false;
    if (x != startX && y != startY)
    {
        redrawContinue = true;
        trackballMove = false;
    }
    else
    {
        angle = 0.0f;
        redrawContinue = false;
        trackballMove = false;
    }

}

void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            spin_axis = eAxis::Y;
        }
    }
    else if (btn == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            spin_axis = eAxis::X;
            y = winHeight - y;
            startMotion(x, y);
        }
        else if (state == GLUT_UP)
        {
            stopMotion(x, y);
        }
    }
}

void specialKey(int Key, int X, int Y)
{
    switch (Key) {
    case GLUT_KEY_LEFT:     //왼쪽 키
        x_d = -0.1f;
        trans = glm::translate(glm::mat4(1.0f), VEC3(x_d, 0.0, 0.0));
        if (teapotMove)
        {
            model_teapotM = trans * model_teapotM;
            teapotTrans[0] += x_d;
        }
        if (cubeMove)
        {
            model_cubeM = trans * model_cubeM;
            cubeTrans[0] += x_d;
        }
        if (sphereMove)
        {
            model_sphereM = trans * model_sphereM;
            sphereTrans[0] += x_d;
        }
        
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:     //오른쪽 키

        x_d = 0.1f;
        trans = glm::translate(glm::mat4(1.0f), VEC3(x_d, 0.0, 0.0));
        if (teapotMove)
        {
            model_teapotM = trans * model_teapotM;
            teapotTrans[0] += x_d;
        }
        if (cubeMove)
        {
            model_cubeM = trans * model_cubeM;
            cubeTrans[0] += x_d;
        }
        if (sphereMove)
        {
            model_sphereM = trans * model_sphereM;
            sphereTrans[0] += x_d;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP:      //위 키

        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:      //아래 키

        glutPostRedisplay();
        break;
    default:
        exit(0);
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 't')
    {
        teapotMove = !teapotMove;
    }
    else if (key == 'c')
    {
        cubeMove = !cubeMove;
    }
    else if (key == 's')
    {
        sphereMove = !sphereMove;
    }
    else if (key == 32)//space bar
    {
        isSpin = !isSpin;
    }
}

int main(int argc, char**argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA| GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Scene");
    glewInit();
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutSpecialFunc(specialKey);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(spinCube);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return EXIT_SUCCESS;
}
