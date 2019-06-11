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

#include <Expr/ISingleton.h>
#include <unordered_map>
#include <memory>

#include <Math/Type/Micellanous/DUuid.h>
#include <IMaterial.hpp>

namespace ray
{

/// @class MMaterial
/// @brief Material management singleton type.
class MMaterial final : public ::dy::expr::ISingleton<MMaterial>
{
public:
  EXPR_SINGLETON_DERIVED(MMaterial);
  EXPR_SINGLETON_PROPERTIES(MMaterial);

private:
  using TKey = ::dy::math::DUuid;
  using TValue = std::unique_ptr<IMaterial>;
  using TContainer = std::unordered_map<TKey, TValue>;

  TContainer mContainer;
};

} /// ::ray namespace