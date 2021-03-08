#include "ShaderLoader.h"
#include <fstream>
#include "core/debugger/public/Logger.h"

std::vector<char> ShaderLoader::readFile(const std::string& shaderSrcFile)
{
    std::ifstream f(shaderSrcFile, std::ios::ate | std::ios::binary);
    if (!f.is_open()) LOG_ERR(("can't open file: " + shaderSrcFile).c_str());

    std::size_t size = (std::size_t)f.tellg();
    std::vector<char> src(size, 0);
    f.seekg(0);
    f.read(src.data(), size);
    f.close();
    return src;
}
