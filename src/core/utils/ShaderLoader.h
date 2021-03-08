#pragma once
#include <string>
#include <vector>
#include <utility>

typedef std::vector<std::vector<char>> ShaderList;

enum class SHADER_POSITION
{
	VERTEX_INDEX,
	FRAGMENT_INDEX,
	TESS_CONTROL_INDEX,
	TESS_EVAL_INDEX,
	GEOMETRY_INDEX,
	COMPUTE_INDEX,
	NONE = -1
};

struct ShaderLoader
{
	
	template<typename ...Args>
	static std::vector<std::vector<char>> loadShaders(Args&&... shadersPathNames)
	{
		std::vector <std::string> files{ std::forward<Args>(shadersPathNames)... };
		std::vector<std::vector<char>> spirvSources;
		spirvSources.reserve(files.size());
		for (std::string& file : files)
		{
			spirvSources.emplace_back(std::move(readFile(file)));
		}
		return spirvSources;
		
	}

	static std::vector<char> readFile(const std::string& shaderSrcFile);
};

