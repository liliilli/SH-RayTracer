///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <Shape/FModel.hpp>
#include <nlohmann/json.hpp>
#include <XHelperJson.hpp>
#include <Manager/MModel.hpp>
#include <Math/Utility/XShapeMath.h>

namespace ray
{

FModel::FModel(const PModelCtor& ctor, IMaterial* mat)
  : IHitable{EShapeType::Model, mat},
    mOrigin { ctor.mOrigin },
    mScale { ctor.mScale },
    mRotQuat { ctor.mAngle }
{
  this->mModelId = DModelId{ctor.mModelResourceName};
  assert(EXPR_SGT(MModel).HasModel(this->mModelId) == true);

  const auto* pModel = EXPR_SGT(MModel).GetModel(this->mModelId);
  for (const auto& meshId : pModel->GetMeshIds())
  {
    FModelMesh::PCtor meshCtor;
    meshCtor.mOrigin  = this->mOrigin;
    meshCtor.mScale   = this->mScale;
    meshCtor.mAngle   = this->mRotQuat.ToDegrees();
    meshCtor.mMeshId  = meshId;

    mpMeshes.emplace_back(std::make_unique<FModelMesh>(meshCtor, this->GetMaterial()));
  }

  // Make Overall AABB from pMesh.
  if (this->mpMeshes.empty() == true) { return; }

  DAABB aabb = *this->mpMeshes.front()->GetAABB();
  for (const auto& pMesh : this->mpMeshes)
  {
    aabb = ::dy::math::GetUnionOf(aabb, *pMesh->GetAABB());
  }
  this->mAABB = std::make_unique<DAABB>(aabb);
}

PModelCtor FModel::GetPCtor() const noexcept
{
  PModelCtor result;
  result.mOrigin  = this->GetOrigin();
  result.mScale   = this->GetScale();
  result.mAngle   = this->GetQuaternion().ToDegrees();
  result.mModelResourceName = this->mModelId.ToString();

  return result;
}

const DVec3& FModel::GetOrigin() const noexcept
{
  return this->mOrigin;
}

const TReal& FModel::GetScale() const noexcept
{
  return this->mScale;
}

const DQuat& FModel::GetQuaternion() const noexcept
{
  return this->mRotQuat;  
}

std::optional<IHitable::TValueResults> FModel::GetRayIntersectedTValues(const DRay& ray) const
{
  // Check Overall AABB of Model.
  if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }

  // Call `GetRayIntersectedTValues` with mesh instances.
  IHitable::TValueResults results;
  for (const auto& smtMesh : this->mpMeshes)
  {
    const auto optResult = smtMesh->GetRayIntserectedTValues2(ray);
    //const auto optResult = smtMesh->GetRayIntersectedTValues(ray);
    if (optResult.has_value() == true)
    {
      results.insert(results.end(), (*optResult).begin(), (*optResult).end());
    }
  }

  return results;
}

std::optional<PScatterResult> FModel::TryScatter(const DRay&, TReal) const
{
  // This must not be called. Need to be refactored.
  assert(false);
  return std::nullopt;
}

} /// ::ray namespace