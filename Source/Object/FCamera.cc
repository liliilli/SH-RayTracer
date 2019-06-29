///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <Object/FCamera.hpp>

#include <iostream>
#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XRandom.h>

#include <XCommon.hpp>
#include <Helper/XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& item, FCamera::PCtor& oCtor)
{
  if (json::HasJsonKey(item, "aperture") == true) 
  { oCtor.mAperture = json::GetValueFrom<TReal>(item, "aperture"); }
  if (json::HasJsonKey(item, "focus_dist") == true) 
  { oCtor.mFocusDistance = json::GetValueFrom<TReal>(item, "focus_dist"); }
  if (json::HasJsonKey(item, "eye") == true) 
  { oCtor.mForwardTo = json::GetValueFrom<DVec3>(item, "eye"); }
  if (json::HasJsonKey(item, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(item, "pos"); }
  if (json::HasJsonKey(item, "sensor_size") == true) 
  { oCtor.mSensorSize = json::GetValueFrom<TReal>(item, "sensor_size"); }
}

template <>
json::FExistanceList JsonCheckExistances<FCamera::PCtor>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(8, EExistance::NotExist);

  if (json::HasJsonKey(json, "aperture") == true) { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "focus_dist") == true) { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "eye") == true) { result[2] = EExistance::Exist; }
  if (json::HasJsonKey(json, "pos") == true) { result[4] = EExistance::Exist; }
  if (json::HasJsonKey(json, "sensor_size") == true) { result[7] = EExistance::Exist; }

  return result;
}

FCamera::PCtor FCamera::PCtor::Overwrite(const PCtor& pctor, const json::FExistanceList& list) const
{
  PCtor result = *this;
  using json::EExistance;
  
  OverwriteWhenExist(list[0], result.mAperture, pctor.mAperture);
  OverwriteWhenExist(list[1], result.mFocusDistance, pctor.mFocusDistance);
  OverwriteWhenExist(list[2], result.mForwardTo, pctor.mForwardTo);
  OverwriteWhenExist(list[3], result.mImgSize, pctor.mImgSize);
  OverwriteWhenExist(list[4], result.mOrigin, pctor.mOrigin);
  OverwriteWhenExist(list[5], result.mSamples, pctor.mSamples);
  OverwriteWhenExist(list[6], result.mScreenRatioXy, pctor.mScreenRatioXy);
  OverwriteWhenExist(list[7], result.mSensorSize, pctor.mSensorSize);

  return result;
} 

FCamera::PCtor FCamera::GetPCtor() const noexcept
{
  PCtor result;
  result.mAperture = this->mAperture;
  result.mFocusDistance = this->mDistance;
  result.mForwardTo = this->mForward + this->mOrigin;
  result.mImgSize = this->mScreenSize;
  result.mOrigin = this->mOrigin;
  result.mSamples = this->mSamples;
  result.mScreenRatioXy = TReal(result.mImgSize.X) / result.mImgSize.Y;
  result.mSensorSize = this->mSensorSize;

  return result;
}

FCamera::FCamera(const FCamera::PCtor& arg)
  : IObject { EObject::Camera },
    mOrigin{ arg.mOrigin },
    mForward{ (arg.mForwardTo - arg.mOrigin).Normalize() },
    mSide{ Cross(this->mForward, decltype(mForward){0, 1, 0}) },
    mUp{ Cross(this->mSide, this->mForward) },
    mScreenSize{ arg.mImgSize },
    mSamples{ arg.mSamples },
    mAperture{ arg.mAperture },
    mDistance{ arg.mFocusDistance },
    mSensorSize{ arg.mSensorSize }
{
  using ::dy::math::kToRadian;
  constexpr TReal defScrHeight = TReal(2.0);
  
  // I moved origin to backward instead of moving lens position (screen) to forward,
  // to avoid the intersection between screen and objects.
  this->mOrigin -= this->mForward * (mDistance - 1.0f);
  this->mLowLeftCorner = 
      this->mOrigin + this->mForward * (mDistance)
    + this->mSide * -arg.mScreenRatioXy * this->mSensorSize
    + this->mUp * -1.0f * this->mSensorSize;

  // Setting up screen cell right (per pixel) and up (per pixel) vector.
  const auto scaledHeight = defScrHeight * this->mSensorSize;
  this->mCellRight  = mSide * (scaledHeight * arg.mScreenRatioXy / TReal(this->mScreenSize.X) );
  this->mCellUp     = mUp * (scaledHeight / TReal(this->mScreenSize.Y) );

  RAY_IF_VERBOSE_MODE()
  {
    std::cout << "* Camera Information\n";
    std::cout << "  Origin : " << this->mOrigin << '\n';
    std::cout << "  Forward : " << this->mForward << '\n';
    std::cout << "  Side : " << this->mSide << '\n';
    std::cout << "  Up : " << this->mUp << '\n';

    std::cout << "  LowLeftCorner : " << this->mLowLeftCorner << '\n';
    std::cout << "  CellRight : " << this->mCellRight << '\n';
    std::cout << "  CellUp : " << this->mCellUp << '\n';

    std::cout << "  ScreenSize : " << this->mScreenSize << '\n';
    std::cout << "  Aperture : " << this->mAperture << '\n';
    std::cout << "  Distance : " << this->mDistance << '\n';
    std::cout << "  Sensor size : " << this->mSensorSize << '\n';
  }
}

const DUVec2& FCamera::GetImageSize() const noexcept
{
  return this->mScreenSize;
}

std::vector<DRay> FCamera::CreateRay(TIndex x, TIndex y) const noexcept
{
  assert(x < this->mScreenSize[0] && y < this->mScreenSize[1]);
  
  const auto screenPos = this->mLowLeftCorner + (this->mCellRight * TReal(x)) + (this->mCellUp * TReal(y));
  const std::vector<DVec3> offsets = GetSampleOffsetsOf(this->mCellRight, this->mCellUp, this->mSamples);

  std::vector<DRay> rayList;
  for (const auto& offset : offsets)
  {
    //DVec3 rd = this->mAperture / 2.0f * ::dy::math::RandomVector3Length<TReal>(1.0f);
    //const auto lensOffset = this->mSide * rd.X + this->mUp * rd.Y;

    const auto orig = screenPos + offset;// + lensOffset;
    const auto dir = orig - this->mOrigin;
    rayList.emplace_back(orig, dir);
  }

  return rayList;
}

std::vector<DVec3> FCamera::GetSampleOffsetsOf(const DVec3& right, const DVec3& up, TU32 samples) const
{
  std::vector<DVec3> offsets;
  static auto fo = [](DVec3 right, TU32 r, DVec3 up, TU32 u) -> DVec3
  {
    return right * r + up * u; 
  };

  if (samples >= 64) // If >= AAx64. Okay... I need to end up this features to 64 maximum.
  {
    const auto r = right / 32.f;
    const auto u = up / 32.f;
    offsets = 
    {
      // LeftDown
      fo(r, 1, u, 5), fo(r, 3, u, 1), fo(r, 5, u, 7), fo(r, 7, u, 3),
      fo(r, 1, u, 13), fo(r, 3, u, 9), fo(r, 5, u, 15), fo(r, 7, u, 11),
      fo(r, 9, u, 13), fo(r, 11, u, 9), fo(r, 13, u, 15), fo(r, 15, u, 11),
      fo(r, 9, u, 5), fo(r, 11, u, 1), fo(r, 13, u, 7), fo(r, 15, u, 3),
      // LeftUp
      fo(r, 1, u, 31), fo(r, 3, u, 17), fo(r, 5, u, 23), fo(r, 7, u, 19),
      fo(r, 1, u, 29), fo(r, 3, u, 25), fo(r, 5, u, 31), fo(r, 7, u, 27),
      fo(r, 9, u, 29), fo(r, 11, u, 25), fo(r, 13, u, 31), fo(r, 15, u, 27),
      fo(r, 9, u, 21), fo(r, 11, u, 17), fo(r, 13, u, 23), fo(r, 15, u, 19),
      // RightUp
      fo(r, 17, u, 31), fo(r, 19, u, 17), fo(r, 21, u, 23), fo(r, 23, u, 19),
      fo(r, 17, u, 29), fo(r, 19, u, 25), fo(r, 21, u, 31), fo(r, 23, u, 27),
      fo(r, 25, u, 29), fo(r, 27, u, 25), fo(r, 29, u, 31), fo(r, 31, u, 27),
      fo(r, 25, u, 21), fo(r, 27, u, 17), fo(r, 29, u, 23), fo(r, 31, u, 19),
      // RightDown
      fo(r, 17, u, 5), fo(r, 19, u, 1), fo(r, 21, u, 7), fo(r, 23, u, 3),
      fo(r, 17, u, 13), fo(r, 19, u, 9), fo(r, 21, u, 15), fo(r, 23, u, 11),
      fo(r, 25, u, 13), fo(r, 27, u, 9), fo(r, 29, u, 15), fo(r, 31, u, 11),
      fo(r, 25, u, 5), fo(r, 27, u, 1), fo(r, 29, u, 7), fo(r, 31, u, 3),
    };
  }
  else if (samples >= 32) // If >= AAx32
  {
    const auto r = right / 32.f;
    const auto u = up / 32.f;
    offsets = 
    {
      // LeftDown
      fo(r, 1, u, 5), fo(r, 3, u, 13), fo(r, 5, u, 1), fo(r, 7, u, 9),
      fo(r, 9, u, 15), fo(r, 11, u, 7), fo(r, 13, u, 3), fo(r, 15, u, 11),
      // LeftUp
      fo(r, 1, u, 21), fo(r, 3, u, 29), fo(r, 5, u, 17), fo(r, 7, u, 25),
      fo(r, 9, u, 31), fo(r, 11, u, 23), fo(r, 13, u, 19), fo(r, 15, u, 27),
      // RightUp
      fo(r, 17, u, 21), fo(r, 19, u, 29), fo(r, 21, u, 17), fo(r, 23, u, 25),
      fo(r, 25, u, 31), fo(r, 27, u, 23), fo(r, 29, u, 19), fo(r, 31, u, 27),
      // RightDown
      fo(r, 17, u, 5), fo(r, 19, u, 13), fo(r, 21, u, 1), fo(r, 23, u, 9),
      fo(r, 25, u, 15), fo(r, 27, u, 7), fo(r, 29, u, 3), fo(r, 31, u, 11)
    };
  }
  else if (samples >= 16) // If >= AAx16
  {
    const DVec3 r = right / 16.f;
    const DVec3 u = up / 16.f;
    offsets = 
    {
      fo(r, 1, u, 5), fo(r, 3, u, 1), fo(r, 5, u, 7), fo(r, 7, u, 3),       // LeftDown
      fo(r, 1, u, 13), fo(r, 3, u, 9), fo(r, 5, u, 15), fo(r, 7, u, 11),    // LeftUp
      fo(r, 9, u, 13), fo(r, 11, u, 9), fo(r, 13, u, 15), fo(r, 15, u, 11), // RightUp
      fo(r, 9, u, 5), fo(r, 11, u, 1), fo(r, 13, u, 7), fo(r, 15, u, 3)     // RightDown
    };
  }
  else if (samples >= 8) // If >= AAx8
  {
    const auto r = right / 16.f;
    const auto u = up / 16.f;
    offsets = 
    {
      fo(r, 1, u, 5), fo(r, 3, u, 13), fo(r, 5, u, 1), fo(r, 7, u, 9),
      fo(r, 9, u, 15), fo(r, 11, u, 7), fo(r, 13, u, 3), fo(r, 15, u, 11)
    };
  }
  else if (samples >= 4) // If >= AAx4
  {
    const DVec3 r = right / 8.f;
    const DVec3 u = up / 8.f;
    offsets =
    {
      fo(r, 1, u, 5), fo(r, 3, u, 1), fo(r, 5, u, 7), fo(r, 7, u, 3)
    };
  }
  else if (samples >= 2) // If >= AAx2
  {
    offsets = { right * 0.25f + up * 0.25f, right * 0.75f + up * 0.75f };
  }
  else // Otherwise
  {
    offsets = { right * .5f + up * .5f };
  }

  return offsets;
}

void FCamera::SetSamples(TU32 sample)
{
  this->mSamples = sample;
}

TU32 FCamera::GetSamples() const noexcept
{
  return this->mSamples;
}

} /// ::ray namespace 