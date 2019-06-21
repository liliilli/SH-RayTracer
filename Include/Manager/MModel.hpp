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
#include <DModelId.hpp>

namespace ray
{

/// @class MModel
/// @brief Material management singleton type.
class MModel final : public ::dy::expr::ISingleton<MModel>
{
public:
  EXPR_SINGLETON_DERIVED(MModel);
  EXPR_SINGLETON_PROPERTIES(MModel);

  /// @brief Create Model
  std::optional<DModelId> AddModel(const std::filesystem::path& path);

  /// @brief 
  bool HasModel(const DModelId& id) const noexcept;

  /// @brief 


private:
  using TModelKey = DModelId; 
  using TModelContainer = std::unordered_map<TModelKey, std::string>;
  
  TModelContainer mModelContainer;
};

} /// ::ray namespace