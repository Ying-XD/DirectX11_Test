#pragma once

#include "math3d.h"
#include <string>
#include <vector>

namespace pugi {
    class xml_node;
};

struct Node
{
    std::string identifier;
    mat4        transform;
    mat4        worldTransform;
};

struct NodeTree
{
    struct NodeVisitor {
        size_t                   nodeIdx;
        std::vector<NodeVisitor> children;
    };

    std::vector<Node> nodes;
    NodeVisitor       root;

    Node&       node(NodeVisitor const& v)       { return nodes[v.nodeIdx]; }
    Node const& node(NodeVisitor const& v) const { return nodes[v.nodeIdx]; }

    bool load(pugi::xml_node const&);
};

struct MaterialAsset
{
    struct ShaderVariable {
        std::string name;
        std::string type;
        std::string value;
    };

    std::string                 identifier;
    std::string                 shaderName;
    std::vector<ShaderVariable> variables;

    bool load(pugi::xml_node const&);
};

struct PrimitiveGroupAsset
{
	int			  primitiveGroupIdx;
	MaterialAsset material;
};

struct RenderSetAsset
{
    std::string              verticesName;
    std::string              indicesName;
    std::vector<std::string> nodeNames;
	std::vector<PrimitiveGroupAsset> primitiveGroups;

    bool load(pugi::xml_node const&);
};

struct VisualAsset
{
    NodeTree                    nodeTree;
    std::vector<RenderSetAsset> renderSets;

    bool read(std::string const& filename);
};

