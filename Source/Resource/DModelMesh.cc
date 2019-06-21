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

#include <Resource/DModelMesh.hpp>
#include <Manager/MModel.hpp>
#include <Manager/MMaterial.hpp>

namespace ray
{

DModelMesh::DModelMesh(const PCtor& ctor)
  : mId { ctor.mId },
    mDefaultMatId { ctor.mpMatId != nullptr ? *ctor.mpMatId : DMatId{} },
    mModelBufferId { ctor.mBufferId },
    mName { ctor.mpShape->name },
    mIndices ( ctor.mpShape->mesh.indices.size() )
{
  // Copy indices.
  static_assert(sizeof(DModelIndex) == sizeof(tinyobj::index_t));
  std::memcpy(this->mIndices.data(), ctor.mpShape->mesh.indices.data(), sizeof(DModelIndex) * this->mIndices.size());

  // Check given id is valid.
  if (this->HasDefaultMaterial() == true)
  {
    [[maybe_unused]] const auto flag = 
          EXPR_SGT(MMaterial).HasMaterial(this->GetDefaultMaterialId())
      ||  EXPR_SGT(MMaterial).HasMaterialCandidate(this->GetDefaultMaterialId());
    assert(flag == true);
  }
  {
    [[maybe_unused]] const auto flag = EXPR_SGT(MModel).HasModelBuffer(this->mModelBufferId);
    assert(flag == true);
  }
}

const DMeshId& DModelMesh::GetId() const noexcept
{
  return this->mId;
}

bool DModelMesh::HasDefaultMaterial() const noexcept
{
  return this->mDefaultMatId.HasId();
}

const DMatId& DModelMesh::GetDefaultMaterialId() const noexcept
{
  return this->mDefaultMatId;
}

const DModelBufferId & DModelMesh::GetModelBufferId() const noexcept
{
  return this->mModelBufferId;
}

const std::string& DModelMesh::GetName() const noexcept
{
  return this->mName;
}

const std::vector<DModelIndex>& DModelMesh::GetIndices() const noexcept
{
  return this->mIndices;
}

} /// ::ray namespace