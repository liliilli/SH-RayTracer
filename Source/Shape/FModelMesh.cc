
#include <Shape/FModelMesh.hpp>
#include <Manager/MModel.hpp>
#include <Resource/DModelMesh.hpp>
#include <Shape/FPlane.hpp>
#include <Math/Utility/XShapeMath.h>

namespace 
{

std::optional<std::pair<ray::TReal, const ray::DModelFace*>> RayIntersectTriangle(
  const ray::DRay& localRay,
  const ray::DModelFace& face) 
{
  using dy::math::Cross;
  using dy::math::Dot;
  using dy::math::IsNearlyZero;
  using ray::TReal;
  using ray::DVec3;
  constexpr ray::TReal e = ray::TReal(1e-5);

  const DVec3 edge1 = *face.mVertex[1] - *face.mVertex[0];
  const DVec3 edge2 = *face.mVertex[2] - *face.mVertex[0];

  const DVec3 h = Cross(localRay.GetDirection(), edge2);
  const auto a = Dot(edge1, h);

  // If ray is parallel, just do next triangle.
  if (IsNearlyZero(a) == true) { return std::nullopt; }

  const TReal f = 1 / a;
  const DVec3 s = localRay.GetOrigin() - *face.mVertex[0];
  const TReal u = f * Dot(s, h);
  if (u < 0 || u > 1) { return std::nullopt; }

  const DVec3 q = Cross(s, edge1);
  const TReal v = f * Dot(localRay.GetDirection(), q);
  if (v < 0 || u + v > 1) { return std::nullopt; }

  // We can find `t` to find out where the intersection point is on the line.
  const TReal t = f * Dot(edge2, q);
  if (t < 0) { return std::nullopt; }

  return std::pair{t, &face};
}

std::vector<TriangleResult> RayIntersectTriangle(
  const ray::DRay& localRay,
  const std::vector<ray::DModelFace>& faces)
{
  using dy::math::Cross;
  using dy::math::Dot;
  using dy::math::IsNearlyZero;
  using ray::TReal;
  using ray::DVec3;
  constexpr ray::TReal e = ray::TReal(1e-5);

  std::vector<TriangleResult> result;
  for (const auto& face : faces)
  {
    //if (::dy::math::IsRayIntersected(localRay, face.mTriangleAABB) == false) { continue; }

    const auto optResult = RayIntersectTriangle(localRay, face);
    if (optResult.has_value() == false) { continue; }

    const auto& [t, pFace] = *optResult;
    TriangleResult item;
    item.mT = t;
    item.mIndex = {pFace->mIndex[0], pFace->mIndex[1], pFace->mIndex[2]};
    result.emplace_back(std::move(item));
  }

  return result;
}

std::vector<TriangleResult> RayIntersectTriangle(
  const ray::DRay& localRay, 
  const std::vector<ray::DModelIndex>& indices,
  const ray::DModelBuffer::TPointVertices& vertices)
{
  using dy::math::Cross;
  using dy::math::Dot;
  using dy::math::IsNearlyZero;
  using ray::TReal;
  using ray::DVec3;
  constexpr ray::TReal e = ray::TReal(1e-5);

  std::vector<TriangleResult> result;
  for (ray::TIndex i = 0, size = indices.size(); i < size; i += 3)
  {
    const DVec3& p0 = vertices[ indices[i+0].mVertexIndex ];
    const DVec3& p1 = vertices[ indices[i+1].mVertexIndex ];
    const DVec3& p2 = vertices[ indices[i+2].mVertexIndex ];

    const DVec3 edge1 = p1 - p0;
    const DVec3 edge2 = p2 - p0;

    const DVec3 h = Cross(localRay.GetDirection(), edge2);
    const auto a = Dot(edge1, h);

    // If ray is parallel, just do next triangle.
    if (IsNearlyZero(a) == true) { continue; }

    const TReal f = 1 / a;
    const DVec3 s = localRay.GetOrigin() - p0;
    const TReal u = f * Dot(s, h);
    if (u < 0 || u > 1) { continue; }

    const DVec3 q = Cross(s, edge1);
    const TReal v = f * Dot(localRay.GetDirection(), q);
    if (v < 0 || u + v > 1) { continue; }

    // We can find `t` to find out where the intersection point is on the line.
    const TReal t = f * Dot(edge2, q);
    if (t < 0) { continue; }

    TriangleResult item;
    item.mT = t;
    item.mIndex = {i, i+1, i+2};
    result.emplace_back(item);
  }

  return result;
}

} /// ::unnamed namespace

namespace ray
{

ray::FModelMesh::FModelMesh(const PCtor& ctor, IMaterial* mat)
  : IHitable { EShapeType::ModelMesh, mat },
    mOrigin { ctor.mOrigin },
    mScale { ctor.mScale },
    mRotQuat { ctor.mAngle },
    mMeshId { ctor.mMeshId }
{
  assert(mMeshId.HasId() == true);
  this->mpMesh          = EXPR_SGT(MModel).GetMesh(this->mMeshId);
  this->mModelBufferId  = this->mpMesh->GetModelBufferId();
  this->mpModelBuffer   = EXPR_SGT(MModel).GetModelBuffer(this->mModelBufferId);

  // Make local-space AABB from mpModelBuffer.
  const auto& indices   = this->mpMesh->GetIndices();
  const auto& vertices  = this->mpModelBuffer->GetVertices();

  // First, get first triangle and make basis meshLocalAABB.
  const DVec3& tp0 = vertices[ indices[0].mVertexIndex ];
  const DVec3& tp1 = vertices[ indices[1].mVertexIndex ];
  const DVec3& tp2 = vertices[ indices[2].mVertexIndex ];
  DAABB meshLocalAABB = {{tp0, tp1, tp2}};

  for (TIndex i = 3, size = indices.size(); i < size; i += 3)
  {
    const DVec3& p0 = vertices[ indices[i+0].mVertexIndex ];
    const DVec3& p1 = vertices[ indices[i+1].mVertexIndex ];
    const DVec3& p2 = vertices[ indices[i+2].mVertexIndex ];

    const DAABB triangleAABB = {{p0, p1, p2}};
    meshLocalAABB = ::dy::math::GetUnionOf(meshLocalAABB, triangleAABB);
  }

  // Scale, Rotate with this->mRotQuat and offset with this->mOrigin.
  meshLocalAABB = {
    meshLocalAABB.GetMaximumPoint() * this->mScale,
    meshLocalAABB.GetMinimumPoint() * this->mScale
  };
  meshLocalAABB = this->mRotQuat * meshLocalAABB;
  this->mAABB = std::make_unique<DAABB>(::dy::math::GetMovedOf(meshLocalAABB, this->mOrigin));
}

std::optional<IHitable::TValueResults> FModelMesh::GetRayIntersectedTValues(const DRay& ray) const
{
  // Check AABB.
  if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }

  // Convert world-space ray into local space.
  const auto matWorldToLocal = this->mRotQuat.ToMatrix3().Transpose();
  const auto offsetedRay = DRay
  {
    (matWorldToLocal * (ray.GetOrigin() - this->mOrigin)) * this->mScale,
    matWorldToLocal * ray.GetDirection()
  };

  // Use Möller–Trumbore intersection algorithm
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
  const auto tResults = RayIntersectTriangle(offsetedRay, this->mpMesh->GetFaces());
  if (tResults.empty() == true)
  {
    return std::nullopt;
  }

  IHitable::TValueResults results;
  results.reserve(tResults.size());
  for (const auto& [t, _] : tResults) { results.emplace_back(t, EShapeType::ModelMesh, this); }
  return results;
}

std::optional<IHitable::TValueResults> FModelMesh::GetRayIntserectedTValues2(const DRay& ray)
{
  // Check AABB.
  if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }
  
   // Convert world-space ray into local space.
  const auto matWorldToLocal = this->mRotQuat.ToMatrix3().Transpose();
  const auto offsetedRay = DRay
  {
    (matWorldToLocal * (ray.GetOrigin() - this->mOrigin)) / this->mScale,
    matWorldToLocal * ray.GetDirection()
  };

  // Get T value (temporary) [#6? Need to be refactored more clean way.]
  const auto& header = this->mpMesh->GetTreeHeader();
  const auto tResults = header.TempGetTValues(offsetedRay);
  if (tResults.empty() == true)
  {
    return std::nullopt;
  }

  IHitable::TValueResults results;
  results.reserve(tResults.size());
  for (const auto& [t, _] : tResults) { results.emplace_back(t * this->mScale, EShapeType::ModelMesh, this); }
  return results;
}

std::optional<PScatterResult> FModelMesh::TryScatter(const DRay& ray, TReal t) const
{
  if (this->GetMaterial() == nullptr) { return std::nullopt; }

  // Convert world-space ray into local space.
  const auto matLocalToWorld = this->mRotQuat.ToMatrix3();
  const auto matWorldToLocal = matLocalToWorld.Transpose();
  const auto offsetedRay = DRay
  {
    (matWorldToLocal * (ray.GetOrigin() - this->mOrigin)) / this->mScale,
    matWorldToLocal * ray.GetDirection()
  };

  // Get result (bruteforce temporary)
  const auto& indices   = this->mpMesh->GetIndices();
  //const auto& vertices  = this->mpModelBuffer->GetVertices();
  const auto& normals   = this->mpModelBuffer->GetNormals();

  auto tResults = this->mpMesh->GetTreeHeader().TempGetTValues(offsetedRay);
  //auto tResults = RayIntersectTriangle(offsetedRay, this->mpMesh->GetFaces());
  assert(tResults.empty() == false);
  std::sort(
    tResults.begin(), tResults.end(), 
    [](const TriangleResult& lhs, const TriangleResult& rhs) { return lhs.mT < rhs.mT; });

  const auto nextRay = DRay{ray.GetPointAtParam(t), ray.GetDirection()};
  for (auto& [nT, nIds] : tResults)
  {
    if (t != nT * this->mScale) { continue; }
    // Get normal.
    const DVec3& n0 = normals[ indices[nIds[0]].mNormalIndex ];
    const DVec3& n1 = normals[ indices[nIds[1]].mNormalIndex ];
    const DVec3& n2 = normals[ indices[nIds[2]].mNormalIndex ];

    const auto normal = matLocalToWorld * ((n0 + n1 + n2) / 3);
    const auto optResult = this->GetMaterial()->Scatter(nextRay, normal);
    const auto& [refDir, attCol, isScattered] = *optResult;

    return PScatterResult{refDir, attCol, isScattered};
  }

  return std::nullopt;
}

} /// ::ray namespace