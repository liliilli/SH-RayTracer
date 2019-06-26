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

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FModel::PCtor>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(4, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "scale") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[2] = EExistance::Exist; }
  if (json::HasJsonKey(json, "path") == true) { result[3] = EExistance::Exist; }

  return result;
}

FModel::PCtor FModel::PCtor::Overwrite(const PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mScale, pctor.mScale);
  OverwriteWhenExist(list[2], result.mAngle, pctor.mAngle);
  OverwriteWhenExist(list[3], result.mPath, pctor.mPath);

  return result;  
}

void from_json(const nlohmann::json& json, FModel::PCtor& oCtor)
{
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "scale") == true) 
  { oCtor.mScale = json::GetValueFrom<TReal>(json, "scale"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }  
  if (json::HasJsonKey(json, "path") == true) 
  { oCtor.mPath = json::GetValueFrom<std::string>(json, "path"); }  
}

FModel::FModel(const PCtor& ctor, IMaterial* mat)
  : IHitable{EShapeType::Model, mat},
    mOrigin { ctor.mOrigin },
    mScale { ctor.mScale },
    mRotQuat { ctor.mAngle }
{
  if (ctor.mModelId.HasId() == false)
  {
    assert(ctor.mPath.empty() == false);
    this->mResource = std::filesystem::path(ctor.mPath);
  }
  else
  {
    this->mResource = ctor.mModelId;
    this->mIsPopulated = true;

    const auto* pModel = EXPR_SGT(MModel).GetModel(ctor.mModelId);
    for (const auto& meshId : pModel->GetMeshIds())
    {
      FModelMesh::PCtor meshCtor;
      meshCtor.mOrigin  = this->mOrigin;
      meshCtor.mScale   = this->mScale;
      meshCtor.mAngle   = this->mRotQuat.ToDegrees();
      meshCtor.mMeshId  = meshId;

      mpMeshes.emplace_back(std::make_unique<FModelMesh>(meshCtor, this->GetMaterial()));
    }
  }
}

FModel::PCtor FModel::GetPCtor() const noexcept
{
  FModel::PCtor result;
  result.mOrigin = this->GetOrigin();
  result.mScale = this->GetScale();
  result.mAngle = this->GetQuaternion().ToDegrees();
  if (this->IsResourcePopulated() == true)
  {
    result.mModelId = *this->TryGetResourceId();
    assert(result.mModelId.HasId() == true);
  }

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
  if (this->IsResourcePopulated() == false) { return std::nullopt; }
  // Check Overall AABB of Model.

  // Call `GetRayIntersectedTValues` with mesh instances.
  IHitable::TValueResults results;
  for (const auto& smtMesh : this->mpMeshes)
  {
    const auto optResult = smtMesh->GetRayIntersectedTValues(ray);
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

bool FModel::TryPopulateResource()
{
  if (this->IsResourcePopulated() == true) { return false; }

  const auto optId = EXPR_SGT(MModel).AddModel(*this->TryGetResourcePath());
  if (optId.has_value() == false)
  {
    return false;
  }
  this->mResource = *optId;
  this->mIsPopulated = true;

  const auto* pModel = EXPR_SGT(MModel).GetModel(*this->TryGetResourceId());
  for (const auto& meshId : pModel->GetMeshIds())
  {
    FModelMesh::PCtor ctor;
    ctor.mOrigin  = this->mOrigin;
    ctor.mScale   = this->mScale;
    ctor.mAngle   = this->mRotQuat.ToDegrees();
    ctor.mMeshId  = meshId;

    mpMeshes.emplace_back(std::make_unique<FModelMesh>(ctor, this->GetMaterial()));
  }

  return true;
}

bool FModel::IsResourcePopulated() const noexcept
{
  return this->mIsPopulated;
}

std::optional<std::filesystem::path> FModel::TryGetResourcePath() const noexcept
{
  if (this->IsResourcePopulated() == true) { return std::nullopt; }
  return std::get<std::filesystem::path>(this->mResource);
}

std::optional<DModelId> FModel::TryGetResourceId() const noexcept
{
  if (this->IsResourcePopulated() == false) { return std::nullopt; }
  return std::get<DModelId>(this->mResource);
}

} /// ::ray namespace