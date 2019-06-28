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

#include <XCommon.hpp>
#include <algorithm>

namespace ray
{

/// @class DModelFace
/// @brief Stores the pointer of triangle vertices, normals, and optional uv0s with index.
/// Stored pointer value must be valid in lifescope.
class DModelFace final
{
public:
  std::array<const DVec3*, 3> mVertex;
  std::array<const DVec3*, 3> mNormal;
  std::array<const DVec2*, 3> mUv0;
  std::array<TIndex, 3>       mIndex;
  DAABB mTriangleAABB;

  /// @brief Generate (Create) AABB with vertex.
  bool CreateAABB() noexcept
  {
    const auto flag = std::any_of(
      EXPR_BIND_BEGIN_END(this->mVertex), 
      [](const auto& pVertex) { return pVertex == nullptr; });
    if (flag == true) { return false; }

    this->mTriangleAABB = DAABB{{*mVertex[0], *mVertex[1], *mVertex[2]}};
    return true;
  }
};

} /// ::ray namespace