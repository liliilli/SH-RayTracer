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
#include <sstream>
#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XRandom.h>

#include <XCommon.hpp>
#include <Helper/XHelperJson.hpp>
#include <Manager/MScene.hpp>

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
  if (json::HasJsonKey(item, "image_size") == true)
  { oCtor.mImgSize = json::GetValueFrom<DUVec2>(item, "image_size"); }
  if (json::HasJsonKey(item, "pos") == true) 
  { oCtor.mOrigin = json::GetValueFrom<DVec3>(item, "pos"); }
  if (json::HasJsonKey(item, "sample_count") == true)
  { oCtor.mSamples = json::GetValueFrom<TU32>(item, "sample_count"); }
  if (json::HasJsonKey(item, "sensor_size") == true) 
  { oCtor.mSensorSize = json::GetValueFrom<TReal>(item, "sensor_size"); }
  if (json::HasJsonKey(item, "depth_of_field") == true)
  { json::GetValueFromTo(item, "depth_of_field", oCtor.mDepthOfField); }
  if (json::HasJsonKey(item, "gamma") == true)
  { json::GetValueFromTo(item, "gamma", oCtor.mGamma); }
  if (json::HasJsonKey(item, "repeat") == true)
  { json::GetValueFromTo(item, "repeat", oCtor.mRepeat); }
  if (json::HasJsonKey(item, "hdr") == true)
  { json::GetValueFromTo(item, "hdr", oCtor.mHdr); }
  if (json::HasJsonKey(item, "hdr_middle_gray") == true)
  { oCtor.mMiddleGray = json::GetValueFrom<TReal>(item, "hdr_middle_gray"); }
}

template <>
json::FExistanceList JsonCheckExistances<FCamera::PCtor>(const nlohmann::json& json)
{
  using json::EExistance;
  json::FExistanceList result(13, EExistance::NotExist);

  if (json::HasJsonKey(json, "aperture") == true)       { result[0] = EExistance::Exist; }
  if (json::HasJsonKey(json, "focus_dist") == true)     { result[1] = EExistance::Exist; }
  if (json::HasJsonKey(json, "eye") == true)            { result[2] = EExistance::Exist; }
  if (json::HasJsonKey(json, "image_size") == true)     { result[3] = EExistance::Exist; }
  if (json::HasJsonKey(json, "pos") == true)            { result[4] = EExistance::Exist; }
  if (json::HasJsonKey(json, "sample_count") == true)   { result[5] = EExistance::Exist; }
  if (json::HasJsonKey(json, "sensor_size") == true)    { result[7] = EExistance::Exist; }
  if (json::HasJsonKey(json, "depth_of_field") == true) { result[8] = EExistance::Exist; }
  if (json::HasJsonKey(json, "gamma") == true)          { result[9] = EExistance::Exist; }
  if (json::HasJsonKey(json, "repeat") == true)         { result[10] = EExistance::Exist; }
  if (json::HasJsonKey(json, "hdr") == true)            { result[11] = EExistance::Exist; }
  if (json::HasJsonKey(json, "hdr_middle_gray") == true){ result[12] = EExistance::Exist; }

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
  OverwriteWhenExist(list[8], result.mDepthOfField, pctor.mDepthOfField);
  OverwriteWhenExist(list[9], result.mGamma, pctor.mGamma);
  OverwriteWhenExist(list[10], result.mRepeat, pctor.mRepeat);
  OverwriteWhenExist(list[11], result.mHdr, pctor.mHdr);
  OverwriteWhenExist(list[12], result.mMiddleGray, pctor.mMiddleGray);

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
  result.mGamma = this->mGamma;
  result.mRepeat = this->mRepeat;

  result.mDepthOfField = this->mIsUsingDepthOfField;
  result.mHdr = this->mIsUsingHdr;
  result.mMiddleGray = this->mHdrMiddleGray;

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
    mRepeat{ arg.mRepeat },
    mAperture{ arg.mAperture },
    mDistance{ arg.mFocusDistance },
    mSensorSize{ arg.mSensorSize },
    mGamma{ arg.mGamma },
    mIsUsingDepthOfField{ arg.mDepthOfField },
    mIsUsingHdr{ arg.mHdr },
    mHdrMiddleGray{ arg.mMiddleGray }
{
  using ::dy::math::kToRadian;
  constexpr TReal defScrHeight = TReal(1.0);
  const auto scaledHeight = defScrHeight * this->mSensorSize;
  
  // I moved origin to backward instead of moving lens position (screen) to forward,
  // to avoid the intersection between screen and objects.
  this->mLowLeftCorner = 
      this->mOrigin 
    - (this->mSide * (scaledHeight * 0.5f * arg.mScreenRatioXy) + this->mUp * (scaledHeight * 0.5f));
    //+ (this->mForward * this->mDistance)

  // Setting up screen cell right (per pixel) and up (per pixel) vector.
  this->mCellRight = this->mSide * (scaledHeight * arg.mScreenRatioXy / TReal(this->mScreenSize.X) );
  this->mCellUp    = this->mUp * (scaledHeight / TReal(this->mScreenSize.Y) );
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
  const auto hole = this->mOrigin + (this->mForward * this->mDistance);

  std::vector<DRay> rayList;
  if (this->IsUsingDepthOfField() == false)
  {
    // If camera is not using depth of field, just model perfect pin-hole camera.
    for (const auto& offset : offsets)
    {
      const auto orig = screenPos + offset;
      const auto dir  = (hole - orig).Normalize();
      rayList.emplace_back(hole, dir);
    }
  }
  else
  {
    using ::dy::math::RandomVector2Range;
    using ::dy::math::ERandomPolicy;
    using ::dy::math::Dot;

    // If camera is using depth of field, model convex (positive) thin-lens camera.
    // f-number = this->mSensorSize (diameter) / this->mDistance;
    // We need to get positive focal plane's focal point.
    for (const auto& offset : offsets)
    {
      const auto orig         = screenPos + offset;
      const auto origDir      = (hole - orig).Normalize();
      const auto rayFocalCos  = Dot(origDir, this->mForward);
      const auto focalPoint   = hole + (origDir * (this->mDistance / rayFocalCos));
      
      // And get uniform random arbitary point of lens with this->mCellRight and this->mCellUp.
      const auto xyPoint = RandomVector2Range<TReal>(ERandomPolicy::Uniform, 0, this->mSensorSize * 0.0625f);
      const auto aperturePoint = 
          hole
        + this->mSide * xyPoint[0]
        + this->mUp * xyPoint[1];
      // Finally get actual direction and insert it as a ray.
      const auto dir = (focalPoint - aperturePoint).Normalize();
      rayList.emplace_back(aperturePoint, dir);
    }
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

TReal FCamera::GetGamma() const noexcept
{
  return this->mGamma;
}

TU32 FCamera::GetRepeat() const noexcept
{
  return this->mRepeat;
}

bool FCamera::IsUsingDepthOfField() const noexcept
{
  return this->mIsUsingDepthOfField;
}

bool FCamera::IsUsingHDR() const noexcept
{
  return this->mIsUsingHdr;
}

TReal FCamera::GetMiddleGray() const noexcept
{
  return this->mHdrMiddleGray;
}

std::string FCamera::ToString() const noexcept
{
  std::stringstream buffer;

  buffer << "* Camera Information\n";
  buffer << "  Origin : " << this->mOrigin << '\n';
  buffer << "  Forward : " << this->mForward << '\n';
  buffer << "  Side : " << this->mSide << '\n';
  buffer << "  Up : " << this->mUp << '\n';

  buffer << "  LowLeftCorner : " << this->mLowLeftCorner << '\n';
  buffer << "  CellRight : " << this->mCellRight << '\n';
  buffer << "  CellUp : " << this->mCellUp << '\n';

  buffer << "  ScreenSize : " << this->mScreenSize << '\n';
  buffer << "  Aperture : " << this->mAperture << '\n';
  buffer << "  Distance (Focal Distance) : " << this->mDistance << '\n';
  buffer << "  Sensor size : " << this->mSensorSize << '\n';
  buffer << "  Gamma : " << this->mGamma << '\n';
  buffer << "  Repeat : " << this->mRepeat << '\n';
  buffer << "  Middle Gray Value (When HDR is enabled) : " << this->mHdrMiddleGray << '\n';
  buffer << "  Using Depth Of Field : " << (this->mIsUsingDepthOfField ? "true" : "false") << '\n';
  buffer << "  Using HDR (tone-mapping) : " << (this->mIsUsingHdr ? "true" : "false") << '\n';
  buffer << '\n';

  return buffer.str();
}

} /// ::ray namespace 