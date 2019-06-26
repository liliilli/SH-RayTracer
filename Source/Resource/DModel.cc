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

#include <Resource/DModel.hpp>

namespace ray
{

DModel::DModel(const PCtor& ctor)
  : mId { ctor.mId },
    mBufferId { ctor.mBufferId },
    mMeshes { ctor.mMeshIds },
    mMaterials { ctor.mMaterialIds }
{ }

const std::vector<DMeshId>& DModel::GetMeshIds() const noexcept
{
  return this->mMeshes;
}

const std::vector<DMatId>& DModel::GetInternalMaterials() const noexcept
{
  return this->mMaterials;
}

} /// ::ray namespace