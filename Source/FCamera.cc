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

#include <FCamera.hpp>

#include <iostream>
#include <nlohmann/json.hpp>
#include <Math/Utility/XLinearMath.h>
#include <Math/Utility/XRandom.h>

#include <XCommon.hpp>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& item, FCamera::PCtor& oCtor)
{
  assert(json::HasJsonKey(item, "pos") == true); 
  assert(json::HasJsonKey(item, "eye") == true);
  assert(json::HasJsonKey(item, "focus_dist") == true); 
  assert(json::HasJsonKey(item, "sensor_size") == true); 
  assert(json::HasJsonKey(item, "aperture") == true); 

  oCtor.mAperture      = json::GetValueFrom<TReal>(item, "aperture");
  oCtor.mFocusDistance = json::GetValueFrom<TReal>(item, "focus_dist");
  oCtor.mForwardTo     = json::GetValueFrom<DVec3>(item, "eye");
  oCtor.mOrigin        = json::GetValueFrom<DVec3>(item, "pos");
  oCtor.mSensorSize    = json::GetValueFrom<TReal>(item, "sensor_size");
}

FCamera::FCamera(const FCamera::PCtor& arg)
  : mOrigin{ arg.mOrigin },
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

  if (samples >= 8) // If >= AAx8
  {
    const auto r = right / 16.f;
    const auto u = this->mCellUp / 16.f;
    offsets = 
    {
      r * 1, u * 5,   r * 3, u * 13, r * 5, u * 1,  r * 7, u * 9,
      r * 9, u * 15,  r * 11, u * 7, r * 13, u * 3, r * 15, u * 11
    };
  }
  else if (samples >= 4) // If >= AAx4
  {
    offsets =
    {
      right * 1.f / 8.f + up * 5.f / 8.f, right * 3.f / 8.f + up * 1.f / 8.f,
      right * 5.f / 8.f + up * 7.f / 8.f, right * 7.f / 8.f + up * 3.f / 8.f,
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