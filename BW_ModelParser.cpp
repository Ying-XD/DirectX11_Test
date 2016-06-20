#include "BW_ModelParser.h"
#include "Log.h"
BW_ModelParser::BW_ModelParser() {
}


BW_ModelParser::~BW_ModelParser() {
	delete m_boneTree;
}

bool BW_ModelParser::Initialize(ID3D11Device * device) {
	m_device = device;
	m_boneTree = new BoneTree;
	return true;
}

bool BW_ModelParser::LoadModelFiles(std::string primitive_fileName, std::string visual_fileName) {
	read_primitives(primitive_fileName);
	read_visual(visual_fileName);

	for each (auto & mInfo in m_modelsInfoList) {
		auto itr_v = m_verticesList.find(mInfo.vertexName);
		auto itr_i = m_indicesList.find(mInfo.indexName);

		if (itr_v == m_verticesList.end() || itr_i == m_indicesList.end()) continue;

		const auto & vertices = itr_v->second;
		const auto & indices = itr_i->second;
		BW_ModelClass* pModel = new BW_ModelClass;
		pModel->LoadModel(m_device, vertices, indices);
		pModel->SetBoneList(mInfo.bnList);
		m_modelList.push_back(pModel);
	}
	return true;
}

BoneTree* BW_ModelParser::GetBoneTree() {
	return m_boneTree;
}

ModelsList BW_ModelParser::GetModels() {
	return m_modelList;
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
		if (m.first.find("indices") != std::string::npos) {
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
		xmlnode = xmlnode.next_sibling();
	}
	m_boneTree->SetRoot(root);

	xmlnode = doc.child("root").first_child();
	while (xmlnode) {
		if (strcmp(xmlnode.name(), "renderSet") == 0) {
			ReadRenderSet(xmlnode);
		}
		if (strcmp(xmlnode.name(), "boundingBox") == 0) {
			ReadBoundingBox(xmlnode);
		}
		xmlnode = xmlnode.next_sibling();
	}
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
		mat.m[i][2] = atof(value.substr(b + 1, value.length()).c_str());
		mat.m[i][3] = (i == 3) ? 1.0f : 0.0f;
		
	}
	return mat;
}

BoneNode * BW_ModelParser::ReadBoneNodes(pugi::xml_node xmlnode) {
	std::string name = xmlnode.child("identifier").first_child().value();
	name = RemoveTabStops(name);

	BoneNode* boneNode		= new BoneNode(name);
	BoneNode* childNode		= nullptr;
	BoneNode* tmp			= nullptr;

	boneNode->_transformMatrix = GetTransformatMatrix(xmlnode.child("transform"));

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

bool BW_ModelParser::ReadRenderSet(pugi::xml_node xmlnode) {
	xmlnode = xmlnode.child("node");
	BoneNodeList  bn_list;
	std::string indexName, vertexName;
	while (xmlnode) {
		if (strcmp(xmlnode.name(), "node") == 0) {
			std::string name = xmlnode.first_child().value();
			name = RemoveTabStops(name);
			bn_list.push_back(m_boneTree->GetBoneNodeFromName(name));
		}
		if (strcmp(xmlnode.name(), "geometry") == 0) {
			vertexName = xmlnode.child("vertices").first_child().value();
			indexName  = xmlnode.child("primitive").first_child().value();
		}
		xmlnode = xmlnode.next_sibling();
	}

	ModelsInfoType modelInfo;
	modelInfo.vertexName = RemoveTabStops(vertexName);
	modelInfo.indexName  = RemoveTabStops(indexName);
	modelInfo.bnList	 = std::move(bn_list);
	m_modelsInfoList.push_back(std::move(modelInfo));
	return true;
}

void BW_ModelParser::ReadBoundingBox(pugi::xml_node xmlnode) {
	std::string value;
	value = xmlnode.child("min").first_child().value();
	value = RemoveTabStops(value);
	
	value = xmlnode.child("max").first_child().value();
	value = RemoveTabStops(value);
}
