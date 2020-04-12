#pragma once

#include <unordered_map>
#include <utility>
#include <vector>
#include "Territory.h"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html
//http://www.sccg.sk/~samuelcik/dgs/quad_edge.pdf

class Delaunay
{
public:
	Delaunay(int numSites = 0, float width = 1024.0f, float height = 2048.0f);
	Delaunay(std::vector<sf::Vector2f*> siteList, float width = 1024.0f, float height = 2048.0f);
	~Delaunay();

	std::vector<Territory*>& getVoronoiCells();

private:
	struct Edge
	{
		Edge(sf::Vector2f* o, sf::Vector2f* d, Edge* ccwo = nullptr, Edge* cwo = nullptr, Edge* ccwd = nullptr, Edge* cwd = nullptr);

		sf::Vector2f* origin;
		sf::Vector2f* destination;
		Edge* ccwAroundOrigin;
		Edge* cwAroundOrigin;
		Edge* ccwAroundDestination;
		Edge* cwAroundDestination;
	};

	const float WIDTH;
	const float HEIGHT;

	std::vector<sf::Vector2f*> sites;
	std::unordered_set<Edge*> edges;
	std::vector<Territory*> voronoiCells;
	std::unordered_set<sf::Vector2f*> voronoiVertices;

	static bool compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b);
	static bool compareVector2fPtrFloatPairPtr(std::pair<sf::Vector2f*, float>* a, std::pair<sf::Vector2f*, float>* b);
	static bool isOutsideCircumcircle(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c, sf::Vector2f* p);
	static int getOrientation(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c);

	std::pair<Edge*, Edge*> triangulate(int firstSiteIndex, int lastSiteIndex);
	std::pair<Edge*, Edge*> triangulate3(int firstSiteIndex, int lastSiteIndex);
	std::pair<Edge*, Edge*> triangulate2(int firstSiteIndex, int lastSiteIndex);
	std::pair<Edge*, Edge*> merge(std::pair<Edge*, Edge*> leftEdges, std::pair<Edge*, Edge*> rightEdges);
	Edge* createBottomEdge(std::pair<Edge*, Edge*>& leftEdges, std::pair<Edge*, Edge*>& rightEdges);
	Edge* getLeftCandidate(Edge* baseEdge);
	Edge* getRightCandidate(Edge* baseEdge);
	Edge* addEdgeUsingLeftCandidate(Edge* baseEdge, Edge* candidateEdge, sf::Vector2f* endpoint);
	Edge* addEdgeUsingRightCandidate(Edge* baseEdge, Edge* candidateEdge, sf::Vector2f* endpoint);
	Edge* replaceEdge(Edge* edge, bool isReverse);
	Edge* reverseEdge(Edge* edge);
	void removeEdge(Edge* edge);
	void createVoronoiCells(std::pair<Delaunay::Edge*, Delaunay::Edge*> bottomEdges);
	sf::Vector2f* getCircumcenter(sf::Vector2f* siteA, sf::Vector2f* siteB, sf::Vector2f* siteC);
	bool isSiteTooClose(sf::Vector2f* site, Edge* edge, float x, float y);
	void getOuterVertex(Edge* edge, bool isOutCw, char direction, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets);
	void getOuterVertex(Edge* edge, bool isOutCw, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets);
	void getOuterVertices(Edge* firstEdge, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets);
	void getOtherVertices(sf::Vector2f* site, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, Edge*>& siteEdges, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets, std::unordered_set<int>& vertexIds);
	int getVertexId(std::unordered_map<sf::Vector2f*, int>& siteIndices, sf::Vector2f* siteA, sf::Vector2f* siteB, sf::Vector2f* siteC);
};