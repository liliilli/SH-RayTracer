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

#include <Shape/FTorus.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FTorus::PCtor>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(4, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "distance") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "radius") == true) { result[2] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[3] = EExistance::Exist; }

  return result;
}

FTorus::PCtor FTorus::PCtor::Overwrite(const PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mDistance, pctor.mDistance);
  OverwriteWhenExist(list[2], result.mRadius, pctor.mRadius);
  OverwriteWhenExist(list[3], result.mAngle, pctor.mAngle);

  return result;
}

void from_json(const nlohmann::json& json, FTorus::PCtor& oCtor)
{
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "distance") == true) 
  { oCtor.mDistance = json::GetValueFrom<TReal>(json, "distance"); }
  if (json::HasJsonKey(json, "radius") == true) 
  { oCtor.mRadius = json::GetValueFrom<TReal>(json, "radius"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

FTorus::FTorus(const FTorus::PCtor& arg, IMaterial* mat)
  : IHitable{EShapeType::Torus, mat},
    ::dy::math::DTorus<TReal>{arg.mOrigin, arg.mDistance, arg.mRadius},
    mRotQuat{arg.mAngle}
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this, this->GetQuaternion()));
}

std::optional<IHitable::TValueResults> FTorus::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }
  if (IsRayIntersected(ray, *this, this->GetQuaternion()) == false) { return std::nullopt; }

  using TResult = IHitable::TValueResults::value_type;
  const auto tValues = GetTValuesOf(ray, *this, this->GetQuaternion());

  IHitable::TValueResults results;
  for (const auto& t : tValues)
  {
    results.emplace_back(t, this->GetType(), this);
  }
  return results;
}

std::optional<PScatterResult> FTorus::TryScatter(const DRay& ray, TReal t) const
{
  if (this->GetMaterial() == nullptr) { return std::nullopt; }

  // Get result
  const auto nextRay = DRay{ray.GetPointAtParam(t), ray.GetDirection()};

  const auto optResult = this->GetMaterial()->Scatter(nextRay, *GetNormalOf(ray, *this, this->GetQuaternion()));
  const auto& [refDir, attCol, isScattered] = *optResult;

  return PScatterResult{refDir, attCol, isScattered};
}

FTorus::PCtor FTorus::GetPCtor() const noexcept
{
  FTorus::PCtor result;
  result.mOrigin    = this->GetOrigin();
  result.mDistance  = this->GetDistance();
  result.mRadius    = this->GetRadius();
  result.mAngle     = this->mRotQuat.ToDegrees();

  return result;
}

} /// ::ray namespace