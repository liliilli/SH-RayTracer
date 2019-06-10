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

#include <Shape/FPlane.hpp>

namespace ray
{

FPlane::FPlane(const DVec3& normal, TReal d, std::unique_ptr<IMaterial>&& mat)
  : IHitable{EShapeType::Plane, std::move(mat)},
    DPlane<TReal>{normal, d}
{ }

FPlane::FPlane(const DVec3& pos1, const DVec3& pos2, const DVec3& pos3, std::unique_ptr<IMaterial>&& mat)
  : IHitable{EShapeType::Plane, std::move(mat)},
    DPlane<TReal>{pos1, pos2, pos3}
{ }

} /// ::ray namespace