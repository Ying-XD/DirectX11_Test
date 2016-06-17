#pragma once
#include <string>
#include <map>
#include <vector>

#include "parser_header.h"
#include "BoneNode.h"

typedef std::map<std::string, VerticesAsset>	VerticesList;
typedef std::map<std::string, IndicesAsset>		IndicesList;

struct BoundingBox {
	D3DXVECTOR3 min, max;
};

class BW_ModelParser {
private:
	struct ModelsGroup {
		size_t	groupID;
		std::string	vertexName;
		std::string	indexName;
		std::vector<std::string>	vBoneName;
	};
	const char* row_tags[4] = { "row0", "row1", "row2", "row3" };
private:
	BoneTree		m_boneTree;
	VerticesList	m_verticesList;
	IndicesList		m_indicesList;

	std::vector<ModelsGroup>	m_modelsGroupList;

public:
	BW_ModelParser();
	~BW_ModelParser();

	void LoadModelFiles(std::string primitive_fileName, std::string visual_fileName);

	void read_primitives(std::string fileName);
	bool read_visual(std::string fileName);
	//void read_animation();

private:
	std::string RemoveTabStops(const std::string& str);
	D3DXMATRIX  GetTransformatMatrix(pugi::xml_node xmlnode);
	BoneNode* ReadBoneNodes(pugi::xml_node xmlnode);
	void ReadRenderSet(pugi::xml_node xmlnode);
	void ReadBoundingBox(pugi::xml_node xmlnode);


};
