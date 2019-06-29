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
    DUVec2  mImageSize;
    TU32    mNumSamples;
  };

  EXPR_SINGLETON_DERIVED(MScene);
  EXPR_SINGLETON_PROPERTIES(MScene);

  /// @brief Add hitable object into scene object list.
  /// @tparam TType TType that derives IHitable. 
  /// @tparam TArgs Constructor arguments of TType.
  template <typename TType, typename... TArgs>
  void AddHitableObject(TArgs&&... args)
  {
    static_assert(
      std::is_base_of_v<IHitable, TType> == true,
      "TTYpe Must be Hitable object.");

    std::unique_ptr<IHitable> obj = std::make_unique<TType>(std::forward<TArgs>(args)...);
    this->mObjects.emplace_back(obj.release());
  }

	/// @brief Add sample objects into scene.
	void AddSampleObjects(const DUVec2& imgSize, TU32 numSamples);

  /// @brief Load scene file.
  /// If succeeded, return true. Otherwise, return false.
  bool LoadSceneFile(const std::string& pathString, const DUVec2& imgSize, TU32 numSamples);

  /// @brief Proceed ray.
  /// @brief RGB Color that has range of [0, 1].
  DVec3 ProceedRay(const DRay& ray, TIndex t = 0, TIndex limit = 8);

  /// @brief Get pointer of camera.
  FCamera* GetCamera() const noexcept { return this->mMainCamera.get(); }

private:
  /// @brief
  bool LoadOldSceneFile(const nlohmann::json& json, const DUVec2& imgSize, TU32 numSamples);

  /// @brief
  bool LoadSceneFile190710(const nlohmann::json& json, const PSceneDefaults& defaults);
  /// @brief
  bool AddModelsFromJson190710(const nlohmann::json& json);
  /// @brief
  bool AddMaterialsFromJson190710(const nlohmann::json& json);
  /// @brief Add prefabs into container automatically.
  /// @param json Json atlas of `prefabs`.
  /// @return Success flag when returned true.
  bool AddPrefabsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults);
  /// @brief 
  bool AddObjectsFromJson190710(const nlohmann::json& json, const PSceneDefaults& defaults);

  std::unordered_map<std::string, std::unique_ptr<IObject>> mPrefabs;
  std::vector<std::unique_ptr<IHitable>> mObjects;
  std::unique_ptr<FCamera> mMainCamera;
};

} /// ::ray namespace