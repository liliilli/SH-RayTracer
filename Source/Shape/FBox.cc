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

#include <Shape/FBox.hpp>
#include <nlohmann/json.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Helper/XHelperJson.hpp>

namespace ray
{

template <> json::FExistanceList JsonCheckExistances<FBox::PCtor>(const nlohmann::json& json)
{
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1: return JsonCheckExistances<FBox::PCtor::PType1>(json);
  case 2: return JsonCheckExistances<FBox::PCtor::PType2>(json);
  case 3: return JsonCheckExistances<FBox::PCtor::PType3>(json);
  default: break;
  }

  return {};
}

FBox::PCtor FBox::PCtor::Overwrite(const FBox::PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;

  // I do not check result and newValue's type value is same or not, just for simplicity.
  switch (result.mCtorType)
  {
  case PCtor::_1:
  { result.mCtor = std::get<PCtor::PType1>(result.mCtor).Overwrite(std::get<PCtor::PType1>(pctor.mCtor), list); } break;
  case PCtor::_2:
  { result.mCtor = std::get<PCtor::PType2>(result.mCtor).Overwrite(std::get<PCtor::PType2>(pctor.mCtor), list); } break;
  case PCtor::_3:
  { result.mCtor = std::get<PCtor::PType3>(result.mCtor).Overwrite(std::get<PCtor::PType3>(pctor.mCtor), list); } break;
  default: break;
  }

  return result;
}

void from_json(const nlohmann::json& json, FBox::PCtor& oCtor)
{
  /// Type 1 ("type": 1)
  /// Type 2 ("type": 2)
  /// Type 3 ("type": 3)
  assert(json::HasJsonKey(json, "type") == true);
  switch (json::GetValueFrom<TU32>(json, "type"))
  {
  case 1:
  {
    oCtor.mCtorType = FBox::PCtor::_1;
    oCtor.mCtor = json.get<FBox::PCtor::PType1>();
  } break;
  case 2:
  {
    oCtor.mCtorType = FBox::PCtor::_2;
    oCtor.mCtor = json.get<FBox::PCtor::PType2>();
  } break;
  case 3:
  {
    oCtor.mCtorType = FBox::PCtor::_3;
    oCtor.mCtor = json.get<FBox::PCtor::PType3>();
  } break;
  default: break;
  }
}

template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType1>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(3, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "length") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[2] = EExistance::Exist; }

  return result;
}

FBox::PCtor::PType1 FBox::PCtor::PType1::Overwrite(const PType1& pctor, const json::FExistanceList& list) const
{
  PCtor::PType1 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mLength, pctor.mLength);
  OverwriteWhenExist(list[2], result.mAngle, pctor.mAngle);
  return result;
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType1& oCtor)
{
  /// Type 1
  /// {
  ///   "detail": { "type": 1, "pos": [ 0, 1, 0 ], "length": [1, 2, 3, 4, 5, 6], "angle": [45, 0, 45] },
  /// }
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "length") == true)
  {
    const auto lenVec = json::GetValueFrom<std::vector<TReal>>(json, "length");
    assert(lenVec.size() == 6);
    for (TIndex i = 0; i < 6; ++i) { oCtor.mLength[i] = lenVec[i]; }
  }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType2>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(3, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "length") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[2] = EExistance::Exist; }

  return result;
}

FBox::PCtor::PType2 FBox::PCtor::PType2::Overwrite(const PType2& pctor, const json::FExistanceList& list) const
{
  PCtor::PType2 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mLength, pctor.mLength);
  OverwriteWhenExist(list[2], result.mAngle, pctor.mAngle);
  return result;
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType2& oCtor)
{
  /// Type 2
  /// {
  ///   "detail": { "type": 2, "pos": [ 0, 1, 0 ], "length": [ 1, 2, 3 ], "angle": [45, 0, 45] },
  /// }
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "length") == true)
  { oCtor.mLength = json::GetValueFrom<DVec3>(json, "length"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType3>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(3, EExistance::NotExist);

  if (json::HasJsonKey(json, "pos") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "length") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "angle") == true) { result[2] = EExistance::Exist; }

  return result;
}

FBox::PCtor::PType3 FBox::PCtor::PType3::Overwrite(const PType3& pctor, const json::FExistanceList& list) const
{
  PCtor::PType3 result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[1], result.mLength, pctor.mLength);
  OverwriteWhenExist(list[2], result.mAngle, pctor.mAngle);
  return result;
}

void from_json(const nlohmann::json& json, FBox::PCtor::PType3& oCtor)
{
  /// Type 3
  /// {
  ///   "detail": { "type": 3, "pos": [ 0, 1, 0 ], "length": 1, "angle": [45, 0, 45] },
  /// }
  if (json::HasJsonKey(json, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(json, "pos"); }
  if (json::HasJsonKey(json, "length") == true)
  { oCtor.mLength = json::GetValueFrom<TReal>(json, "length"); }
  if (json::HasJsonKey(json, "angle") == true) 
  { oCtor.mAngle = json::GetValueFrom<DVec3>(json, "angle"); }
}

FBox::FBox(const PCtor& arg, const IMaterial* mat)
  : IHitable{EShapeType::Box, mat},
    DBox{}
{
  switch (arg.mCtorType)
  {
  case FBox::PCtor::_1:
  {
    const auto ctor = std::get<FBox::PCtor::PType1>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = ctor.mLength;
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_2:
  {
    const auto ctor = std::get<FBox::PCtor::PType2>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    this->mLength   = {
      ctor.mLength[0], ctor.mLength[0], ctor.mLength[1], 
      ctor.mLength[1], ctor.mLength[2], ctor.mLength[2]};
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  case FBox::PCtor::_3:
  {
    const auto ctor = std::get<FBox::PCtor::PType3>(arg.mCtor);
    this->mOrigin   = ctor.mOrigin;
    std::fill(this->mLength.begin(), this->mLength.end(), ctor.mLength);
    this->mRotQuat  = DQuat{ctor.mAngle};
  } break;
  }

  using ::dy::math::GetDBounds3DOf;
  this->mAABB = std::make_unique<DAABB>(GetDBounds3DOf(*this, this->GetQuaternion()));
}

std::optional<IHitable::TValueResults> FBox::GetRayIntersectedTValues(const DRay& ray) const
{
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

std::optional<PScatterResult> FBox::TryScatter(const DRay& ray, TReal t, const DVec3& normal) const
{
  if (this->GetMaterial() == nullptr) { return std::nullopt; }

  // Get result
  const auto nextRay = DRay{ray.GetPointAtParam(t), ray.GetDirection()};
  const auto optResult = this->GetMaterial()->Scatter(nextRay, normal);
  assert(optResult.has_value() == true);

  return *optResult;
}

FBox::PCtor FBox::GetPCtor(FBox::PCtor::EType type) const noexcept
{
  FBox::PCtor result; result.mCtorType = type;
  switch (type)
  {
  case PCtor::_1:
  {
    FBox::PCtor::PType1 pctor;
    pctor.mAngle  = this->mRotQuat.ToDegrees();
    pctor.mLength = this->mLength;
    pctor.mOrigin = this->mOrigin;
    result.mCtor  = pctor;
  } break;
  case PCtor::_2:
  {
    FBox::PCtor::PType2 pctor;
    pctor.mAngle  = this->mRotQuat.ToDegrees();
    pctor.mLength = {this->mLength[0], this->mLength[2], this->mLength[4]};
    pctor.mOrigin = this->mOrigin;
    result.mCtor  = pctor;
  } break;
  case PCtor::_3:
  {
    FBox::PCtor::PType3 pctor;
    pctor.mAngle  = this->mRotQuat.ToDegrees();
    pctor.mLength = this->mLength.front();
    pctor.mOrigin = this->mOrigin;
    result.mCtor  = pctor;
  } break;
  }

  return result;
}

} /// ::ray namespace