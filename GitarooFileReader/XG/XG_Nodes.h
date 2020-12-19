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
	virtual void writeTXT(FILE* outTXT, bool fromXgm) = 0;
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
	struct Vertices
	{
		unsigned long m_vertexFlags = 0;
		unsigned long m_numVerts = 0;
		float** m_vertices = nullptr;
		Vertices() = default;
		Vertices(const Vertices& verts);
		~Vertices();
	} m_vertexData;
	std::vector<SharedNode> m_inputGeometryNames;
	xgBgGeometry() = default;
	xgBgGeometry(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
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
	xgBgMatrix(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgBgMatrix() {}
	const char* getType() { return "xgBgMatrix"; }
	static bool compare(const PString& str) { return strcmp("xgBgMatrix", str.m_pstring) == 0; }
};

struct xgBone : public XGNode
{
	float m_restMatrix[16] = { 0 };
	SharedNode m_inputMatrix;
	xgBone() = default;
	xgBone(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
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
	std::vector<SharedNode> m_inputGeometry;
	std::vector<SharedNode> m_inputMaterial;
	xgDagMesh() = default;
	xgDagMesh(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgDagMesh"; }
	static bool compare(const PString& str) { return strcmp("xgDagMesh", str.m_pstring) == 0; }
};

struct xgDagTransform : public XGNode
{
	std::vector<SharedNode> m_inputMatrix;
	xgDagTransform() = default;
	xgDagTransform(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgDagTransform"; }
	static bool compare(const PString& str) { return strcmp("xgDagTransform", str.m_pstring) == 0; }
};

struct xgEnvelope : public XGNode
{
	unsigned long m_startVertex = 0;
	struct Key
	{
		unsigned long m_numweights = 0;
		float(*m_weights)[4] = nullptr;
		Key() = default;
		Key(const Key& key);
		~Key();
	} m_key;
	struct Targets
	{
		unsigned long m_numTargets = 0;
		long* m_vertexTargets = nullptr;
		Targets() = default;
		Targets(const Targets& targets);
		~Targets();
	} m_vertices;
	std::vector<SharedNode> m_inputMatrix1;
	std::vector<SharedNode> m_inputGeometry;
	xgEnvelope() = default;
	xgEnvelope(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
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
	std::vector<SharedNode> m_inputTexture;
	xgMaterial() = default;
	xgMaterial(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgMaterial"; }
	static bool compare(const PString& str) { return strcmp("xgMaterial", str.m_pstring) == 0; }
};

struct xgMultiPassMaterial : public XGNode
{
	std::vector<SharedNode> m_inputMaterial;
	xgMultiPassMaterial() = default;
	xgMultiPassMaterial(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgMultiPassMaterial"; }
	static bool compare(const PString& str) { return strcmp("xgMultiPassMaterial", str.m_pstring) == 0; }
};

struct xgNormalInterpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Time
	{
		unsigned long m_numtimes = 0;
		float* m_times = nullptr;
		Time() = default;
		Time(const Time& key);
		~Time();
	} m_times;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numNormals = 0; //equal to number of targets, below
		float(*m_normals)[3] = nullptr; // numNormals
		Key& operator=(const Key& key);
		~Key();
	} *m_keys = nullptr; // Array, obviously
	struct Targets
	{
		unsigned long m_numTargets = 0;
		unsigned long* m_targets = nullptr;
		Targets() = default;
		Targets(const Targets& targets);
		~Targets();
	} m_targets;
	SharedNode m_inputTime;
	xgNormalInterpolator() = default;
	xgNormalInterpolator(PString& name) { m_name = name; }
	xgNormalInterpolator(xgNormalInterpolator& norm);
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgNormalInterpolator();
	const char* getType() { return "xgNormalInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgNormalInterpolator", str.m_pstring) == 0; }
};

struct xgQuatInterpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Key
	{
		unsigned long m_numkeys = 0;
		float(*m_keys)[4] = nullptr;
		Key() = default;
		Key(const Key& keyStruct);
		~Key();
	} m_keyStruct;
	SharedNode m_inputTime;
	xgQuatInterpolator() = default;
	xgQuatInterpolator(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgQuatInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgQuatInterpolator", str.m_pstring) == 0; }
};

struct xgShapeInterpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Time
	{
		unsigned long m_numtimes = 0;
		float* m_times = nullptr;
		Time() = default;
		Time(const Time& key);
		~Time();
	} m_times;
	unsigned long m_numkeys = 0; //same as number of times?
	struct Key
	{
		unsigned long m_vertexType = 0;
		unsigned long m_numVerts = 0;
		float** m_vertices = nullptr; // [numVerts][vertSize] vertSize is gotten from vertexType
		Key& operator=(const Key & keyStruct);
		~Key();
	} *m_keys = nullptr; // numKeys
	SharedNode m_inputTime;
	xgShapeInterpolator() = default;
	xgShapeInterpolator(PString& name) { m_name = name; }
	xgShapeInterpolator(xgShapeInterpolator& shape);
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgShapeInterpolator();
	const char* getType() { return "xgShapeInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgShapeInterpolator", str.m_pstring) == 0; }
};

struct xgTexCoordInterpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Time
	{
		unsigned long m_numtimes = 0;
		float* m_times = nullptr;
		Time() = default;
		Time(const Time& key);
		~Time();
	} m_times;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numVerts = 0;
		float(*m_texcoords)[2] = nullptr; // numVerts
		Key& operator=(const Key&);
		~Key();
	} *m_keys = nullptr; // numkeys; same as numtargets, below?
	struct Targets
	{
		unsigned long m_numTargets = 0;
		unsigned long* m_targets = nullptr;
		Targets() = default;
		Targets(const Targets& targets);
		~Targets();
	} m_targets;
	SharedNode m_inputTime;
	xgTexCoordInterpolator() = default;
	xgTexCoordInterpolator(PString& name) { m_name = name; }
	xgTexCoordInterpolator(xgTexCoordInterpolator& tex);
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgTexCoordInterpolator();
	const char* getType() { return "xgTexCoordInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgTexCoordInterpolator", str.m_pstring) == 0; }
};

struct xgTexture : public XGNode
{
	PString m_imxName;
	unsigned long m_mipmap_depth = 0;
	xgTexture() = default;
	xgTexture(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgTexture() {}
	const char* getType() { return "xgTexture"; }
	static bool compare(const PString& str) { return strcmp("xgTexture", str.m_pstring) == 0; }
};

struct xgTime : public XGNode
{
	float m_numFrames = 0;
	float m_time = 0;
	xgTime() = default;
	xgTime(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgTime() {}
	const char* getType() { return "xgTime"; }
	static bool compare(const PString& str) { return strcmp("xgTime", str.m_pstring) == 0; }
};

struct xgVec3Interpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Key
	{
		unsigned long m_numkeys = 0;
		float(*m_keys)[3] = nullptr;
		Key() = default;
		Key(const Key& keyStruct);
		~Key();
	} m_keyStruct;
	SharedNode m_inputTime;
	xgVec3Interpolator() = default;
	xgVec3Interpolator(PString& name) { m_name = name; }
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	const char* getType() { return "xgVec3Interpolator"; }
	static bool compare(const PString& str) { return strcmp("xgVec3Interpolator", str.m_pstring) == 0; }
};

struct xgVertexInterpolator : public XGNode
{
	unsigned long m_type = 0;
	struct Time
	{
		unsigned long m_numtimes = 0;
		float* m_times = nullptr;
		Time() = default;
		Time(const Time& key);
		~Time();
	} m_times;
	unsigned long m_numkeys = 0;
	struct Key
	{
		unsigned long m_numsize = 0;
		float(*m_nums)[3] = nullptr; // numVerts
		Key& operator=(const Key&);
		~Key();
	} *m_keys = nullptr; // numkeys; same as numtargets, below?
	struct Targets
	{
		unsigned long m_numTargets = 0;
		unsigned long* m_targets = nullptr;
		Targets() = default;
		Targets(const Targets& targets);
		~Targets();
	} m_targets;
	std::vector<SharedNode> m_inputTimes;
	xgVertexInterpolator() = default;
	xgVertexInterpolator(PString& name) { m_name = name; }
	xgVertexInterpolator(xgVertexInterpolator& vert);
	void read(FILE* inFile, const std::vector<std::shared_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full);
	void writeTXT(FILE* outTXT, bool fromXgm);
	~xgVertexInterpolator();
	const char* getType() { return "xgVertexInterpolator"; }
	static bool compare(const PString& str) { return strcmp("xgVertexInterpolator", str.m_pstring) == 0; }
};
