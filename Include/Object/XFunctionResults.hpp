#pragma once
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

#include <Shape/EShapeType.hpp>
#include <XCommon.hpp>

namespace ray
{

class IHitable; // Forward declaration

/// @class PTValueResult
/// @brief GetRayIntersectedTValues returning type.
class PTValueResult final
{
public:
  TReal       mT;
  EShapeType  mShapeType;
  const IHitable* mpHitable;
  DVec3       mSurfaceNormal;

  PTValueResult(TReal t, EShapeType type, const IHitable* pHitable, const DVec3& normal)
    : mT { t },
      mShapeType { type },
      mpHitable { pHitable },
      mSurfaceNormal { normal }
  { }
};
// Memory alignment optimization?
static_assert(sizeof(PTValueResult) == 32);

/// @class PScatterResult
/// @brief TryScatter (Diffuse) returing type.
class PScatterResult final
{
public:
  DVec3 mReflectionDir;
  DVec3 mAttenuationCol;
  alignas(8) bool mIsScattered;

  PScatterResult(const DVec3& refDir, const DVec3& attCol, bool isScattered)
    : mReflectionDir { refDir },
      mAttenuationCol { attCol },
      mIsScattered { isScattered }
  { }
};
// Memory alignment optimization?
static_assert(sizeof(PScatterResult) == 32);

/// @class PTriangleResult
/// @brief GetIntersectedTriangleTValue returning type.
class PTriangleResult final
{
public:
  using TReal   = ray::TReal;
  using TIndex  = ray::TIndex;
  TReal mT;
  std::array<TIndex, 3> mIndex;
};

} /// ::ray namespace