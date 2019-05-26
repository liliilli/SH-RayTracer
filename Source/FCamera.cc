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
    this->mForward * zoom + 
    this->mSide * (-scrWidth / 2.0f) + this->mUp * (-scrHeight / 2.0f); 
  this->mCellRight = mSide * (scrWidth / imgWidth);
  this->mCellUp = mUp * (scrHeight / imgHeight);
}

const DUVec2& FCamera::GetImageSize() const noexcept
{
  return this->mScreenSize;
}

DRay<TReal> FCamera::CreateRay(TIndex x, TIndex y) const noexcept
{
  assert(x < this->mScreenSize[0] && y < this->mScreenSize[1]);
  
  const auto pos = 
      this->mLowLeftCorner 
    + (this->mCellRight * TReal(x)) 
    + (this->mCellUp * TReal(y));
  const auto dir = (pos - this->mOrigin).Normalize();

  return {pos, dir};
}

} /// ::ray namespace 