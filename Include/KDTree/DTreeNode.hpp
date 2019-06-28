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

#include <memory>
#include <XCommon.hpp>

struct TriangleResult 
{
  using TReal   = ray::TReal;
  using TIndex  = ray::TIndex;
  TReal mT;
  std::array<TIndex, 3> mIndex;
};

namespace ray
{

class DModelFace;

/// @class DTreeNode
/// @brief Triangle KDTree node for optimization
class DTreeNode final
{
public:
  /// @brief 
  /// @param
  void BuildTree(const std::vector<const DModelFace*>& pTriangles);

  /// @brief
  std::vector<TriangleResult> TempGetTValues(const DRay& localRay) const;

private:
  DAABB mOverallBoundingBox;
  std::unique_ptr<DTreeNode>      mLeftNode;
  std::unique_ptr<DTreeNode>      mRightNode;
  std::vector<const DModelFace*>  mTriangles;

  DVec3 mBarycentric = DVec3{};
  ::dy::math::EAxis mAxis; 
};

} /// ::ray namespace
