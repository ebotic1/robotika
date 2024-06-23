#include "./../include/bufferManager.h"


void bufferManager::appendToBuffer(const GLfloat* vertices, unsigned int vertixCnt)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLsizeiptr newSize = currentSizeVBO + vertixCnt * sizeof(GLfloat);

    if (newSize > maxSizeVBO) {
        std::vector<GLfloat> tempBuffer(newSize / sizeof(GLfloat));
        maxSizeVBO = 1.3 * newSize; // 30% mjesta viska

        glGetBufferSubData(GL_ARRAY_BUFFER, 0, currentSizeVBO, tempBuffer.data());
        glBufferData(GL_ARRAY_BUFFER, maxSizeVBO, nullptr, GL_DYNAMIC_DRAW);
        std::copy(vertices, vertices + vertixCnt, tempBuffer.begin() + currentSizeVBO / sizeof(GLfloat));

        glBufferSubData(GL_ARRAY_BUFFER, 0, tempBuffer.size() * sizeof(GLfloat), tempBuffer.data());
        
    }
    else {
        glBufferSubData(GL_ARRAY_BUFFER, currentSizeVBO, vertixCnt * sizeof(GLfloat), vertices);
    }

    currentSizeVBO = newSize;
    
}

void bufferManager::appendToBuffer(const GLuint* indexes, unsigned int indexSize)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    GLsizeiptr newSize = currentSizeEBO + indexSize * sizeof(GLuint);

    if (newSize > maxSizeEBO) {
        std::vector<GLuint> tempBuffer(newSize / sizeof(GLuint));
        maxSizeEBO = 1.3 * newSize; // 30% mjesta viska
        

        glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, currentSizeEBO, tempBuffer.data());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxSizeEBO, nullptr, GL_DYNAMIC_DRAW);
        std::copy(indexes, indexes + indexSize, tempBuffer.begin() + currentSizeEBO / sizeof(GLuint));

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, tempBuffer.size() * sizeof(GLuint), tempBuffer.data());

    }
    else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentSizeEBO, indexSize * sizeof(GLuint), indexes);
    }

    currentSizeEBO = newSize;
}

bufferManager::bufferManager(GLuint program, const char* positionVec3Name):
    program(program)
{
    
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, maxSizeVBO, nullptr, GL_DYNAMIC_DRAW);

    GLint positionAttrib = glGetAttribLocation(program, positionVec3Name);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(positionAttrib);


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxSizeEBO, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

	
}

bufferManager::bufferManager(GLuint program, GLuint VAO, GLuint VBO, GLuint EBO, int pointsPerVertex):
    EBO(EBO),
    VAO(VAO),
    VBO(VBO),
    program(program),
    pointsPerVertex(pointsPerVertex)
{
}



object* bufferManager::createArrow(glm::vec3 startPos, glm::vec3 arrow, float width, float arrowHeadWidth, td::ColorID color, unsigned int basePointCnt)
{
    const glm::vec3 zAxis = { 0,0,1.0f };
    glm::vec3 dir = arrow;
    float len = glm::length(arrow);
    dir /= len;
    glm::mat4 mat;
    
    const float arrowHeadHeight = arrowHeadWidth * 3;
    len -= arrowHeadHeight;

    if (glm::all(glm::epsilonEqual(dir, zAxis, 0.0001f))) {
        mat = glm::mat4(1.0f);
    }
    else {
        glm::vec3 rotAxis = glm::cross(zAxis, dir);
        mat = glm::rotate(glm::mat4(1.0f), glm::acos(glm::dot(dir, zAxis)), rotAxis);
    }


    int pointCnt = ((basePointCnt * 3 + 3) * 3);
    GLfloat* points = new GLfloat[pointCnt];

    int ind = -1;
    float con = 2 * glm::pi<float>() / basePointCnt;
    for (int i = 0; i < basePointCnt; ++i) {
        points[++ind] = width * glm::cos(con * i);
        points[++ind] = width * glm::sin(con * i);
        points[++ind] = 0;
    }


    std::memcpy(points + basePointCnt*3, points, 3*basePointCnt * sizeof(GLfloat));

    for (int i = basePointCnt * 3 + 2; i < basePointCnt * 6; i+=3) 
        points[i] = len; //z val
    

    ind = basePointCnt * 6 - 1;
    for (int i = 0; i < basePointCnt; ++i) { //arrowhead base
        points[++ind] = arrowHeadWidth * glm::cos(con * i);
        points[++ind] = arrowHeadWidth * glm::sin(con * i);
        points[++ind] = len;
    }

    points[pointCnt - 9] = 0;
    points[pointCnt - 8] = 0;
    points[pointCnt - 7] = 0;

    points[pointCnt - 6] = 0;
    points[pointCnt - 5] = 0;
    points[pointCnt - 4] = len;

    points[pointCnt - 3] = 0;
    points[pointCnt - 2] = 0;
    points[pointCnt - 1] = len + arrowHeadHeight;


    appendToBuffer(points, pointCnt);

    int elemCnt = (basePointCnt * 5) * 3;
    GLuint* elems = new GLuint[elemCnt];


    ind = -1;
    for (int i = 0; i < basePointCnt; ++i) 
    {
        elems[++ind] = i + basePointCnt;
        elems[++ind] = i;
        elems[++ind] = i + 1;
    }

    elems[ind] = 0;


    for (int i = 0; i < basePointCnt; ++i)
    {
        elems[++ind] = i + 1 + basePointCnt;
        elems[++ind] = i + basePointCnt;
        elems[++ind] = i + 1;
    }

    elems[ind - 2] = basePointCnt;
    elems[ind] = 0;

    unsigned int off, centerPoint;

    for (int j = 0; j < 3; ++j) {
        switch (j)
        {
            case 0:
                off = basePointCnt * 2;
                centerPoint = basePointCnt * 3 + 2;
                break;
            case 1:
                centerPoint = basePointCnt * 3 + 1;
                break;
            case 2:
                off = 0;
                centerPoint = basePointCnt * 3;
        }

        if(j == 0)
            for (int i = 0; i < basePointCnt; ++i)
            {
                elems[++ind] = off + i;
                elems[++ind] = off + i + 1;
                elems[++ind] = centerPoint;
            }
        else
            for (int i = 0; i < basePointCnt; ++i)
            {
                elems[++ind] = centerPoint;
                elems[++ind] = off + i + 1;
                elems[++ind] = off + i;
               
            }

        elems[ind - 1] = off;

    }



    appendToBuffer(elems, elemCnt);

    delete[] points;
    delete[] elems;

   

    mat[3] = glm::vec4(startPos, 1.0f);
    auto ptr = new object(mat, program, VAO, offsetEl, elemCnt, color);
    adjustObject(pointCnt, elemCnt, ptr);
    return ptr;
}

object* bufferManager::createCustom(GLfloat* vertices, unsigned int vertixCnt, GLuint* indexes, unsigned int indexSize, glm::vec3 position, td::ColorID color)
{
    

    appendToBuffer(vertices, vertixCnt);
    appendToBuffer(indexes, indexSize);


    auto ptr = new object(position, program, VAO, offsetEl, indexSize, color);
    adjustObject(vertixCnt, indexSize, ptr);
   
    return ptr;
}

bufferManager::~bufferManager()
{
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &VBO);
        EBO = 0;
    }
}
