//OpenCL Cutting Algorithms
/*
#define cl_amd_fp64
#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#else
    #error "float precision floating point not supported by OpenCL implementation."
#endif
*/

#define EPSILON 1E-5f

//Comfort Types
typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;
typedef			 char		I8;
typedef			 short		I16;
typedef			 int		I32;

//CellParam
typedef struct MeshInfo {
	U32 ctVertices;
	U32 ctTets;
} MeshInfo;

/*
 * Computes the Intersection of a ray and a Triangle
	vec3d e1 = p[1] - p[0];
	vec3d e2 = p[2] - p[0];
	vec3d q = vec3d::cross(rd, e2);

	//Test Determinant
	double a = vec3d::dot(e1, q);
	if(fabs(a) < EPSILON)
		return 0;

	//Test U
	double f = 1.0 / a;
	vec3d s = ro - p[0];
	uvt.x = f * vec3d::dot(s, q);
	if(uvt.x < 0.0)
		return 0;

	//Test V
	vec3d r = vec3d::cross(s, e1);
	uvt.y = f * vec3d::dot(rd, r);
	if((uvt.y < 0.0) || ((uvt.x + uvt.y) > 1.0))
		return 0;

	//Test T
	uvt.z = f * vec3d::dot(e2, r);

	return 1;

 */
int IntersectRayTriangle(float3 ro, float3 rd, float3 p[3], float3* lpOutUVT) {

	*lpOutUVT = (float3)(0.0f, 0.0f, 0.0f);
	float3 e1 = p[1] - p[0];
	float3 e2 = p[2] - p[0];
	float3 q = cross(rd, e2);

	//Print
	//printf((__constant char *)"rd: %.2f, %.2f, %.2f\n", rd.x, rd.y, rd.z);
	//printf((__constant char *)"e1: %.2f, %.2f, %.2f\n", e1.x, e1.y, e1.z);
	//printf((__constant char *)"e2: %.2f, %.2f, %.2f\n", e2.x, e2.y, e2.z);	
	//printf((__constant char *)"q: %.2f, %.2f, %.2f\n", q.x, q.y, q.z);

	//Test Determinant
	float a = dot(e1, q);
	//printf((__constant char *)"RET: Determinant a is %.2f\n", a);	
	if(isless(fabs(a), EPSILON)) {
		return 0;
	}

	//Test U
	float f = 1.0f / a;
	float3 s = ro - p[0];
	float u = f * dot(s, q);
	//printf((__constant char *)"RET: U is %.2f\n", u);		
	if(isless(u, 0.0f)) {
		return 0;
	}

	//Test V
	float3 r = cross(s, e1);
	float v = f * dot(rd, r);
	//printf((__constant char *)"RET: V is %.2f\n", v);		
	if(isless(v, 0.0f) || isgreater(u + v,  1.0f)) {
		return 0;
	}

	//Test T
	float t = f * dot(e2, r);
	//printf((__constant char *)"RET: t is %.2f\n", t);
	*lpOutUVT = (float3)(u, v, t);

	return 1;
}

/*
 * Computes Intersection of a segment and a triangle
	vec3d delta = s1 - s0;
	vec3d rd = delta.normalized();
	double deltaLen = delta.length();
	vec3d uvt;
	int res = IntersectRayTriangle(s0, rd, p, uvt);
	if(res > 0) {
		if(uvt.z >= 0.0 && uvt.z <= deltaLen) {
			//Barycentric coordinate
			uvw = vec3d(uvt.x, uvt.y, 1.0 - uvt.x - uvt.y);

			//Cartesian Coordinate
			xyz = s0 + rd * uvt.z;
			return res;
		}
	}
	return 0;

 */
int IntersectSegmentTriangle(float3 s0, float3 s1, float3 tri[3], float3* lpOutXYZ) {

	//s0.w = 0.0f;
	float3 delta = s1 - s0;
	float deltaLen = length(delta);
	float3 rd = normalize(delta);
	
	//printf((__constant char *)"rd: %.2f, %.2f, %.2f\n", rd.x, rd.y, rd.z);
  
	float3 uvt;
	int res = IntersectRayTriangle(s0, rd, tri, &uvt);
	if(res > 0) {
		//printf((__constant char *)"Ray triangle intersected!\n");
		if(isgreaterequal(uvt.z, 0.0f) && islessequal(uvt.z, deltaLen))
		{
			//Barycentric coordinate
			//uvw = float4(uvt.x, uvt.y, 1.0 - uvt.x - uvt.y);

			//Cartesian Coordinate
			*lpOutXYZ = s0 + rd * uvt.z;
			return res;
		}
	}
	return 0;
}


/*!
 * \brief Computes face intersections for all tetrahedral faces of the mesh.
 * 4 tests per each tetrahedra will be done.
 * @param arrInVertices the vertices of the tetrahedra mesh 
 * @param arrInTets the indices of the tetrahedra mesh
 * 
 */ 
__kernel void ComputePerTetCentroids(__global float4* arrInVertices,
				     __global U32* arrInTets, 
				     __constant struct MeshInfo* minfo,
			             __global U32* arrOutFaceFlags,
			             __global float4* arrOutFaceCentroids)
{
	int idxTet = get_global_id(0);
	if(idxTet >= minfo->ctTets)
		return;

	//Fetch tet indices
	U32 tet[4];
	for(int i=0; i<4; i++)
		tet[i] = arrInTets[idxTet*4 + i];

	//Face Mask
	int faceMask[4][3] = {
	   {0, 1, 2}, {1, 2, 3}, {2, 3, 0}, {0, 1, 3}
	};


	//Init output
	float4 p[3];
	const float oneThird = 1.0f / 3.0f;
	for(int i=0; i<4; i++) {
		U32 idxFP = idxTet * 4 + i;
		
		int tri0 = tet[faceMask[i][0]];
		int tri1 = tet[faceMask[i][1]];
		int tri2 = tet[faceMask[i][2]];
		p[0] = arrInVertices[tri0];
		p[1] = arrInVertices[tri1];
		p[2] = arrInVertices[tri2];

		arrOutFaceFlags[idxFP] = 1;
		arrOutFaceCentroids[idxFP] = oneThird * (p[0] + p[1] + p[2]);
		//arrOutFaceCentroids[idxFP] = p[0];
		arrOutFaceCentroids[idxFP].w = 1.0f;
	}
}


/*!
 * \brief Computes face intersections for all tetrahedral faces of the mesh.
 * 4 tests per each tetrahedra will be done.
 * @param arrInVertices the vertices of the tetrahedra mesh 
 * @param arrInTets the indices of the tetrahedra mesh
 * 
 */ 
__kernel void ComputePerTetFaceIntersections(__global float4* arrInVertices,
				       	     __global U32* arrInTets, 
					     __constant struct MeshInfo* minfo,					     
					     __global float4* inScalpelEdge,					     
					     __global U32* arrOutFaceFlags,
					     __global float4* arrOutFacePoint)
{
	int idxTet = get_global_id(0);
	if(idxTet >= minfo->ctTets)
		return;

	//Fetch tet indices
	U32 tet[4];
	for(int i=0; i<4; i++)
		tet[i] = arrInTets[idxTet*4 + i];

	//Face Mask
	int faceMask[4][3] = {
	   {0, 1, 2}, {1, 2, 3}, {2, 3, 0}, {0, 1, 3}
	};


	//Init output
	float3 p[3];
	float3 xp;
	const float oneThird = 1.0f / 3.0f;
	for(int i=0; i<4; i++) {
		U32 idxFP = idxTet * 4 + i;

		arrOutFaceFlags[idxFP] = 0;
		int tri0 = tet[faceMask[i][0]];
		int tri1 = tet[faceMask[i][1]];
		int tri2 = tet[faceMask[i][2]];
		p[0] = arrInVertices[tri0].xyz;
		p[1] = arrInVertices[tri1].xyz;
		p[2] = arrInVertices[tri2].xyz;

		arrOutFacePoint[idxFP].xyz = oneThird * (p[0] + p[1] + p[2]);
		arrOutFacePoint[idxFP].w = 1.0f;


		int res = IntersectSegmentTriangle(inScalpelEdge[0].xyz, 
						   inScalpelEdge[1].xyz, 
						   p, &xp);
		if(res > 0) {
			arrOutFaceFlags[idxFP] = 1;
			arrOutFacePoint[idxFP].xyz = xp;
		}
	}
}


/*!
 * \brief Computes edge intersections per tetrahedra
 */
__kernel void ComputePerTetEdgeIntersections(__global float4* arrInVertices,
				       	     __global U32* arrInTets, 
					     __global float4 inSweptQuad[4],
					     __constant struct MeshInfo* minfo,
					     __global U32* arrOutEdgeFlags,
					     __global float4* arrOutEdgePoints)
{
	int idxTet = get_global_id(0);
	if(idxTet >= minfo->ctTets)
		return;

	//Fetch tet indices
	U32 tet[4];
	for(int i=0; i<4; i++)
		tet[i] = arrInTets[idxTet*4 + i];

	float3 sweptTri0[3];
	float3 sweptTri1[3];

	sweptTri0[0] = inSweptQuad[0].xyz;
	sweptTri0[1] = inSweptQuad[3].xyz;
	sweptTri0[2] = inSweptQuad[1].xyz;

	sweptTri1[0] = inSweptQuad[0].xyz;
	sweptTri1[1] = inSweptQuad[2].xyz;
	sweptTri1[2] = inSweptQuad[3].xyz;


	int edgeMask[6][2] = {
	   { 0, 1 }, { 1, 2 }, { 2, 0 },
	   { 0, 3 }, { 1, 3 }, { 2, 3 } };

	float3 e[2];
	float3 xp;
	for(int i=0; i<6; i++) {
		U32 idxEP = idxTet * 6 + i;
		arrOutEdgeFlags[idxEP] = 0;
		arrOutEdgePoints[idxEP].w = 1.0f;
		
		int edge0 = tet[edgeMask[i][0]];
		int edge1 = tet[edgeMask[i][1]];
		e[0] = arrInVertices[edge0].xyz;
		e[1] = arrInVertices[edge1].xyz;

		
		if(IntersectSegmentTriangle(e[0], e[1], sweptTri0, &xp) > 0) {
			arrOutEdgeFlags[idxEP] = 1;
			arrOutEdgePoints[idxEP].xyz = xp;		
		} 
		else if(IntersectSegmentTriangle(e[0], e[1], sweptTri1, &xp) > 0) {
			arrOutEdgeFlags[idxEP] = 1;
			arrOutEdgePoints[idxEP].xyz = xp;									
		}
	}
}

/*!
 * \brief Computes the intersection between a line segment and a triangle
 */
__kernel void ComputeSegmentTriIntersections(__global float4* arrInVertices,
					     __global float4 segment[2],
					     __constant struct MeshInfo* minfo,					     					 
					     __global float4* arrOutFacePoint)
{
	int idxTri = get_global_id(0);
	if(idxTri >= minfo->ctTets)
		return;

	float3 xp;
	float3 p[3];
	
	p[0] = arrInVertices[idxTri*3].xyz;
	p[1] = arrInVertices[idxTri*3 + 1].xyz;
	p[2] = arrInVertices[idxTri*3 + 2].xyz;
	arrOutFacePoint[idxTri] = (float4)(-1.0f, -1.0f, -1.0f, 1.0f);
	
	if(IntersectSegmentTriangle(segment[0].xyz, segment[1].xyz, p, &xp) > 0) {
		arrOutFacePoint[idxTri].xyz = xp;
	}
}
