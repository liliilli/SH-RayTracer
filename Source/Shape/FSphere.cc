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

#include <Shape/FSphere.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FSphere::PCtor>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(2, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "radius") == true) { result[1] = EExistance::Exist; }

  return result;
}

FSphere::PCtor FSphere::PCtor::Overwrite(const PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mRadius, pctor.mRadius);

  return result;
}

void from_json(const nlohmann::json& json, FSphere::PCtor& oCtor)
{
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "radius") == true) 
  { oCtor.mRadius = json::GetValueFrom<TReal>(json, "radius"); }
}

FSphere::FSphere(const FSphere::PCtor& arg, const IMaterial* mat)
  : IHitable{EShapeType::Sphere, mat},
    DSphere<TReal>{ arg.mOrigin, arg.mRadius }
{ 
  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FSphere::PCtor FSphere::GetPCtor() const noexcept
{
  FSphere::PCtor result;
  result.mOrigin = this->GetOrigin();
  result.mRadius = this->GetRadius();

  return result;
}

std::optional<PScatterResult> FSphere::TryScatter(const DRay& ray, TReal t, const DVec3& normal) const
{
  if (this->GetMaterial() == nullptr) { return std::nullopt; }

  // Get result
  const auto nextRay = DRay{ray.GetPointAtParam(t), ray.GetDirection()};
  const auto optResult = this->GetMaterial()->Scatter(nextRay, normal);
  assert(optResult.has_value() == true);

  return *optResult;
}

std::optional<IHitable::TValueResults> FSphere::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this) == false) { return std::nullopt; }

  using TResult = IHitable::TValueResults::value_type;
  const auto tValues = GetTValuesOf(ray, *this);

  IHitable::TValueResults results;
  for (const auto& t : tValues)
  {
    results.emplace_back(t, this->GetType(), this, *GetNormalOf(ray, *this));
  }
  return results;
}

} /// ::ray namespace
