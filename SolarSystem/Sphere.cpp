#include "pch.h"

// 최소한의 위도, 경도 수 초기화
const int MIN_LONG_COUNT = 3;
const int MIN_LAT_COUNT = 2;

Sphere::Sphere(float radius, int longs, int lats, bool smooth) : interleavedStride(32) {
	set(radius, longs, lats, smooth);
}

void Sphere::set(float radius, int longs, int lats, bool smooth) {
	this->radius = radius;
	this->longCount = longs;
	if (longs < MIN_LONG_COUNT)
		this->longCount = MIN_LONG_COUNT;
	this->latCount = lats;
	if (lats < MIN_LAT_COUNT)
		this->latCount = MIN_LAT_COUNT;
	this->smooth = smooth;

	if (smooth)
		buildVerticesSmooth();
	else
		buildVerticesFlat();
}

void Sphere::setRadius(float radius) {
	if (radius != this->radius)
		set(radius, longCount, latCount, smooth);
}

void Sphere::setLongCount(int longs) {
	if (longs != this->longCount)
		set(radius, longs, latCount, smooth);
}

void Sphere::setLatCount(int lats) {
	if (lats != this->latCount)
		set(radius, longCount, lats, smooth);
}

void Sphere::setSmooth(bool smooth)
{
	if (this->smooth == smooth)
		return;

	this->smooth = smooth;
	if (smooth)
		buildVerticesSmooth();
	else
		buildVerticesFlat();
}


void Sphere::clearArrays() {
	std::vector<float>().swap(vertices);
	std::vector<float>().swap(normals);
	std::vector<float>().swap(texCoords);
	std::vector<unsigned int>().swap(indices);
	std::vector<unsigned int>().swap(lineIndices);
}

void Sphere::buildVerticesSmooth() {
	const float m_PI = acos(-1.0f);

	clearArrays();

	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / radius;
	float s, t;

	float longStep = 2 * m_PI / longCount;
	float latStep = m_PI / latCount;
	float longAngle, latAngle;

	for (int i = 0; i <= latCount; ++i) {
		latAngle = m_PI / 2 - i * latStep;
		xy = radius * cosf(latAngle);
		z = radius * sinf(latAngle);

		for (int j = 0; j <= longCount; ++j) {
			longAngle = j * longStep;

			x = xy * cosf(longAngle);
			y = xy * sinf(longAngle);
			addVertex(x, y, z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			addNormal(nx, ny, nz);

			s = (float)j / longCount;
			t = (float)i / latCount;
			addTexCoord(s, t);
		}
	}

	unsigned int v1, v2;
	for (int i = 0; i < latCount; ++i) {
		v1 = i * (longCount + 1);
		v2 = v1 + longCount + 1;

		for (int j = 0; j < longCount; ++j, ++v1, ++v2) {
			if (i != 0)
				addIndices(v1, v2, v1 + 1);

			if (i != (latCount - 1))
				addIndices(v1 + 1, v2, v2 + 1);

			lineIndices.push_back(v1);
			lineIndices.push_back(v2);
			if (i != 0) {
				lineIndices.push_back(v1);
				lineIndices.push_back(v1 + 1);
			}
		}
	}

	buildInterleavedVertices();
}

void Sphere::buildVerticesFlat() {
	const float m_PI = acos(-1.0f);

	struct Vertex {
		float x, y, z, s, t;
	};

	std::vector<Vertex> tmpVertices;

	float longStep = 2 * m_PI / longCount;
	float latStep = PI / latCount;
	float longAngle, latAngle;

	for (int i = 0; i < latCount; ++i) {
		latAngle = PI / 2 - i * latStep;
		float xy = radius * cosf(latAngle);
		float z = radius * sinf(latAngle);

		for (int j = 0; j <= longCount; ++j) {
			longAngle = j * longStep;

			Vertex vertex;
			vertex.x = xy * cosf(longAngle);
			vertex.y = xy * sinf(longAngle);
			vertex.z = z;
			vertex.s = (float)j / longCount;
			vertex.t = (float)i / latCount;
			tmpVertices.push_back(vertex);
		}
	}

	clearArrays();

	Vertex v1, v2, v3, v4;
	std::vector<float> n;

	int i, j, k, vi1, vi2;
	int index = 0;
	for (i = 0; i < latCount; i++) {
		vi1 = i * (longCount + 1);
		vi2 = (i + 1) * (longCount + 1);

		for (j = 0; j < longCount; ++j, ++vi1, ++vi2) {
			v1 = tmpVertices[vi1];
			v2 = tmpVertices[vi2];
			v3 = tmpVertices[vi1 + 1];
			v4 = tmpVertices[vi2 + 1];

			if (i == 0)
			{
				addVertex(v1.x, v1.y, v1.z);
				addVertex(v2.x, v2.y, v2.z);
				addVertex(v4.x, v4.y, v4.z);

				addTexCoord(v1.s, v1.t);
				addTexCoord(v2.s, v2.t);
				addTexCoord(v4.s, v4.t);

				n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v4.x, v4.y, v4.z);
				for (k = 0; k < 3; ++k)
				{
					addNormal(n[0], n[1], n[2]);
				}

				addIndices(index, index + 1, index + 2);

				lineIndices.push_back(index);
				lineIndices.push_back(index + 1);

				index += 3;
			}
			else if (i == (latCount - 1))
			{
				addVertex(v1.x, v1.y, v1.z);
				addVertex(v2.x, v2.y, v2.z);
				addVertex(v3.x, v3.y, v3.z);

				addTexCoord(v1.s, v1.t);
				addTexCoord(v2.s, v2.t);
				addTexCoord(v3.s, v3.t);

				n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
				for (k = 0; k < 3; ++k)
				{
					addNormal(n[0], n[1], n[2]);
				}
				addIndices(index, index + 1, index + 2);

				lineIndices.push_back(index);
				lineIndices.push_back(index + 1);
				lineIndices.push_back(index);
				lineIndices.push_back(index + 2);

				index += 3;
			}
			else
			{
				addVertex(v1.x, v1.y, v1.z);
				addVertex(v2.x, v2.y, v2.z);
				addVertex(v3.x, v3.y, v3.z);
				addVertex(v4.x, v4.y, v4.z);

				addTexCoord(v1.s, v1.t);
				addTexCoord(v2.s, v2.t);
				addTexCoord(v3.s, v3.t);
				addTexCoord(v4.s, v4.t);

				n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
				for (k = 0; k < 4; ++k)
				{
					addNormal(n[0], n[1], n[2]);
				}

				addIndices(index, index + 1, index + 2);
				addIndices(index + 2, index + 1, index + 3);

				lineIndices.push_back(index);
				lineIndices.push_back(index + 1);
				lineIndices.push_back(index);
				lineIndices.push_back(index + 2);

				index += 4;
			}
		}
	}

	buildInterleavedVertices();
}

void Sphere::buildInterleavedVertices()
{
	std::vector<float>().swap(interleavedVertices);

	std::size_t i, j;
	std::size_t count = vertices.size();
	for (i = 0, j = 0; i < count; i += 3, j += 2)
	{
		interleavedVertices.push_back(vertices[i]);
		interleavedVertices.push_back(vertices[i + 1]);
		interleavedVertices.push_back(vertices[i + 2]);

		interleavedVertices.push_back(normals[i]);
		interleavedVertices.push_back(normals[i + 1]);
		interleavedVertices.push_back(normals[i + 2]);

		interleavedVertices.push_back(texCoords[j]);
		interleavedVertices.push_back(texCoords[j + 1]);
	}
}

void Sphere::addVertex(float x, float y, float z)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}

void Sphere::addNormal(float nx, float ny, float nz)
{
	normals.push_back(nx);
	normals.push_back(ny);
	normals.push_back(nz);
}

void Sphere::addTexCoord(float s, float t)
{
	texCoords.push_back(s);
	texCoords.push_back(t);
}

void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

std::vector<float> Sphere::computeFaceNormal(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3)
{
	const float EPSILON = 0.000001f;

	std::vector<float> normal(3, 0.0f);
	float nx, ny, nz;

	float ex1 = x2 - x1;
	float ey1 = y2 - y1;
	float ez1 = z2 - z1;
	float ex2 = x3 - x1;
	float ey2 = y3 - y1;
	float ez2 = z3 - z1;

	nx = ey1 * ez2 - ez1 * ey2;
	ny = ez1 * ex2 - ex1 * ez2;
	nz = ex1 * ey2 - ey1 * ex2;

	float length = sqrtf(nx * nx + ny * ny + nz * nz);
	if (length > EPSILON)
	{
		float lengthInv = 1.0f / length;
		normal[0] = nx * lengthInv;
		normal[1] = ny * lengthInv;
		normal[2] = nz * lengthInv;
	}

	return normal;
}

void Sphere::draw(const float lineColor[4]) const
{
	// interleaved array
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, interleavedStride, &interleavedVertices[0]);
	glNormalPointer(GL_FLOAT, interleavedStride, &interleavedVertices[3]);
	glTexCoordPointer(2, GL_FLOAT, interleavedStride, &interleavedVertices[6]);

	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
