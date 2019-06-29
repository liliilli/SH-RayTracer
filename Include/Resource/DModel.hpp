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

#include <vector>
#include <XCommon.hpp>
#include <Id/DModelId.hpp>
#include <Id/DMeshId.hpp>
#include <Id/DModelBufferId.hpp>
#include <Id/DMatId.hpp>

namespace ray
{

/// @class DModel
/// @brief Model resource type.
class DModel final 
{
public:
  class PCtor final
  {
  public:
    DModelId mId;
    DModelBufferId mBufferId;
    std::vector<DMeshId>  mMeshIds;
    std::vector<DMatId>   mMaterialIds;
  };
  DModel(const PCtor& ctor); 

  /// @brief Get mesh id list from this instance.
  /// @return Valid Mesh Id list.
  const std::vector<DMeshId>& GetMeshIds() const noexcept;

  /// @brief Get internal material's id list from this instance.
  /// @return Valid material id list.
  const std::vector<DMatId>& GetInternalMaterials() const noexcept;

private:
  DModelId              mId;
  DModelBufferId        mBufferId;
  std::vector<DMeshId>  mMeshes;
  std::vector<DMatId>   mMaterials;
};

} /// ::ray namespace