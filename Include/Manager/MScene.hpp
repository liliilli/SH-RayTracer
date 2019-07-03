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
#include <vector>
#include <type_traits>
#include <unordered_map>

#include <Expr/ISingleton.h>

#include <XCommon.hpp>
#include <Interface/IHitable.hpp>
#include <Object/FCamera.hpp>
#include <KDTree/DObjectNode.hpp>
#include <Interface/IObject.hpp>

namespace ray
{
 
/// @class MScene
/// @brief Scene Manager
class MScene final : public ::dy::expr::ISingleton<MScene>
{
public:
  /// @struct PSceneDefaults
  /// @brief Parameter type for handling values into functions.
  struct PSceneDefaults final
  {
    /// @brief Image size to create.
    DUVec2  mImageSize;
    /// @brief The count of samples per pixel.
    TU32    mNumSamples;
  };

  EXPR_SINGLETON_DERIVED(MScene);
  EXPR_SINGLETON_PROPERTIES(MScene);

  /// @brief Add hitable object into scene object list.
  /// @tparam TType TType that derives IHitable. 
  /// @tparam TArgs Constructor arguments of TType.
  template <typename TType, typename... TArgs>
  void AddHitableObject(TArgs&&... args);

	/// @brief Add sample objects into scene.
  /// @param defaults Default values.
	void AddSampleObjects(const PSceneDefaults& defaults);

  /// @brief Load scene file.
  /// If succeeded, return true. Otherwise, return false.
  /// @param defaults Default values.
  /// @return If successful, return true.
  bool LoadSceneFile(const std::string& pathString, const PSceneDefaults& defaults);

  /// @brief Proceed ray.
  /// @param ray The ray to be proceeded, in world-space.
  /// @param cnt Depth count of function.
  /// @param limit Depth count limit. If cnt hits limit, function will be suspended.
  /// @return RGB Color that has range of [0, 1].
  DVec3 ProceedRay(const DRay& ray, TIndex cnt = 0, TIndex limit = 8);

  /// @brief Get immutable pointer of camera.
  const FCamera* GetCamera() const noexcept;

private:
  /// @brief Load scene with v190710 structure.
  /// @param json Json atlas.
  /// @return Success flag when returned true.
  bool LoadSceneFile190710(const nlohmann::json& json, const PSceneDefaults& defaults);
  /// @brief Load model from v190710 scene structure.
  /// @param json Json atlas of `models`.
  /// @return Success flag when returned true.
  bool AddModelsFromJson190710(const nlohmann::json& json);
  /// @brief Load materials from v190710 scene structure.
  /// @param json Json atlas of `materials`.
  /// @return Success flag when returned true.
  bool AddMaterialsFromJson190710(const nlohmann::json& json);
  /// @brief Add prefabs into container automatically from v190710 scene structure.
  /// @param json Json atlas of `prefabs`.
  /// @return Success flag when returned true.
  bool AddPrefabsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults);
  /// @brief Add prefabs into container automatically from v190710 scene structure.
  /// @param json Json atlas of `objects`.
  /// @return Success flag when returned true.
  bool AddObjectsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults);

  std::unordered_map<std::string, std::unique_ptr<IObject>> mPrefabs;
  std::vector<std::unique_ptr<IHitable>> mObjects;
  std::unique_ptr<FCamera>      mMainCamera;
  std::unique_ptr<DObjectNode>  mObjectTree;
};

} /// ::ray namespace
#include <Inline/MScene.inl>