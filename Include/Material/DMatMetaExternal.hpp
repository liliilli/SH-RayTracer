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

#include <nlohmann/json_fwd.hpp>
#include <tinyobj/tiny_obj_loader.h>
#include <Math/Type/Micellanous/DUuid.h>

#include <IMaterial.hpp>
#include <XCommon.hpp>
#include <DMatId.hpp>
#include <Resource/DMaterialProperty.hpp>

namespace ray
{

/// @class DMatMetaExternal
/// @brief Meta information type of external material candidate (not used yet)
class DMatMetaExternal final
{
public:
  DMatMetaExternal(const DMatId& id, const tinyobj::material_t& info);

  /// @brief Get material property.
  const DMaterialProperty& GetMaterialProperty() const noexcept;

  /// @brief Get material ID.
  const DMatId& GetId() const noexcept;

private:
  DMatId mId;
  DMaterialProperty mProperty;
};

} /// ::ray namespace