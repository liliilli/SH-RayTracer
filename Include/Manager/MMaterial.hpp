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
#include <memory>
#include <optional>
#include <iostream>

#include <nlohmann/json_fwd.hpp>
#include <Expr/ISingleton.h>
#include <Math/Type/Micellanous/DUuid.h>
#include <IMaterial.hpp>
#include <DMatId.hpp>

namespace ray
{

class FMatLambertian;
class FMatMetal;
class FMatDielectric;

/// @class MMaterial
/// @brief Material management singleton type.
class MMaterial final : public ::dy::expr::ISingleton<MMaterial>
{
public:
  EXPR_SINGLETON_DERIVED(MMaterial);
  EXPR_SINGLETON_PROPERTIES(MMaterial);

  /// @brief Add material from old structured json atlas with TType material type.
  /// @param json Json atlas of old material structure.
  /// @tparam TType Material Type
  /// @return If succcessful, return created material's id instance.
  template <typename TType>
  std::optional<DMatId> AddOldMaterial(const nlohmann::json& json);

  /// @brief Add material from v190710 structured json atlas with TType material type.
  /// @param json Json atlas of v190710 structure.
  /// @tparam TType Material Type
  /// @return If successful, return created material's id instance.
  template <typename TType>
  std::optional<DMatId> AddMaterial(const nlohmann::json& json, const std::string& name);

  /// @brief Add material with internal constructor type of material type.
  /// @param ctor Construction paratmeter type.
  /// @tparam TType Material Type
  /// @return If successful, return created material's id instance.
  template <typename TType>
  std::optional<DMatId> AddMaterial(const typename TType::PCtor& ctor);

  /// @brief Check container has material that named `id`.
  /// @param id Id of material to find.
  /// @return If found, return true or false.
  bool HasMaterial(const DMatId& id) const noexcept;

  /// @brief Get pointer of material that has id.
  /// @param id Id of material.
  /// @return The pointer of material instance. If failed, just return nullptr.
  IMaterial* GetMaterial(const DMatId& id);

private:
  std::optional<DMatId> AddOldMaterial_FMatLambertian(const nlohmann::json& json);
  std::optional<DMatId> AddOldMaterial_FMatMetal(const nlohmann::json& json);
  std::optional<DMatId> AddOldMaterial_FMatDielectric(const nlohmann::json& json);

  std::optional<DMatId> AddMaterial_FMatLambertian(const nlohmann::json& json, const std::string& id);
  std::optional<DMatId> AddMaterial_FMatMetal(const nlohmann::json& json, const std::string& id);
  std::optional<DMatId> AddMaterial_FMatDielectric(const nlohmann::json& json, const std::string& id);

  using TKey = DMatId; 
  using TValue = std::unique_ptr<IMaterial>;
  using TContainer = std::unordered_map<TKey, TValue>;

  TContainer mContainer;
};

} /// ::ray namespace
#include <Inline/MMaterial.inl>