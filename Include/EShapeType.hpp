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

#include <Expr/XEnumConversion.h>

namespace ray
{

/// @enum class EShapeType
/// @brief EShapeType value.
enum class EShapeType 
{
  Sphere,
  Plane,
  Box,
  Torus,
  Cone,
  Capsule,
  Model,
};

//!
//! Forward declaration
//!

class FSphere;
class FPlane;
class FBox;
class FTorus;
class FCone;
class FCapsule;
class FModel;

EXPR_INIT_ENUMTOTYPE(ShapeType, EShapeType);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Sphere, FSphere);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Plane, FPlane);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Box, FBox);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Torus, FTorus);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Cone, FCone);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Capsule, FCapsule);
EXPR_SET_ENUMTOTYPE_CONVERSION(ShapeType, EShapeType::Model, FModel);

} /// ::ray namespace