#pragma once
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

#include <XCommon.hpp>
#include <Math/Type/Shape/DRay.h>

namespace ray
{

/// @class FCamera
/// @brief Camera class that manage image size, eye / forward and screen size to focus.
class FCamera final
{
public:
  FCamera(
    const DVec3& origin, const DVec3& forwardTo, 
    TU32 imgWidth, TU32 imgHeight,
    TReal scrWidth, TReal scrHeight);

  /// @brief Get image size.
  const DUVec2& GetImageSize() const noexcept;

  /// @brief Get ray calculated by [x, y] of Image size and eye / forward.
  DRay<TReal> CreateRay(TIndex x, TIndex y) const noexcept;

private:
  DVec3 mOrigin;
  DVec3 mForward;
  DVec3 mSide;
  DVec3 mUp;

  DVec3 mLowLeftCorner;
  DVec3 mCellRight, mCellUp;
  DUVec2 mScreenSize;
};

} /// ::ray namespace