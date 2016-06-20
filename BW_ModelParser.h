#pragma once
#include <D3D11.h>
#include <string>
#include <map>
#include <vector>

#include "parser_header.h"
#include "BoneNode.h"
#include "BW_ModelClass.h"

typedef std::map<std::string, VerticesAsset>	VerticesList;
typedef std::map<std::string, IndicesAsset>		IndicesList;
typedef std::vector<BW_ModelClass*>				ModelsList;
struct BoundingBox {
	D3DXVECTOR3 min, max;
};

class BW_ModelParser {
private:
	struct ModelsInfoType {
		std::string	vertexName;
		std::string	indexName;
		BoneNodeList	bnList;
	};
	const char* row_tags[4] = { "row0", "row1", "row2", "row3" };
private:
	BoneTree*		m_boneTree;
	VerticesList	m_verticesList;
	IndicesList		m_indicesList;
	ID3D11Device*	m_device;
	ModelsList		m_modelList;

	std::vector<ModelsInfoType>	m_modelsInfoList;

public:
	BW_ModelParser();
	~BW_ModelParser();
	bool Initialize(ID3D11Device* device);
	bool LoadModelFiles(std::string primitive_fileName, std::string visual_fileName);

	BoneTree*   GetBoneTree();
	ModelsList GetModels();
	void read_primitives(std::string fileName);
	bool read_visual(std::string fileName);
	//void read_animation();

private:
	std::string RemoveTabStops(const std::string& str);
	D3DXMATRIX  GetTransformatMatrix(pugi::xml_node xmlnode);
	BoneNode* ReadBoneNodes(pugi::xml_node xmlnode);
	bool ReadRenderSet(pugi::xml_node xmlnode);
	void ReadBoundingBox(pugi::xml_node xmlnode);



};
