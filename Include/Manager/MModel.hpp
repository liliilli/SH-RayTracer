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

#include <unordered_map>
#include <filesystem>
#include <memory>
#include <optional>

#include <nlohmann/json_fwd.hpp>
#include <Expr/ISingleton.h>
#include <Math/Type/Micellanous/DUuid.h>
#include <IMaterial.hpp>
#include <Id/DModelId.hpp>
#include <Id/DModelBufferId.hpp>
#include <Id/DMeshId.hpp>
#include <Resource/DModelBuffer.hpp>
#include <Resource/DModelMesh.hpp>
#include <Resource/DModel.hpp>
#include <Resource/DModelPrefab.hpp>

namespace ray
{

/// @class MModel
/// @brief Material management singleton type.
class MModel final : public ::dy::expr::ISingleton<MModel>
{
public:
  EXPR_SINGLETON_DERIVED(MModel);
  EXPR_SINGLETON_PROPERTIES(MModel);

  /// @brief Try add model prefab with model id.
  /// If successful, return true. Otherwise, return false.
  /// @param id Model id to use.
  /// @param prefab Prefab.
  /// @return If successful, return true. Otherwise, return false.
  bool AddModelPrefab(const DModelId& id, const DModelPrefab& prefab);
  /// @brief Check given id has valid, so DModelPrefab is exist in container.
  /// @param id Model id.
  /// @return If found, return true. Otherwise, return false.
  bool HasModelPrefab(const DModelId& id) const noexcept;
  /// @brief Get pointer of model prefab that has given id.
  /// @param id Model id.
  /// @return The pointer of DModelPrefab when find, If not find just return nullptr.
  const DModelPrefab* GetModelPrefab(const DModelId& id) const noexcept;

  /// @brief Create Model
  std::optional<DModelId> AddModel(const std::filesystem::path& path);

  /// @brief Check given id has valid, so DModel is exist in container.
  /// @param id Model id.
  /// @return If found, return true. Otherwise, return false.
  bool HasModel(const DModelId& id) const noexcept;
  /// @brief Get pointer of model that has given id.
  /// @param id Model id.
  /// @return The pointer of DModel when found, If not find just return nullptr.
  DModel* GetModel(const DModelId& id) noexcept;
  /// @brief Get pointer of model that has given id.
  /// @param id Model id.
  /// @return The pointer of DModel when found, If not find just return nullptr.
  const DModel* GetModel(const DModelId& id) const noexcept;

  /// @brief 
  bool HasModelBuffer(const DModelBufferId& id) const noexcept;
  /// @brief Get pointer of model buffer that has given id.
  /// @param id Model Buffer Id.
  /// @return The pointer of DModelBuffer when found, If not find just return nullptr.
  DModelBuffer* GetModelBuffer(const DModelBufferId& id) noexcept;
  /// @brief Get pointer of model buffer that has given id.
  /// @param id Model Buffer Id.
  /// @return The pointer of DModelBuffer when found, If not find just return nullptr.
  const DModelBuffer* GetModelBuffer(const DModelBufferId& id) const noexcept;

  /// @brief Check given id has valid, so DModelMesh is in container.
  /// @param id Model Mesh Id.
  /// @return If found, return true. Otherwise, return false.
  bool HasMesh(const DMeshId& id) const noexcept;
  /// @brief Get pointer of mesh that has given id.
  /// @param id Model Mesh Id.
  /// @return The pointer of DModelMesh when found, If not find just return nullptr.
  DModelMesh* GetMesh(const DMeshId& id) noexcept;
  /// @brief Get pointer of mesh that has given id.
  /// @param id Model Mesh Id.
  /// @return The pointer of DModelMesh when found, If not find just return nullptr.
  const DModelMesh* GetMesh(const DMeshId& id) const noexcept;

private:
  using TModelKey = DModelId; 
  using TModelContainer = std::unordered_map<TModelKey, DModel>;
  TModelContainer mModelContainer;

  using TModelBufferKey = DModelBufferId;
  using TBufferContainer = std::unordered_map<TModelBufferKey, DModelBuffer>;
  TBufferContainer mBufferContainer;

  using TMeshKey = DMeshId;
  using TMeshContainer = std::unordered_map<TMeshKey, DModelMesh>;
  TMeshContainer mMeshContainer;

  using TModelPrefabs = std::unordered_map<TModelKey, DModelPrefab>;
  TModelPrefabs mModelPrefabs;
};

} /// ::ray namespace