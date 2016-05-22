#include "meshPlane.h"
#include <fstream>
#include <cmath>
#include <assert.h>

const DWORD MeshPlane::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

MeshPlane::MeshPlane(IDirect3DDevice9* device,
				 const char* heightmapFileName,
				 int numVertsPerRow,
				 int numVertsPerCol,
				 int cellSpacing,
				 float heightScale,
				 int meshType,
				 Ocean::OceanPara oceanPara)
{
	_device         = device;
	_meshType		= meshType;
	_heightScale	= heightScale;
	_cellSpacing    = cellSpacing;

	_numVertsPerRow = numVertsPerRow;
	_numVertsPerCol = numVertsPerCol;
	
	if (meshType == OCEAN) {
		assert(_numVertsPerCol == _numVertsPerRow);
		assert((_numVertsPerRow & (_numVertsPerRow - 1)) == 0);
	}

	_numCellsPerRow = _numVertsPerRow - 1;
	_numCellsPerCol = _numVertsPerCol - 1;

	_width = _numCellsPerRow * _cellSpacing;
	_depth = _numCellsPerCol * _cellSpacing;

	_numVertices  = _numVertsPerRow * _numVertsPerCol;
	_numTriangles = _numCellsPerRow * _numCellsPerCol * 2;




	HRESULT hr = _device->CreateVertexBuffer(_numVertices * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &_vb, 0);
	if (FAILED(hr))  PostQuitMessage(0);

	if (meshType == TERRAIN) {
		_heightmap.resize(_numVertices);			// if meshType is OCEAN, _heightmap is not used(size 0)

		if (!heightmapFileName)
			for (int i = 0; i < _numVertices; i++)
				_heightmap[i] = 0;					// height default to 0

		// load heightmap
		else if (!readRawFile(heightmapFileName)) {
			MessageBox(0, "readRawFile - FAILED", 0, 0);
			PostQuitMessage(0);
		}

		// scale heights
		for (int i = 0; i < _heightmap.size(); i++)
			_heightmap[i] *= heightScale;

		// compute the vertices
		if (!computeVertices())
		{
			::MessageBox(0, "computeVertices - FAILED", 0, 0);
			::PostQuitMessage(0);
		}
	}

	else if (meshType == OCEAN) {
#ifdef DLENTHFIXED
		_oceanMesh = new Ocean(numVertsPerRow, oceanPara.A, oceanPara.w, DLENTHFIXED);
#else
		_oceanMesh = new Ocean(numVertsPerRow, oceanPara.A, oceanPara.w, _width);
#endif
	}

	else {
		MessageBox(0, "Unknow mesh type!", 0, 0);
		PostQuitMessage(0);
	}

	// compute the indices
	if( !computeIndices() ) {
		MessageBox(0, "computeIndices - FAILED", 0, 0);
		PostQuitMessage(0);
	}
}

MeshPlane::~MeshPlane()
{
	d3d::Release<IDirect3DVertexBuffer9*>(_vb);
	d3d::Release<IDirect3DIndexBuffer9*>(_ib);
	d3d::Delete<Ocean*>(_oceanMesh);
}

bool MeshPlane::computeVertices()
{
	// coordinates to start generating vertices at
	int startX = -_width / 2;
	int startZ =  _depth / 2;

	// coordinates to end generating vertices at
	int endX =  _width / 2;
	int endZ = -_depth / 2;

	Vertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int i = 0;
	for(int z = startZ; z >= endZ; z -= _cellSpacing)
	{
		int j = 0;
		for(int x = startX; x <= endX; x += _cellSpacing)
		{
			// compute the correct index into the vertex buffer and heightmap
			// based on where we are in the nested loop.
			int index = i * _numVertsPerRow + j;

			v[index] = Vertex(
				(float)x,
				(float)_heightmap[index],
				(float)z,
				0, 0, 0);

			j++; // next column
		}
		i++; // next row
	}

	_vb->Unlock();

	return true;
}

bool MeshPlane::computeIndices()
{
	HRESULT hr = 0;

	hr = _device->CreateIndexBuffer(
		_numTriangles * 3 * sizeof(WORD), // 3 indices per triangle
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib,
		0);

	if(FAILED(hr))
		return false;

	WORD* indices = 0;
	_ib->Lock(0, 0, (void**)&indices, 0);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0;

	// loop through and compute the triangles of each quad
	for(int i = 0; i < _numCellsPerCol; i++)
	{
		for(int j = 0; j < _numCellsPerRow; j++)
		{
			indices[baseIndex]     =   i   * _numVertsPerRow + j;
			indices[baseIndex + 1] =   i   * _numVertsPerRow + j + 1;
			indices[baseIndex + 2] = (i+1) * _numVertsPerRow + j;

			indices[baseIndex + 3] = (i+1) * _numVertsPerRow + j;
			indices[baseIndex + 4] =   i   * _numVertsPerRow + j + 1;
			indices[baseIndex + 5] = (i+1) * _numVertsPerRow + j + 1;

			// next quad
			baseIndex += 6;
		}
	}

	_ib->Unlock();

	return true;
}

bool MeshPlane::readRawFile(const char* fileName)	// RAW file dimensions must be >= to the dimensions of the terrain.
{
	// A height for each vertex
	std::vector<BYTE> in( _numVertices );

	std::ifstream inFile(fileName, std::ios_base::binary);
	if( !inFile.is_open() )	return false;

	inFile.read( (char*)&in[0], in.size() );
	inFile.close();

	// copy BYTE vector to int vector
	for(int i = 0; i < in.size(); i++)
		_heightmap[i] = in[i];

	return true;
}


void MeshPlane::updateVertices(float t)
{
#if defined(EVAL_CPUFFT)
	_oceanMesh->evalWavesCPUFFT(t);
	//_oceanMesh->evaluateWavesFFT(t);
#elif defined(EVAL_GPUFFT)
	_oceanMesh->evalWavesGPUFFT(t);
#endif

#ifdef DEBUGHTILDE0
	_oceanMesh->dispHeightToFile();
#endif

#ifdef DEBUGH
	FILE* f = fopen(HEIGHT_FILE, "w");
	Vertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int nVerts = _oceanMesh->N;
	for (int i = 0; i < nVerts; i++) {
		for (int j = 0; j < nVerts; j++) {	
			int index = i * nVerts + j;
			Ocean::Vertex vto = _oceanMesh->vertices[index];
			float hh = vto.y * _heightScale;
			v[index] = Vertex(vto.x, hh, vto.z, d3d::WHITE);
			fprintf(f, "%f ", hh);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
#else
	Vertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int nVerts = _oceanMesh->N;
	for (int i = 0; i < nVerts; i++) {
		for (int j = 0; j < nVerts; j++) {
			int index = i * nVerts + j;
			Ocean::Vertex vto = _oceanMesh->vertices[index];
			v[index] = Vertex(vto.x, vto.y * HEIGHTSCALE, vto.z, vto.nx, vto.ny, vto.nz);
		}
	}
}
#endif


bool MeshPlane::draw(D3DXMATRIX* world, float t)
{
	HRESULT hr = 0;

	if( _device )
	{
		_device->SetTransform(D3DTS_WORLD, world);

		_device->SetStreamSource(0, _vb, 0, sizeof(Vertex));
		_device->SetFVF(Vertex::FVF);
		_device->SetIndices(_ib);
	
		
		if (_meshType == OCEAN) {
#ifdef DSTATIC
			updateVertices(1.0f);
#else
			updateVertices(t);
#endif
		}

		hr =_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			_numVertices,
			0,
			_numTriangles);
		

		if(FAILED(hr))
			return false;
	}

	return true;
}

