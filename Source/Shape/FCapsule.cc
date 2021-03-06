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

#include <Shape/FCapsule.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor>(const nlohmann::json& json)
{
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1: return JsonCheckExistances<FCapsule::PCtor::PType1>(json);
  case 2: return JsonCheckExistances<FCapsule::PCtor::PType2>(json);
  default: break;
  }

  return {};
}

FCapsule::PCtor FCapsule::PCtor::Overwrite(const PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;

  // I do not check result and newValue's type value is same or not, just for simplicity.
  switch (result.mCtorType)
  {
  case PCtor::_1:
  { result.mCtor = std::get<PCtor::PType1>(result.mCtor).Overwrite(std::get<PCtor::PType1>(pctor.mCtor), list); } break;
  case PCtor::_2:
  { result.mCtor = std::get<PCtor::PType2>(result.mCtor).Overwrite(std::get<PCtor::PType2>(pctor.mCtor), list); } break;
  default: break;
  }

  return result; 
}

void from_json(const nlohmann::json& json, FCapsule::PCtor& oCtor)
{
  /// Type 1 ("type" : 1)
  /// Type 2 ("type" : 2)
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1:
  {
    oCtor.mCtorType = FCapsule::PCtor::_1;
    oCtor.mCtor = json.get<FCapsule::PCtor::PType1>();
  } break;
  case 2:
  {
    oCtor.mCtorType = FCapsule::PCtor::_2;
    oCtor.mCtor = json.get<FCapsule::PCtor::PType2>();
  } break;
  default: break;
  }
}

template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor::PType1>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(4, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "height") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "raidus") == true) { result[2] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[3] = EExistance::Exist; }

  return result; 
}

FCapsule::PCtor::PType1 FCapsule::PCtor::PType1::Overwrite(const PType1& pctor, const json::FExistanceList& list) const
{
  PCtor::PType1 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mHeight, pctor.mHeight);
  OverwriteWhenExist(list[2], result.mRadius, pctor.mRadius);
  OverwriteWhenExist(list[3], result.mAngle, pctor.mAngle);
  return result;
}

void from_json(const nlohmann::json& json, FCapsule::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "pos": [,,], "height": 2, "radius": 1, "angle": [,,] }
  /// }
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "height") == true) 
  { oCtor.mHeight = json::GetValueFrom<TReal>(json, "height"); }
  if (json::HasJsonKey(json, "radius") == true) 
  { oCtor.mRadius = json::GetValueFrom<TReal>(json, "radius"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

template <> json::FExistanceList JsonCheckExistances<FCapsule::PCtor::PType2>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(3, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "length") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[2] = EExistance::Exist; }

  return result; 
}

FCapsule::PCtor::PType2 FCapsule::PCtor::PType2::Overwrite(const PType2& pctor, const json::FExistanceList& list) const
{
  PCtor::PType2 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mLength, pctor.mLength);
  OverwriteWhenExist(list[2], result.mAngle, pctor.mAngle);
  return result;
}

void from_json(const nlohmann::json& json, FCapsule::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos": [,,], "length": 1, "angle": [,,] }
  /// }
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "length") == true) 
  { oCtor.mLength = json::GetValueFrom<TReal>(json, "length"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

FCapsule::FCapsule(const FCapsule::PCtor& arg, const IMaterial* mat)
  : IHitable{ EShapeType::Capsule, mat },
    ::dy::math::DCapsule<TReal>{DVec3{}, 1}
{ 
  switch (arg.mCtorType)
  {
  case FCapsule::PCtor::_1:
  {
    const auto ctor = std::get<FCapsule::PCtor::PType1>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mHeight   = ctor.mHeight;
    this->mRadius   = ctor.mRadius;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FCapsule::PCtor::_2:
  {
    const auto ctor = std::get<FCapsule::PCtor::PType2>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mHeight   = ctor.mLength;
    this->mRadius   = ctor.mLength;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  }

  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this));
}

FCapsule::PCtor FCapsule::GetPCtor(FCapsule::PCtor::EType type) const noexcept
{
  FCapsule::PCtor result; result.mCtorType = type;
  switch (type)
  {
  case PCtor::_1:
  {
    FCapsule::PCtor::PType1 pctor;
    pctor.mOrigin = this->GetOrigin();
    pctor.mHeight = this->GetHeight();
    pctor.mRadius = this->GetRadius();
    pctor.mAngle  = this->mRotQuat.ToDegrees();
    result.mCtor  = pctor;
  } break;
  case PCtor::_2:
  {
    FCapsule::PCtor::PType2 pctor;
    pctor.mOrigin = this->GetOrigin();
    pctor.mLength = this->GetHeight();
    pctor.mAngle  = this->mRotQuat.ToDegrees();
    result.mCtor  = pctor;
  } break;
  }

  return result;
}

std::optional<PScatterResult> FCapsule::TryScatter(const DRay& ray, TReal t, const DVec3& normal) const
{
  if (this->GetMaterial() == nullptr) { return std::nullopt; }

  // Get result
  const auto nextRay = DRay{ray.GetPointAtParam(t), ray.GetDirection()};
  const auto optResult = this->GetMaterial()->Scatter(nextRay, normal);
  assert(optResult.has_value() == true);

  return *optResult;
}

std::optional<IHitable::TValueResults> FCapsule::GetRayIntersectedTValues(const DRay& ray) const
{
  if (IsRayIntersected(ray, *this->GetAABB()) == false) { return std::nullopt; }
  if (IsRayIntersected(ray, *this, this->GetQuaternion()) == false) { return std::nullopt; }

  using TResult = IHitable::TValueResults::value_type;
  const auto tValues = GetTValuesOf(ray, *this, this->GetQuaternion());

  IHitable::TValueResults results;
  for (const auto& t : tValues)
  {
    results.emplace_back(t, this->GetType(), this, *GetNormalOf(ray, *this, this->GetQuaternion()));
  }
  return results;
}

} /// ::ray namespace
