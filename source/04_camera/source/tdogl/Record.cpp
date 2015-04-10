#include "Record.h"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace tdogl;

Record::Record()
{
    
}

void Record::setViewMat(glm::mat4 view)
{
    
}
void Record::setProjMat(glm::mat3 proj)
{
    
}

float Record::rand_FloatRange(float a, float b)
{
    return ((b-a)*((float)rand()/RAND_MAX))+a;
}

