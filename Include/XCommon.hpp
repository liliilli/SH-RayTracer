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

#include <Math/Common/TGlobalTypes.h>
#include <Math/Type/Math/DVector3.h>
#include <Math/Type/Micellanous/DDynamicGrid2D.h>

using namespace dy::math;

namespace ray
{

using DVec3 = DVector3<TReal>;
using DIVec3 = DVector3<TI32>;
  
/// @brief Create image ppm with grid2d container.
/// @return If successful, return true. Otherwise, return false.
bool CreateImagePpm(const char* const path, DDynamicGrid2D<DVector3<TI32>>& container);

} /// ::ray namespace