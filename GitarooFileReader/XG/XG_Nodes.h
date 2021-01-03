#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020 Gitaroo Pals
 *
 *  Gitaroo Man File Reader is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  Gitaroo Man File Reader is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Gitaroo Man File Reader.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Global_Functions.h"
#include "PString\PString.h"

struct XGNode
{
	PString m_name;
	virtual void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList) = 0;
	virtual void create(FILE* outFile, bool full) = 0;
	virtual void writeTXT(FILE* outTXT, const char* tabs = "") = 0;
	virtual const char* getType() = 0;
};

struct SharedNode
{
	XGNode* m_node = nullptr;
	SharedNode() = default;
	SharedNode(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void fill(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	// Warning: Does NOT perform a nullptr check
	void push(FILE* outFile);
	bool isValid() { return m_node != nullptr; }
	// Warning: Does NOT perform a nullptr check
	PString* getPString();
};

struct xgBgGeometry : public XGNode
{
	float m_density = 0;
	unsigned long m_vertexFlags = 0;
	unsigned long m_numVerts = 0;
	float** m_vertices = nullptr;
	std::vector<SharedNode> m_inputGeometries;
	xgBgGeometry() = default;
	xgBgGeometry(const PString& name) { m_name = name; }
	xgBgGeometry(const xgBgGeometry& geo);
	~xgBgGeometry();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgBgGeometry"; }
	static bool compare(const PString& str) { return strcmp("xgBgGeometry", str.m_pstring) == 0; }
};

struct xgBgMatrix : public XGNode
{
	float m_position[3] = { 0 };
	float m_rotation[4] = { 0 };
	float m_scale[3] = { 0 };
	SharedNode m_inputPosition;
	SharedNode m_inputRotation;
	SharedNode m_inputScale;
	SharedNode m_inputParentMatrix;
	xgBgMatrix() = default;
	xgBgMatrix(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	~xgBgMatrix() {}
	const char* getType() { return "xgBgMatrix"; }
	static bool compare(const PString& str) { return strcmp("xgBgMatrix", str.m_pstring) == 0; }
};

struct xgBone : public XGNode
{
	float m_restMatrix[4][4] = { 0 };
	SharedNode m_inputMatrix;
	xgBone() = default;
	xgBone(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	~xgBone() {}
	const char* getType() { return "xgBone"; }
	static bool compare(const PString& str) { return strcmp("xgBone", str.m_pstring) == 0; }
};

struct xgDagMesh : public XGNode
{
	struct Data
	{
		unsigned long m_arraySize = 0;
		unsigned long* m_arrayData = nullptr;
		Data() = default;
		Data(const Data& data);
		~Data();
	};
	unsigned long m_primType = 0;
	unsigned long m_primCount = 0;
	Data m_primData; //primDataSize
	unsigned long m_triFanCount = 0; //number of triFans
	Data m_triFanData; // triFanSize
	unsigned long m_triStripCount = 0; //number of triStrips
	Data m_triStripData; // triStripSize
	unsigned long m_triListCount = 0; //number of triLists
	Data m_triListData; // triListsize
	unsigned long m_cullFunc = 0;
	std::vector<SharedNode> m_inputGeometries;
	std::vector<SharedNode> m_inputMaterials;
	xgDagMesh() = default;
	xgDagMesh(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgDagMesh"; }
	static bool compare(const PString& str) { return strcmp("xgDagMesh", str.m_pstring) == 0; }
};

struct xgDagTransform : public XGNode
{
	std::vector<SharedNode> m_inputMatrices;
	xgDagTransform() = default;
	xgDagTransform(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgDagTransform"; }
	static bool compare(const PString& str) { return strcmp("xgDagTransform", str.m_pstring) == 0; }
};

struct xgEnvelope : public XGNode
{
	unsigned long m_startVertex = 0;
	unsigned long m_numweights = 0;
	float(*m_weights)[4] = nullptr;
	unsigned long m_numTargets = 0;
	long* m_vertexTargets = nullptr;
	std::vector<SharedNode> m_inputMatrices;
	std::vector<SharedNode> m_inputGeometries;
	xgEnvelope() = default;
	xgEnvelope(const PString& name) { m_name = name; }
	xgEnvelope(const xgEnvelope& env);
	~xgEnvelope();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgEnvelope"; }
	static bool compare(const PString& str) { return strcmp("xgEnvelope", str.m_pstring) == 0; }
};

struct xgMaterial : public XGNode
{
	unsigned long m_blendType = 0;
	unsigned long m_shadingType = 0;
	struct Diffuse
	{
		float red = 0;
		float green = 0;
		float blue = 0;
		float alpha = 0;
	} m_diffuse;
	struct Specular
	{
		float red = 0;
		float green = 0;
		float blue = 0;
		float exponent = 0;
	} m_specular;
	unsigned long m_flags = 0;
	unsigned long m_textureEnv = 0;
	unsigned long m_uTile = 0;
	unsigned long m_vTile = 0;
	std::vector<SharedNode> m_inputTextures;
	xgMaterial() = default;
	xgMaterial(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgMaterial"; }
	static bool compare(const PString& str) { return strcmp("xgMaterial", str.m_pstring) == 0; }
};

struct xgMultiPassMaterial : public XGNode
{
	std::vector<SharedNode> m_inputMaterials;
	xgMultiPassMaterial() = default;
	xgMultiPassMaterial(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgMultiPassMaterial"; }
	static bool compare(const PString& str) { return strcmp("xgMultiPassMaterial", str.m_pstring) == 0; }
};

struct xgNormalInterpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numtimes = 0;
	float* m_times = nullptr;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numNormals = 0; //equal to number of targets, below
		float(*m_normals)[3] = nullptr; // numNormals
		Key& operator=(const Key& key);
		~Key();
	} *m_keys = nullptr; // Array, obviously
	unsigned long m_numTargets = 0;
	unsigned long* m_targets = nullptr;
	SharedNode m_inputTime;
	xgNormalInterpolator() = default;
	xgNormalInterpolator(const PString& name) { m_name = name; }
	xgNormalInterpolator(const xgNormalInterpolator& norm);
	~xgNormalInterpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgNormalInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgNormalInterpolator", str.m_pstring) == 0; }
};

struct xgQuatInterpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numkeys = 0;
	float(*m_keys)[4] = nullptr;
	SharedNode m_inputTime;
	xgQuatInterpolator() = default;
	xgQuatInterpolator(const PString& name) { m_name = name; }
	xgQuatInterpolator(const xgQuatInterpolator& quat);
	~xgQuatInterpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgQuatInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgQuatInterpolator", str.m_pstring) == 0; }
};

struct xgShapeInterpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numtimes = 0;
	float* m_times = nullptr;
	unsigned long m_numkeys = 0; //same as number of times?
	struct Key
	{
		unsigned long m_vertexType = 0;
		unsigned long m_numVerts = 0;
		float** m_vertices = nullptr; // [numVerts][vertSize] vertSize is gotten from vertexType
		Key& operator=(const Key & keyStruct);
		~Key();
	} *m_keys = nullptr; // Array, obviously
	SharedNode m_inputTime;
	xgShapeInterpolator() = default;
	xgShapeInterpolator(const PString& name) { m_name = name; }
	xgShapeInterpolator(const xgShapeInterpolator& shape);
	~xgShapeInterpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgShapeInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgShapeInterpolator", str.m_pstring) == 0; }
};

struct xgTexCoordInterpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numtimes = 0;
	float* m_times = nullptr;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numVerts = 0;
		float(*m_texcoords)[2] = nullptr; // numVerts
		Key& operator=(const Key&);
		~Key();
	} *m_keys = nullptr; // numkeys; same as numtargets, below?
	unsigned long m_numTargets = 0;
	unsigned long* m_targets = nullptr;
	SharedNode m_inputTime;
	xgTexCoordInterpolator() = default;
	xgTexCoordInterpolator(const PString& name) { m_name = name; }
	xgTexCoordInterpolator(const xgTexCoordInterpolator& tex);
	~xgTexCoordInterpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgTexCoordInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgTexCoordInterpolator", str.m_pstring) == 0; }
};

struct xgTexture : public XGNode
{
	PString m_imxName;
	unsigned long m_mipmap_depth = 0;
	xgTexture() = default;
	xgTexture(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	~xgTexture() {}
	const char* getType() { return "xgTexture"; }
	static bool compare(const PString& str) { return strcmp("xgTexture", str.m_pstring) == 0; }
};

struct xgTime : public XGNode
{
	float m_numFrames = 0;
	float m_time = 0;
	xgTime() = default;
	xgTime(const PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	~xgTime() {}
	const char* getType() { return "xgTime"; }
	static bool compare(const PString& str) { return strcmp("xgTime", str.m_pstring) == 0; }
};

struct xgVec3Interpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numkeys = 0;
	float(*m_keys)[3] = nullptr;
	SharedNode m_inputTime;
	xgVec3Interpolator() = default;
	xgVec3Interpolator(const PString& name) { m_name = name; }
	xgVec3Interpolator(const xgVec3Interpolator& vec3);
	~xgVec3Interpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgVec3Interpolator"; }
	static bool compare(const PString& str) { return strcmp("xgVec3Interpolator", str.m_pstring) == 0; }
};

struct xgVertexInterpolator : public XGNode
{
	unsigned long m_type = 0;
	unsigned long m_numtimes = 0;
	float* m_times = nullptr;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numsize = 0;
		float(*m_nums)[3] = nullptr; // numVerts
		Key& operator=(const Key&);
		~Key();
	} *m_keys = nullptr; // numkeys; same as numtargets, below?
	unsigned long m_numTargets = 0;
	unsigned long* m_targets = nullptr;
	std::vector<SharedNode> m_inputTimes;
	xgVertexInterpolator() = default;
	xgVertexInterpolator(const PString& name) { m_name = name; }
	xgVertexInterpolator(const xgVertexInterpolator& vert);
	~xgVertexInterpolator();
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, const char* tabs = "");
	const char* getType() { return "xgVertexInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgVertexInterpolator", str.m_pstring) == 0; }
};
