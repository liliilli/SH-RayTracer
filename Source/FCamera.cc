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

namespace ray
{

FCamera::FCamera(
    const DVec3& origin, const DVec3& forwardTo, 
    TU32 imgWidth, TU32 imgHeight,
    TReal scrWidth, TReal scrHeight)
  : mOrigin{ origin },
    mForward{ (forwardTo - origin).Normalize() },
    mSide{ Cross(this->mForward, decltype(mForward){0, 1, 0}) },
    mUp{ Cross(this->mSide, this->mForward) },
    mScreenSize{ imgWidth, imgHeight }
{
  constexpr TReal zoom = TReal(1.0);

  this->mLowLeftCorner = 
    this->mOrigin + this->mForward * zoom + 
    this->mSide * (-scrWidth / 2.0f) + this->mUp * (-scrHeight / 2.0f); 
  {
    auto& pos = this->mLowLeftCorner;
    std::printf("%f, %f, %f\n", pos.X, pos.Y, pos.Z);
  }
  {
    auto& pos = this->mForward;
    std::printf("%f, %f, %f\n", pos.X, pos.Y, pos.Z);
  }
  {
    auto& pos = this->mSide;
    std::printf("%f, %f, %f\n", pos.X, pos.Y, pos.Z);
  }
  {
    auto& pos = this->mUp;
    std::printf("%f, %f, %f\n", pos.X, pos.Y, pos.Z);
  }
  this->mCellRight = mSide * (scrWidth / imgWidth);
  this->mCellUp = mUp * (scrHeight / imgHeight);
}

const DUVec2& FCamera::GetImageSize() const noexcept
{
  return this->mScreenSize;
}

std::vector<DRay<TReal>> FCamera::CreateRay(TIndex x, TIndex y) const noexcept
{
  assert(x < this->mScreenSize[0] && y < this->mScreenSize[1]);
  
  auto pos = 
      this->mLowLeftCorner 
    + (this->mCellRight * TReal(x)) 
    + (this->mCellUp * TReal(y));

  std::vector<DVec3> offsets;
  std::vector<DRay<TReal>> rayList;
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