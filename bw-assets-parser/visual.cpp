#include "visual.h"
#include "pugixml.hpp"
#include "binaryfile.h"
#include <sstream>

static std::string strip(char const* str) {
    for(;*str && (*str == ' ' || *str == '\r' || *str == '\n' || *str == '\t'); ++str)
        ;
    size_t len=strlen(str)-1;
    for(;len>0 && str[len] != ' ' && str[len] != '\r' && str[len] != '\n' && str[len] != '\t'; --len)
        ;
    return std::string(str, str+len);
}

static NodeTree::NodeVisitor readNode(std::vector<Node> &nodes, pugi::xml_node const& xmlnode) {
    nodes.push_back(Node());
    NodeTree::NodeVisitor nodevisitor;
    nodevisitor.nodeIdx = nodes.size()-1;

    Node &node = nodes.back();
    node.identifier = strip(xmlnode.child_value("identifier"));

    pugi::xml_node transform = xmlnode.child("transform");
    std::stringstream row0(transform.child_value("row0"));
    std::stringstream row1(transform.child_value("row1"));
    std::stringstream row2(transform.child_value("row2"));
    std::stringstream row3(transform.child_value("row3"));

    row0 >> node.transform.row(0).x >> node.transform.row(0).y >> node.transform.row(0).z;
    row1 >> node.transform.row(1).x >> node.transform.row(1).y >> node.transform.row(1).z;
    row2 >> node.transform.row(2).x >> node.transform.row(2).y >> node.transform.row(2).z;
    row3 >> node.transform.row(3).x >> node.transform.row(3).y >> node.transform.row(3).z;


    for(pugi::xml_node itr = xmlnode.child("node"); itr; itr = itr.next_sibling("node")) {
        nodevisitor.children.push_back(readNode(nodes, itr));
    }

    return nodevisitor;
}

bool NodeTree::load(pugi::xml_node const& xml)
{
    nodes.clear();
    root = readNode(nodes, xml);

    return true;
}

bool RenderSetAsset::load(pugi::xml_node const& xml)
{
    nodeNames.clear();
    for(pugi::xml_node node = xml.child("node"); node; node = node.next_sibling("node")) {
        nodeNames.push_back(strip(node.child_value()));
    }
    pugi::xml_node geomNode = xml.child("geometry");
    if(!geomNode)
        return false;
    verticesName = strip(geomNode.child_value("vertices"));
    indicesName  = strip(geomNode.child_value("primitive"));
    pugi::xml_node pgNode = geomNode.child("primitiveGroup");
    if(!pgNode)
        return false;
	for (; pgNode; pgNode = pgNode.next_sibling("primitiveGroup")) {
		int primitiveGroupIdx = atoi(pgNode.text().as_string());
		MaterialAsset material;
		bool succ = material.load(pgNode.child("material"));
		if (!succ)
			return false;
		primitiveGroups.push_back({ primitiveGroupIdx, material });
	}
	return true;
}

bool MaterialAsset::load(pugi::xml_node const& xml)
{
    if(!xml)
        return false;
    identifier = xml.child_value("identifier");
    shaderName = xml.child_value("fx");
    variables.clear();
    for(pugi::xml_node property = xml.child("property"); property; property = property.next_sibling("property")) {
        ShaderVariable var;
        var.name = property.text().as_string();
        pugi::xml_node vardef = property.first_child();
        
        var.type = vardef.name();
        var.value = vardef.child_value();

        variables.push_back(var);
    }
    return true;
}

bool VisualAsset::read(std::string const& filename)
{
    pugi::xml_document visualFile;
    visualFile.load_file(filename.c_str());
    if(!visualFile)
        return false;
    pugi::xml_node root = visualFile.child("root");
    if(!root)
        return false;

    nodeTree.load(root.child("node"));
    renderSets.clear();
    for(pugi::xml_node rs = root.child("renderSet"); rs; rs = rs.next_sibling("renderSet")) {
        RenderSetAsset rsasset;
        if(rsasset.load(rs))
            renderSets.push_back(rsasset);
    }
    
    return true;
}

#ifdef TEST_VISUAL

#include <iostream>
int main(int argc, char** argv)
{
    VisualAsset visual;
    if(argc>=2) {
        visual.read(argv[1]);
        for(Node const& node:visual.nodeTree.nodes) {
            std::cout<<node.identifier<<"\n";
        }
    }
    return 0;
}

#endif

