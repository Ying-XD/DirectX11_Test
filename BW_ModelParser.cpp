#include "BW_ModelParser.h"
BW_ModelParser::BW_ModelParser() {
}


BW_ModelParser::~BW_ModelParser() {
}

void BW_ModelParser::LoadModelFiles(std::string primitive_fileName, std::string visual_fileName) {
	read_primitives(primitive_fileName);

	read_visual(visual_fileName);
}

void BW_ModelParser::read_primitives(std::string fileName) {
	IBinaryFile inputFile(fileName);

	PrimitivesAsset primitives;
	primitives.read(inputFile);
	for each(const auto& m in primitives.sections) {
		auto& name = m.first;
		auto& var = m.second;
		if (m.first.find("vertices") != std::string::npos) {
			m_verticesList.insert(std::make_pair(m.first, std::move(VerticesAsset(m.second))));
		}
		if (m.first.find("inidces") != std::string::npos) {
			m_indicesList.insert(std::make_pair(m.first, std::move(IndicesAsset(m.second))));
		}
	}
}

bool BW_ModelParser::read_visual(std::string fileName) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());
	if (!result) return false;
	BoneNode* root = NULL;
	auto xmlnode = doc.child("root").first_child();
	while (xmlnode) {
		if (strcmp(xmlnode.name(), "node") == 0) {
			root = ReadBoneNodes(xmlnode);
		}
		if (strcmp(xmlnode.name(), "renderSet") == 0) {
			ReadRenderSet(xmlnode);
		}
		if (strcmp(xmlnode.name(), "boundingBox") == 0) {
			ReadBoundingBox(xmlnode);
		}
		xmlnode = xmlnode.next_sibling();
	}
	m_boneTree.SetRoot(root);
	return true;
}



std::string BW_ModelParser::RemoveTabStops(const std::string& str) {
	return str.substr(str.find_first_not_of('\t'), str.find_last_not_of('\t'));
}

D3DXMATRIX BW_ModelParser::GetTransformatMatrix(pugi::xml_node xmlnode) {
	float f[4];
	D3DXMATRIX mat;
	for (size_t i = 0;i < 4;i++) {
		std::string value = xmlnode.child(row_tags[i]).first_child().value();;
		value = RemoveTabStops(value);
		size_t a = value.find_first_of(' ');
		size_t b = value.find_last_of(' ');

		mat.m[i][0] = atof(value.substr(0, a).c_str());
		mat.m[i][1] = atof(value.substr(a + 1, b).c_str());
		mat.m[i][4] = atof(value.substr(b + 1, value.length()).c_str());
		mat.m[i][3] = (i == 3) ? 1.0f : 0.0f;
		
	}
	return mat;
}

BoneNode * BW_ModelParser::ReadBoneNodes(pugi::xml_node xmlnode) {
	std::string name = xmlnode.child("identifier").first_child().value();
	name = name.substr(name.find_first_not_of("\t"), name.find_last_not_of("\t"));

	BoneNode* boneNode		= new BoneNode(name);
	BoneNode* childNode		= nullptr;
	BoneNode* tmp			= nullptr;

	boneNode->_transformMatrix = GetTransformatMatrix(xmlnode.child("transformat"));

	xmlnode = xmlnode.child("node");
	while (xmlnode) {
		tmp = ReadBoneNodes(xmlnode);
		if (!childNode)
			boneNode->SetFirstChild(tmp);
		else
			childNode->SetSibling(tmp);

		childNode = tmp;
		xmlnode = xmlnode.next_sibling();
	}
	return boneNode;
}

void BW_ModelParser::ReadRenderSet(pugi::xml_node xmlnode) {
	xmlnode = xmlnode.child("node");
	ModelsGroup model;
	std::vector<std::string> vecBoneName;
	std::string indexName, vertexName;
	while (xmlnode) {
		if (strcmp(xmlnode.name(), "node") == 0) {
			std::string name = xmlnode.first_child().value();
			vecBoneName.push_back(std::move(RemoveTabStops(name)));
		}
		if (strcmp(xmlnode.name(), "geometry") == 0) {
			vertexName = xmlnode.child("vertices").first_child().value();
			indexName  = xmlnode.child("primitive").first_child().value();
		}
		xmlnode = xmlnode.next_sibling();
	}

	model.vertexName = std::move(RemoveTabStops(vertexName));
	model.indexName	 = std::move(RemoveTabStops(indexName));
	model.vBoneName  = std::move(vecBoneName);

	m_modelsGroupList.push_back(std::move(model));
}

void BW_ModelParser::ReadBoundingBox(pugi::xml_node xmlnode) {
	std::string value;
	value = xmlnode.child("min").first_child().value();
	value = RemoveTabStops(value);
	
	value = xmlnode.child("max").first_child().value();
	value = RemoveTabStops(value);
}
