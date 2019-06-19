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

#include <Expr/TEnumString.h>

namespace ray
{

/// @enum EObject
/// @brief Object type value.
EXPR_DEFINE_ENUM(EObject, 
  Camera,
  Hitable
);

/// @class IObject
/// @brief Object interface type.
class IObject
{
public:
  IObject(EObject::_ type);
  virtual ~IObject() = 0;

  /// @brief Get Object Type value.
  /// @return Object Type Value.
  EObject::_ GetObjectType() const noexcept; 
 
private:
  EObject::_ mObjectType = EObject::__Error;
};

inline IObject::~IObject() = default; 

} /// ::ray namespace