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
#include <nlohmann/json_fwd.hpp>
#include <Math/Type/Shape/DBox.h>
#include <Expr/XEnumConversion.h>
#include <Interface/IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>
#include <Id/DMeshId.hpp>
#include <Id/DModelBufferId.hpp>
#include <Resource/DModelMesh.hpp>
#include <Resource/DModelBuffer.hpp>

namespace ray
{

/// @class FModelMesh
/// @brief Box type.
class FModelMesh final : public IHitable
{
public:
  /// @brief Constructor type of FModelMesh.
  struct PCtor final
  {
    DVec3 mOrigin; 
    TReal mScale; 
    DVec3 mAngle; 
    DMeshId mMeshId;
    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FModelMesh(const PCtor& ctor, IMaterial* mat);
  virtual ~FModelMesh() = default;

  const DQuat& GetQuaternion() const noexcept { return this->mRotQuat; }

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const override final;

  std::optional<TValueResults> GetRayIntserectedTValues2(const DRay& ray);

  ///
  std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t) const override final;

  /// @brief Get PCtor instance from instance, with given type value.
  /// @param type Type value.
  FModelMesh::PCtor GetPCtor() const noexcept;

private:
  DVec3 mOrigin;
  TReal mScale;
  DQuat mRotQuat;

  DMeshId         mMeshId;
  DModelBufferId  mModelBufferId;
  DModelMesh*     mpMesh = nullptr;
  DModelBuffer*   mpModelBuffer = nullptr;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FModelMesh::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FModelMesh::PCtor& oCtor);

/// @brief FModelMesh specialized function.
template <> json::FExistanceList JsonCheckExistances<FModelMesh::PCtor>(const nlohmann::json& json);

} /// ::ray namespace
