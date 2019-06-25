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

#include <string>
#include <vector>
#include <tinyobj/tiny_obj_loader.h>
#include <Id/DMeshId.hpp>
#include <Id/DModelBufferId.hpp>
#include <Id/DMatId.hpp>

#include <XCommon.hpp>
#include <Resource/DModelIndex.hpp>

namespace ray
{

/// @class DModelMesh
/// @brief Model mesh instance.
class DModelMesh final
{
public:
  class PCtor final
  {
  public:
    DMeshId         mId;
    DMatId*         mpMatId = nullptr;
    DModelBufferId  mBufferId;
    const tinyobj::shape_t* mpShape = nullptr;
  };

  DModelMesh(const PCtor& ctor);

  /// @brief Get Model Mesh ID.
  const DMeshId& GetId() const noexcept;
  /// @brief Check model mesh has default metarial.
  bool HasDefaultMaterial() const noexcept;
  /// @brief Get default material id. This function does not guarantee that returned DMatId is valid or not.
  const DMatId& GetDefaultMaterialId() const noexcept;
  /// @brief Get Model Buffer Id that this mesh forwards to.
  const DModelBufferId& GetModelBufferId() const noexcept;

  /// @brief Get name of mesh. Name may be empty by loaded model file.
  const std::string& GetName() const noexcept;
  /// @brief Get index list of mesh.
  std::vector<DModelIndex>& GetIndices() noexcept;
  /// @brief Get index list of mesh.
  const std::vector<DModelIndex>& GetIndices() const noexcept;

private:
  DMeshId         mId;
  DMatId          mDefaultMatId;
  DModelBufferId  mModelBufferId;

  std::string mName;
  std::vector<DModelIndex> mIndices;
};

} /// ::ray namespace