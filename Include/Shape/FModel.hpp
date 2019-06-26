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

#include <array>
#include <filesystem>

#include <nlohmann/json_fwd.hpp>
#include <Expr/XEnumConversion.h>
#include <IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>
#include <Id/DModelId.hpp>
#include <Shape/FModelMesh.hpp>

namespace ray
{

/// @class FModel
/// @brief Model type.
class FModel final : public IHitable
{
public:
  /// @brief Constructor type of FModel.
  struct PCtor final
  {
    DVec3 mOrigin; 
    TReal mScale; 
    DVec3 mAngle; 
    std::string mPath;
    DModelId    mModelId;

    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FModel(const PCtor& ctor, IMaterial* mat);
  virtual ~FModel() = default;

  /// @brief Get (x, y, z) position value.
  const DVec3& GetOrigin() const noexcept;
  /// @brief Get uniformized scale value.
  const TReal& GetScale() const noexcept;
  /// @brief Get angle quaternion.
  const DQuat& GetQuaternion() const noexcept;

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const override final;

  std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t) const override final;

  /// @brief Get PCtor instance from instance.
  /// @param type Type value.
  FModel::PCtor GetPCtor() const noexcept;

  /// @brief Try populate resource when resource is not populated yet.
  /// If populated, just return true.
  bool TryPopulateResource();
  /// @brief Check model resource is populated or not.
  /// @return If resource is populated, just return true.
  bool IsResourcePopulated() const noexcept;
  /// @brief 
  /// @return If `IsResourcePopulated()` returned value is false, path will be returned.
  std::optional<std::filesystem::path> TryGetResourcePath() const noexcept;
  /// @brief
  /// @return If `IsResourcePopulated()` returned value is true, model id will be returned.
  std::optional<DModelId> TryGetResourceId() const noexcept;

private:
  DVec3 mOrigin;
  TReal mScale;
  DQuat mRotQuat;

  std::variant<std::filesystem::path, DModelId> mResource;
  bool mIsPopulated = false;

  std::vector<std::unique_ptr<FModelMesh>> mpMeshes;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FModel::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FModel::PCtor& oCtor);

/// @brief FModel specialized function.
template <> json::FExistanceList JsonCheckExistances<FModel::PCtor>(const nlohmann::json& json);

} /// ::ray namespace
