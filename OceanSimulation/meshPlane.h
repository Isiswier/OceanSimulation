#ifndef _MESH_PLANE_H
#define _MESH_PLANE_H

#include "d3dUtility.h"
#include "Ocean.h"
#include <vector>

class MeshPlane
{
public:
	MeshPlane(
		IDirect3DDevice9* device,
		const char* heightmapFileName,
		int numVertsPerRow,
		int numVertsPerCol,
		int cellSpacing,
		float heightScale,
		int meshType,
		Ocean::OceanPara oceanPara);

	~MeshPlane();

	bool  draw(D3DXMATRIX* world, float t);

	enum MeshType{ TERRAIN, OCEAN };

private:
	IDirect3DDevice9*       _device;
	IDirect3DTexture9*      _tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;

	int   _numVertsPerRow;
	int   _numVertsPerCol;
	int   _cellSpacing;
	int	  _numCellsPerRow;
	int	  _numCellsPerCol;
	int	  _width;
	int	  _depth;
	int	  _numVertices;
	int	  _numTriangles;
	int   _meshType;
	float _heightScale;

	Ocean*			   _oceanMesh;
	std::vector<float> _heightmap;

	// helper methods
	bool  readRawFile(const char* fileName);
	bool  computeVertices();
	bool  computeIndices();
	void  updateVertices(float t);

	//struct Vertex
	//{
	//	Vertex(){}

	//	Vertex(float x, float y, float z, D3DCOLOR c) {
	//		_x = x; _y = y; _z = z;
	//		color = c;
	//	}

	//	float _x, _y, _z;
	//	D3DCOLOR color;
	//	static const DWORD FVF;
	//};

	struct Vertex
	{
		Vertex(){}

		Vertex(float x, float y, float z, float nx, float ny, float nz) {
			_x = x; _y = y; _z = z;
			_nx = nx; _ny = ny; _nz = nz;
		}

		float _x, _y, _z;
		float _nx, _ny, _nz;
		static const DWORD FVF;
	};
};

#endif
