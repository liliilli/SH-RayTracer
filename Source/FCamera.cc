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
#include <Math/Utility/XLinearMath.h>
#include <XCommon.hpp>
#include <iostream>

namespace ray
{

FCamera::FCamera(
    const DVec3& origin, const DVec3& forwardTo, 
    DUVec2 imgSize, TReal scrWidth, TReal scrHeight,
    TU32 samples)
  : mOrigin{ origin },
    mForward{ (forwardTo - origin).Normalize() },
    mSide{ Cross(this->mForward, decltype(mForward){0, 1, 0}) },
    mUp{ Cross(this->mSide, this->mForward) },
    mScreenSize{ imgSize },
    mSamples{ samples }
{
  constexpr TReal zoom = TReal(1.0);

  this->mLowLeftCorner = 
    this->mOrigin + this->mForward * zoom + 
    this->mSide * (-scrWidth / 2.0f) + this->mUp * (-scrHeight / 2.0f); 

  this->mCellRight = mSide * (scrWidth / TReal(this->mScreenSize.X) );
  this->mCellUp = mUp * (scrHeight / TReal(this->mScreenSize.Y) );

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
  }
}

const DUVec2& FCamera::GetImageSize() const noexcept
{
  return this->mScreenSize;
}

std::vector<DRay> FCamera::CreateRay(TIndex x, TIndex y) const noexcept
{
  assert(x < this->mScreenSize[0] && y < this->mScreenSize[1]);
  
  auto pos = 
      this->mLowLeftCorner 
    + (this->mCellRight * TReal(x)) 
    + (this->mCellUp * TReal(y));

  std::vector<DVec3> offsets;
  std::vector<DRay> rayList;
  if (this->mSamples == 1)
  {
    offsets = { this->mCellRight * .5f + this->mCellUp * .5f };
  }
  else if (this->mSamples == 2)
  {
    offsets =
    {
      this->mCellRight * 0.25f + this->mCellUp * 0.25f,
      this->mCellRight * 0.75f + this->mCellUp * 0.75f
    };
  }
  else if (this->mSamples == 4)
  {
    offsets =
    {
      this->mCellRight * 1.f / 8.f + this->mCellUp * 5.f / 8.f,
      this->mCellRight * 3.f / 8.f + this->mCellUp * 1.f / 8.f,
      this->mCellRight * 5.f / 8.f + this->mCellUp * 7.f / 8.f,
      this->mCellRight * 7.f / 8.f + this->mCellUp * 3.f / 8.f,
    };
  }
  else if (this->mSamples == 8)
  {
    const auto right = this->mCellRight / 16.f;
    const auto up    = this->mCellUp / 16.f;
    offsets = 
    {
      right * 1, up * 5,
      right * 3, up * 13,
      right * 5, up * 1,
      right * 7, up * 9,
      right * 9, up * 15,
      right * 11, up * 7,
      right * 13, up * 3,
      right * 15, up * 11
    };
  }

  for (const auto& offset : offsets)
  {
    const auto orig = pos + offset;
    const auto dir = orig - this->mOrigin;
    rayList.emplace_back(orig, dir);
  }
  return rayList;
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